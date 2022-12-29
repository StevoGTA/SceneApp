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

static	I<CDataSource>				sSceneAppPlayerCreateDataSource(const CString& resourceFilename, void* userData);
static	I<CRandomAccessDataSource>	sSceneAppPlayerCreateRandomAccessDataSource(const CString& resourceFilename,
											void* userData);
static	void						sSceneAppInstallPeriodic(void* userData);
static	void						sSceneAppRemovePeriodic(void* userData);
static	void						sSceneAppPlayerOpenURL(const CURL& url, bool useWebView, void* userData);
static	void						sSceneAppPlayerHandleCommand(const CString& command, const CDictionary& commandInfo,
											void* userData);
//static	CImageX	sSceneAppPlayerGetCurrentViewportImage(void* userData);

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - SceneAppViewController
@interface SceneAppViewController ()

@property (nonatomic, assign)	CFilesystemPath*	sceneAppContentRootFilesystemPath;
@property (nonatomic, assign)	CSceneAppPlayer*	sceneAppPlayerInternal;
@property (nonatomic, assign)	S2DSizeF32			scenePackageSize;
@property (nonatomic, assign)	BOOL				didReceiveApplicationWillResignActiveNotification;

@end

@implementation SceneAppViewController

// MARK: Property methods

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

// MARK: Class methods

//----------------------------------------------------------------------------------------------------------------------
+ (TArray<CScenePackage::Info>) scenePackageInfosIn:(const CFolder&) folder
//----------------------------------------------------------------------------------------------------------------------
{
	return CScenePackage::getScenePackageInfos(CFilesystem::getFiles(folder).getValue());
}

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
- (instancetype) initWithView:(UIView<UKTGPUView>*) view
		scenePackageInfo:(const CScenePackage::Info&) scenePackageInfo
		sceneAppContentFolder:(const CFolder&) sceneAppContentFolder
{
	return [self initWithView:view scenePackageInfo:scenePackageInfo sceneAppContentFolder:sceneAppContentFolder
			sceneAppPlayerCreationProc:nil];
}

//----------------------------------------------------------------------------------------------------------------------
- (instancetype) initWithView:(UIView<UKTGPUView>*) view
		scenePackageInfo:(const CScenePackage::Info&) scenePackageInfo
		sceneAppContentFolder:(const CFolder&) sceneAppContentFolder
		sceneAppPlayerCreationProc:(nullable CreateSceneAppPlayerProc) sceneAppPlayerCreationProc
{
	self = [super initWithNibName:nil bundle:nil];
	if (self != nil) {
		// Store
		self.scenePackageSize = scenePackageInfo.mSize;
		self.sceneAppContentRootFilesystemPath = new CFilesystemPath(sceneAppContentFolder.getFilesystemPath());

		// Setup the view
		self.view = view;

		__weak	__typeof(self)	weakSelf = self;
		((UIView<UKTGPUView>*) self.view).touchesBeganProc = ^(NSSet<UITouch*>* touches, UIEvent* event){
			// Setup
			CGSize	viewSize = weakSelf.view.bounds.size;

			// Convert native touches to SceneApp touches
			TNArray<CSceneAppPlayer::TouchBeganInfo>	touchBeganInfosArray;
			for (UITouch* touch in touches) {
				// Add info
				CGPoint	pt = [touch locationInView:weakSelf.view];
				touchBeganInfosArray +=
						CSceneAppPlayer::TouchBeganInfo(
								S2DPointF32(pt.x / viewSize.width * self.scenePackageSize.mWidth,
										pt.y / viewSize.height * self.scenePackageSize.mHeight),
								(UInt32) touch.tapCount, (__bridge void*) touch);
			}

			// Inform SceneAppPlayer
			weakSelf.sceneAppPlayerInternal->touchesBegan(touchBeganInfosArray);
		};
		((UIView<UKTGPUView>*) self.view).touchesMovedProc = ^(NSSet<UITouch*>* touches, UIEvent* event){
			// Setup
			CGSize	viewSize = weakSelf.view.bounds.size;

			// Convert native touches to SceneApp touches
			TNArray<CSceneAppPlayer::TouchMovedInfo>	touchMovedInfosArray;
			for (UITouch* touch in touches) {
				// Add info
				CGPoint	previousPt = [touch previousLocationInView:weakSelf.view];
				CGPoint	currentPt = [touch locationInView:weakSelf.view];
				touchMovedInfosArray +=
						CSceneAppPlayer::TouchMovedInfo(
								S2DPointF32(previousPt.x / viewSize.width * self.scenePackageSize.mWidth,
										previousPt.y / viewSize.height * self.scenePackageSize.mHeight),
								S2DPointF32(currentPt.x / viewSize.width * self.scenePackageSize.mWidth,
										currentPt.y / viewSize.height * self.scenePackageSize.mHeight),
								(__bridge void*) touch);
			}

			// Inform SceneAppPlayer
			weakSelf.sceneAppPlayerInternal->touchesMoved(touchMovedInfosArray);
		};
		((UIView<UKTGPUView>*) self.view).touchesEndedProc = ^(NSSet<UITouch*>* touches, UIEvent* event){
			// Setup
			CGSize	viewSize = weakSelf.view.bounds.size;

			// Convert native touches to SceneApp touches
			TNArray<CSceneAppPlayer::TouchMovedInfo>	touchMovedInfosArray;
			TNArray<CSceneAppPlayer::TouchEndedInfo>	touchEndedInfosArray;
			for (UITouch* touch in touches) {
				// Add info
				CGPoint	previousPt = [touch previousLocationInView:weakSelf.view];
				CGPoint	currentPt = [touch locationInView:weakSelf.view];
				if (!CGPointEqualToPoint(previousPt, currentPt))
					// Need to add a move to final point
					touchMovedInfosArray +=
							CSceneAppPlayer::TouchMovedInfo(
									S2DPointF32(previousPt.x / viewSize.width * self.scenePackageSize.mWidth,
											previousPt.y / viewSize.height * self.scenePackageSize.mHeight),
									S2DPointF32(currentPt.x / viewSize.width * self.scenePackageSize.mWidth,
											currentPt.y / viewSize.height * self.scenePackageSize.mHeight),
									(__bridge void*) touch);
				touchEndedInfosArray +=
						CSceneAppPlayer::TouchEndedInfo(
								S2DPointF32(currentPt.x / viewSize.width * self.scenePackageSize.mWidth,
										currentPt.y / viewSize.height * self.scenePackageSize.mHeight),
								(__bridge void*) touch);
			}

			// Inform SceneAppPlayer
			weakSelf.sceneAppPlayerInternal->touchesMoved(touchMovedInfosArray);
			weakSelf.sceneAppPlayerInternal->touchesEnded(touchEndedInfosArray);
		};
		((UIView<UKTGPUView>*) self.view).touchesCancelledProc = ^(NSSet<UITouch*>* touches, UIEvent* event){
			// Collect info
			TNArray<CSceneAppPlayer::TouchCancelledInfo>	touchCancelledInfosArray;
			for (UITouch* touch in touches)
				// Add info
				touchCancelledInfosArray += CSceneAppPlayer::TouchCancelledInfo((__bridge void*) touch);

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

		((UIView<UKTGPUView>*) self.view).periodicProc = ^(UniversalTimeInterval outputTimeInterval){
			// Inform SceneAppPlayer
			weakSelf.sceneAppPlayerInternal->handlePeriodic(outputTimeInterval);
		};

		// Setup Scene App Player
		CGPU&						gpu = ((UIView<UKTGPUView>*) self.view).gpu;
		CSceneAppPlayer::Procs		procs(sSceneAppInstallPeriodic, sSceneAppRemovePeriodic, sSceneAppPlayerOpenURL,
											sSceneAppPlayerHandleCommand, (__bridge void*) self);
		SSceneAppResourceLoading	sceneAppResourceLoading(sSceneAppPlayerCreateDataSource,
											sSceneAppPlayerCreateRandomAccessDataSource, (__bridge void*) self);
		self.sceneAppPlayerInternal =
				(sceneAppPlayerCreationProc != nil) ?
						sceneAppPlayerCreationProc(gpu, procs, sceneAppResourceLoading) :
						new CSceneAppPlayer(gpu, procs, sceneAppResourceLoading);
		self.sceneAppPlayerInternal->loadScenes(scenePackageInfo);

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
	Delete(self.sceneAppContentRootFilesystemPath);
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
I<CDataSource> sSceneAppPlayerCreateDataSource(const CString& resourceFilename, void* userData)
{
	// Setup
	SceneAppViewController*	sceneAppViewController = (__bridge SceneAppViewController*) userData;

	return I<CDataSource>(
			new CMappedFileDataSource(
					CFile(
							sceneAppViewController.sceneAppContentRootFilesystemPath->appendingComponent(
									resourceFilename))));
}

//----------------------------------------------------------------------------------------------------------------------
I<CRandomAccessDataSource> sSceneAppPlayerCreateRandomAccessDataSource(const CString& resourceFilename, void* userData)
{
	// Setup
	SceneAppViewController*	sceneAppViewController = (__bridge SceneAppViewController*) userData;

	return I<CRandomAccessDataSource>(
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

////----------------------------------------------------------------------------------------------------------------------
//CImageX sSceneAppPlayerGetCurrentViewportImage(void* userData)
//{
//#warning FINISH
////	UIImage*	image = [(__bridge SceneAppGLView*) userData image];
////
////	return CImageX(image.CGImage);
//}
