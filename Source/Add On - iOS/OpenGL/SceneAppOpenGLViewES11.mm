//----------------------------------------------------------------------------------------------------------------------
//	SceneAppOpenGLView.mm			©2019 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#import "SceneAppOpenGLView.h"

#import "COpenGLES11GPU.h"
#import "CLogServices.h"
#import "COpenGLGPU.h"

#import <OpenGLES/ES1/glext.h>
#import <QuartzCore/QuartzCore.h>

//----------------------------------------------------------------------------------------------------------------------
// MARK: Local procs declaration

static	void	sAcquireContextProc(SceneAppOpenGLView* sceneAppOpenGLView);
static	bool	sTryAcquireContextProc(SceneAppOpenGLView* sceneAppOpenGLView);
static	void	sReleaseContextProc(SceneAppOpenGLView* sceneAppOpenGLView);

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - SceneAppOpenGLView

@interface SceneAppOpenGLView () {
				// MARK: Properties
	COpenGLGPU*	mOpenGLGPU;
}

@property (nonatomic, strong)	EAGLContext*	context;
@property (nonatomic, strong)	NSLock*			contextLock;

@property (nonatomic, strong)	CADisplayLink*	displayLink;
@property (nonatomic, strong)	NSLock*			displayLinkLock;

@end

@implementation SceneAppOpenGLView

@synthesize touchesBeganProc;
@synthesize touchesMovedProc;
@synthesize touchesEndedProc;
@synthesize touchesCancelledProc;

@synthesize motionBeganProc;
@synthesize motionEndedProc;

@synthesize periodicProc;

// MARK: SceneAppGPUView methods

//----------------------------------------------------------------------------------------------------------------------
- (CGPU&) gpu
{
	return *mOpenGLGPU;
}

//----------------------------------------------------------------------------------------------------------------------
- (void) installPeriodic
{
	// Setup
	self.displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(displayLinkProc:)];

	// Install
	[self.displayLink addToRunLoop:NSRunLoop.currentRunLoop forMode:NSDefaultRunLoopMode];
}

//----------------------------------------------------------------------------------------------------------------------
- (void) removePeriodic
{
	// Lock to make sure we are not removing while in output callback
	[self.displayLinkLock lock];
	[self.displayLink invalidate];
	[self.displayLinkLock unlock];

	// Clear
	self.displayLink = nil;
}

// MARK: Class methods

//----------------------------------------------------------------------------------------------------------------------
+ (Class) layerClass
{
	return [CAEAGLLayer class];
}

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
- (instancetype) initWithFrame:(CGRect) frame
{
	// Do super init
	self = [super initWithFrame:frame];
	if (self != nil) {
		// Setup
		[self setup];
	}

	return self;
}

//----------------------------------------------------------------------------------------------------------------------
- (instancetype) initWithCoder:(NSCoder*) coder
{
	// Do super init
	self = [super initWithCoder:coder];
	if (self != nil) {
		// Setup
		[self setup];
	}

	return self;
}

//----------------------------------------------------------------------------------------------------------------------
- (void) deinit
{
	// Cleanup
	if(EAGLContext.currentContext == self.context)
		// Reset
		[EAGLContext setCurrentContext:nil];

	[self removePeriodic];

	DisposeOf(mOpenGLGPU);
}

// MARK: NSResponder methods

//----------------------------------------------------------------------------------------------------------------------
- (BOOL) canBecomeFirstResponder
{
	return YES;
}

//----------------------------------------------------------------------------------------------------------------------
- (void) touchesBegan:(NSSet<UITouch*>*) touches withEvent:(UIEvent*) event
{
	self.touchesBeganProc(touches, event);
}

//----------------------------------------------------------------------------------------------------------------------
- (void) touchesMoved:(NSSet<UITouch*>*) touches withEvent:(UIEvent*) event
{
	self.touchesMovedProc(touches, event);
}

//----------------------------------------------------------------------------------------------------------------------
- (void) touchesEnded:(NSSet<UITouch*>*) touches withEvent:(UIEvent*) event
{
	self.touchesEndedProc(touches, event);
}

//----------------------------------------------------------------------------------------------------------------------
- (void) touchesCancelled:(NSSet<UITouch*>*) touches withEvent:(UIEvent*) event
{
	self.touchesCancelledProc(touches, event);
}

//----------------------------------------------------------------------------------------------------------------------
- (void) motionBegan:(UIEventSubtype) eventSubtype withEvent:(UIEvent*) event
{
	self.motionBeganProc(eventSubtype, event);
}

//----------------------------------------------------------------------------------------------------------------------
- (void) motionEnded:(UIEventSubtype) eventSubtype withEvent:(UIEvent*) event
{
	self.motionEndedProc(eventSubtype, event);
}

// MARK: Internal methods

//----------------------------------------------------------------------------------------------------------------------
- (void) setup
{
	// Setup
	CGFloat	scale = UIScreen.mainScreen.scale;
	self.contentScaleFactor = scale;

	// Setup the layer
	CAEAGLLayer*	eaglLayer = (CAEAGLLayer*) self.layer;
	eaglLayer.opaque = YES;
	eaglLayer.drawableProperties =
			@{
				kEAGLDrawablePropertyRetainedBacking: @FALSE,
				kEAGLDrawablePropertyColorFormat: kEAGLColorFormatRGBA8
			 };

	// Setup the context
	self.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
	if (!self.context || ![EAGLContext setCurrentContext:self.context])
		return;

	// Finish setup
	self.contextLock = [[NSLock alloc] init];

	self.displayLinkLock = [[NSLock alloc] init];

	mOpenGLGPU =
			new COpenGLGPU(
					COpenGLGPUProcsInfo((COpenGLGPUAcquireContextProc) sAcquireContextProc,
							(COpenGLGPUTryAcquireContextProc) sTryAcquireContextProc,
							(COpenGLGPUReleaseContextProc) sReleaseContextProc, (__bridge void*) self));

	SOpenGLES11GPUSetupInfo	openGLES11GPUSetupInfo(scale, (__bridge void*) self.layer);
	mOpenGLGPU->setup(S2DSize32(self.bounds.size.width * scale, self.bounds.size.height * scale),
			&openGLES11GPUSetupInfo);

	// Check for errors
	if (glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES) != GL_FRAMEBUFFER_COMPLETE_OES) {
		// Error
		CLogServices::logError(CString("OpenGL ES 1.1 setup failed with error ") +
				CString(glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES), 6, false, true));
	}
}

//----------------------------------------------------------------------------------------------------------------------
- (void) displayLinkProc:(CADisplayLink*) displayLink
{
	// Try to acquire lock
	if ([self.displayLinkLock tryLock]) {
		@autoreleasepool {
			// Call proc
			self.periodicProc(displayLink.targetTimestamp);

			// Flush
			[self.context presentRenderbuffer:GL_RENDERBUFFER_OES];
		}

		// All done
		[self.displayLinkLock unlock];
	}
}

//----------------------------------------------------------------------------------------------------------------------
- (void) acquireContext
{
	// Lock
	[self.contextLock lock];

	// Make current
	[EAGLContext setCurrentContext:self.context];
}

//----------------------------------------------------------------------------------------------------------------------
- (BOOL) tryAcquireContext
{
	// Try lock
	if ([self.contextLock tryLock]) {
		// Make current
		[EAGLContext setCurrentContext:self.context];

		return YES;
	} else
		// Failed
		return NO;
}

//----------------------------------------------------------------------------------------------------------------------
- (void) releaseContext
{
	// Release lock
	[self.contextLock unlock];
}

@end

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - Local proc definitions

//----------------------------------------------------------------------------------------------------------------------
void sAcquireContextProc(SceneAppOpenGLView* sceneAppOpenGLView)
{
	[sceneAppOpenGLView acquireContext];
}

//----------------------------------------------------------------------------------------------------------------------
bool sTryAcquireContextProc(SceneAppOpenGLView* sceneAppOpenGLView)
{
	return [sceneAppOpenGLView tryAcquireContext];
}

//----------------------------------------------------------------------------------------------------------------------
void sReleaseContextProc(SceneAppOpenGLView* sceneAppOpenGLView)
{
	[sceneAppOpenGLView releaseContext];
}
