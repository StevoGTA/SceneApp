//----------------------------------------------------------------------------------------------------------------------
//	CSceneAppPlayer.cpp			©2019 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CSceneAppPlayer.h"

#include "CBinaryPropertyList.h"
#include "CLogServices.h"
#include "CPreferences.h"
#include "CSceneItemPlayerAnimation.h"
#include "CSceneItemPlayerButton.h"
#include "CSceneItemPlayerButtonArray.h"
#include "CSceneItemPlayerHotspot.h"
#include "CScenePackage.h"
#include "CScenePlayer.h"
#include "CSceneTransitionPlayerCoverUncover.h"
#include "CSceneTransitionPlayerPush.h"
#include "TBuffer.h"
#include "TLockingArray.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: STouchHandlerInfo

struct STouchHandlerInfo {
	// Lifecycle methods
	STouchHandlerInfo() : mReference(nil) {}
	STouchHandlerInfo(const void* reference) : mReference(reference) {}

	// Properties
	const	void*	mReference;
};

//----------------------------------------------------------------------------------------------------------------------
// MARK: - Local proc declarations

static	CByteParceller	sSceneAppPlayerCreateByteParceller(const CString& resourceFilename, void* userData);

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneAppPlayerInternals

class CSceneAppPlayerInternals {
	public:
									CSceneAppPlayerInternals(CSceneAppPlayer& sceneAppPlayer, CGPU& gpu,
												const SSceneAppPlayerProcsInfo& sceneAppPlayerProcsInfo);

				void				handleTouchesBegan();
				void				handleTouchesMoved();
				void				handleTouchesEnded();
				void				handleTouchesCancelled();
				void				cancelAllSceneTouches();
				void				cancelAllSceneTransitionTouches();

				OR<CScenePlayer>	getScenePlayer(const CString& sceneName);
				void				setCurrent(CScenePlayer& scenePlayer, CSceneIndex sceneIndex);
				void				setCurrent(CSceneTransitionPlayer* sceneTransitionPlayer, CSceneIndex sceneIndex);

		static	bool				compareReference(const STouchHandlerInfo& touchHandlerInfo, void* reference)
										{ return touchHandlerInfo.mReference == reference; }
		static	S2DSizeF32			scenePlayerGetViewportSizeProc(void* userData);
		static	CSceneItemPlayer*	scenePlayerCreateSceneItemPlayerProc(const CSceneItem& sceneItem,
											const SSceneAppResourceManagementInfo& sceneAppResourceManagementInfo,
											const SSceneItemPlayerProcsInfo& sceneItemPlayerProcsInfo, void* userData);
		static	void				scenePlayerActionsPerformProc(const CActions& actions, const S2DPointF32& point,
											void* userData);

		ESceneAppPlayerOptions								mOptions;

		CGPU&												mGPU;
		CGPUTextureManager									mGPUTextureManager;
		CSceneAppPlayer&									mSceneAppPlayer;

		SSceneAppPlayerProcsInfo							mSceneAppPlayerProcsInfo;
		SSceneAppResourceManagementInfo						mSceneAppResourceManagementInfo;
		SScenePlayerProcsInfo								mScenePlayerProcsInfo;
		SSceneTransitionPlayerProcsInfo						mSceneTransitionPlayerProcsInfo;

		CScenePackage										mScenePackage;

		TNArray<CScenePlayer>								mScenePlayers;
		OR<CScenePlayer>									mCurrentScenePlayer;
		OO<CSceneTransitionPlayer>							mCurrentSceneTransitionPlayer;
		OV<CSceneIndex>										mCurrentSceneTransitionPlayerToSceneIndex;

		TNLockingArray<SSceneAppPlayerTouchBeganInfo>		mTouchBeganInfos;
		TNLockingArray<SSceneAppPlayerTouchMovedInfo>		mTouchMovedInfos;
		TNLockingArray<SSceneAppPlayerTouchEndedInfo>		mTouchEndedInfos;
		TNLockingArray<SSceneAppPlayerTouchCancelledInfo>	mTouchCancelledInfos;
		TNArray<STouchHandlerInfo>							mSceneTouchHandlerInfos;
		TNArray<STouchHandlerInfo>							mSceneTransitionTouchHandlerInfos;

		UniversalTime										mStopTime;
		UniversalTime										mLastPeriodicOutputTime;
};

//----------------------------------------------------------------------------------------------------------------------
CSceneAppPlayerInternals::CSceneAppPlayerInternals(CSceneAppPlayer& sceneAppPlayer, CGPU& gpu,
		const SSceneAppPlayerProcsInfo& sceneAppPlayerProcsInfo) :
	mOptions(kSceneAppPlayerOptionsDefault), mGPU(gpu), mGPUTextureManager(gpu), mSceneAppPlayer(sceneAppPlayer),
			mSceneAppPlayerProcsInfo(sceneAppPlayerProcsInfo),
			mSceneAppResourceManagementInfo(sSceneAppPlayerCreateByteParceller, mGPUTextureManager, this),
			mScenePlayerProcsInfo(scenePlayerGetViewportSizeProc, scenePlayerCreateSceneItemPlayerProc,
					scenePlayerActionsPerformProc, this),
			mSceneTransitionPlayerProcsInfo(scenePlayerGetViewportSizeProc, this),
			mStopTime(0.0), mLastPeriodicOutputTime(0.0)
//----------------------------------------------------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppPlayerInternals::handleTouchesBegan()
//----------------------------------------------------------------------------------------------------------------------
{
	// Check for info
	if (mTouchBeganInfos.isEmpty())
		return;

	// Collect infos to handle
	TNArray<SSceneAppPlayerTouchBeganInfo>	touchBeganInfos;
	while (!mTouchBeganInfos.isEmpty()) {
		// Get the next info
		SSceneAppPlayerTouchBeganInfo	touchBeganInfo = mTouchBeganInfos.popFirst();

		// Check if handling multitouch
		if (
				(mCurrentSceneTransitionPlayer.hasObject() &&
						!mCurrentSceneTransitionPlayer->supportsMultitouch()) ||
				((mOptions & kSceneAppPlayerOptionsTouchHandlingMask) == kSceneAppPlayerOptionsLastSingleTouchOnly))
			// Last single touch only
			touchBeganInfos.removeAll();

		// Add
		touchBeganInfos += touchBeganInfo;
	}

	// Check if need to cancel current touches
	if (mCurrentSceneTransitionPlayer.hasObject() ||
			((mOptions & kSceneAppPlayerOptionsTouchHandlingMask) == kSceneAppPlayerOptionsLastSingleTouchOnly))
		// Cancel current scene touches
		cancelAllSceneTouches();

	// Handle
	while (!touchBeganInfos.isEmpty()) {
		// Pop first info
		SSceneAppPlayerTouchBeganInfo	touchBeganInfo = touchBeganInfos.popFirst();

		// Check for scene transition player
		STouchHandlerInfo	touchHandlerInfo(touchBeganInfo.mReference);
		if (!mCurrentSceneTransitionPlayer.hasObject()) {
			// Pass to scene player
			mSceneTouchHandlerInfos += touchHandlerInfo;

			mCurrentScenePlayer->touchBeganOrMouseDownAtPoint(touchBeganInfo.mViewportPoint, touchBeganInfo.mTapCount,
					touchBeganInfo.mReference);

			// Check if transmogrophied into a transition
			if ((mSceneTouchHandlerInfos.getCount() == 0) && mCurrentSceneTransitionPlayer.hasObject())
				// Touch began transmogrophied into a transition
				mSceneTransitionTouchHandlerInfos += touchHandlerInfo;
		} else {
			// Pass to scene transition player
			mSceneTransitionTouchHandlerInfos += touchHandlerInfo;

			mCurrentSceneTransitionPlayer->touchBeganOrMouseDownAtPoint(touchBeganInfo.mViewportPoint,
					touchBeganInfo.mTapCount, touchBeganInfo.mReference);
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppPlayerInternals::handleTouchesMoved()
//----------------------------------------------------------------------------------------------------------------------
{
	// Process all
	while (!mTouchMovedInfos.isEmpty()) {
		// Get first info
		SSceneAppPlayerTouchMovedInfo	touchMovedInfo = mTouchMovedInfos.popFirst();

		// Check for scene transition player
		if (!mCurrentSceneTransitionPlayer.hasObject()) {
			// Look for active touch handler info
			OR<STouchHandlerInfo>	touchHandlerInfo =
											mSceneTouchHandlerInfos.getFirst(compareReference,
													touchMovedInfo.mReference);
			if (touchHandlerInfo.hasReference())
				// Pass to scene player
				mCurrentScenePlayer->touchOrMouseMovedFromPoint(touchMovedInfo.mViewportPreviousPoint,
						touchMovedInfo.mViewportCurrentPoint, touchHandlerInfo->mReference);
		} else {
			// Look for active touch handler info
			OR<STouchHandlerInfo>	touchHandlerInfo =
											mSceneTransitionTouchHandlerInfos.getFirst(compareReference,
													touchMovedInfo.mReference);
			if (touchHandlerInfo.hasReference())
				// Pass to scene transition player
				mCurrentSceneTransitionPlayer->touchOrMouseMovedFromPoint(touchMovedInfo.mViewportPreviousPoint,
						touchMovedInfo.mViewportCurrentPoint,touchHandlerInfo->mReference);
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppPlayerInternals::handleTouchesEnded()
//----------------------------------------------------------------------------------------------------------------------
{
	// Process all
	while (!mTouchEndedInfos.isEmpty()) {
		// Get first info
		SSceneAppPlayerTouchEndedInfo	touchEndedInfo = mTouchEndedInfos.popFirst();

		// Check for scene transition player
		if (!mCurrentSceneTransitionPlayer.hasObject()) {
			// Look for active touch handler info
			OV<STouchHandlerInfo>	touchHandlerInfoValue =
											mSceneTouchHandlerInfos.popFirst(compareReference,
													touchEndedInfo.mReference);
			if (touchHandlerInfoValue.hasValue()) {
				// Pass to scene player
				STouchHandlerInfo	touchHandlerInfo = *touchHandlerInfoValue;
				mCurrentScenePlayer->touchEndedOrMouseUpAtPoint(touchEndedInfo.mViewportPoint,
						touchHandlerInfo.mReference);
			}
		} else {
			// Look for active touch handler info
			OV<STouchHandlerInfo>	touchHandlerInfoValue =
											mSceneTransitionTouchHandlerInfos.popFirst(compareReference,
															touchEndedInfo.mReference);
			if (touchHandlerInfoValue.hasValue()) {
				// Pass to scene transition player
				STouchHandlerInfo	touchHandlerInfo = *touchHandlerInfoValue;
				mCurrentSceneTransitionPlayer->touchEndedOrMouseUpAtPoint(touchEndedInfo.mViewportPoint,
						touchHandlerInfo.mReference);
			}
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppPlayerInternals::handleTouchesCancelled()
//----------------------------------------------------------------------------------------------------------------------
{
	// Process all
	while (!mTouchCancelledInfos.isEmpty()) {
		// Get first info
		SSceneAppPlayerTouchCancelledInfo	touchCancelledInfo = mTouchCancelledInfos.popFirst();

		// Check for scene transition player
		if (!mCurrentSceneTransitionPlayer.hasObject()) {
			// Look for active touch handler info
			OV<STouchHandlerInfo>	touchHandlerInfoValue =
											mSceneTouchHandlerInfos.popFirst(compareReference,
													touchCancelledInfo.mReference);
			if (touchHandlerInfoValue.hasValue()) {
				// Pass to scene player
				STouchHandlerInfo	touchHandlerInfo = *touchHandlerInfoValue;
				mCurrentScenePlayer->touchOrMouseCancelled(touchHandlerInfo.mReference);
			}
		} else {
			// Look for active touch handler info
			OV<STouchHandlerInfo>	touchHandlerInfoValue =
											mSceneTransitionTouchHandlerInfos.popFirst(compareReference,
													touchCancelledInfo.mReference);
			if (touchHandlerInfoValue.hasValue()) {
				// Pass to scene transition player
				STouchHandlerInfo	touchHandlerInfo = *touchHandlerInfoValue;
				mCurrentSceneTransitionPlayer->touchesOrMouseCancelled(touchHandlerInfo.mReference);
			}
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppPlayerInternals::cancelAllSceneTouches()
//----------------------------------------------------------------------------------------------------------------------
{
	// Iterate all
	while (!mSceneTouchHandlerInfos.isEmpty()) {
		// Pop first
		STouchHandlerInfo	touchHandlerInfo = mSceneTouchHandlerInfos.popFirst();

		// Cancel
		mCurrentScenePlayer->touchOrMouseCancelled(touchHandlerInfo.mReference);
	}
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppPlayerInternals::cancelAllSceneTransitionTouches()
//----------------------------------------------------------------------------------------------------------------------
{
	// Check if have current transition scene player
	if (!mCurrentSceneTransitionPlayer.hasObject())
		return;

	// Iterate all
	while (!mSceneTransitionTouchHandlerInfos.isEmpty()) {
		// Pop first
		STouchHandlerInfo	touchHandlerInfo = mSceneTransitionTouchHandlerInfos.popFirst();

		// Cancel
		mCurrentSceneTransitionPlayer->touchesOrMouseCancelled(touchHandlerInfo.mReference);
	}
}

//----------------------------------------------------------------------------------------------------------------------
OR<CScenePlayer> CSceneAppPlayerInternals::getScenePlayer(const CString& sceneName)
//----------------------------------------------------------------------------------------------------------------------
{
	// Iterate all scene players
	for (TIteratorD<CScenePlayer> iterator = mScenePlayers.getIterator(); iterator.hasValue(); iterator.advance()) {
		// Check if the target scene player
		if (iterator.getValue().getScene().getName() == sceneName)
			// Found
			return OR<CScenePlayer>(iterator.getValue());
	}

	return OR<CScenePlayer>();
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppPlayerInternals::setCurrent(CScenePlayer& scenePlayer, CSceneIndex sceneIndex)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	TBuffer<bool>	performLoad(mScenePlayers.getCount());
	for (CArrayItemIndex i = 0; i < mScenePlayers.getCount(); i++)
		// Setup
		(*performLoad)[i] = mScenePlayers[i].getScene() == scenePlayer.getScene();

	// Iterate all actions for this scene player
	CActions	actions = scenePlayer.getAllActions();
	for (TIteratorD<CAction> iterator = actions.getIterator(); iterator.hasValue(); iterator.advance()) {
		// Setup
		const	CAction&	action = iterator.getValue();

		// Query linked scene index
		OV<CSceneIndex>	testSceneIndex = mSceneAppPlayer.getSceneIndex(action);
		if (!testSceneIndex.hasValue())
			// No linked scene index
			continue;

		// Get linked scene player
		if (!(action.getOptions() & kActionOptionsDontPreload))
			// Preload
			(*performLoad)[*testSceneIndex] = true;
	}

	// Load new scene first to put its resources in the front of the queue
	scenePlayer.load(mGPU);

	// Load linked scenes
	for (CArrayItemIndex i = 0; i < mScenePlayers.getCount(); i++) {
		// Check if loading
		if (performLoad[i])
			// Load
			mScenePlayers[i].load(mGPU);
	}

	// Unload other scenes
	for (CArrayItemIndex i = 0; i < mScenePlayers.getCount(); i++) {
		// Check if loading
		if (!performLoad[i])
			// Unoad
			mScenePlayers[i].unload();
	}

	// Update
	mCurrentScenePlayer = OR<CScenePlayer>(scenePlayer);

	// Check if need to store the scene index
	const	CString&	storeSceneIndexAsString = scenePlayer.getScene().getStoreSceneIndexAsString();
	if (!storeSceneIndexAsString.isEmpty())
		// Store
		CPreferences::mDefault.set(SSInt32Pref(storeSceneIndexAsString.getOSString(), 0), sceneIndex);

	cancelAllSceneTouches();
	cancelAllSceneTransitionTouches();

	// Start
	mCurrentScenePlayer->start();
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppPlayerInternals::setCurrent(CSceneTransitionPlayer* sceneTransitionPlayer, CSceneIndex sceneIndex)
//----------------------------------------------------------------------------------------------------------------------
{
	// Store new transition
	mCurrentSceneTransitionPlayer = OO<CSceneTransitionPlayer>(sceneTransitionPlayer);
	mCurrentSceneTransitionPlayerToSceneIndex = OV<CSceneIndex>(sceneIndex);

	// Clear all touch handlers
	cancelAllSceneTouches();
	cancelAllSceneTransitionTouches();
}

//----------------------------------------------------------------------------------------------------------------------
S2DSizeF32 CSceneAppPlayerInternals::scenePlayerGetViewportSizeProc(void* userData)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	CSceneAppPlayerInternals*	internals = (CSceneAppPlayerInternals*) userData;

	return internals->mScenePackage.getSize();
}

//----------------------------------------------------------------------------------------------------------------------
CSceneItemPlayer* CSceneAppPlayerInternals::scenePlayerCreateSceneItemPlayerProc(const CSceneItem& sceneItem,
		const SSceneAppResourceManagementInfo& sceneAppResourceManagementInfo,
		const SSceneItemPlayerProcsInfo& sceneItemPlayerProcsInfo, void* userData)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	CSceneAppPlayerInternals&	internals = *((CSceneAppPlayerInternals*) userData);

	// Check type
	const	CString&	type = sceneItem.getType();
	if (type == CSceneItemAnimation::mType)
		// Animation
		return new CSceneItemPlayerAnimation((const CSceneItemAnimation&) sceneItem, sceneAppResourceManagementInfo,
				sceneItemPlayerProcsInfo);
	else if (type == CSceneItemButton::mType)
		// Button
		return new CSceneItemPlayerButton((const CSceneItemButton&) sceneItem, sceneAppResourceManagementInfo,
				sceneItemPlayerProcsInfo);
	else if (type == CSceneItemButtonArray::mType)
		// Button array
		return new CSceneItemPlayerButtonArray((const CSceneItemButtonArray&) sceneItem,
				sceneAppResourceManagementInfo, sceneItemPlayerProcsInfo);
	else if (type == CSceneItemHotspot::mType)
		// Hotspot
		return new CSceneItemPlayerHotspot((const CSceneItemHotspot&) sceneItem,sceneAppResourceManagementInfo,
				sceneItemPlayerProcsInfo);
	else
		// Custom
		return internals.mSceneAppPlayer.createSceneItemPlayer((CSceneItemCustom&) sceneItem,
				sceneAppResourceManagementInfo, sceneItemPlayerProcsInfo);
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppPlayerInternals::scenePlayerActionsPerformProc(const CActions& actions, const S2DPointF32& point,
		void* userData)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	CSceneAppPlayerInternals&	internals = *((CSceneAppPlayerInternals*) userData);

	// Iterate all actions
	for (TIteratorD<CAction> iterator = actions.getIterator(); iterator.hasValue(); iterator.advance()) {
		// Setup
		const	CAction&		action = iterator.getValue();
		const	CString&		actionName = action.getName();
		const	CDictionary&	actionInfo = action.getActionInfo();

		// Check action name
		if (actionName == CAction::mNameOpenURL)
			// Open URL
			internals.mSceneAppPlayerProcsInfo.openURL(CURL(actionInfo.getString(CAction::mInfoURLKey)),
					actionInfo.contains(CAction::mInfoUseWebView));
		else if (actionName == CAction::mNamePlayAudio) {
			// Play Audio
//			Delete(internals.mAudioOutputTrackReference);
//
//			internals.mAudioOutputTrackReference =
//					CAudioOutputTrackCache::newAudioOutputTrackReferenceFromURL(
//							eGetURLForResourceFilename(actionInfo.getString(mInfoAudioFilenameKey)));
//			internals.mAudioOutputTrackReference->play();
		} else if (actionName == CAction::mNamePlayMovie) {
			// Play movie
//			CString	movieFilename = actionInfo.getString(CAction::mInfoMovieFilenameKey);
//			CURLX	movieURL = eGetURLForResourceFilename(movieFilename);
//			internals.mSceneAppMoviePlayer = new CSceneAppMoviePlayer(movieURL);
//			internals.mSceneAppMoviePlayer->setControlMode(
//					(ESceneAppMoviePlayerControlMode) actionInfo.getUInt32(mInfoControlModeKey,
//							kSceneAppMoviePlayerControlModeDefault));
//			internals.mSceneAppMoviePlayer->play();
		} else if (actionName == CAction::mNameSceneCover) {
			// Cover to scene
			CSceneIndex		sceneIndex = internals.mSceneAppPlayer.getSceneIndex(action).getValue();
			CScenePlayer&	scenePlayer = internals.mSceneAppPlayer.loadAndStartScenePlayer(sceneIndex);

			// Create transition
			internals.setCurrent(
					new CSceneTransitionPlayerCoverUncover(*internals.mCurrentScenePlayer, scenePlayer, actionInfo,
							point, true, internals.mSceneTransitionPlayerProcsInfo), sceneIndex);
		} else if (actionName == CAction::mNameSceneCut) {
			// Cut to scene
			CSceneIndex	sceneIndex = internals.mSceneAppPlayer.getSceneIndex(action).getValue();
			internals.setCurrent(internals.mScenePlayers[sceneIndex], sceneIndex);
		} else if (actionName == CAction::mNameScenePush) {
			// Push to scene
			CSceneIndex		sceneIndex = internals.mSceneAppPlayer.getSceneIndex(action).getValue();
			CScenePlayer&	scenePlayer = internals.mSceneAppPlayer.loadAndStartScenePlayer(sceneIndex);

			// Create transition
			internals.setCurrent(
					new CSceneTransitionPlayerPush(*internals.mCurrentScenePlayer, scenePlayer, actionInfo, point,
							internals.mSceneTransitionPlayerProcsInfo), sceneIndex);
		} else if (actionName == CAction::mNameSceneUncover) {
			// Uncover to scene
			CSceneIndex		sceneIndex = internals.mSceneAppPlayer.getSceneIndex(action).getValue();
			CScenePlayer&	scenePlayer = internals.mSceneAppPlayer.loadAndStartScenePlayer(sceneIndex);

			// Create transition
			internals.setCurrent(
					new CSceneTransitionPlayerCoverUncover(*internals.mCurrentScenePlayer, scenePlayer, actionInfo,
							point, false, internals.mSceneTransitionPlayerProcsInfo), sceneIndex);
		} else if (actionName == CAction::mNameSetItemNameValue) {
			// Set Item Name/Value
			const	CString&		sceneName = actionInfo.getString(CAction::mInfoSceneNameKey);
					CScenePlayer&	scenePlayer =
											!sceneName.isEmpty() ?
													*internals.getScenePlayer(sceneName) :
													*internals.mCurrentScenePlayer;
			scenePlayer.setItemPlayerProperty(actionInfo.getString(CAction::mInfoItemNameKey),
					actionInfo.getString(CAction::mInfoPropertyNameKey),
					actionInfo.getValue(CAction::mInfoPropertyValueKey));
		} else if (actionName == CAction::mNameSendItemCommand) {
			// Send Item Command
			const	CString&		sceneName = actionInfo.getString(CAction::mInfoSceneNameKey);
					CScenePlayer&	scenePlayer =
											!sceneName.isEmpty() ?
													*internals.getScenePlayer(sceneName) :
													*internals.mCurrentScenePlayer;
			scenePlayer.handleItemPlayerCommand(internals.mGPU, actionInfo.getString(CAction::mInfoItemNameKey),
					actionInfo.getString(CAction::mInfoCommandKey), actionInfo, point);
		} else if (actionName == CAction::mNameSendAppCommand)
			// Send App Command
			internals.mSceneAppPlayerProcsInfo.handleCommand(actionInfo.getString(CAction::mInfoCommandKey),
					actionInfo);
		else
			// Other
			internals.mSceneAppPlayer.performAction(action, point);
	}
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneAppPlayer

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
CSceneAppPlayer::CSceneAppPlayer(CGPU& gpu, const SSceneAppPlayerProcsInfo& sceneAppPlayerProcsInfo)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new CSceneAppPlayerInternals(*this, gpu, sceneAppPlayerProcsInfo);
}

//----------------------------------------------------------------------------------------------------------------------
CSceneAppPlayer::~CSceneAppPlayer()
//----------------------------------------------------------------------------------------------------------------------
{
	Delete(mInternals);
}

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
ESceneAppPlayerOptions CSceneAppPlayer::getOptions() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mOptions;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppPlayer::setOptions(ESceneAppPlayerOptions options)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mOptions = options;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppPlayer::loadScenes(const SScenePackageInfo& scenePackageInfo)
//----------------------------------------------------------------------------------------------------------------------
{
// Load scenes
#if defined(DEBUG)
	CLogServices::logMessage(CString(OSSTR("Loading scenes from ")) + scenePackageInfo.mFilename);
#endif

	UError	error;
	CDictionary	info =
						CBinaryPropertyList::dictionaryFrom(
								mInternals->mSceneAppPlayerProcsInfo.createByteParceller(scenePackageInfo.mFilename),
								error);
	ReturnIfError(error);

	mInternals->mOptions =
			(ESceneAppPlayerOptions) info.getUInt64(CString(OSSTR("options")), kSceneAppPlayerOptionsDefault);
	mInternals->mScenePackage = CScenePackage(scenePackageInfo.mSize, info);

	// Create scene players
	mInternals->mScenePlayers.removeAll();

	const	CScene&	initialScene = mInternals->mScenePackage.getInitialScene();
	for (UInt32 i = 0; i < mInternals->mScenePackage.getScenesCount(); i++) {
		// Create scene player
		const	CScene&	scene = mInternals->mScenePackage.getSceneAtIndex(i);
		mInternals->mScenePlayers +=
				CScenePlayer(scene, mInternals->mSceneAppResourceManagementInfo, mInternals->mScenePlayerProcsInfo);

		// Handle initial scene
		if (scene == initialScene) {
			// Is initial scene
			mInternals->mCurrentScenePlayer = OR<CScenePlayer>(mInternals->mScenePlayers.getLast());
			mInternals->mCurrentScenePlayer->load(mInternals->mGPU);
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppPlayer::start(bool loadAllTextures)
//----------------------------------------------------------------------------------------------------------------------
{
	// Start
	mInternals->mCurrentScenePlayer->start();

	// Update last periodic output time
	if ((mInternals->mLastPeriodicOutputTime != 0.0) && (mInternals->mStopTime != 0.0))
		// Advance last periodic time to ignore time between stop and start
		mInternals->mLastPeriodicOutputTime += SUniversalTime::getCurrentUniversalTime() - mInternals->mStopTime;

	// Install periodic
	mInternals->mSceneAppPlayerProcsInfo.installPeriodic();

	// Check if loading all textures
	if (loadAllTextures)
		// Load all textures
		mInternals->mGPUTextureManager.loadAll();
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppPlayer::stop(bool unloadAllTextures)
//----------------------------------------------------------------------------------------------------------------------
{
	// Remove periodic
	mInternals->mSceneAppPlayerProcsInfo.removePeriodic();

	// Note stop time
	mInternals->mStopTime = SUniversalTime::getCurrentUniversalTime();

	// Cleanup touches
	mInternals->mSceneTouchHandlerInfos.removeAll();
	mInternals->mSceneTransitionTouchHandlerInfos.removeAll();

	// Check if unloading all textures
	if (unloadAllTextures)
		// Unload all textures
		mInternals->mGPUTextureManager.unloadAll();
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppPlayer::handlePeriodic(UniversalTime outputTime)
//----------------------------------------------------------------------------------------------------------------------
{
	// Update time info
	UniversalTimeInterval	deltaTimeInterval;
	if (mInternals->mLastPeriodicOutputTime == 0.0)
		// First iteration
		deltaTimeInterval = 0.0;
	else
		// Next iteration
		deltaTimeInterval = outputTime - mInternals->mLastPeriodicOutputTime;
	mInternals->mLastPeriodicOutputTime = outputTime;

	// Handle input
	mInternals->handleTouchesBegan();
	mInternals->handleTouchesMoved();
	mInternals->handleTouchesEnded();
	mInternals->handleTouchesCancelled();

	// Update
	if (deltaTimeInterval > 0.0) {
		// Update
		if (mInternals->mCurrentSceneTransitionPlayer.hasObject()) {
			// Update transition
			mInternals->mCurrentSceneTransitionPlayer->update(deltaTimeInterval);

			switch (mInternals->mCurrentSceneTransitionPlayer->getState()) {
				case kSceneTransitionStateActive:
					// Active
					break;

				case kSceneTransitionStateCompleted:
					// Completed
					mInternals->setCurrent(mInternals->mCurrentSceneTransitionPlayer->getToScenePlayer(),
							*mInternals->mCurrentSceneTransitionPlayerToSceneIndex);
					mInternals->mCurrentSceneTransitionPlayer = OO<CSceneTransitionPlayer>();
					break;

				case kSceneTransitionStateReset:
					// Reset
					mInternals->mCurrentSceneTransitionPlayer = OO<CSceneTransitionPlayer>();
					break;
			}
		} else
			// Update current scene
			mInternals->mCurrentScenePlayer->update(deltaTimeInterval);
	}

	// Render - camera is at (0, 0.7, 1.5), looking at point (0, -0.1, 0) with the up-vector along the y-axis.
	Float32	fieldOfViewAngle3D = T2DUtilities<Float32>::toRadians(70.0f);
	Float32	aspectRatio3D = mInternals->mScenePackage.getSize().aspectRatio();

	S3DPointF32	camera3D(0.0f, 0.7f, 1.5f);
	S3DPointF32	target3D(0.0f, -0.1f, 0.0f);
	S3DPointF32	up3D(0.0f, 1.0f, 0.0f);
	mInternals->mGPU.renderStart(mInternals->mScenePackage.getSize(), fieldOfViewAngle3D, aspectRatio3D, 0.01f, 100.0f,
			camera3D, target3D, up3D);
	if (mInternals->mCurrentSceneTransitionPlayer.hasObject())
		// Render transition
		mInternals->mCurrentSceneTransitionPlayer->render(mInternals->mGPU);
	else
		// Render scene
		mInternals->mCurrentScenePlayer->render(mInternals->mGPU);
	mInternals->mGPU.renderEnd();
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppPlayer::mouseDown(const SSceneAppPlayerMouseDownInfo& mouseDownInfo)
//----------------------------------------------------------------------------------------------------------------------
{
	// Map mouse input to touch input
	mInternals->mTouchBeganInfos +=
			SSceneAppPlayerTouchBeganInfo(mouseDownInfo.mViewportPoint, mouseDownInfo.mClickCount);
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppPlayer::mouseDragged(const SSceneAppPlayerMouseDraggedInfo& mouseDraggedInfo)
//----------------------------------------------------------------------------------------------------------------------
{
	// Map mouse input to touch input
	mInternals->mTouchMovedInfos +=
			SSceneAppPlayerTouchMovedInfo(mouseDraggedInfo.mViewportPreviousPoint,
					mouseDraggedInfo.mViewportCurrentPoint);
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppPlayer::mouseUp(const SSceneAppPlayerMouseUpInfo& mouseUpInfo)
//----------------------------------------------------------------------------------------------------------------------
{
	// Map mouse input to touch input
	mInternals->mTouchEndedInfos += SSceneAppPlayerTouchEndedInfo(mouseUpInfo.mViewportPoint);
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppPlayer::mouseCancelled(const SSceneAppPlayerMouseCancelledInfo& mouseCancelledInfo)
//----------------------------------------------------------------------------------------------------------------------
{
	// Map mouse input to touch input
	mInternals->mTouchCancelledInfos += SSceneAppPlayerTouchCancelledInfo();
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppPlayer::touchesBegan(const TArray<SSceneAppPlayerTouchBeganInfo>& touchBeganInfosArray)
//----------------------------------------------------------------------------------------------------------------------
{
	// Note for later
	mInternals->mTouchBeganInfos += touchBeganInfosArray;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppPlayer::touchesMoved(const TArray<SSceneAppPlayerTouchMovedInfo>& touchMovedInfosArray)
//----------------------------------------------------------------------------------------------------------------------
{
	// Note for later
	mInternals->mTouchMovedInfos += touchMovedInfosArray;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppPlayer::touchesEnded(const TArray<SSceneAppPlayerTouchEndedInfo>& touchEndedInfosArray)
//----------------------------------------------------------------------------------------------------------------------
{
	// Note for later
	mInternals->mTouchEndedInfos += touchEndedInfosArray;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppPlayer::touchesCancelled(const TArray<SSceneAppPlayerTouchCancelledInfo>& touchCancelledInfosArray)
//----------------------------------------------------------------------------------------------------------------------
{
	// Note for later
	mInternals->mTouchCancelledInfos += touchCancelledInfosArray;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppPlayer::shakeBegan()
//----------------------------------------------------------------------------------------------------------------------
{
	// Check for scene transition player
	if (!mInternals->mCurrentSceneTransitionPlayer.hasObject())
		// Pass to scene player
		mInternals->mCurrentScenePlayer->shakeBegan();
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppPlayer::shakeEnded()
//----------------------------------------------------------------------------------------------------------------------
{
	// Check for scene transition player
	if (!mInternals->mCurrentSceneTransitionPlayer.hasObject())
		// Pass to scene player
		mInternals->mCurrentScenePlayer->shakeEnded();
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppPlayer::shakeCancelled()
//----------------------------------------------------------------------------------------------------------------------
{
	// Check for scene transition player
	if (!mInternals->mCurrentSceneTransitionPlayer.hasObject())
		// Pass to scene player
		mInternals->mCurrentScenePlayer->shakeCancelled();
}

//----------------------------------------------------------------------------------------------------------------------
CSceneItemPlayer* CSceneAppPlayer::createSceneItemPlayer(const CSceneItemCustom& sceneItemCustom,
		const SSceneAppResourceManagementInfo& sceneAppResourceManagementInfo,
		const SSceneItemPlayerProcsInfo& sceneItemPlayerProcsInfo) const
//----------------------------------------------------------------------------------------------------------------------
{
	AssertFailUnimplemented();

	return nil;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppPlayer::performAction(const CAction& action, const S2DPointF32& point)
//----------------------------------------------------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------------------------------------------------
OV<CSceneIndex> CSceneAppPlayer::getSceneIndex(const CAction& action) const
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	const	CDictionary&	actionInfo = action.getActionInfo();

	// Try to find scene with matching name
	const	CString&	sceneName = actionInfo.getString(CAction::mInfoSceneNameKey);
	if (!sceneName.isEmpty()) {
		// Iterate all scenes to find scene with matching name
		for (UInt32 sceneIndex = 0; sceneIndex < mInternals->mScenePackage.getScenesCount(); sceneIndex++) {
			// Check this scene
			if (mInternals->mScenePackage.getSceneAtIndex(sceneIndex).getName() == sceneName)
				// Found match
				return OV<CSceneIndex>(sceneIndex);
		}
	}

	// Try to decode and retrieve scene index
	const	CString&	loadSceneIndexFrom = actionInfo.getString(CAction::mInfoLoadSceneIndexFromKey);
	if (!loadSceneIndexFrom.isEmpty()) {
		// Retrieve from prefs
		SSInt32Pref	pref(loadSceneIndexFrom.getOSString(), 0);

		return CPreferences::mDefault.hasValue(pref) ?
				OV<CSceneIndex>(CPreferences::mDefault.getSInt32(pref)) : OV<CSceneIndex>();
	}

	// Look for scene index
	if (actionInfo.contains(CAction::mInfoSceneIndexKey))
		// Have scene index
		return OV<CSceneIndex>(actionInfo.getUInt32(CAction::mInfoSceneIndexKey));

	return OV<CSceneIndex>();
}

//----------------------------------------------------------------------------------------------------------------------
CScenePlayer& CSceneAppPlayer::loadAndStartScenePlayer(CSceneIndex sceneIndex) const
//----------------------------------------------------------------------------------------------------------------------
{
	// Get scene player
	CScenePlayer&	scenePlayer = mInternals->mScenePlayers[sceneIndex];

	// Load and start
	scenePlayer.load(mInternals->mGPU);
	scenePlayer.reset();

	return scenePlayer;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppPlayer::setCurrent(CSceneTransitionPlayer* sceneTransitionPlayer, CSceneIndex sceneIndex)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->setCurrent(sceneTransitionPlayer, sceneIndex);
}

//----------------------------------------------------------------------------------------------------------------------
const SSceneAppResourceManagementInfo& CSceneAppPlayer::getSceneAppResourceManagementInfo() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mSceneAppResourceManagementInfo;
}

//----------------------------------------------------------------------------------------------------------------------
const SSceneTransitionPlayerProcsInfo& CSceneAppPlayer::getSceneTransitionPlayerProcsInfo() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mSceneTransitionPlayerProcsInfo;
}

//----------------------------------------------------------------------------------------------------------------------
CScenePlayer& CSceneAppPlayer::getCurrentScenePlayer() const
//----------------------------------------------------------------------------------------------------------------------
{
	return *mInternals->mCurrentScenePlayer;
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - Local proc definitions

//----------------------------------------------------------------------------------------------------------------------
CByteParceller sSceneAppPlayerCreateByteParceller(const CString& resourceFilename, void* userData)
{
	// Setup
	CSceneAppPlayerInternals*	sceneAppPlayerInternals = (CSceneAppPlayerInternals*) userData;

	return sceneAppPlayerInternals->mSceneAppPlayerProcsInfo.createByteParceller(resourceFilename);
}
