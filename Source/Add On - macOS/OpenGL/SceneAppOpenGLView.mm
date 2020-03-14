//----------------------------------------------------------------------------------------------------------------------
//	SceneAppOpenGLView.mm			©2019 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#import "SceneAppOpenGLView.h"

#import "CLogServices.h"
#import "COpenGLGPU.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: Local procs declaration

static	void		sAcquireContextProc(SceneAppOpenGLView* sceneAppOpenGLView);
static	bool		sTryAcquireContextProc(SceneAppOpenGLView* sceneAppOpenGLView);
static	void		sReleaseContextProc(SceneAppOpenGLView* sceneAppOpenGLView);
static	CVReturn	sDisplayLinkOutputCallback(CVDisplayLinkRef displayLink, const CVTimeStamp* inNow,
							const CVTimeStamp* inOutputTime, CVOptionFlags flagsIn, CVOptionFlags* flagsOut,
							void* context);

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - SceneAppOpenGLView

@interface SceneAppOpenGLView () {
				// MARK: Properties
	COpenGLGPU*	mOpenGLGPU;
}

@property (nonatomic, strong)	NSLock*				contextLock;

@property (nonatomic, assign)	CVDisplayLinkRef	displayLinkRef;
@property (nonatomic, strong)	NSLock*				displayLinkLock;

@end

@implementation SceneAppOpenGLView

@synthesize mouseDownProc;
@synthesize mouseDraggedProc;
@synthesize mouseUpProc;

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
	// Check display link ref
	if (self.displayLinkRef == nil) {
		// Create
		CVDisplayLinkRef	displayLinkRef;
		CVReturn			error;
		error = CVDisplayLinkCreateWithActiveCGDisplays(&displayLinkRef);
		if (error != kCVReturnSuccess) {
			CLogServices::logError(
					CString(OSSTR("CVDisplayLinkCreateWithActiveCGDisplays() returned error ")) + CString(error));

			return;
		}

		// Store
		self.displayLinkRef = displayLinkRef;

		// Set output callback
		error = CVDisplayLinkSetOutputCallback(self.displayLinkRef, sDisplayLinkOutputCallback, (__bridge void*) self);
		if (error != kCVReturnSuccess) {
			CLogServices::logError(CString(OSSTR("CVDisplayLinkSetOutputCallback() returned error ")) + CString(error));

			return;
		}

		// Set current CGDisplay from OpenGL context
		error =
				CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(self.displayLinkRef, self.openGLContext.CGLContextObj,
						self.pixelFormat.CGLPixelFormatObj);
		if (error != kCVReturnSuccess) {
			CLogServices::logError(
					CString(OSSTR("CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext() returned error ")) +
							CString(error));

			return;
		}

		// Start
		error = CVDisplayLinkStart(self.displayLinkRef);
		if (error != kCVReturnSuccess) {
			CLogServices::logError(CString(OSSTR("CVDisplayLinkStart() returned error ")) + CString(error));

			return;
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------
- (void) removePeriodic
{
	// Lock to make sure we are not removing while in output callback
	[self.displayLinkLock lock];
	CVDisplayLinkStop(self.displayLinkRef);
	CVDisplayLinkRelease(self.displayLinkRef);
	[self.displayLinkLock unlock];

	// Clear
	self.displayLinkRef = nil;
}

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
- (instancetype) initWithFrame:(NSRect) frame
{
	// Setup pixel format
	NSOpenGLPixelFormatAttribute pixelFormatAttributes[] = {
																NSOpenGLPFADoubleBuffer,
																NSOpenGLPFADepthSize, 24,
																0,
														   };
	NSOpenGLPixelFormat*	pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:pixelFormatAttributes];

	self = [super initWithFrame:frame pixelFormat:pixelFormat];
	if (self != nil) {
		// Complete setup
		self.openGLContext = [[NSOpenGLContext alloc] initWithFormat:pixelFormat shareContext:nil];

		self.contextLock = [[NSLock alloc] init];

		self.displayLinkLock = [[NSLock alloc] init];

		mOpenGLGPU =
				new COpenGLGPU(
						COpenGLGPUProcsInfo((COpenGLGPUAcquireContextProc) sAcquireContextProc,
								(COpenGLGPUTryAcquireContextProc) sTryAcquireContextProc,
								(COpenGLGPUReleaseContextProc) sReleaseContextProc, (__bridge void*) self));
		mOpenGLGPU->setup(S2DSize32(frame.size.width, frame.size.height));
	}

	return self;
}

//----------------------------------------------------------------------------------------------------------------------
- (void) deinit
{
	// Cleanup
	[self removePeriodic];

	DisposeOf(mOpenGLGPU);
}

// MARK: NSResponder methods

//----------------------------------------------------------------------------------------------------------------------
- (void) mouseDown:(NSEvent*) event
{
	self.mouseDownProc(event);
}

//----------------------------------------------------------------------------------------------------------------------
- (void) mouseDragged:(NSEvent*) event
{
	self.mouseDraggedProc(event);
}

//----------------------------------------------------------------------------------------------------------------------
- (void) mouseUp:(NSEvent*) event
{
	self.mouseUpProc(event);
}

// MARK: NSView methods

//----------------------------------------------------------------------------------------------------------------------
- (void) renewGState
{
	// Called whenever graphics state has been updated (such as window resize).  OpenGL rendering is not synchronous
	//	with other rendering on macOS.  Therefore, call disableScreenUpdatesUntilFlush so the window server doesn't
	//	render non-OpenGL content in the window asynchronously from OpenGL content, which could cause flickering.
	//	(non-OpenGL content includes the title bar and drawing done by the app with other APIs)
	[self.window disableScreenUpdatesUntilFlush];

	// Do super
	[super renewGState];
}

// MARK: NSOpenGLView methods

//----------------------------------------------------------------------------------------------------------------------
- (void) prepareOpenGL
{
	// Do super
	[super prepareOpenGL];

	// Setup context
	[self.openGLContext makeCurrentContext];

	// Synchronize buffer swaps with vertical refresh rate
	GLint	swapInt = 1;
	[self.openGLContext setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];
}

// MARK: Internal methods

//----------------------------------------------------------------------------------------------------------------------
- (void) acquireContext
{
	// Lock
	[self.contextLock lock];

	// Make current
	[self.openGLContext makeCurrentContext];
}
//----------------------------------------------------------------------------------------------------------------------
- (BOOL) tryAcquireContext
{
	// Try lock
	if ([self.contextLock tryLock]) {
		// Make current
		[self.openGLContext makeCurrentContext];

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

//----------------------------------------------------------------------------------------------------------------------
CVReturn sDisplayLinkOutputCallback(CVDisplayLinkRef displayLink, const CVTimeStamp* inNow,
		const CVTimeStamp* inOutputTime, CVOptionFlags flagsIn, CVOptionFlags* flagsOut, void* context)
{
	// Setup
	SceneAppOpenGLView*	sceneAppOpenGLView = (__bridge SceneAppOpenGLView*) context;

	// Try to acquire lock
	if ([sceneAppOpenGLView.displayLinkLock tryLock]) {
		@autoreleasepool {
			// Lock our context
			CGLLockContext(sceneAppOpenGLView.openGLContext.CGLContextObj);

//			// Call start if needed
//			if (sNeedToCallStart) {
//				CSceneAppPlayerX::shared().start();
//				sNeedToCallStart = false;
//			}
//
			// Call proc
			sceneAppOpenGLView.periodicProc(
					(UniversalTime) inOutputTime->videoTime / (UniversalTime) inOutputTime->videoTimeScale);

			// Flush
			CGLFlushDrawable(sceneAppOpenGLView.openGLContext.CGLContextObj);
			CGLUnlockContext(sceneAppOpenGLView.openGLContext.CGLContextObj);
		}

		// All done
		[sceneAppOpenGLView.displayLinkLock unlock];
	}

	return kCVReturnSuccess;
}
