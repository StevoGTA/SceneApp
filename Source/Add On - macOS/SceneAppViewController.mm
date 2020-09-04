//----------------------------------------------------------------------------------------------------------------------
//	SceneAppViewController.mm			©2019 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#import "SceneAppViewController.h"

#import "CCoreFoundation.h"
#import "CFileDataSource.h"
#import "CFilesystem.h"
#import "CSceneAppPlayer.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: Local proc declarations

static	CByteParceller	sSceneAppPlayerCreateByteParceller(const CString& resourceFilename, void* userData);
static	void			sSceneAppInstallPeriodic(void* userData);
static	void			sSceneAppRemovePeriodic(void* userData);
static	void			sSceneAppPlayerOpenURL(const CURL& url, bool useWebView, void* userData);
static	void			sSceneAppPlayerHandleCommand(const CString& command, const CDictionary& commandInfo,
										void* userData);
//static	CImageX	sSceneAppPlayerGetCurrentViewportImage(void* userData);

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - SceneAppViewController
@interface SceneAppViewController ()

@property (nonatomic, assign)	CFilesystemPath*	sceneAppContentRootFilesystemPath;
@property (nonatomic, assign)	CSceneAppPlayer*	sceneAppPlayerInternal;
@property (nonatomic, assign)	S2DSizeF32			scenePackageSize;
@property (nonatomic, assign)	S2DPointF32			previousPoint;

@end

@implementation SceneAppViewController

// MARK: Property methods

//----------------------------------------------------------------------------------------------------------------------
- (CSceneAppPlayer&) sceneAppPlayer
{
	return *self.sceneAppPlayerInternal;
}

// MARK: Class methods

//----------------------------------------------------------------------------------------------------------------------
+ (TArray<SScenePackageInfo>) scenePackageInfosIn:(const CFolder&) folder
//----------------------------------------------------------------------------------------------------------------------
{
	// Get files in folder
	TNArray<CFile>	files;
	CFilesystem::getFiles(folder, files);

	return CScenePackage::getScenePackageInfos(files);
}

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
- (instancetype) initWithView:(NSView<AKTGPUView>*) view
{
	self = [super initWithNibName:nil bundle:nil];
	if (self != nil) {
		// Setup the view
		self.view = view;

		__weak	__typeof(self)	weakSelf = self;
		((NSView<AKTGPUView>*) self.view).mouseDownProc = ^(NSEvent* event){
			// Setup
			NSSize	viewSize = weakSelf.view.bounds.size;

			NSPoint	mousePoint = [weakSelf.view convertPoint:event.locationInWindow fromView:nil];
			mousePoint.y = viewSize.height - mousePoint.y;

			S2DPointF32	point(mousePoint.x / viewSize.width * self.scenePackageSize.mWidth,
								mousePoint.y / viewSize.height * self.scenePackageSize.mHeight);

			// Inform SceneAppPlayer
			weakSelf.sceneAppPlayerInternal->mouseDown(SSceneAppPlayerMouseDownInfo(point, (UInt32) event.clickCount));

			// Store
			weakSelf.previousPoint = point;
		};
		((NSView<AKTGPUView>*) self.view).mouseDraggedProc = ^(NSEvent* event){
			// Get event info
			NSSize	viewSize = weakSelf.view.bounds.size;

			NSPoint	mousePoint = [weakSelf.view convertPoint:event.locationInWindow fromView:nil];
			mousePoint.y = viewSize.height - mousePoint.y;

			S2DPointF32	point(mousePoint.x / viewSize.width * self.scenePackageSize.mWidth,
								mousePoint.y / viewSize.height * self.scenePackageSize.mHeight);

			// Inform SceneAppPlayer
			weakSelf.sceneAppPlayerInternal->mouseDragged(
					SSceneAppPlayerMouseDraggedInfo(weakSelf.previousPoint, point));

			// Store
			weakSelf.previousPoint = point;
		};
		((NSView<AKTGPUView>*) self.view).mouseUpProc = ^(NSEvent* event){
			// Get event info
			NSSize	viewSize = weakSelf.view.bounds.size;

			NSPoint	mousePoint = [weakSelf.view convertPoint:event.locationInWindow fromView:nil];
			mousePoint.y = viewSize.height - mousePoint.y;

			S2DPointF32	point(mousePoint.x / viewSize.width * self.scenePackageSize.mWidth,
								mousePoint.y / viewSize.height * self.scenePackageSize.mHeight);

			// Inform SceneAppPlayer
			weakSelf.sceneAppPlayerInternal->mouseUp(SSceneAppPlayerMouseUpInfo(point));
		};

		((NSView<AKTGPUView>*) self.view).periodicProc = ^(UniversalTime outputTime){
			// Inform SceneAppPlayer
			weakSelf.sceneAppPlayerInternal->handlePeriodic(outputTime);
		};

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
	Delete(self.sceneAppContentRootFilesystemPath);
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
		sceneAppContentFolder:(const CFolder&) sceneAppContentFolder
{
	[self loadScenesFrom:scenePackageInfo sceneAppContentFolder:sceneAppContentFolder sceneAppPlayerCreationProc:nil];
}

//----------------------------------------------------------------------------------------------------------------------
- (void) loadScenesFrom:(const SScenePackageInfo&) scenePackageInfo
		sceneAppContentFolder:(const CFolder&) sceneAppContentFolder
		sceneAppPlayerCreationProc:
				(nullable CSceneAppPlayer* (^)(CGPU& gpu, const SSceneAppPlayerProcsInfo& sceneAppPlayerProcsInfo))
						sceneAppPlayerCreationProc
{
	// Cleanup
	Delete(self.sceneAppContentRootFilesystemPath);
	Delete(self.sceneAppPlayerInternal);

	// Store
	self.scenePackageSize = scenePackageInfo.mSize;
	self.sceneAppContentRootFilesystemPath = new CFilesystemPath(sceneAppContentFolder.getFilesystemPath());

	// Setup Scene App Player
	CGPU&						gpu = ((NSView<AKTGPUView>*) self.view).gpu;
	SSceneAppPlayerProcsInfo	sceneAppPlayerProcsInfo(sSceneAppPlayerCreateByteParceller, sSceneAppInstallPeriodic,
										sSceneAppRemovePeriodic, sSceneAppPlayerOpenURL, sSceneAppPlayerHandleCommand,
										(__bridge void*) self);
	self.sceneAppPlayerInternal =
			(sceneAppPlayerCreationProc != nil) ?
					sceneAppPlayerCreationProc(gpu, sceneAppPlayerProcsInfo) :
					new CSceneAppPlayer(gpu, sceneAppPlayerProcsInfo);

	// Load scenes
	self.sceneAppPlayerInternal->loadScenes(scenePackageInfo);
}

// MARK: Notification methods

//----------------------------------------------------------------------------------------------------------------------
- (void) applicationWillResignActiveNotification:(NSNotification*) notification
{
// TODO: What to do here?
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
// TODO: What to do here?
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
CByteParceller sSceneAppPlayerCreateByteParceller(const CString& resourceFilename, void* userData)
{
	// Setup
	SceneAppViewController*	sceneAppViewController = (__bridge SceneAppViewController*) userData;

	return CByteParceller(
			new CMappedFileDataSource(
					CFile(
							sceneAppViewController.sceneAppContentRootFilesystemPath->appendingComponent(
									resourceFilename))));
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
									[NSURL
											URLWithString:
													(NSString*) CFBridgingRelease(
															CCoreFoundation::createStringRefFrom(url.getString()))];

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
		NSString*						commandString =
												(NSString*) CFBridgingRelease(
														CCoreFoundation::createStringRefFrom(command));
		NSDictionary<NSString*, id>*	commandInfoDictionary =
												(NSDictionary<NSString*, id>*)
														CFBridgingRelease(
																CCoreFoundation::createDictionaryRefFrom(commandInfo));

		// Call on Main thread
		dispatch_async(dispatch_get_main_queue(), ^{
			// Setup
			CString		commandUse((__bridge CFStringRef) commandString);
			CDictionary	commandInfoUse =
								CCoreFoundation::dictionaryFrom((__bridge CFDictionaryRef) commandInfoDictionary);

			// Call proc
			sceneAppViewController.handleCommandProc(commandUse, commandInfoUse);
		});
	}
}

////----------------------------------------------------------------------------------------------------------------------
//CImageX sSceneAppPlayerGetCurrentViewportImage(void* userData)
//{
//#warning FINISH
////	UIImage*	image = [(__bridge SceneAppGLView*) userData image];
////
////	return CImageX(image.CGImage);
//}
