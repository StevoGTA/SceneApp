//----------------------------------------------------------------------------------------------------------------------
//	CSceneAppPlayer.cpp			©2019 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CSceneAppPlayer.h"

#include "CBinaryPropertyList.h"
#include "CLogServices.h"
#include "CPreferences.h"
#include "CSceneAppMediaEngine.h"
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

static	CByteParceller	sCreateByteParceller(const CString& resourceFilename, void* userData);

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneAppPlayerInternals

class CSceneAppPlayerInternals {
	public:
									CSceneAppPlayerInternals(CSceneAppPlayer& sceneAppPlayer, CGPU& gpu,
												const CSceneAppPlayer::Procs& procs);

				void				handleTouchesBegan();
				void				handleTouchesMoved();
				void				handleTouchesEnded();
				void				handleTouchesCancelled();
				void				cancelAllSceneTouches();
				void				cancelAllSceneTransitionTouches();

				OR<CScenePlayer>	getScenePlayer(const CString& sceneName);
				void				setCurrentSceneIndex(CScene::Index sceneIndex);
				void				setCurrent(CSceneTransitionPlayer* sceneTransitionPlayer, CScene::Index sceneIndex);

		static	bool				compareReference(const STouchHandlerInfo& touchHandlerInfo, void* reference)
										{ return touchHandlerInfo.mReference == reference; }
		static	S2DSizeF32			scenePlayerGetViewportSizeProc(void* userData);
		static	CSceneItemPlayer*	scenePlayerCreateSceneItemPlayerProc(const CSceneItem& sceneItem,
											const SSceneAppResourceManagementInfo& sceneAppResourceManagementInfo,
											const CSceneItemPlayer::Procs& procs, void* userData);
		static	void				scenePlayerActionsPerformProc(const CActions& actions, const S2DPointF32& point,
											void* userData);

		CSRSWMessageQueues									mMessageQueues;
		CSceneAppPlayer::Options							mOptions;

		CGPU&												mGPU;
		CGPUTextureManager									mGPUTextureManager;
		CSceneAppMediaEngine								mSceneAppMediaEngine;
		CSceneAppPlayer&									mSceneAppPlayer;

		CSceneAppPlayer::Procs								mSceneAppPlayerProcs;
		SSceneAppResourceManagementInfo						mSceneAppResourceManagementInfo;
		CScenePlayer::Procs									mScenePlayerProcsInfo;
		CSceneTransitionPlayer::Procs						mSceneTransitionPlayerProcs;

		CScenePackage										mScenePackage;

		TNArray<CScenePlayer>								mScenePlayers;
		OR<CScenePlayer>									mCurrentScenePlayer;
		OI<CMediaPlayer>									mCurrentScenePlayerBackground1MediaPlayer;
		OI<CMediaPlayer>									mCurrentScenePlayerBackground2MediaPlayer;
		OI<CSceneTransitionPlayer>							mCurrentSceneTransitionPlayer;
		OV<CScene::Index>									mCurrentSceneTransitionPlayerToSceneIndex;

		TNLockingArray<CSceneAppPlayer::TouchBeganInfo>		mTouchBeganInfos;
		TNLockingArray<CSceneAppPlayer::TouchMovedInfo>		mTouchMovedInfos;
		TNLockingArray<CSceneAppPlayer::TouchEndedInfo>		mTouchEndedInfos;
		TNLockingArray<CSceneAppPlayer::TouchCancelledInfo>	mTouchCancelledInfos;
		TNArray<STouchHandlerInfo>							mSceneTouchHandlerInfos;
		TNArray<STouchHandlerInfo>							mSceneTransitionTouchHandlerInfos;

		UniversalTime										mStopTime;
		UniversalTime										mLastPeriodicOutputTime;
};

//----------------------------------------------------------------------------------------------------------------------
CSceneAppPlayerInternals::CSceneAppPlayerInternals(CSceneAppPlayer& sceneAppPlayer, CGPU& gpu,
		const CSceneAppPlayer::Procs& procs) :
	mOptions(CSceneAppPlayer::kOptionsDefault), mGPU(gpu), mGPUTextureManager(gpu),
			mSceneAppMediaEngine(mMessageQueues, CSceneAppMediaEngine::Info(sCreateByteParceller, this)),
			mSceneAppPlayer(sceneAppPlayer), mSceneAppPlayerProcs(procs),
			mSceneAppResourceManagementInfo(sCreateByteParceller, mGPUTextureManager, mSceneAppMediaEngine, this),
			mScenePlayerProcsInfo(scenePlayerGetViewportSizeProc, scenePlayerCreateSceneItemPlayerProc,
					scenePlayerActionsPerformProc, this),
			mSceneTransitionPlayerProcs(scenePlayerGetViewportSizeProc, this),
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
	TNArray<CSceneAppPlayer::TouchBeganInfo>	touchBeganInfos;
	while (!mTouchBeganInfos.isEmpty()) {
		// Get the next info
		CSceneAppPlayer::TouchBeganInfo	touchBeganInfo = mTouchBeganInfos.popFirst();

		// Check if handling multitouch
		if ((mCurrentSceneTransitionPlayer.hasInstance() && !mCurrentSceneTransitionPlayer->supportsMultitouch()) ||
				((mOptions & CSceneAppPlayer::kOptionsTouchHandlingMask) != 0))
			// Last single touch only
			touchBeganInfos.removeAll();

		// Add
		touchBeganInfos += touchBeganInfo;
	}

	// Check if need to cancel current touches
	if (mCurrentSceneTransitionPlayer.hasInstance() || ((mOptions & CSceneAppPlayer::kOptionsTouchHandlingMask) != 0))
		// Cancel current scene touches
		cancelAllSceneTouches();

	// Handle
	while (!touchBeganInfos.isEmpty()) {
		// Pop first info
		CSceneAppPlayer::TouchBeganInfo	touchBeganInfo = touchBeganInfos.popFirst();

		// Check for scene transition player
		STouchHandlerInfo	touchHandlerInfo(touchBeganInfo.mReference);
		if (!mCurrentSceneTransitionPlayer.hasInstance()) {
			// Pass to scene player
			mSceneTouchHandlerInfos += touchHandlerInfo;

			mCurrentScenePlayer->touchBeganOrMouseDownAtPoint(touchBeganInfo.mViewportPoint, touchBeganInfo.mTapCount,
					touchBeganInfo.mReference);

			// Check if transmogrophied into a transition
			if ((mSceneTouchHandlerInfos.getCount() == 0) && mCurrentSceneTransitionPlayer.hasInstance())
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
		CSceneAppPlayer::TouchMovedInfo	touchMovedInfo = mTouchMovedInfos.popFirst();

		// Check for scene transition player
		if (!mCurrentSceneTransitionPlayer.hasInstance()) {
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
		CSceneAppPlayer::TouchEndedInfo	touchEndedInfo = mTouchEndedInfos.popFirst();

		// Check for scene transition player
		if (!mCurrentSceneTransitionPlayer.hasInstance()) {
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
		CSceneAppPlayer::TouchCancelledInfo	touchCancelledInfo = mTouchCancelledInfos.popFirst();

		// Check for scene transition player
		if (!mCurrentSceneTransitionPlayer.hasInstance()) {
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
	if (!mCurrentSceneTransitionPlayer.hasInstance())
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
void CSceneAppPlayerInternals::setCurrentSceneIndex(CScene::Index sceneIndex)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	CScenePlayer&	scenePlayer = mScenePlayers[sceneIndex];

	TBuffer<bool>	performLoad(mScenePlayers.getCount());
	for (CArray::ItemIndex i = 0; i < mScenePlayers.getCount(); i++)
		// Setup
		(*performLoad)[i] = i == (CArray::ItemIndex) sceneIndex;

	// Iterate all actions for this scene player
	CActions	actions = scenePlayer.getAllActions();
	for (TIteratorD<CAction> iterator = actions.getIterator(); iterator.hasValue(); iterator.advance()) {
		// Setup
		const	CAction&	action = iterator.getValue();

		// Query linked scene index
		OV<CScene::Index>	testSceneIndex = mSceneAppPlayer.getSceneIndex(action);
		if (!testSceneIndex.hasValue())
			// No linked scene index
			continue;

		// Get linked scene player
		if (!(action.getOptions() & CAction::kOptionsDontPreload))
			// Preload
			(*performLoad)[*testSceneIndex] = true;
	}

	// Load new scene first to put its resources in the front of the queue
	scenePlayer.load(mGPU);

	// Load linked scenes
	for (CArray::ItemIndex i = 0; i < mScenePlayers.getCount(); i++) {
		// Check if loading
		if (performLoad[i])
			// Load
			mScenePlayers[i].load(mGPU);
	}

	// Unload other scenes
	for (CArray::ItemIndex i = 0; i < mScenePlayers.getCount(); i++) {
		// Check if loading
		if (!performLoad[i])
			// Unoad
			mScenePlayers[i].unload();
	}

	// Update
	OR<CScenePlayer>	previousScenePlayer = mCurrentScenePlayer;
	mCurrentScenePlayer = OR<CScenePlayer>(scenePlayer);

	// Check if need to store the scene index
	const	CString&	storeSceneIndexAsString = scenePlayer.getScene().getStoreSceneIndexAsString();
	if (!storeSceneIndexAsString.isEmpty())
		// Store
		CPreferences::mDefault.set(CPreferences::SInt32Pref(storeSceneIndexAsString.getOSString(), 0), sceneIndex);

	cancelAllSceneTouches();
	cancelAllSceneTransitionTouches();

	// Start new scene player
	mCurrentScenePlayer->start();

	// Setup Background Audio
	const	OI<CAudioInfo>&	background1AudioInfo = mCurrentScenePlayer->getScene().getBackground1AudioInfo();
	if (background1AudioInfo.hasInstance()) {
		// Have background audio
		mCurrentScenePlayerBackground1MediaPlayer = mSceneAppMediaEngine.getMediaPlayer(*background1AudioInfo);
		if (mCurrentScenePlayerBackground1MediaPlayer.hasInstance())
			// Play
			mCurrentScenePlayerBackground1MediaPlayer->play();
	} else
		// No background audio
		mCurrentScenePlayerBackground1MediaPlayer = OI<CMediaPlayer>();

	const	OI<CAudioInfo>&	background2AudioInfo = mCurrentScenePlayer->getScene().getBackground2AudioInfo();
	if (background2AudioInfo.hasInstance()) {
		// Have background audio
		mCurrentScenePlayerBackground2MediaPlayer = mSceneAppMediaEngine.getMediaPlayer(*background2AudioInfo);
		if (mCurrentScenePlayerBackground2MediaPlayer.hasInstance())
			// Play
			mCurrentScenePlayerBackground2MediaPlayer->play();
	} else
		// No background audio
		mCurrentScenePlayerBackground2MediaPlayer = OI<CMediaPlayer>();

	// Check if had previous scene player
	if (previousScenePlayer.hasReference())
		// Reset
		previousScenePlayer->reset();
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppPlayerInternals::setCurrent(CSceneTransitionPlayer* sceneTransitionPlayer, CScene::Index sceneIndex)
//----------------------------------------------------------------------------------------------------------------------
{
	// Store new transition
	mCurrentSceneTransitionPlayer = OI<CSceneTransitionPlayer>(sceneTransitionPlayer);
	mCurrentSceneTransitionPlayerToSceneIndex = OV<CScene::Index>(sceneIndex);

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
		const CSceneItemPlayer::Procs& procs, void* userData)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	CSceneAppPlayerInternals&	internals = *((CSceneAppPlayerInternals*) userData);

	// Check type
	const	CString&	type = sceneItem.getType();
	if (type == CSceneItemAnimation::mType)
		// Animation
		return new CSceneItemPlayerAnimation((const CSceneItemAnimation&) sceneItem, sceneAppResourceManagementInfo,
				procs);
	else if (type == CSceneItemButton::mType)
		// Button
		return new CSceneItemPlayerButton((const CSceneItemButton&) sceneItem, sceneAppResourceManagementInfo,
				procs);
	else if (type == CSceneItemButtonArray::mType)
		// Button array
		return new CSceneItemPlayerButtonArray((const CSceneItemButtonArray&) sceneItem, sceneAppResourceManagementInfo,
				procs);
	else if (type == CSceneItemHotspot::mType)
		// Hotspot
		return new CSceneItemPlayerHotspot((const CSceneItemHotspot&) sceneItem,sceneAppResourceManagementInfo, procs);
	else
		// Custom
		return internals.mSceneAppPlayer.createSceneItemPlayer((CSceneItemCustom&) sceneItem,
				sceneAppResourceManagementInfo, procs);
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
			internals.mSceneAppPlayerProcs.openURL(CURL(actionInfo.getString(CAction::mInfoURLKey)),
					actionInfo.contains(CAction::mInfoUseWebView));
		else if (actionName == CAction::mNamePlayMovie) {
			// Play movie
//			CString	movieFilename = actionInfo.getString(CAction::mInfoFilenameKey);
//			CURLX	movieURL = eGetURLForResourceFilename(movieFilename);
//			internals.mSceneAppMoviePlayer = new CSceneAppMoviePlayer(movieURL);
//			internals.mSceneAppMoviePlayer->setControlMode(
//					(ESceneAppMoviePlayerControlMode) actionInfo.getUInt32(mInfoControlModeKey,
//							kSceneAppMoviePlayerControlModeDefault));
//			internals.mSceneAppMoviePlayer->play();
		} else if (actionName == CAction::mNameSceneCover) {
			// Cover to scene
			CScene::Index	sceneIndex = internals.mSceneAppPlayer.getSceneIndex(action).getValue();
			CScenePlayer&	scenePlayer = internals.mSceneAppPlayer.loadAndStartScenePlayer(sceneIndex);

			// Create transition
			internals.setCurrent(
					new CSceneTransitionPlayerCoverUncover(*internals.mCurrentScenePlayer, scenePlayer, actionInfo,
							point, true, internals.mSceneTransitionPlayerProcs), sceneIndex);
		} else if (actionName == CAction::mNameSceneCut)
			// Cut to scene
			internals.setCurrentSceneIndex(internals.mSceneAppPlayer.getSceneIndex(action).getValue());
		else if (actionName == CAction::mNameScenePush) {
			// Push to scene
			CScene::Index	sceneIndex = internals.mSceneAppPlayer.getSceneIndex(action).getValue();
			CScenePlayer&	scenePlayer = internals.mSceneAppPlayer.loadAndStartScenePlayer(sceneIndex);

			// Create transition
			internals.setCurrent(
					new CSceneTransitionPlayerPush(*internals.mCurrentScenePlayer, scenePlayer, actionInfo, point,
							internals.mSceneTransitionPlayerProcs), sceneIndex);
		} else if (actionName == CAction::mNameSceneUncover) {
			// Uncover to scene
			CScene::Index	sceneIndex = internals.mSceneAppPlayer.getSceneIndex(action).getValue();
			CScenePlayer&	scenePlayer = internals.mSceneAppPlayer.loadAndStartScenePlayer(sceneIndex);

			// Create transition
			internals.setCurrent(
					new CSceneTransitionPlayerCoverUncover(*internals.mCurrentScenePlayer, scenePlayer, actionInfo,
							point, false, internals.mSceneTransitionPlayerProcs), sceneIndex);
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
			internals.mSceneAppPlayerProcs.handleCommand(actionInfo.getString(CAction::mInfoCommandKey), actionInfo);
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
CSceneAppPlayer::CSceneAppPlayer(CGPU& gpu, const Procs& procs)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new CSceneAppPlayerInternals(*this, gpu, procs);
}

//----------------------------------------------------------------------------------------------------------------------
CSceneAppPlayer::~CSceneAppPlayer()
//----------------------------------------------------------------------------------------------------------------------
{
	Delete(mInternals);
}

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
CSceneAppPlayer::Options CSceneAppPlayer::getOptions() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mOptions;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppPlayer::setOptions(Options options)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mOptions = options;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppPlayer::loadScenes(const CScenePackage::Info& scenePackageInfo)
//----------------------------------------------------------------------------------------------------------------------
{
// Load scenes
#if defined(DEBUG)
	CLogServices::logMessage(CString(OSSTR("Loading scenes from ")) + scenePackageInfo.mFilename);
#endif

	CDictionary	info =
						*CBinaryPropertyList::dictionaryFrom(
								mInternals->mSceneAppPlayerProcs.createByteParceller(scenePackageInfo.mFilename))
								.mDictionary;
	mInternals->mOptions = (Options) info.getUInt64(CString(OSSTR("options")), kOptionsDefault);
	mInternals->mScenePackage = CScenePackage(scenePackageInfo.mSize, info);

	// Create scene players
	mInternals->mScenePlayers.removeAll();

	for (UInt32 i = 0; i < mInternals->mScenePackage.getScenesCount(); i++)
		// Create scene player
		mInternals->mScenePlayers +=
				CScenePlayer(mInternals->mScenePackage.getSceneAtIndex(i),
						mInternals->mSceneAppResourceManagementInfo, mInternals->mScenePlayerProcsInfo);

	// Set initial scene
	mInternals->setCurrentSceneIndex(mInternals->mScenePackage.getInitialSceneIndex());
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
		mInternals->mLastPeriodicOutputTime += SUniversalTime::getCurrent() - mInternals->mStopTime;

	// Install periodic
	mInternals->mSceneAppPlayerProcs.installPeriodic();

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
	mInternals->mSceneAppPlayerProcs.removePeriodic();

	// Note stop time
	mInternals->mStopTime = SUniversalTime::getCurrent();

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
		// Flush notifications
		mInternals->mMessageQueues.handleAll();
		
		// Update
		if (mInternals->mCurrentSceneTransitionPlayer.hasInstance()) {
			// Update transition
			mInternals->mCurrentSceneTransitionPlayer->update(deltaTimeInterval);

			switch (mInternals->mCurrentSceneTransitionPlayer->getState()) {
				case CSceneTransitionPlayer::kStateActive:
					// Active
					break;

				case CSceneTransitionPlayer::kStateCompleted:
					// Completed
					mInternals->setCurrentSceneIndex(*mInternals->mCurrentSceneTransitionPlayerToSceneIndex);
					mInternals->mCurrentSceneTransitionPlayer = OI<CSceneTransitionPlayer>();
					break;

				case CSceneTransitionPlayer::kStateReset:
					// Reset
					mInternals->mCurrentSceneTransitionPlayer = OI<CSceneTransitionPlayer>();
					break;
			}
		} else
			// Update current scene
			mInternals->mCurrentScenePlayer->update(deltaTimeInterval);
	}

	// Render - camera is at (0, 0.7, 1.5), looking at point (0, -0.1, 0) with the up-vector along the y-axis.
	Float32	fieldOfViewAngle3D = T2DUtilities<Float32>::toRadians(70.0f);
	Float32	aspectRatio3D = mInternals->mScenePackage.getSize().aspectRatio();

	S3DPointF32		camera3D(0.0f, 0.7f, 1.5f);
	S3DPointF32		target3D(0.0f, -0.1f, 0.0f);
	S3DVectorF32	up3D(0.0f, 1.0f, 0.0f);
	mInternals->mGPU.renderStart(mInternals->mScenePackage.getSize(), fieldOfViewAngle3D, aspectRatio3D, 0.01f, 100.0f,
			camera3D, target3D, up3D);
	if (mInternals->mCurrentSceneTransitionPlayer.hasInstance())
		// Render transition
		mInternals->mCurrentSceneTransitionPlayer->render(mInternals->mGPU);
	else
		// Render scene
		mInternals->mCurrentScenePlayer->render(mInternals->mGPU);
	mInternals->mGPU.renderEnd();
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppPlayer::mouseDown(const MouseDownInfo& mouseDownInfo)
//----------------------------------------------------------------------------------------------------------------------
{
	// Map mouse input to touch input
	mInternals->mTouchBeganInfos += TouchBeganInfo(mouseDownInfo.mViewportPoint, mouseDownInfo.mClickCount);
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppPlayer::mouseDragged(const MouseDraggedInfo& mouseDraggedInfo)
//----------------------------------------------------------------------------------------------------------------------
{
	// Map mouse input to touch input
	mInternals->mTouchMovedInfos +=
			TouchMovedInfo(mouseDraggedInfo.mViewportPreviousPoint, mouseDraggedInfo.mViewportCurrentPoint);
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppPlayer::mouseUp(const MouseUpInfo& mouseUpInfo)
//----------------------------------------------------------------------------------------------------------------------
{
	// Map mouse input to touch input
	mInternals->mTouchEndedInfos += TouchEndedInfo(mouseUpInfo.mViewportPoint);
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppPlayer::mouseCancelled(const MouseCancelledInfo& mouseCancelledInfo)
//----------------------------------------------------------------------------------------------------------------------
{
	// Map mouse input to touch input
	mInternals->mTouchCancelledInfos += TouchCancelledInfo();
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppPlayer::touchesBegan(const TArray<TouchBeganInfo>& touchBeganInfosArray)
//----------------------------------------------------------------------------------------------------------------------
{
	// Note for later
	mInternals->mTouchBeganInfos += touchBeganInfosArray;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppPlayer::touchesMoved(const TArray<TouchMovedInfo>& touchMovedInfosArray)
//----------------------------------------------------------------------------------------------------------------------
{
	// Note for later
	mInternals->mTouchMovedInfos += touchMovedInfosArray;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppPlayer::touchesEnded(const TArray<TouchEndedInfo>& touchEndedInfosArray)
//----------------------------------------------------------------------------------------------------------------------
{
	// Note for later
	mInternals->mTouchEndedInfos += touchEndedInfosArray;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppPlayer::touchesCancelled(const TArray<TouchCancelledInfo>& touchCancelledInfosArray)
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
	if (!mInternals->mCurrentSceneTransitionPlayer.hasInstance())
		// Pass to scene player
		mInternals->mCurrentScenePlayer->shakeBegan();
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppPlayer::shakeEnded()
//----------------------------------------------------------------------------------------------------------------------
{
	// Check for scene transition player
	if (!mInternals->mCurrentSceneTransitionPlayer.hasInstance())
		// Pass to scene player
		mInternals->mCurrentScenePlayer->shakeEnded();
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppPlayer::shakeCancelled()
//----------------------------------------------------------------------------------------------------------------------
{
	// Check for scene transition player
	if (!mInternals->mCurrentSceneTransitionPlayer.hasInstance())
		// Pass to scene player
		mInternals->mCurrentScenePlayer->shakeCancelled();
}

//----------------------------------------------------------------------------------------------------------------------
CSceneItemPlayer* CSceneAppPlayer::createSceneItemPlayer(const CSceneItemCustom& sceneItemCustom,
		const SSceneAppResourceManagementInfo& sceneAppResourceManagementInfo,
		const CSceneItemPlayer::Procs& procs) const
//----------------------------------------------------------------------------------------------------------------------
{
	return nil;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppPlayer::performAction(const CAction& action, const S2DPointF32& point)
//----------------------------------------------------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------------------------------------------------
OV<CScene::Index> CSceneAppPlayer::getSceneIndex(const CAction& action) const
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
				return OV<CScene::Index>(sceneIndex);
		}
	}

	// Try to decode and retrieve scene index
	const	CString&	loadSceneIndexFrom = actionInfo.getString(CAction::mInfoLoadSceneIndexFromKey);
	if (!loadSceneIndexFrom.isEmpty()) {
		// Retrieve from prefs
		CPreferences::SInt32Pref	pref(loadSceneIndexFrom.getOSString(), 0);

		return CPreferences::mDefault.hasValue(pref) ?
				OV<CScene::Index>(CPreferences::mDefault.getSInt32(pref)) : OV<CScene::Index>();
	}

	// Look for scene index
	if (actionInfo.contains(CAction::mInfoSceneIndexKey))
		// Have scene index
		return OV<CScene::Index>(actionInfo.getUInt32(CAction::mInfoSceneIndexKey));

	return OV<CScene::Index>();
}

//----------------------------------------------------------------------------------------------------------------------
CScenePlayer& CSceneAppPlayer::loadAndStartScenePlayer(CScene::Index sceneIndex) const
//----------------------------------------------------------------------------------------------------------------------
{
	// Get scene player
	CScenePlayer&	scenePlayer = mInternals->mScenePlayers[sceneIndex];

	// Load and start
	scenePlayer.load(mInternals->mGPU);
	scenePlayer.start();

	return scenePlayer;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppPlayer::setCurrent(CSceneTransitionPlayer* sceneTransitionPlayer, CScene::Index sceneIndex)
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
const CSceneTransitionPlayer::Procs& CSceneAppPlayer::getSceneTransitionPlayerProcs() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mSceneTransitionPlayerProcs;
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
CByteParceller sCreateByteParceller(const CString& resourceFilename, void* userData)
{
	// Setup
	CSceneAppPlayerInternals*	sceneAppPlayerInternals = (CSceneAppPlayerInternals*) userData;

	return sceneAppPlayerInternals->mSceneAppPlayerProcs.createByteParceller(resourceFilename);
}
