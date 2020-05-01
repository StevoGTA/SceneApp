//----------------------------------------------------------------------------------------------------------------------
//	SceneAppViewController.mm			©2019 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#import "SceneAppViewController.h"

#import "CFUtilities.h"
#import "CFileDataSource.h"
#import "CSceneAppPlayer.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: Local data

static	NSString*	sResourceFolderSubpath = @"SceneAppContent";

//----------------------------------------------------------------------------------------------------------------------
// MARK: - Local proc declarations

static	CByteParceller	sSceneAppPlayerCreateByteParceller(const CString& resourceFilename);
static	void			sSceneAppInstallPeriodic(void* userData);
static	void			sSceneAppRemovePeriodic(void* userData);
static	void			sSceneAppPlayerOpenURL(const CURL& url, bool useWebView, void* userData);
static	void			sSceneAppPlayerHandleCommand(const CString& command, const CDictionary& commandInfo,
										void* userData);
static	S2DSizeF32		sSceneAppPlayerGetViewportSizeProc(void* userData);
//static	void			sSceneAppPlayerBeginDraw(void* userData);
//static	void			sSceneAppPlayerEndDraw(void* userData);
//static	CImageX	sSceneAppPlayerGetCurrentViewportImage(void* userData);

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: SceneAppViewController
@interface SceneAppViewController ()

@property (nonatomic, assign)	CSceneAppPlayer*	sceneAppPlayerInternal;
@property (nonatomic, assign)	S2DSizeF32			viewSize;
@property (nonatomic, assign)	NSPoint				previousLocationInWindow;

@end

@implementation SceneAppViewController

// MARK: Property methods

//----------------------------------------------------------------------------------------------------------------------
+ (TArray<CString>) scenePackageFilenames
{
	// Collect all Scenes*.plist files
	TNArray<CString>	scenePackageFilenames;
	for (NSURL* URL in
			[[NSBundle mainBundle] URLsForResourcesWithExtension:@"plist" subdirectory:sResourceFolderSubpath]) {
		NSString*	filename = [[URL path] lastPathComponent];
		if ([filename hasPrefix:@"Scenes_"])
			// Found scene package
			scenePackageFilenames += CString((__bridge CFStringRef) filename);
	}

	return scenePackageFilenames;
}

//----------------------------------------------------------------------------------------------------------------------
- (CSceneAppPlayer&) sceneAppPlayer
{
	return *self.sceneAppPlayerInternal;
}

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
- (instancetype) initWithView:(NSView<AKTGPUView>*) view
{
	return [self initWithView:view sceneAppPlayerCreationProc:nil];
}

//----------------------------------------------------------------------------------------------------------------------
- (instancetype) initWithView:(NSView<AKTGPUView>*) view
		sceneAppPlayerCreationProc:
				(nullable CSceneAppPlayer* (^)(CGPU& gpu, const SSceneAppPlayerProcsInfo& sceneAppPlayerProcsInfo))
						sceneAppPlayerCreationProc
{
	self = [super initWithNibName:nil bundle:nil];
	if (self != nil) {
		// Setup the view
		self.view = view;

		__weak	__typeof(self)	weakSelf = self;
		((NSView<AKTGPUView>*) self.view).mouseDownProc = ^(NSEvent* event){
			// Setup
			NSPoint	point = [weakSelf.view convertPoint:event.locationInWindow fromView:nil];
			point.y = weakSelf.view.bounds.size.height - point.y;

			// Inform SceneAppPlayer
			weakSelf.sceneAppPlayerInternal->mouseDown(
					SSceneAppPlayerMouseDownInfo(S2DPointF32(point.x, point.y), (UInt32) event.clickCount));

			// Store
			weakSelf.previousLocationInWindow = point;
		};
		((NSView<AKTGPUView>*) self.view).mouseDraggedProc = ^(NSEvent* event){
			// Get event info
			NSPoint	point = [weakSelf.view convertPoint:event.locationInWindow fromView:nil];
			point.y = weakSelf.view.bounds.size.height - point.y;

			// Inform SceneAppPlayer
			weakSelf.sceneAppPlayerInternal->mouseDragged(
					SSceneAppPlayerMouseDraggedInfo(
							S2DPointF32(weakSelf.previousLocationInWindow.x, weakSelf.previousLocationInWindow.y),
							S2DPointF32(point.x, point.y)));

			// Store
			weakSelf.previousLocationInWindow = point;
		};
		((NSView<AKTGPUView>*) self.view).mouseUpProc = ^(NSEvent* event){
			// Get event info
			NSPoint	point = [weakSelf.view convertPoint:event.locationInWindow fromView:nil];
			point.y = weakSelf.view.bounds.size.height - point.y;

			// Inform SceneAppPlayer
			weakSelf.sceneAppPlayerInternal->mouseUp(SSceneAppPlayerMouseUpInfo(S2DPointF32(point.x, point.y)));
		};

		((NSView<AKTGPUView>*) self.view).periodicProc = ^(UniversalTime outputTime){
			// Inform SceneAppPlayer
			weakSelf.sceneAppPlayerInternal->handlePeriodic(outputTime);
		};

		// Setup Scene App Player
		self.sceneAppPlayerInternal =
				(sceneAppPlayerCreationProc != nil) ?
					sceneAppPlayerCreationProc(view.gpu,
							SSceneAppPlayerProcsInfo(sSceneAppPlayerCreateByteParceller, sSceneAppInstallPeriodic,
									sSceneAppRemovePeriodic, sSceneAppPlayerOpenURL, sSceneAppPlayerHandleCommand,
									sSceneAppPlayerGetViewportSizeProc, (__bridge void*) self)) :
					new CSceneAppPlayer(view.gpu,
							SSceneAppPlayerProcsInfo(sSceneAppPlayerCreateByteParceller, sSceneAppInstallPeriodic,
									sSceneAppRemovePeriodic, sSceneAppPlayerOpenURL, sSceneAppPlayerHandleCommand,
									sSceneAppPlayerGetViewportSizeProc, (__bridge void*) self));

		// Store view size
		CGSize	size = self.view.bounds.size;
		self.viewSize = S2DSizeF32(size.width, size.height);

		// Setup Notifications
		NSNotificationCenter*	notificationCenter = [NSNotificationCenter defaultCenter];
		[notificationCenter addObserver:self selector:@selector(applicationWillResignActiveNotification:)
				name:NSApplicationWillResignActiveNotification object:nil];
		[notificationCenter addObserver:self selector:@selector(applicationDidBecomeActiveNotification:)
				name:NSApplicationDidBecomeActiveNotification object:nil];
		[notificationCenter addObserver:self selector:@selector(applicationWillTerminateNotification:)
				name:NSApplicationWillTerminateNotification object:nil];
	}

	return self;
}

//----------------------------------------------------------------------------------------------------------------------
- (void) dealloc
{
	Delete(self.sceneAppPlayerInternal);
}

// MARK: NSViewController methods

//----------------------------------------------------------------------------------------------------------------------
- (void) viewWillAppear
{
	// Do super
	[super viewWillAppear];

//	[self.view layoutIfNeeded];

	// Start SceneAppPlayer
	self.sceneAppPlayerInternal->start();
}

//----------------------------------------------------------------------------------------------------------------------
- (void) viewWillDisappear
{
	// Do super
	[super viewWillDisappear];

	// Stop SceneAppPlayer
	self.sceneAppPlayerInternal->stop();
}

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
- (void) loadScenesFrom:(const SScenePackageInfo&) scenePackageInfo
{
	self.sceneAppPlayerInternal->loadScenes(scenePackageInfo);
}

// MARK: Notification methods

//----------------------------------------------------------------------------------------------------------------------
- (void) applicationWillResignActiveNotification:(NSNotification*) notification
{
#warning What to do here?
//	// Send notification
//	CEvent	event(kSceneAppWillResignActiveEventKind);
//	CNotificationCenterX::broadcast(event);
//
//	// Pause SceneAppPlayer
//	CSceneAppPlayerX::shared().pause();
//
//	// Dump textures
//	CGLTextureCache::clear();
//	mApplicationDidResignActive = YES;
}

//----------------------------------------------------------------------------------------------------------------------
- (void) applicationDidBecomeActiveNotification:(NSNotification*) notification
{
#warning What to do here?
//	// Send notification
//	CEvent	event(kSceneAppDidBecomeActiveEventKind);
//	CNotificationCenterX::broadcast(event);
//
//	// Reload textures
//	if (mApplicationDidResignActive)
//		CGPUTextureManager::mDefault.loadAll();
//
//	// Resume SceneAppPlayer
//	CSceneAppPlayerX::shared().resume();
}

//----------------------------------------------------------------------------------------------------------------------
- (void) applicationWillTerminateNotification:(NSNotification*) notification
{
	// Cleanup
//	Delete(self.sceneAppPlayer);
}

// MARK: Private methods

//----------------------------------------------------------------------------------------------------------------------
- (void) installPeriodic
{
	[(NSView<AKTGPUView>*) self.view installPeriodic];
}

//----------------------------------------------------------------------------------------------------------------------
- (void) removePeriodic
{
	[(NSView<AKTGPUView>*) self.view removePeriodic];
}

@end

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - Local proc definitions

//----------------------------------------------------------------------------------------------------------------------
CByteParceller sSceneAppPlayerCreateByteParceller(const CString& resourceFilename)
{
	CFStringRef	resourceFilenameStringRef = eStringCopyCFStringRef(resourceFilename);
	NSString*	path =
						[[NSBundle mainBundle] pathForResource:(__bridge NSString*) resourceFilenameStringRef ofType:@""
								inDirectory:sResourceFolderSubpath];
	::CFRelease(resourceFilenameStringRef);

	return CByteParceller(new CMappedFileDataSource(CFile(CFilesystemPath(CString((__bridge CFStringRef) path)))));
}

//----------------------------------------------------------------------------------------------------------------------
void sSceneAppInstallPeriodic(void* userData)
{
	[(__bridge SceneAppViewController*) userData installPeriodic];
}

//----------------------------------------------------------------------------------------------------------------------
void sSceneAppRemovePeriodic(void* userData)
{
	[(__bridge SceneAppViewController*) userData removePeriodic];
}

//----------------------------------------------------------------------------------------------------------------------
void sSceneAppPlayerOpenURL(const CURL& url, bool useWebView, void* userData)
{
	// Setup
	SceneAppViewController*	sceneAppViewController = (__bridge SceneAppViewController*) userData;
	NSURL*					nsURL =
									[NSURL URLWithString:
											(NSString*) CFBridgingRelease(eStringCopyCFStringRef(url.getString()))];

	// Call on Main thread
	dispatch_async(dispatch_get_main_queue(), ^{
		// Check if using web view
		if (useWebView) {
			// Check if have proc
			if (sceneAppViewController.openURLProc != nil)
				// Call proc
				sceneAppViewController.openURLProc(nsURL);
		} else
			// Open URL
			[NSWorkspace.sharedWorkspace openURL:nsURL];
	});
}

//----------------------------------------------------------------------------------------------------------------------
void sSceneAppPlayerHandleCommand(const CString& command, const CDictionary& commandInfo, void* userData)
{
	// Setup
	SceneAppViewController*	sceneAppViewController = (__bridge SceneAppViewController*) userData;

	// Check if have proc
	if (sceneAppViewController.handleCommandProc != nil) {
		NSString*						commandString = (NSString*) CFBridgingRelease(eStringCopyCFStringRef(command));
		NSDictionary<NSString*, id>*	commandInfoDictionary =
												(NSDictionary<NSString*, id>*)
														CFBridgingRelease(eDictionaryCopyCFDictionaryRef(commandInfo));

		// Call on Main thread
		dispatch_async(dispatch_get_main_queue(), ^{
			// Setup
			CString		commandUse((__bridge CFStringRef) commandString);
			CDictionary	commandInfoUse = eDictionaryFrom((__bridge CFDictionaryRef) commandInfoDictionary);

			// Call proc
			sceneAppViewController.handleCommandProc(commandUse, commandInfoUse);
		});
	}
}

//----------------------------------------------------------------------------------------------------------------------
S2DSizeF32 sSceneAppPlayerGetViewportSizeProc(void* userData)
{
	return ((__bridge SceneAppViewController*) userData).viewSize;
}

////----------------------------------------------------------------------------------------------------------------------
//CImageX sSceneAppPlayerGetCurrentViewportImage(void* userData)
//{
//#warning FINISH
////	UIImage*	image = [(__bridge SceneAppGLView*) userData image];
////
////	return CImageX(image.CGImage);
//}
