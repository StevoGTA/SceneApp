//----------------------------------------------------------------------------------------------------------------------
//	CSceneAppPlayerX.cpp			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CSceneAppPlayerX.h"

#include "CCelAnimationPlayer.h"
#include "CSceneAppMoviePlayer.h"
#include "CScenePlayer.h"
#include "CScenes.h"
#include "CSceneTransitionPlayerPush.h"
#include "CSceneTransitionPlayerSlide.h"

#include "CAudioOutputMixer.h"
#include "CAudioOutputTrackCache.h"
#include "CBinaryPropertyList.h"
#include "CGPUTextureManager.h"
#include "CLogServices.h"
#include "CNotificationCenterX.h"
#include "CPreferences.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: Local data

static	const	UInt32	kSceneViewControllerMaxTouchHandlerInfos = 10;

static	CString							sViewportSizeToReplaceString("<VIEWPORTSIZE>");

static	CSceneAppPlayerX*				sSceneAppPlayer = nil;
static	CString*						sViewportSizeReplacementString = nil;

// Action names
CString	CSceneAppPlayerX::mActionNameOpenURL("openURL");
CString	CSceneAppPlayerX::mActionNamePlayAudio("playAudio");
CString	CSceneAppPlayerX::mActionNamePlayMovie("playMovie");
CString	CSceneAppPlayerX::mActionNameSceneCut("sceneCut");
CString	CSceneAppPlayerX::mActionNameScenePush("scenePush");
CString	CSceneAppPlayerX::mActionNameSceneSlide("sceneSlide");
CString	CSceneAppPlayerX::mActionNameSetItemNameValue("setItemNameValue");
CString	CSceneAppPlayerX::mActionNameSendItemCommand("sendItemCommand");
CString	CSceneAppPlayerX::mActionNameSendAppCommand("sendAppCommand");

// Action Dictionary Keys and value types
CString	CSceneAppPlayerX::mActionInfoAudioFilenameKey("audioFilename");
CString	CSceneAppPlayerX::mActionInfoCommandKey("command");
CString	CSceneAppPlayerX::mActionInfoControlModeKey("controlMode");
CString	CSceneAppPlayerX::mActionInfoItemNameKey("itemName");
CString	CSceneAppPlayerX::mActionInfoLoadSceneIndexFromKey("loadSceneIndexFrom");
CString	CSceneAppPlayerX::mActionInfoMovieFilenameKey("movieFilename");
CString	CSceneAppPlayerX::mActionInfoPropertyNameKey("propertyName");
CString	CSceneAppPlayerX::mActionInfoPropertyValueKey("propertyValue");
CString	CSceneAppPlayerX::mActionInfoSceneIndexKey("sceneIndex");
CString	CSceneAppPlayerX::mActionInfoSceneNameKey("sceneName");
CString	CSceneAppPlayerX::mActionInfoURLKey("URL");
CString	CSceneAppPlayerX::mActionInfoUseWebView("useWebView");

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - SSceneAppPlayerXTouchHandlerInfo

struct SSceneAppPlayerXTouchHandlerInfo {
	CSceneAppTouchObjectRef	mTouchObjectRef;
	CSceneItemPlayer*		mSceneItemPlayerHandlingTouch;
};

static	SSceneAppPlayerXTouchHandlerInfo	sTouchHandlerInfoNoHandler = {0};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneAppPlayerInternalsX

class CSceneAppPlayerInternalsX {
	public:
		CSceneAppPlayerInternalsX();
		~CSceneAppPlayerInternalsX();

		bool								mAreUpdatesDisabled;
		bool								mIsPaused;
		bool								mIgnoreElapsedTime;

		CScenes*							mScenes;
		CScenePlayer*						mCurrentScenePlayer;
		TPtrArray<CScenePlayer*>				mScenePlayersArray;

		CAudioOutputTrackReference*			mAudioOutputTrackReference;
		CSceneAppMoviePlayer*				mSceneAppMoviePlayer;
		CSceneTransitionPlayer*				mCurrentSceneTransitionPlayer;

		ESceneAppPlayerTouchHandlingOption	mTouchHandlingOption;
		SSceneAppPlayerXTouchHandlerInfo		mTouchHandlerInfos[kSceneViewControllerMaxTouchHandlerInfos];

		UInt32								mFramesPerSecond;
		UniversalTime						mLastFrameUpdateAndDrawTime;
		void*								mPeriodicReference;

		SSceneAppPlayerProcsInfo			mSceneAppPlayerProcsInfo;
		SSceneAppPlayerDrawProcsInfo		mSceneAppPlayerDrawProcsInfo;
		SScenePlayerProcsInfo				mScenePlayerProcsInfo;

		S2DPoint32							mViewportOffset;
		S2DPoint32							mViewportScale;
		S2DSize32							mViewportSize;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - Reference Scene Item Players

REFERENCE_SCENE_ITEM_PLAYER(CSceneItemPlayerAnimation);
REFERENCE_SCENE_ITEM_PLAYER(CSceneItemPlayerButton);
REFERENCE_SCENE_ITEM_PLAYER(CSceneItemPlayerButtonArray);
REFERENCE_SCENE_ITEM_PLAYER(CSceneItemPlayerHotspot);
REFERENCE_SCENE_ITEM_PLAYER(CSceneItemPlayerMovie);
REFERENCE_SCENE_ITEM_PLAYER(CSceneItemPlayerText);

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - Local proc declarations

static	void			sSetCurrentScenePlayer(CSceneAppPlayerInternalsX& internals, CScenePlayer& scenePlayer);
static	void			sUpdateLinkedScenes(CSceneAppPlayerInternalsX& internals);
static	void			sClearAllTouchHandlerInfos(CSceneAppPlayerInternalsX& internals);
static	void			sActionsHandler(const CActions& actions, const S2DPoint32& point, void* userData);
static	ECompareResult	sScenesFilenamesArrayCompareProc(CString* const filename1, CString* const filename2,
								void* userData);

//static	CImageX			sScenePlayerGetCurrentViewportImage(CScenePlayer& scenePlayer, bool performRedraw,
//								void* userData);

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneAppPlayerInternalsX

//----------------------------------------------------------------------------------------------------------------------
CSceneAppPlayerInternalsX::CSceneAppPlayerInternalsX() :
		mSceneAppPlayerProcsInfo(nil, nil, nil, nil), mSceneAppPlayerDrawProcsInfo(nil, nil, nil),
		mScenePlayerProcsInfo(nil, nil), mScenePlayersArray(true)
//----------------------------------------------------------------------------------------------------------------------
{
	mAreUpdatesDisabled = false;
	mIsPaused = false;
	mIgnoreElapsedTime = false;
	
	mScenes = nil;
	mCurrentScenePlayer = nil;

	mAudioOutputTrackReference = nil;
	mSceneAppMoviePlayer = nil;
	mCurrentSceneTransitionPlayer = nil;

	mTouchHandlingOption = kSceneAppPlayerTouchHandlingOptionLastSingleTouchOnly;
	::bzero(&mTouchHandlerInfos, sizeof(mTouchHandlerInfos));

	mFramesPerSecond = 60;
	mLastFrameUpdateAndDrawTime = 0.0;
	mPeriodicReference = nil;

	::bzero(&mSceneAppPlayerProcsInfo, sizeof(SSceneAppPlayerProcsInfo));
	::bzero(&mSceneAppPlayerDrawProcsInfo, sizeof(SSceneAppPlayerDrawProcsInfo));

//	mScenePlayerProcsInfo.mGetCurrentViewportImageProc = sScenePlayerGetCurrentViewportImage;
	mScenePlayerProcsInfo.mUserData = this;

	mViewportOffset = S2DPoint32::mZero;
	mViewportScale = S2DPoint32(1.0, 1.0);
	mViewportSize = S2DSize32::mZero;
}

//----------------------------------------------------------------------------------------------------------------------
CSceneAppPlayerInternalsX::~CSceneAppPlayerInternalsX()
//----------------------------------------------------------------------------------------------------------------------
{
	mScenePlayersArray.removeAll();
	
	Delete(mScenes);

	Delete(mAudioOutputTrackReference);
	Delete(mSceneAppMoviePlayer);
	Delete(mCurrentSceneTransitionPlayer);
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneAppPlayerX

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
CSceneAppPlayerX::CSceneAppPlayerX() : CEventHandler()
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new CSceneAppPlayerInternalsX();

	CCelAnimationPlayer::setFramesPerSecond(mInternals->mFramesPerSecond);

	CNotificationCenterX::registerHandler(MAKE_CUSTOM_EVENT_KIND(kSceneAppMoviePlayerMovieWillStartEventKind), *this);

	sSceneAppPlayer = this;
	CSceneItemPlayer::setActionsHandler(sActionsHandler, nil);
}

//----------------------------------------------------------------------------------------------------------------------
CSceneAppPlayerX::~CSceneAppPlayerX()
//----------------------------------------------------------------------------------------------------------------------
{
	CNotificationCenterX::unregisterHandler(*this);
	
	removePeriodic(&mInternals->mPeriodicReference);
	Delete(mInternals);

	sSceneAppPlayer = nil;
}

// MARK: CEventHandler methods

//----------------------------------------------------------------------------------------------------------------------
UError CSceneAppPlayerX::handleEvent(CEvent& event)
//----------------------------------------------------------------------------------------------------------------------
{
	switch (event.getClassAndKind()) {
		case MAKE_CUSTOM_EVENT_KIND(kSceneAppMoviePlayerMovieWillStartEventKind):
			// Movie will start
#if defined(DEBUG)
			CLogServices::logMessage(CString("CSceneAppPlayerX - movie will start event"));
#endif

			// Pause the animation
			mInternals->mAreUpdatesDisabled = true;

			// Pause audio
			CAudioOutputMixer::pause();

			// Register notifications
			CNotificationCenterX::registerHandler(MAKE_CUSTOM_EVENT_KIND(kSceneAppMoviePlayerMovieDidFinishEventKind),
					*this);

			return kNoError;

		case MAKE_CUSTOM_EVENT_KIND(kSceneAppMoviePlayerMovieDidFinishEventKind):
			// Movie did finish
#if defined(DEBUG)
			CLogServices::logMessage(CString("CSceneAppPlayerX - movie did finish event"));
#endif

			// Unregister notifications
			CNotificationCenterX::unregisterHandler(MAKE_CUSTOM_EVENT_KIND(kSceneAppMoviePlayerMovieDidFinishEventKind),
					*this);

			// Cleanup
			if (mInternals->mSceneAppMoviePlayer != nil) {
				mInternals->mSceneAppMoviePlayer->stop();
				Delete(mInternals->mSceneAppMoviePlayer);
			}
			
			// Resume animation
			mInternals->mAreUpdatesDisabled = false;
			mInternals->mIgnoreElapsedTime = true;

			// Do it now
#if TARGET_OS_IPHONE
			updateAndDraw();
#endif

			return kNoError;
	}

	return kEventEventNotHandled;
}

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
S2DPoint32 CSceneAppPlayerX::viewportPointFromScreenPoint(const S2DPoint32& screenPoint) const
//----------------------------------------------------------------------------------------------------------------------
{
	return S2DPoint32((screenPoint.mX - mInternals->mViewportOffset.mX) / mInternals->mViewportScale.mX,
			(screenPoint.mY - mInternals->mViewportOffset.mY) / mInternals->mViewportScale.mY);
}

//----------------------------------------------------------------------------------------------------------------------
S2DPoint32 CSceneAppPlayerX::screenPointFromViewportPoint(const S2DPoint32& viewportPoint) const
//----------------------------------------------------------------------------------------------------------------------
{
	return S2DPoint32(mInternals->mViewportOffset.mX + viewportPoint.mX * mInternals->mViewportScale.mX,
			mInternals->mViewportOffset.mY + viewportPoint.mY * mInternals->mViewportScale.mY);
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppPlayerX::setFramesPerSecond(UInt32 framesPerSecond)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mFramesPerSecond = framesPerSecond;
	CCelAnimationPlayer::setFramesPerSecond(mInternals->mFramesPerSecond);
}

//----------------------------------------------------------------------------------------------------------------------
UInt32 CSceneAppPlayerX::getFramesPerSecond() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mFramesPerSecond;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppPlayerX::start()
//----------------------------------------------------------------------------------------------------------------------
{
#if defined(DEBUG)
	CLogServices::logMessage(CString("CSceneAppPlayerX - start"));
#endif

	mInternals->mAreUpdatesDisabled = false;

	mInternals->mCurrentScenePlayer->start();
	sUpdateLinkedScenes(*mInternals);

	mInternals->mLastFrameUpdateAndDrawTime = SUniversalTime::getCurrentUniversalTime();
	updateAndDraw();

	installPeriodic(&mInternals->mPeriodicReference);
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppPlayerX::pause()
//----------------------------------------------------------------------------------------------------------------------
{
#if defined(DEBUG)
	CLogServices::logMessage(CString("CSceneAppPlayerX - pause"));
#endif

	mInternals->mIsPaused = true;

	removePeriodic(&mInternals->mPeriodicReference);

	if (!mInternals->mAreUpdatesDisabled)
		// Pause audio
		CAudioOutputMixer::pause();
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppPlayerX::resume()
//----------------------------------------------------------------------------------------------------------------------
{
#if defined(DEBUG)
	CLogServices::logMessage(CString("CSceneAppPlayerX - resume"));
#endif

	mInternals->mIsPaused = false;
	mInternals->mIgnoreElapsedTime = true;

	if (!mInternals->mAreUpdatesDisabled)
		installPeriodic(&mInternals->mPeriodicReference);
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppPlayerX::stop()
//----------------------------------------------------------------------------------------------------------------------
{
#if defined(DEBUG)
	CLogServices::logMessage(CString("CSceneAppPlayerX - stop"));
#endif

	removePeriodic(&mInternals->mPeriodicReference);

	sClearAllTouchHandlerInfos(*mInternals);
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppPlayerX::updateAndDraw()
//----------------------------------------------------------------------------------------------------------------------
{
	// Are updates disabled?  (Means a movie is playing)
	if (mInternals->mAreUpdatesDisabled || mInternals->mIsPaused)
		return;

	// Make sure audio is playing
	CAudioOutputMixer::resume();
	
	// Update time
	UniversalTime			newFrameUpdateAndDrawTime = SUniversalTime::getCurrentUniversalTime();
	UniversalTimeInterval	deltaTimeInterval;
	if (!mInternals->mIgnoreElapsedTime)
		// Normal
		deltaTimeInterval = newFrameUpdateAndDrawTime - mInternals->mLastFrameUpdateAndDrawTime;
	else {
		// Movie just finished
		mInternals->mIgnoreElapsedTime = false;
		deltaTimeInterval = 0.0;
	}

	mInternals->mLastFrameUpdateAndDrawTime = newFrameUpdateAndDrawTime;

	// Prepare to update system
//	CGPUTextureManager::mDefault.pauseLoading();

	// Update system
	if (mInternals->mCurrentSceneTransitionPlayer != nil)
		mInternals->mCurrentSceneTransitionPlayer->update(deltaTimeInterval);
	else
		mInternals->mCurrentScenePlayer->update(deltaTimeInterval);

	// Draw
	mInternals->mSceneAppPlayerDrawProcsInfo.mBeginDrawProc(mInternals->mSceneAppPlayerDrawProcsInfo.mUserData);

	if (mInternals->mCurrentSceneTransitionPlayer != nil)
		mInternals->mCurrentSceneTransitionPlayer->drawGL();
	else
		mInternals->mCurrentScenePlayer->drawGL(S2DPoint32::mZero);

//	GLfloat	triangleVertices[] = {10.0, 10.0,	310.0, 10.0,	10.0, 470.0,	310.0, 470.0};
//	int		vertexCount = sizeof(triangleVertices) / sizeof(GLfloat) / 2;
//	glEnableClientState(GL_VERTEX_ARRAY);
//	glVertexPointer(2, GL_FLOAT, 0, triangleVertices);
//	glColor4f(0.9f, 0.3f, 0.3f, 0.5f);
//	glDrawArrays(GL_TRIANGLE_STRIP, 0, vertexCount);
//	glDisableClientState(GL_VERTEX_ARRAY);

	mInternals->mSceneAppPlayerDrawProcsInfo.mEndDrawProc(mInternals->mSceneAppPlayerDrawProcsInfo.mUserData);

	// We done with the transition?
	if ((mInternals->mCurrentSceneTransitionPlayer != nil) &&
			(mInternals->mCurrentSceneTransitionPlayer->getState() != kSceneTransitionStateActive)) {
		if (mInternals->mCurrentSceneTransitionPlayer->getState() == kSceneTransitionStateCompleted) {
			// Completed, go to next scene
			if (&mInternals->mCurrentSceneTransitionPlayer->getScenePlayerA() == mInternals->mCurrentScenePlayer)
				// Make Scene B active
				sSetCurrentScenePlayer(*mInternals, mInternals->mCurrentSceneTransitionPlayer->getScenePlayerB());
			else
				// Make Scene A active
				sSetCurrentScenePlayer(*mInternals, mInternals->mCurrentSceneTransitionPlayer->getScenePlayerA());
		}

		// Dispose transition
		Delete(mInternals->mCurrentSceneTransitionPlayer);
	}
	
	// Turn stuff back on
//	CGPUTextureManager::mDefault.resumeLoading();
}

//----------------------------------------------------------------------------------------------------------------------
bool CSceneAppPlayerX::doesActionHaveALinkedScene(const CAction& action) const
//----------------------------------------------------------------------------------------------------------------------
{
	return (action.getName() == mActionNameSceneCut) || (action.getName() == mActionNameScenePush) ||
			(action.getName() == mActionNameSceneSlide);
}

//----------------------------------------------------------------------------------------------------------------------
CSceneIndex CSceneAppPlayerX::getLinkedSceneIndexForAction(const CAction& action) const
//----------------------------------------------------------------------------------------------------------------------
{
	if (doesActionHaveALinkedScene(action)) {
		// Get from info
		// Try name
		CString	string = action.getActionInfo().getString(mActionInfoSceneNameKey);
		if (string.getLength() > 0) {
			for (UInt32 i = 0; i < mInternals->mScenes->getScenesCount(); i++) {
				CScene&	scene = mInternals->mScenes->getSceneAtIndex(i);
				if (scene.getName() == string)
					return mInternals->mScenes->getIndexOfScene(scene);
			}
		}
		
		// Try load scene index from key
		string = action.getActionInfo().getString(mActionInfoLoadSceneIndexFromKey);
		if (string.getLength() > 0) {
			// Get from prefs
			SSInt32Pref	pref(string.getCString(), kSceneIndexInvalid);

			return CPreferences::getSInt32(pref);
		}

		// Use scene index
		return action.getActionInfo().getUInt32(mActionInfoSceneIndexKey, kSceneIndexInvalid);
	} else
		return kSceneIndexInvalid;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppPlayerX::performAction(const CAction& action, const S2DPoint32& point)
//----------------------------------------------------------------------------------------------------------------------
{
	CArrayItemIndex	i;
	CString			actionName = action.getName();
	CDictionary		actionInfo = action.getActionInfo();
	if (actionName == mActionNameOpenURL) {
		// Open URL
		if (mInternals->mSceneAppPlayerProcsInfo.mOpenURLProc != nil) {
			// Inform
			CURLX	URL(actionInfo.getString(mActionInfoURLKey));
			mInternals->mSceneAppPlayerProcsInfo.mOpenURLProc(URL, actionInfo.contains(mActionInfoUseWebView),
					mInternals->mSceneAppPlayerProcsInfo.mUserData);
		}
	} else if (actionName == mActionNamePlayAudio) {
		// Play Audio
		Delete(mInternals->mAudioOutputTrackReference);

		mInternals->mAudioOutputTrackReference =
				CAudioOutputTrackCache::newAudioOutputTrackReferenceFromURL(
						eGetURLForResourceFilename(actionInfo.getString(mActionInfoAudioFilenameKey)));
		mInternals->mAudioOutputTrackReference->play();
	} else if (actionName == mActionNamePlayMovie) {
		// Play movie
		CString	movieFilename = actionInfo.getString(mActionInfoMovieFilenameKey);
		CURLX	movieURL = eGetURLForResourceFilename(movieFilename);
		mInternals->mSceneAppMoviePlayer = new CSceneAppMoviePlayer(movieURL);
		mInternals->mSceneAppMoviePlayer->setControlMode(
				(ESceneAppMoviePlayerControlMode) actionInfo.getUInt32(mActionInfoControlModeKey,
						kSceneAppMoviePlayerControlModeDefault));
		mInternals->mSceneAppMoviePlayer->play();
	} else if (actionName == mActionNameSceneCut) {
		// Do Cut
		CSceneIndex	sceneIndex = getLinkedSceneIndexForAction(action);
		CScene&		scene = mInternals->mScenes->getSceneAtIndex(sceneIndex);
		for (i = 0; i < mInternals->mScenePlayersArray.getCount(); i++) {
			CScenePlayer*	scenePlayer = mInternals->mScenePlayersArray[i];
			if (&scenePlayer->getScene() == &scene) {
				// We unloading ourselves?
				if (action.getOptions() & kActionOptionsUnloadCurrent)
					// Yes
					mInternals->mCurrentScenePlayer->unload();
					
				// Load scene (if not already loaded)
				scenePlayer->load();

				// Go there
				sSetCurrentScenePlayer(*mInternals, *scenePlayer);
				
				break;
			}
		}
	} else if (actionName == mActionNameScenePush) {
		// Start Push
		CSceneIndex	sceneIndex = getLinkedSceneIndexForAction(action);
		CScene&		scene = mInternals->mScenes->getSceneAtIndex(sceneIndex);
		for (i = 0; i < mInternals->mScenePlayersArray.getCount(); i++) {
			CScenePlayer*	scenePlayer = mInternals->mScenePlayersArray[i];
			if (&scenePlayer->getScene() == &scene) {
				// Load scene (if not already loaded)
				scenePlayer->load();
				
				// Create transition
				setCurrentTransition(new CSceneTransitionPlayerPush(*mInternals->mCurrentScenePlayer, *scenePlayer,
						actionInfo, point));
				
				break;
			}
		}
	} else if (actionName == mActionNameSceneSlide) {
		// Start Slide
		CSceneIndex	sceneIndex = getLinkedSceneIndexForAction(action);
		CScene&		scene = mInternals->mScenes->getSceneAtIndex(sceneIndex);
		for (i = 0; i < mInternals->mScenePlayersArray.getCount(); i++) {
			CScenePlayer*	scenePlayer = mInternals->mScenePlayersArray[i];
			if (&scenePlayer->getScene() == &scene) {
				// Load scene (if not already loaded)
				scenePlayer->load();
				
				// Create transition
				setCurrentTransition(new CSceneTransitionPlayerSlide(*mInternals->mCurrentScenePlayer, *scenePlayer,
						actionInfo, point));
				
				break;
			}
		}
	} else if (actionName == mActionNameSetItemNameValue) {
		// Set Item Name/Value
		CString	itemName = actionInfo.getString(mActionInfoItemNameKey);
		CString	propertyName = actionInfo.getString(mActionInfoPropertyNameKey);
		mInternals->mCurrentScenePlayer->setItemPlayerProperty(itemName, propertyName,
				actionInfo.getValue(mActionInfoPropertyValueKey));
	} else if (actionName == mActionNameSendItemCommand) {
		// Send Item Command
		CString		itemName = actionInfo.getString(mActionInfoItemNameKey);
		CString		command = actionInfo.getString(mActionInfoCommandKey);

		actionInfo.set(CSceneItemPlayer::mCommandInfoTouchPointKey, point.asString());
		mInternals->mCurrentScenePlayer->handleItemPlayerCommand(itemName, command, actionInfo);
	} else if (actionName == mActionNameSendAppCommand) {
		// Send App Command
		if (mInternals->mSceneAppPlayerProcsInfo.mHandleCommandProc != nil) {
			// Inform
			CString	command = actionInfo.getString(mActionInfoCommandKey);
			mInternals->mSceneAppPlayerProcsInfo.mHandleCommandProc(command, actionInfo,
					mInternals->mSceneAppPlayerProcsInfo.mUserData);
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------
CScenePlayer& CSceneAppPlayerX::getCurrentScenePlayer() const
//----------------------------------------------------------------------------------------------------------------------
{
	return *mInternals->mCurrentScenePlayer;
}

//----------------------------------------------------------------------------------------------------------------------
CScenePlayer* CSceneAppPlayerX::getScenePlayerForSceneName(const CString& sceneName) const
//----------------------------------------------------------------------------------------------------------------------
{
	for (CArrayItemIndex i = 0; i < mInternals->mScenePlayersArray.getCount(); i++) {
		CScenePlayer*	scenePlayer = mInternals->mScenePlayersArray[i];
		if (scenePlayer->getScene().getName() == sceneName)
			return scenePlayer;
	}
	
	return nil;
}

//----------------------------------------------------------------------------------------------------------------------
CScenePlayer* CSceneAppPlayerX::getScenePlayerForSceneIndex(CSceneIndex sceneIndex) const
//----------------------------------------------------------------------------------------------------------------------
{
	// Until proven otherwise, the players should be a parallel array to scenes
	if (sceneIndex < mInternals->mScenePlayersArray.getCount())
		return mInternals->mScenePlayersArray[sceneIndex];
	
	return nil;

}

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppPlayerX::setTouchHandlingOption(ESceneAppPlayerTouchHandlingOption touchHandlingOption)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mTouchHandlingOption = touchHandlingOption;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppPlayerX::touchesBegan(const TPtrArray<SSceneAppTouchBeganInfo*>& touchBeganInfosArray)
//----------------------------------------------------------------------------------------------------------------------
{
	// Touch input disabled during movie
	if (mInternals->mAreUpdatesDisabled)
		return;
		
	CSceneItemPlayer*					sceneItemPlayer;
	SSceneAppTouchBeganInfo*			touchBeganInfo;
	SSceneAppPlayerXTouchHandlerInfo*	touchHandlerInfo;
	switch (mInternals->mTouchHandlingOption) {
		case kSceneAppPlayerTouchHandlingOptionLastSingleTouchOnly:
			// Multiple touches arriving at the same time when we're only interested in the "last"
			// Cancel previous touch
			if (mInternals->mTouchHandlerInfos[0].mSceneItemPlayerHandlingTouch != nil) {
				touchHandlerInfo = &mInternals->mTouchHandlerInfos[0];
				mInternals->mCurrentScenePlayer->touchOrMouseCancelled(&touchHandlerInfo->mSceneItemPlayerHandlingTouch,
						touchHandlerInfo->mTouchObjectRef);
				mInternals->mTouchHandlerInfos[0].mTouchObjectRef = nil;
				mInternals->mTouchHandlerInfos[0].mSceneItemPlayerHandlingTouch = nil;
			}

			// One - just use first one
			touchBeganInfo = touchBeganInfosArray[0];
			if ((touchBeganInfo->mTapCount == 2) &&
					(mInternals->mCurrentScenePlayer->getScene().getDoubleTapActionsOrNil() != nil))
				// Do double tap action
				sActionsHandler(*mInternals->mCurrentScenePlayer->getScene().getDoubleTapActionsOrNil(),
						touchBeganInfo->mViewportPoint, nil);
			else {
				// Handle
				if (mInternals->mCurrentSceneTransitionPlayer != nil)
					// Transition will handle
					mInternals->mCurrentSceneTransitionPlayer->touchBeganOrMouseDownAtPoint(
							touchBeganInfo->mSceneAppTouchObjectRef, touchBeganInfo->mViewportPoint,
							touchBeganInfo->mTapCount);
				else {
					// Scene Player will handle
					sceneItemPlayer =
							mInternals->mCurrentScenePlayer->getSceneItemPlayerOrNilForTouchOrMouseAtPoint(
									touchBeganInfo->mViewportPoint);

					// Store
					touchHandlerInfo = &sTouchHandlerInfoNoHandler;
					if (sceneItemPlayer != nil) {
						touchHandlerInfo = &mInternals->mTouchHandlerInfos[0];
						touchHandlerInfo->mTouchObjectRef = touchBeganInfo->mSceneAppTouchObjectRef;
						touchHandlerInfo->mSceneItemPlayerHandlingTouch = sceneItemPlayer;
					}

					// Handle
					mInternals->mCurrentScenePlayer->touchBeganOrMouseDownAtPoint(touchBeganInfo->mViewportPoint,
							touchBeganInfo->mTapCount, &touchHandlerInfo->mSceneItemPlayerHandlingTouch,
							touchHandlerInfo->mTouchObjectRef);
				}
			}
			break;
		
		case kSceneAppPlayerTouchHandlingOptionMultipleIndividualTouches:
			// Handle each touch separately
			if (mInternals->mCurrentSceneTransitionPlayer != nil) {
				// Cancel all touch handlers
				sClearAllTouchHandlerInfos(*mInternals);

				// Only send one touch to the transition - just use first one
				touchBeganInfo = touchBeganInfosArray[0];

				// Handle
				mInternals->mCurrentSceneTransitionPlayer->touchBeganOrMouseDownAtPoint(
						touchBeganInfo->mSceneAppTouchObjectRef, touchBeganInfo->mViewportPoint,
						touchBeganInfo->mTapCount);
			} else {
				for (CArrayItemIndex i = 0; i < touchBeganInfosArray.getCount(); i++) {
					// Get info
					touchBeganInfo = touchBeganInfosArray[i];

					// Get scene item player
					sceneItemPlayer =
							mInternals->mCurrentScenePlayer->getSceneItemPlayerOrNilForTouchOrMouseAtPoint(
									touchBeganInfo->mViewportPoint);

					// Store
					SSceneAppPlayerXTouchHandlerInfo*	touchHandlerInfo = &sTouchHandlerInfoNoHandler;
					for (UInt32 i = 0; i < kSceneViewControllerMaxTouchHandlerInfos; i++) {
						if (mInternals->mTouchHandlerInfos[i].mTouchObjectRef == nil) {
							touchHandlerInfo = &mInternals->mTouchHandlerInfos[i];
							touchHandlerInfo->mTouchObjectRef = touchBeganInfo->mSceneAppTouchObjectRef;
							touchHandlerInfo->mSceneItemPlayerHandlingTouch = sceneItemPlayer;
							break;
						}
					}

					// Handle
					mInternals->mCurrentScenePlayer->touchBeganOrMouseDownAtPoint(touchBeganInfo->mViewportPoint,
							touchBeganInfo->mTapCount, &touchHandlerInfo->mSceneItemPlayerHandlingTouch,
							touchHandlerInfo->mTouchObjectRef);
				}
			}
			break;
	}
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppPlayerX::touchesMoved(const TPtrArray<SSceneAppTouchMovedInfo*>& touchMovedInfosArray)
//----------------------------------------------------------------------------------------------------------------------
{
	// Touch input disabled during movie
	if (mInternals->mAreUpdatesDisabled)
		return;

	for (CArrayItemIndex i = 0; i < touchMovedInfosArray.getCount(); i++) {
		// Get info
		SSceneAppTouchMovedInfo*	touchMovedInfo = touchMovedInfosArray[i];

		// Handle
		if (mInternals->mCurrentSceneTransitionPlayer != nil)
			// Transition will handle
			mInternals->mCurrentSceneTransitionPlayer->touchOrMouseMovedFromPoint(
					touchMovedInfo->mSceneAppTouchObjectRef, touchMovedInfo->mViewportPreviousPoint,
					touchMovedInfo->mViewportCurrentPoint);
		else {
			// Scene Player will handle
			SSceneAppPlayerXTouchHandlerInfo*	touchHandlerInfo = &sTouchHandlerInfoNoHandler;
			for (UInt32 j = 0; j < kSceneViewControllerMaxTouchHandlerInfos; j++) {
				// Look for handler
				if (mInternals->mTouchHandlerInfos[j].mTouchObjectRef == touchMovedInfo->mSceneAppTouchObjectRef) {
					// Handle
					touchHandlerInfo = &mInternals->mTouchHandlerInfos[j];
					break;
				}
			}

			mInternals->mCurrentScenePlayer->touchOrMouseMovedFromPoint(touchMovedInfo->mViewportPreviousPoint,
					touchMovedInfo->mViewportCurrentPoint, &touchHandlerInfo->mSceneItemPlayerHandlingTouch,
					touchHandlerInfo->mTouchObjectRef);
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppPlayerX::touchesEnded(const TPtrArray<SSceneAppTouchEndedInfo*>& touchEndedInfosArray)
//----------------------------------------------------------------------------------------------------------------------
{
	// Touch input disabled during movie
	if (mInternals->mAreUpdatesDisabled)
		return;
		
	for (CArrayItemIndex i = 0; i < touchEndedInfosArray.getCount(); i++) {
		// Get info
		SSceneAppTouchEndedInfo*	touchEndedInfo = touchEndedInfosArray[i];

		// Handle
		if (mInternals->mCurrentSceneTransitionPlayer != nil)
			// Transition will handle
			mInternals->mCurrentSceneTransitionPlayer->touchEndedOrMouseUpAtPoint(
					touchEndedInfo->mSceneAppTouchObjectRef, touchEndedInfo->mViewportPoint);
		else {
			// Scene Player will handle
			SSceneAppPlayerXTouchHandlerInfo*	touchHandlerInfo = &sTouchHandlerInfoNoHandler;
			for (UInt32 j = 0; j < kSceneViewControllerMaxTouchHandlerInfos; j++) {
				// Look for handler
				if (mInternals->mTouchHandlerInfos[j].mTouchObjectRef == touchEndedInfo->mSceneAppTouchObjectRef) {
					// Handle
					touchHandlerInfo = &mInternals->mTouchHandlerInfos[j];
					break;
				}
			}

			mInternals->mCurrentScenePlayer->touchEndedOrMouseUpAtPoint(touchEndedInfo->mViewportPoint,
					&touchHandlerInfo->mSceneItemPlayerHandlingTouch, touchHandlerInfo->mTouchObjectRef);

			// Clear
			touchHandlerInfo->mTouchObjectRef = nil;
			touchHandlerInfo->mSceneItemPlayerHandlingTouch = nil;
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppPlayerX::touchesCancelled(const TPtrArray<SSceneAppTouchCancelledInfo*>& touchCancelledInfosArray)
//----------------------------------------------------------------------------------------------------------------------
{
	// Touch input disabled during movie
	if (mInternals->mAreUpdatesDisabled)
		return;
		
	for (CArrayItemIndex i = 0; i < touchCancelledInfosArray.getCount(); i++) {
		// Get info
		SSceneAppTouchCancelledInfo*	touchCancelledInfo = touchCancelledInfosArray[i];

		// Handle
		if (mInternals->mCurrentSceneTransitionPlayer != nil)
			// Transition will handle
			mInternals->mCurrentSceneTransitionPlayer->touchesOrMouseCancelled();
		else {
			// Scene Player will handle
			SSceneAppPlayerXTouchHandlerInfo*	touchHandlerInfo = &sTouchHandlerInfoNoHandler;
			for (UInt32 j = 0; j < kSceneViewControllerMaxTouchHandlerInfos; j++) {
				// Look for handler
				if (mInternals->mTouchHandlerInfos[j].mTouchObjectRef == touchCancelledInfo->mSceneAppTouchObjectRef) {
					// Handle
					touchHandlerInfo = &mInternals->mTouchHandlerInfos[j];
					break;
				}
			}

			mInternals->mCurrentScenePlayer->touchOrMouseCancelled(&touchHandlerInfo->mSceneItemPlayerHandlingTouch,
					touchHandlerInfo->mTouchObjectRef);

			// Clear
			touchHandlerInfo->mTouchObjectRef = nil;
			touchHandlerInfo->mSceneItemPlayerHandlingTouch = nil;
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppPlayerX::motionBegan(ESceneAppMotionInputType motionInputType)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mCurrentScenePlayer->motionBegan(motionInputType);
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppPlayerX::motionEnded(ESceneAppMotionInputType motionInputType)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mCurrentScenePlayer->motionEnded(motionInputType);
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppPlayerX::motionCancelled(ESceneAppMotionInputType motionInputType)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mCurrentScenePlayer->motionCancelled(motionInputType);
}

////----------------------------------------------------------------------------------------------------------------------
//CImageX CSceneAppPlayerX::getCurrentViewportImage(bool performRedraw) const
////----------------------------------------------------------------------------------------------------------------------
//{
//	if (performRedraw) {
//		// Prepare to update system
//		CGPUTextureManager::mDefault.pauseLoading();
//
//		// Draw
//		mInternals->mSceneAppPlayerDrawProcsInfo.mBeginDrawProc(mInternals->mSceneAppPlayerDrawProcsInfo.mUserData);
//		if (mInternals->mCurrentSceneTransitionPlayer != nil)
//			mInternals->mCurrentSceneTransitionPlayer->drawGL();
//		else
//			mInternals->mCurrentScenePlayer->drawGL(S2DPoint32::mZero);
//
//		mInternals->mSceneAppPlayerDrawProcsInfo.mEndDrawProc(mInternals->mSceneAppPlayerDrawProcsInfo.mUserData);
//
//		// Turn stuff back on
//		CGPUTextureManager::mDefault.resumeLoading();
//	}
//
//	return mInternals->mSceneAppPlayerDrawProcsInfo.mGetCurrentViewportImageProc(
//			mInternals->mSceneAppPlayerDrawProcsInfo.mUserData);
//}

////----------------------------------------------------------------------------------------------------------------------
//void CSceneAppPlayerX::setSceneAppPlayerProcsInfo(const SSceneAppPlayerProcsInfo& sceneAppPlayerProcsInfo)
////----------------------------------------------------------------------------------------------------------------------
//{
//	mInternals->mSceneAppPlayerProcsInfo = sceneAppPlayerProcsInfo;
//}
//
////----------------------------------------------------------------------------------------------------------------------
//void CSceneAppPlayerX::setSceneAppPlayerDrawProcsInfo(const SSceneAppPlayerDrawProcsInfo& sceneAppPlayerDrawProcsInfo)
////----------------------------------------------------------------------------------------------------------------------
//{
//	mInternals->mSceneAppPlayerDrawProcsInfo = sceneAppPlayerDrawProcsInfo;
//}

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppPlayerX::shutdown()
//----------------------------------------------------------------------------------------------------------------------
{
	Delete(sSceneAppPlayer);
}

// MARK: Class methods

//----------------------------------------------------------------------------------------------------------------------
CSceneAppPlayerX& CSceneAppPlayerX::shared()
//----------------------------------------------------------------------------------------------------------------------
{
	if (sSceneAppPlayer == nil)
		sSceneAppPlayer = new CSceneAppPlayerX();

	return *sSceneAppPlayer;
}

//----------------------------------------------------------------------------------------------------------------------
CString CSceneAppPlayerX::resourceFilenameForViewport(const CString& resourceFilename)
//----------------------------------------------------------------------------------------------------------------------
{
	CString	resourceFilenameForViewport = resourceFilename;
	resourceFilenameForViewport.replaceSubStrings(sViewportSizeToReplaceString, *sViewportSizeReplacementString);

	return resourceFilenameForViewport;
}

// MARK: Protected instance methods

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppPlayerX::setCurrentTransition(CSceneTransitionPlayer* sceneTransitionPlayer)
//----------------------------------------------------------------------------------------------------------------------
{
	// Clear all touch handlers
	sClearAllTouchHandlerInfos(*mInternals);

	// Store new transition
	mInternals->mCurrentSceneTransitionPlayer = sceneTransitionPlayer;
}

// MARK: Internal methods

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppPlayerX::loadScenes(const S2DSize32& viewportPixelSize, const TPtrArray<CString*>& scenesFilenamesArray)
//----------------------------------------------------------------------------------------------------------------------
{
	// Store and calculate viewport info
	mInternals->mViewportSize = viewportPixelSize;
	Float32	viewportAspectRatio = viewportPixelSize.mWidth / viewportPixelSize.mHeight;
	Float32	viewportTotalPixels = viewportPixelSize.mWidth * viewportPixelSize.mHeight;
	
	// Sort scenes files by aspect ratio difference
	TPtrArray<CString*>	scenesFilenamesArraySorted;
	scenesFilenamesArraySorted.addFrom(scenesFilenamesArray);
	scenesFilenamesArraySorted.sort(sScenesFilenamesArrayCompareProc, &viewportAspectRatio);

	// Find scenes file
	CString*	scenesFilename = nil;
	Float32		scenesCurrentTotalPixels = 0.0, scenesCurrentAspectRatioDelta = -1.0;
	for (CArrayItemIndex i = 0; i < scenesFilenamesArraySorted.getCount(); i++) {
		// Get info for this scenes file
		CString*		filename = scenesFilenamesArraySorted[i];
		TArray<CString>	dimensions = filename->breakUp(CString("."))[0].breakUp(CString("_"))[2].breakUp(CString("x"));
		Float32			width = dimensions[0].getFloat32();
		Float32			height = dimensions[1].getFloat32();
		Float32			totalPixels = width * height;
		Float32			aspectRatioDelta = fabsf(width / height - viewportAspectRatio);
		
		if (scenesCurrentAspectRatioDelta == -1.0)
			scenesCurrentAspectRatioDelta = aspectRatioDelta;
		
		if (fabsf(scenesCurrentAspectRatioDelta - aspectRatioDelta) > 0.01)
			break;
			
		// We're looking for the scenes file that is the smallest dimensions that fully covers
		// the viewport pixels, or the largest (or the only!)
		if (((totalPixels > scenesCurrentTotalPixels) && (totalPixels <= viewportTotalPixels)) ||
				((totalPixels > scenesCurrentTotalPixels) && (scenesCurrentTotalPixels < viewportTotalPixels)) ||
				((totalPixels < scenesCurrentTotalPixels) && (totalPixels >= viewportTotalPixels))) {
			// Use this scene
			scenesFilename = filename;
			eSceneAppViewportPixelSize.mWidth = width;
			eSceneAppViewportPixelSize.mHeight = height;
			scenesCurrentTotalPixels = totalPixels;
		}
	}

#if DEBUG
	CLogServices::logMessage(CString("Loading scenes from ") + *scenesFilename);
#endif
	sViewportSizeReplacementString =
			new CString(CString(eSceneAppViewportPixelSize.mWidth, 0, 0) + CString("x") +
					CString(eSceneAppViewportPixelSize.mHeight, 0, 0));

	// Load scenes
	Delete(mInternals->mScenes);

//	const	CDataSource*	dataProvider = eCreateDataProvider(*scenesFilename);

	UError	error;
	CDictionary	info = CBinaryPropertyList::dictionaryFrom(CByteParceller(dataProvider), error);
	ReturnIfError(error);

	mInternals->mScenes = new CScenes(info);

	// Setup viewport parameters
	switch (mInternals->mScenes->getViewportZoom()) {
		case kSceneAppViewportZoomCenter: {
			// Center scenes in viewport
			mInternals->mViewportOffset =
					S2DPoint32((viewportPixelSize.mWidth - eSceneAppViewportPixelSize.mWidth) * 0.5,
							(viewportPixelSize.mHeight - eSceneAppViewportPixelSize.mHeight) * 0.5);
			mInternals->mViewportScale = S2DPoint32(1.0, 1.0);
			} break;

		case kSceneAppViewportZoomScale:
			// Scale scenes to fill viewport completely.
			mInternals->mViewportOffset = S2DPoint32::mZero;
			mInternals->mViewportScale =
					S2DPoint32(viewportPixelSize.mWidth / eSceneAppViewportPixelSize.mWidth,
							viewportPixelSize.mHeight / eSceneAppViewportPixelSize.mHeight);
			break;

		case kSceneAppViewportZoomScaleAspectFit: {
			// Scale scenes to be as large as possible and still fit all content
			Float32	scaleX = viewportPixelSize.mWidth / eSceneAppViewportPixelSize.mWidth;
			Float32	scaleY = viewportPixelSize.mHeight / eSceneAppViewportPixelSize.mHeight;
			if (scaleX < scaleY) {
				// Fill width, offset height
				mInternals->mViewportOffset.mY =
						(viewportPixelSize.mHeight - eSceneAppViewportPixelSize.mHeight * scaleX) * 0.5;
				mInternals->mViewportScale = S2DPoint32(scaleX, scaleX);
			} else {
				// Fill height, offset width
				mInternals->mViewportOffset.mX =
						(viewportPixelSize.mWidth - eSceneAppViewportPixelSize.mWidth * scaleY) * 0.5;
				mInternals->mViewportScale = S2DPoint32(scaleY, scaleY);
			}
			} break;

		case kSceneAppViewportZoomScaleAspectFill: {
			// Scale scenes to be as small as possible and fill every pixel
			Float32	scaleX = viewportPixelSize.mWidth / eSceneAppViewportPixelSize.mWidth;
			Float32	scaleY = viewportPixelSize.mHeight / eSceneAppViewportPixelSize.mHeight;
			if (scaleX < scaleY) {
				// Extend and offset width, fill height
				mInternals->mViewportOffset.mX =
						(viewportPixelSize.mWidth - eSceneAppViewportPixelSize.mWidth * scaleY) * 0.5;
				mInternals->mViewportScale = S2DPoint32(scaleY, scaleY);
			} else {
				// Extend and offset height, fill width
				mInternals->mViewportOffset.mY =
						(viewportPixelSize.mHeight - eSceneAppViewportPixelSize.mHeight * scaleX) * 0.5;
				mInternals->mViewportScale = S2DPoint32(scaleX, scaleX);
			}
			} break;
	}

	// Create scene players
	mInternals->mScenePlayersArray.removeAll();

	CScene&	initialScene = mInternals->mScenes->getInitialScene();
	for (UInt32 i = 0; i < mInternals->mScenes->getScenesCount(); i++) {
		// Create scene player
		CScene&			scene = mInternals->mScenes->getSceneAtIndex(i);
		CScenePlayer*	scenePlayer = new CScenePlayer(scene, mInternals->mScenePlayerProcsInfo);
		mInternals->mScenePlayersArray += scenePlayer;

		// Handle initial scene
		if (&scene == &initialScene) {
			mInternals->mCurrentScenePlayer = scenePlayer;
			mInternals->mCurrentScenePlayer->load();
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - Local proc definitions

//----------------------------------------------------------------------------------------------------------------------
void sSetCurrentScenePlayer(CSceneAppPlayerInternalsX& internals, CScenePlayer& scenePlayer)
//----------------------------------------------------------------------------------------------------------------------
{
	// Stop and dispose of movie player
	if (internals.mSceneAppMoviePlayer != nil) {
		internals.mSceneAppMoviePlayer->stop();
		Delete(internals.mSceneAppMoviePlayer);

		internals.mIgnoreElapsedTime = true;
	}
	
	// Stop any audio we have
	Delete(internals.mAudioOutputTrackReference);
	
	// Save
	internals.mCurrentScenePlayer = &scenePlayer;

	// Start
	internals.mCurrentScenePlayer->start();
	
	// Do we need to store the scene index?
	if (internals.mCurrentScenePlayer->getScene().getStoreSceneIndexAsString().getLength() > 0) {
		// Yes
		SSInt32Pref	pref(internals.mCurrentScenePlayer->getScene().getStoreSceneIndexAsString().getCString(), 0);
		CPreferences::set(pref, internals.mScenes->getIndexOfScene(internals.mCurrentScenePlayer->getScene()));
	}

	// Update the linked scenes
	sUpdateLinkedScenes(internals);
	
	// Disable any touches that will come through for the old SceneController
	::bzero(internals.mTouchHandlerInfos, sizeof(internals.mTouchHandlerInfos));
	
	// Resume animation
	internals.mAreUpdatesDisabled = false;

	CSceneAppPlayerX::shared().resume();
}

//----------------------------------------------------------------------------------------------------------------------
void sUpdateLinkedScenes(CSceneAppPlayerInternalsX& internals)
//----------------------------------------------------------------------------------------------------------------------
{
	// Load linked scenes that can preload
	// Unload all the rest
	TPtrArray<CScenePlayer*>	toUnloadScenePlayersArray;
	toUnloadScenePlayersArray.addFrom(internals.mScenePlayersArray);
	toUnloadScenePlayersArray -= internals.mCurrentScenePlayer;

	CArrayItemIndex		i;
	CSceneAppPlayerX&	sceneAppPlayer = CSceneAppPlayerX::shared();
	CScenePlayer*		scenePlayer;
	TPtrArray<CAction*>	actionsArray = internals.mCurrentScenePlayer->getAllActions();
	for (i = 0; i < actionsArray.getCount(); i++) {
		// Get next action
		CAction*	action = actionsArray[i];

		// Get linked scene index
		CSceneIndex	sceneIndex = sceneAppPlayer.getLinkedSceneIndexForAction(*action);
		if (sceneIndex == kSceneIndexInvalid)
			continue;
		
		// Get scene player
		scenePlayer = sceneAppPlayer.getScenePlayerForSceneIndex(sceneIndex);

		// Update
		if (!(action->getOptions() & kActionOptionsDontPreload)) {
			// Remove from unload array
			toUnloadScenePlayersArray -= scenePlayer;

			// Load
			scenePlayer->load();
		}
		
		// Reset
		scenePlayer->reset();
	}

	// Unload the reset
	for (i = 0; i < toUnloadScenePlayersArray.getCount(); i++) {
		scenePlayer = toUnloadScenePlayersArray[i];
		if (!(scenePlayer->getScene().getOptions() & kSceneOptionsDontUnload))
			scenePlayer->unload();
	}
}

//----------------------------------------------------------------------------------------------------------------------
void sClearAllTouchHandlerInfos(CSceneAppPlayerInternalsX& internals)
//----------------------------------------------------------------------------------------------------------------------
{
	for (UInt32 i = 0; i < kSceneViewControllerMaxTouchHandlerInfos; i++) {
		if (internals.mTouchHandlerInfos[i].mSceneItemPlayerHandlingTouch != nil) {
			// Handle
			SSceneAppPlayerXTouchHandlerInfo*	touchHandlerInfo = &internals.mTouchHandlerInfos[i];
			internals.mCurrentScenePlayer->touchOrMouseCancelled(&touchHandlerInfo->mSceneItemPlayerHandlingTouch,
					touchHandlerInfo->mTouchObjectRef);

			// Clear
			internals.mTouchHandlerInfos[i].mTouchObjectRef = nil;
			internals.mTouchHandlerInfos[i].mSceneItemPlayerHandlingTouch = nil;
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------
void sActionsHandler(const CActions& actions, const S2DPoint32& point, void* userData)
//----------------------------------------------------------------------------------------------------------------------
{
	CSceneAppPlayerX&	sceneAppPlayer = CSceneAppPlayerX::shared();
	for (CArrayItemIndex i = 0; i < actions.getCount(); i++)
		sceneAppPlayer.performAction(*actions[i], point);
}

//----------------------------------------------------------------------------------------------------------------------
ECompareResult sScenesFilenamesArrayCompareProc(CString* const filename1, CString* const filename2, void* userData)
//----------------------------------------------------------------------------------------------------------------------
{
	Float32*		viewportAspectRatio = (Float32*) userData;
	TArray<CString>	dimensions1 = filename1->breakUp(CString("."))[0].breakUp(CString("_"))[2].breakUp(CString("x"));
	Float32			width1 = dimensions1[0].getFloat32();
	Float32			height1 = dimensions1[1].getFloat32();
	Float32			aspectRatioDelta1 = fabsf(width1 / height1 - *viewportAspectRatio);

	TArray<CString>	dimensions2 = filename2->breakUp(CString("."))[0].breakUp(CString("_"))[2].breakUp(CString("x"));
	Float32			width2 = dimensions2[0].getFloat32();
	Float32			height2 = dimensions2[1].getFloat32();
	Float32			aspectRatioDelta2 = fabsf(width2 / height2 - *viewportAspectRatio);
	
	if (aspectRatioDelta1 < aspectRatioDelta2)
		return kCompareResultBefore;
	else if (aspectRatioDelta1 > aspectRatioDelta2)
		return kCompareResultAfter;
	else
		return kCompareResultEquivalent;
}

////----------------------------------------------------------------------------------------------------------------------
//CImageX sScenePlayerGetCurrentViewportImage(CScenePlayer& scenePlayer, bool performRedraw, void* userData)
////----------------------------------------------------------------------------------------------------------------------
//{
//	return CSceneAppPlayerX::shared().getCurrentViewportImage(performRedraw);
//}
