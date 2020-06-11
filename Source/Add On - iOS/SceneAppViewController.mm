//----------------------------------------------------------------------------------------------------------------------
//	SceneAppViewController.mm			©2019 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#import "SceneAppViewController.h"

#import "CCoreFoundation.h"
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
//static	CImageX	sSceneAppPlayerGetCurrentViewportImage(void* userData);

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - SceneAppViewController
@interface SceneAppViewController ()

@property (nonatomic, assign)	CSceneAppPlayer*	sceneAppPlayerInternal;
@property (nonatomic, assign)	S2DSizeF32			viewSize;
@property (nonatomic, assign)	BOOL				didReceiveApplicationWillResignActiveNotification;

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
- (BOOL) prefersStatusBarHidden
{
    return YES;
}

//----------------------------------------------------------------------------------------------------------------------
- (CSceneAppPlayer&) sceneAppPlayer
{
	return *self.sceneAppPlayerInternal;
}

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
- (instancetype) initWithView:(UIView<UKTGPUView>*) view
{
	return [self initWithView:view sceneAppPlayerCreationProc:nil];
}

//----------------------------------------------------------------------------------------------------------------------
- (instancetype) initWithView:(UIView<UKTGPUView>*) view
		sceneAppPlayerCreationProc:
				(nullable CSceneAppPlayer* (^)(CGPU& gpu, const SSceneAppPlayerProcsInfo& sceneAppPlayerProcsInfo))
						sceneAppPlayerCreationProc
{
	self = [super initWithNibName:nil bundle:nil];
	if (self != nil) {
		// Setup the view
		self.view = view;

		__weak	__typeof(self)	weakSelf = self;
		((UIView<UKTGPUView>*) self.view).touchesBeganProc = ^(NSSet<UITouch*>* touches, UIEvent* event){
			// Setup
			const	SMatrix4x4_32&	viewMatrix = ((UIView<UKTGPUView>*) self.view).gpu.getViewMatrix();

			// Convert native touches to SceneApp touches
			TNArray<SSceneAppPlayerTouchBeganInfo>	touchBeganInfosArray;
			for (UITouch* touch in touches) {
				// Add info
				CGPoint	pt = [touch locationInView:weakSelf.view];
				touchBeganInfosArray +=
						SSceneAppPlayerTouchBeganInfo(S2DPointF32(pt.x / viewMatrix.m1_1, pt.y / viewMatrix.m2_2),
								(UInt32) touch.tapCount, (__bridge void*) touch);
			}

			// Inform SceneAppPlayer
			weakSelf.sceneAppPlayerInternal->touchesBegan(touchBeganInfosArray);
		};
		((UIView<UKTGPUView>*) self.view).touchesMovedProc = ^(NSSet<UITouch*>* touches, UIEvent* event){
			// Setup
			const	SMatrix4x4_32&	viewMatrix = ((UIView<UKTGPUView>*) self.view).gpu.getViewMatrix();

			// Convert native touches to SceneApp touches
			TNArray<SSceneAppPlayerTouchMovedInfo>	touchMovedInfosArray;
			for (UITouch* touch in touches) {
				// Add info
				CGPoint	previousPt = [touch previousLocationInView:weakSelf.view];
				CGPoint	currentPt = [touch locationInView:weakSelf.view];
				touchMovedInfosArray +=
						SSceneAppPlayerTouchMovedInfo(
								S2DPointF32(previousPt.x / viewMatrix.m1_1, previousPt.y / viewMatrix.m2_2),
								S2DPointF32(currentPt.x / viewMatrix.m1_1, currentPt.y / viewMatrix.m2_2),
								(__bridge void*) touch);
			}

			// Inform SceneAppPlayer
			weakSelf.sceneAppPlayerInternal->touchesMoved(touchMovedInfosArray);
		};
		((UIView<UKTGPUView>*) self.view).touchesEndedProc = ^(NSSet<UITouch*>* touches, UIEvent* event){
			// Setup
			const	SMatrix4x4_32&	viewMatrix = ((UIView<UKTGPUView>*) self.view).gpu.getViewMatrix();

			// Convert native touches to SceneApp touches
			TNArray<SSceneAppPlayerTouchMovedInfo>	touchMovedInfosArray;
			TNArray<SSceneAppPlayerTouchEndedInfo>	touchEndedInfosArray;
			for (UITouch* touch in touches) {
				// Add info
				CGPoint	previousPt = [touch previousLocationInView:weakSelf.view];
				CGPoint	currentPt = [touch locationInView:weakSelf.view];
				if (!CGPointEqualToPoint(previousPt, currentPt))
					// Need to add a move to final point
					touchMovedInfosArray +=
							SSceneAppPlayerTouchMovedInfo(
									S2DPointF32(previousPt.x / viewMatrix.m1_1, previousPt.y / viewMatrix.m2_2),
									S2DPointF32(currentPt.x / viewMatrix.m1_1, currentPt.y / viewMatrix.m2_2),
									(__bridge void*) touch);
				touchEndedInfosArray +=
						SSceneAppPlayerTouchEndedInfo(
								S2DPointF32(currentPt.x / viewMatrix.m1_1, currentPt.y / viewMatrix.m2_2),
								(__bridge void*) touch);
			}

			// Inform SceneAppPlayer
			weakSelf.sceneAppPlayerInternal->touchesMoved(touchMovedInfosArray);
			weakSelf.sceneAppPlayerInternal->touchesEnded(touchEndedInfosArray);
		};
		((UIView<UKTGPUView>*) self.view).touchesCancelledProc = ^(NSSet<UITouch*>* touches, UIEvent* event){
			// Collect info
			TNArray<SSceneAppPlayerTouchCancelledInfo>	touchCancelledInfosArray;
			for (UITouch* touch in touches)
				// Add info
				touchCancelledInfosArray += SSceneAppPlayerTouchCancelledInfo((__bridge void*) touch);

			// Inform SceneAppPlayer
			weakSelf.sceneAppPlayerInternal->touchesCancelled(touchCancelledInfosArray);
		};

		((UIView<UKTGPUView>*) self.view).motionBeganProc = ^(UIEventSubtype eventSubtype, UIEvent* event){
			// Check motion type
			if (eventSubtype == UIEventSubtypeMotionShake)
				// Inform SceneAppPlayer
				weakSelf.sceneAppPlayerInternal->shakeBegan();
		};
		((UIView<UKTGPUView>*) self.view).motionEndedProc = ^(UIEventSubtype eventSubtype, UIEvent* event){
			// Check motion type
			if (eventSubtype == UIEventSubtypeMotionShake)
				// Inform SceneAppPlayer
				weakSelf.sceneAppPlayerInternal->shakeEnded();
		};

		((UIView<UKTGPUView>*) self.view).periodicProc = ^(UniversalTime outputTime){
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
		const	SMatrix4x4_32&	viewMatrix = ((UIView<UKTGPUView>*) self.view).gpu.getViewMatrix();
				CGSize			size = self.view.bounds.size;
		self.viewSize = S2DSizeF32(size.width / viewMatrix.m1_1, size.height / viewMatrix.m2_2);

		// Setup Notifications
		NSNotificationCenter*	notificationCenter = [NSNotificationCenter defaultCenter];
		[notificationCenter addObserver:self selector:@selector(applicationWillResignActiveNotification:)
				name:UIApplicationWillResignActiveNotification object:nil];
		[notificationCenter addObserver:self selector:@selector(applicationDidBecomeActiveNotification:)
				name:UIApplicationDidBecomeActiveNotification object:nil];
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
- (void) viewWillAppear:(BOOL) animated
{
	// Do super
	[super viewWillAppear:animated];

	// Start SceneAppPlayer
	self.sceneAppPlayerInternal->start();
}

//----------------------------------------------------------------------------------------------------------------------
- (void) viewWillDisappear:(BOOL) animated
{
	// Do super
	[super viewWillDisappear:animated];

	// Stop SceneAppPlayer
	self.sceneAppPlayerInternal->stop();
}

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
- (void) loadScenesFrom:(const SScenePackageInfo&) scenePackageInfo
{
	// Load scenes
	self.sceneAppPlayerInternal->loadScenes(scenePackageInfo);
}

// MARK: Notification methods

//----------------------------------------------------------------------------------------------------------------------
- (void) applicationWillResignActiveNotification:(NSNotification*) notification
{
	// Stop SceneAppPlayer
	self.sceneAppPlayerInternal->stop(true);

	// Note
	self.didReceiveApplicationWillResignActiveNotification = YES;
}

//----------------------------------------------------------------------------------------------------------------------
- (void) applicationDidBecomeActiveNotification:(NSNotification*) notification
{
	// Check if previously received will resign
	if (self.didReceiveApplicationWillResignActiveNotification)
		// Start SceneAppPlayer
		self.sceneAppPlayerInternal->start(true);
}

// MARK: Private methods

//----------------------------------------------------------------------------------------------------------------------
- (void) installPeriodic
{
	[(UIView<UKTGPUView>*) self.view installPeriodic];
}

//----------------------------------------------------------------------------------------------------------------------
- (void) removePeriodic
{
	[(UIView<UKTGPUView>*) self.view removePeriodic];
}

@end

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - Local proc definitions

//----------------------------------------------------------------------------------------------------------------------
CByteParceller sSceneAppPlayerCreateByteParceller(const CString& resourceFilename)
{
	CFStringRef	resourceFilenameStringRef = CCoreFoundation::createStringRefFrom(resourceFilename);
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
			[UIApplication.sharedApplication openURL:nsURL options:@{} completionHandler:nil];
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
