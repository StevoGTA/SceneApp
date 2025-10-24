//----------------------------------------------------------------------------------------------------------------------
//	CSceneAppPlayer.cpp			©2019 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CSceneAppPlayer.h"

#include "CBinaryPropertyList.h"
#include "CBits.h"
#include "CLogServices.h"
#include "CPreferences.h"
#include "CSceneAppMediaEngine.h"
#include "CSceneItemPlayerAnimation.h"
#include "CSceneItemPlayerButton.h"
#include "CSceneItemPlayerButtonArray.h"
#include "CSceneItemPlayerHotspot.h"
#include "CSceneItemPlayerText.h"
#include "CSceneItemPlayerVideo.h"
#include "CScenePackage.h"
#include "CScenePlayer.h"
#include "CSceneTransitionPlayerCoverUncover.h"
#include "CSceneTransitionPlayerPush.h"
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
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneAppPlayer::Internals

class CSceneAppPlayer::Internals {
	public:
									Internals(CSceneAppPlayer& sceneAppPlayer, CGPU& gpu,
												const CSceneAppPlayer::Procs& procs,
												const SSceneAppResourceLoading& sceneAppResourceLoading);

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
		static	S2DSizeF32			scenePlayerGetViewportSize(Internals* internals);
		static	I<CSceneItemPlayer>	scenePlayerCreateSceneItemPlayer(CSceneItem& sceneItem,
											const SSceneAppResourceManagementInfo& sceneAppResourceManagementInfo,
											const CSceneItemPlayer::Procs& procs, Internals* internals);
		static	void				scenePlayerActionsPerform(const CActions& actions, const S2DPointF32& point,
											Internals* internals);

		CSRSWMessageQueues									mMessageQueues;
		CSceneAppPlayer::Options							mOptions;

		CGPU&												mGPU;
		CGPUTextureManager									mGPUTextureManager;
		CSceneAppMediaEngine								mSceneAppMediaEngine;
		CSceneAppPlayer&									mSceneAppPlayer;

		CSceneAppPlayer::Procs								mSceneAppPlayerProcs;
		SSceneAppResourceLoading							mSceneAppResourceLoading;
		CScenePlayer::Procs									mScenePlayerProcs;
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
		UniversalTimeInterval								mLastPeriodicOutputTimeInterval;
};

//----------------------------------------------------------------------------------------------------------------------
CSceneAppPlayer::Internals::Internals(CSceneAppPlayer& sceneAppPlayer, CGPU& gpu, const CSceneAppPlayer::Procs& procs,
		const SSceneAppResourceLoading& sceneAppResourceLoading) :
	mOptions(CSceneAppPlayer::kOptionsDefault), mGPU(gpu), mGPUTextureManager(gpu),
			mSceneAppMediaEngine(mMessageQueues, sceneAppResourceLoading),
			mSceneAppPlayer(sceneAppPlayer), mSceneAppPlayerProcs(procs),
			mSceneAppResourceLoading(sceneAppResourceLoading),
			mScenePlayerProcs((CScenePlayer::Procs::GetViewportSizeProc) scenePlayerGetViewportSize,
					(CScenePlayer::Procs::CreateSceneItemPlayerProc) scenePlayerCreateSceneItemPlayer,
					(CScenePlayer::Procs::PerformActionsProc) scenePlayerActionsPerform, this),
			mSceneTransitionPlayerProcs((CSceneTransitionPlayer::Procs::GetViewportSizeProc) scenePlayerGetViewportSize,
					this),
			mStopTime(0.0), mLastPeriodicOutputTimeInterval(0.0)
//----------------------------------------------------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppPlayer::Internals::handleTouchesBegan()
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
		STouchHandlerInfo	touchHandlerInfo(touchBeganInfo.getReference());
		if (!mCurrentSceneTransitionPlayer.hasInstance()) {
			// Pass to scene player
			mSceneTouchHandlerInfos += touchHandlerInfo;

			mCurrentScenePlayer->touchBeganOrMouseDownAtPoint(touchBeganInfo.getViewportPoint(),
					touchBeganInfo.getTapCount(), touchBeganInfo.getReference());

			// Check if transmogrophied into a transition
			if ((mSceneTouchHandlerInfos.getCount() == 0) && mCurrentSceneTransitionPlayer.hasInstance())
				// Touch began transmogrophied into a transition
				mSceneTransitionTouchHandlerInfos += touchHandlerInfo;
		} else {
			// Pass to scene transition player
			mSceneTransitionTouchHandlerInfos += touchHandlerInfo;

			mCurrentSceneTransitionPlayer->touchBeganOrMouseDownAtPoint(touchBeganInfo.getViewportPoint(),
					touchBeganInfo.getTapCount(), touchBeganInfo.getReference());
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppPlayer::Internals::handleTouchesMoved()
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
													touchMovedInfo.getReference());
			if (touchHandlerInfo.hasReference())
				// Pass to scene player
				mCurrentScenePlayer->touchOrMouseMovedFromPoint(touchMovedInfo.getViewportPreviousPoint(),
						touchMovedInfo.getViewportCurrentPoint(), touchHandlerInfo->mReference);
		} else {
			// Look for active touch handler info
			OR<STouchHandlerInfo>	touchHandlerInfo =
											mSceneTransitionTouchHandlerInfos.getFirst(compareReference,
													touchMovedInfo.getReference());
			if (touchHandlerInfo.hasReference())
				// Pass to scene transition player
				mCurrentSceneTransitionPlayer->touchOrMouseMovedFromPoint(touchMovedInfo.getViewportPreviousPoint(),
						touchMovedInfo.getViewportCurrentPoint(),touchHandlerInfo->mReference);
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppPlayer::Internals::handleTouchesEnded()
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
													touchEndedInfo.getReference());
			if (touchHandlerInfoValue.hasValue()) {
				// Pass to scene player
				STouchHandlerInfo	touchHandlerInfo = *touchHandlerInfoValue;
				mCurrentScenePlayer->touchEndedOrMouseUpAtPoint(touchEndedInfo.getViewportPoint(),
						touchHandlerInfo.mReference);
			}
		} else {
			// Look for active touch handler info
			OV<STouchHandlerInfo>	touchHandlerInfoValue =
											mSceneTransitionTouchHandlerInfos.popFirst(compareReference,
															touchEndedInfo.getReference());
			if (touchHandlerInfoValue.hasValue()) {
				// Pass to scene transition player
				STouchHandlerInfo	touchHandlerInfo = *touchHandlerInfoValue;
				mCurrentSceneTransitionPlayer->touchEndedOrMouseUpAtPoint(touchEndedInfo.getViewportPoint(),
						touchHandlerInfo.mReference);
			}
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppPlayer::Internals::handleTouchesCancelled()
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
													touchCancelledInfo.getReference());
			if (touchHandlerInfoValue.hasValue()) {
				// Pass to scene player
				STouchHandlerInfo	touchHandlerInfo = *touchHandlerInfoValue;
				mCurrentScenePlayer->touchOrMouseCancelled(touchHandlerInfo.mReference);
			}
		} else {
			// Look for active touch handler info
			OV<STouchHandlerInfo>	touchHandlerInfoValue =
											mSceneTransitionTouchHandlerInfos.popFirst(compareReference,
													touchCancelledInfo.getReference());
			if (touchHandlerInfoValue.hasValue()) {
				// Pass to scene transition player
				STouchHandlerInfo	touchHandlerInfo = *touchHandlerInfoValue;
				mCurrentSceneTransitionPlayer->touchesOrMouseCancelled(touchHandlerInfo.mReference);
			}
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppPlayer::Internals::cancelAllSceneTouches()
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
void CSceneAppPlayer::Internals::cancelAllSceneTransitionTouches()
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
OR<CScenePlayer> CSceneAppPlayer::Internals::getScenePlayer(const CString& sceneName)
//----------------------------------------------------------------------------------------------------------------------
{
	// Iterate all scene players
	for (TIteratorD<CScenePlayer> iterator = mScenePlayers.getIterator(); iterator.hasValue(); iterator.advance()) {
		// Check if the target scene player
		if (iterator->getScene().getName() == sceneName)
			// Found
			return OR<CScenePlayer>(*iterator);
	}

	return OR<CScenePlayer>();
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppPlayer::Internals::setCurrentSceneIndex(CScene::Index sceneIndex)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	CScenePlayer&	scenePlayer = mScenePlayers[sceneIndex];

	CBits	performLoad(mScenePlayers.getCount());
	for (CArray::ItemIndex i = 0; i < mScenePlayers.getCount(); i++)
		// Setup
		performLoad.set((UInt32) i, i == (CArray::ItemIndex) sceneIndex);

	// Iterate all actions for this scene player
	CActions	actions = scenePlayer.getAllActions();
	for (TIteratorD<CAction> iterator = actions.getIterator(); iterator.hasValue(); iterator.advance()) {
		// Setup
		const	CAction&	action = *iterator;

		// Query linked scene index
		OV<CScene::Index>	testSceneIndex = mSceneAppPlayer.getSceneIndex(action);
		if (!testSceneIndex.hasValue())
			// No linked scene index
			continue;

		// Get linked scene player
		if (!(action.getOptions() & CAction::kOptionsDontPreload))
			// Preload
			performLoad.set(*testSceneIndex);
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
	mCurrentScenePlayer->start(mGPU);

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
void CSceneAppPlayer::Internals::setCurrent(CSceneTransitionPlayer* sceneTransitionPlayer, CScene::Index sceneIndex)
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
S2DSizeF32 CSceneAppPlayer::Internals::scenePlayerGetViewportSize(Internals* internals)
//----------------------------------------------------------------------------------------------------------------------
{
	return internals->mScenePackage.getSize();
}

//----------------------------------------------------------------------------------------------------------------------
I<CSceneItemPlayer> CSceneAppPlayer::Internals::scenePlayerCreateSceneItemPlayer(CSceneItem& sceneItem,
		const SSceneAppResourceManagementInfo& sceneAppResourceManagementInfo,
		const CSceneItemPlayer::Procs& procs, Internals* internals)
//----------------------------------------------------------------------------------------------------------------------
{
	return internals->mSceneAppPlayer.createSceneItemPlayer(sceneItem, sceneAppResourceManagementInfo, procs);
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppPlayer::Internals::scenePlayerActionsPerform(const CActions& actions, const S2DPointF32& point,
		Internals* internals)
//----------------------------------------------------------------------------------------------------------------------
{
	// Iterate all actions
	for (TIteratorD<CAction> iterator = actions.getIterator(); iterator.hasValue(); iterator.advance()) {
		// Setup
		const	CAction&		action = *iterator;
		const	CString&		actionName = action.getName();
		const	CDictionary&	actionInfo = action.getActionInfo();

		// Check action name
		if (actionName == CAction::mNameOpenURL)
			// Open URL
			internals->mSceneAppPlayerProcs.openURL(CURL(actionInfo.getString(CAction::mInfoURLKey)),
					actionInfo.contains(CAction::mInfoUseWebViewKey));
		else if (actionName == CAction::mPauseBackgroundAudio) {
			// Pause background audio
			if (internals->mCurrentScenePlayerBackground1MediaPlayer.hasInstance())
				// Pause
				internals->mCurrentScenePlayerBackground1MediaPlayer->pause();
			if (internals->mCurrentScenePlayerBackground2MediaPlayer.hasInstance())
				// Pause
				internals->mCurrentScenePlayerBackground2MediaPlayer->pause();
		} else if (actionName == CAction::mResumeBackgroundAudio) {
			// Resume background audio
			if (internals->mCurrentScenePlayerBackground1MediaPlayer.hasInstance())
				// Resume
				internals->mCurrentScenePlayerBackground1MediaPlayer->play();
			if (internals->mCurrentScenePlayerBackground2MediaPlayer.hasInstance())
				// Resume
				internals->mCurrentScenePlayerBackground2MediaPlayer->play();
		} else if (actionName == CAction::mNameSceneCover) {
			// Cover to scene
			CScene::Index	sceneIndex = *internals->mSceneAppPlayer.getSceneIndex(action);
			CScenePlayer&	scenePlayer = internals->mSceneAppPlayer.loadAndStartScenePlayer(sceneIndex);

			// Create transition
			internals->setCurrent(
					new CSceneTransitionPlayerCoverUncover(*internals->mCurrentScenePlayer, scenePlayer, actionInfo,
							point, true, internals->mSceneTransitionPlayerProcs), sceneIndex);
		} else if (actionName == CAction::mNameSceneCut)
			// Cut to scene
			internals->setCurrentSceneIndex(*internals->mSceneAppPlayer.getSceneIndex(action));
		else if (actionName == CAction::mNameScenePush) {
			// Push to scene
			CScene::Index	sceneIndex = *internals->mSceneAppPlayer.getSceneIndex(action);
			CScenePlayer&	scenePlayer = internals->mSceneAppPlayer.loadAndStartScenePlayer(sceneIndex);

			// Create transition
			internals->setCurrent(
					new CSceneTransitionPlayerPush(*internals->mCurrentScenePlayer, scenePlayer, actionInfo, point,
							internals->mSceneTransitionPlayerProcs), sceneIndex);
		} else if (actionName == CAction::mNameSceneUncover) {
			// Uncover to scene
			CScene::Index	sceneIndex = *internals->mSceneAppPlayer.getSceneIndex(action);
			CScenePlayer&	scenePlayer = internals->mSceneAppPlayer.loadAndStartScenePlayer(sceneIndex);

			// Create transition
			internals->setCurrent(
					new CSceneTransitionPlayerCoverUncover(*internals->mCurrentScenePlayer, scenePlayer, actionInfo,
							point, false, internals->mSceneTransitionPlayerProcs), sceneIndex);
		} else if (actionName == CAction::mNameSetItemNameValue) {
			// Set Item Name/Value
			const	CString&		sceneName = actionInfo.getString(CAction::mInfoSceneNameKey);
					CScenePlayer&	scenePlayer =
											!sceneName.isEmpty() ?
													*internals->getScenePlayer(sceneName) :
													*internals->mCurrentScenePlayer;
			scenePlayer.setItemPlayerProperty(actionInfo.getString(CAction::mInfoItemNameKey),
					actionInfo.getString(CAction::mInfoPropertyNameKey),
					actionInfo.getValue(CAction::mInfoPropertyValueKey));
		} else if (actionName == CAction::mNameSendItemCommand) {
			// Send Item Command
			const	CString&		sceneName = actionInfo.getString(CAction::mInfoSceneNameKey);
					CScenePlayer&	scenePlayer =
											!sceneName.isEmpty() ?
													*internals->getScenePlayer(sceneName) :
													*internals->mCurrentScenePlayer;
			scenePlayer.handleItemPlayerCommand(internals->mGPU, actionInfo.getString(CAction::mInfoItemNameKey),
					actionInfo.getString(CAction::mInfoCommandKey), actionInfo, point);
		} else if (actionName == CAction::mNameSendAppCommand)
			// Send App Command
			internals->mSceneAppPlayerProcs.handleCommand(actionInfo.getString(CAction::mInfoCommandKey), actionInfo);
		else
			// Other
			internals->mSceneAppPlayer.performAction(action, point);
	}
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneAppPlayer

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
CSceneAppPlayer::CSceneAppPlayer(CGPU& gpu, const Procs& procs, const SSceneAppResourceLoading& sceneAppResourceLoading)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	mInternals = new Internals(*this, gpu, procs, sceneAppResourceLoading);
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
	CLogServices::logMessage(CString(OSSTR("Loading scenes from ")) + scenePackageInfo.getFilename());
#endif

	CDictionary	info =
						*CBinaryPropertyList::dictionaryFrom(
								mInternals->mSceneAppResourceLoading.createRandomAccessDataSource(
										scenePackageInfo.getFilename()));
	mInternals->mOptions = (Options) info.getUInt64(CString(OSSTR("options")), kOptionsDefault);
	mInternals->mScenePackage = CScenePackage(scenePackageInfo.getSize(), info);

	// Create scene players
	mInternals->mScenePlayers.removeAll();

	for (UInt32 i = 0; i < mInternals->mScenePackage.getScenesCount(); i++)
		// Create scene player
		mInternals->mScenePlayers +=
				CScenePlayer(mInternals->mScenePackage.getSceneAtIndex(i),
						SSceneAppResourceManagementInfo(mInternals->mGPUTextureManager,
								mInternals->mSceneAppMediaEngine, mInternals->mSceneAppResourceLoading),
						mInternals->mScenePlayerProcs);

	// Set initial scene
	mInternals->setCurrentSceneIndex(mInternals->mScenePackage.getInitialSceneIndex());
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppPlayer::start(bool loadAllTextures)
//----------------------------------------------------------------------------------------------------------------------
{
	// Start
	mInternals->mCurrentScenePlayer->start(mInternals->mGPU);

	// Update last periodic output time
	if ((mInternals->mLastPeriodicOutputTimeInterval != 0.0) && (mInternals->mStopTime != 0.0))
		// Advance last periodic time to ignore time between stop and start
		mInternals->mLastPeriodicOutputTimeInterval += SUniversalTime::getCurrent() - mInternals->mStopTime;

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
void CSceneAppPlayer::handlePeriodic(UniversalTimeInterval outputTimeInterval)
//----------------------------------------------------------------------------------------------------------------------
{
	// Update time info
	UniversalTimeInterval	deltaTimeInterval;
	if (mInternals->mLastPeriodicOutputTimeInterval == 0.0)
		// First iteration
		deltaTimeInterval = 0.0;
	else
		// Next iteration
		deltaTimeInterval = outputTimeInterval - mInternals->mLastPeriodicOutputTimeInterval;
	mInternals->mLastPeriodicOutputTimeInterval = outputTimeInterval;

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
			mInternals->mCurrentSceneTransitionPlayer->update(mInternals->mGPU, deltaTimeInterval);

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
			mInternals->mCurrentScenePlayer->update(mInternals->mGPU, deltaTimeInterval);
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
	mInternals->mTouchBeganInfos += TouchBeganInfo(mouseDownInfo.getViewportPoint(), mouseDownInfo.getClickCount());
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppPlayer::mouseDragged(const MouseDraggedInfo& mouseDraggedInfo)
//----------------------------------------------------------------------------------------------------------------------
{
	// Map mouse input to touch input
	mInternals->mTouchMovedInfos +=
			TouchMovedInfo(mouseDraggedInfo.getViewportPreviousPoint(), mouseDraggedInfo.getViewportCurrentPoint());
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppPlayer::mouseUp(const MouseUpInfo& mouseUpInfo)
//----------------------------------------------------------------------------------------------------------------------
{
	// Map mouse input to touch input
	mInternals->mTouchEndedInfos += TouchEndedInfo(mouseUpInfo.getViewportPoint());
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
I<CSceneItemPlayer> CSceneAppPlayer::createSceneItemPlayer(CSceneItem& sceneItem,
		const SSceneAppResourceManagementInfo& sceneAppResourceManagementInfo,
		const CSceneItemPlayer::Procs& procs) const
//----------------------------------------------------------------------------------------------------------------------
{
	// Check type
	const	CString&	type = sceneItem.getType();
	if (type == CSceneItemAnimation::mType)
		// Animation
		return I<CSceneItemPlayer>(
				new CSceneItemPlayerAnimation((CSceneItemAnimation&) sceneItem, sceneAppResourceManagementInfo, procs));
	else if (type == CSceneItemButton::mType)
		// Button
		return I<CSceneItemPlayer>(
				new CSceneItemPlayerButton((CSceneItemButton&) sceneItem, sceneAppResourceManagementInfo, procs));
	else if (type == CSceneItemButtonArray::mType)
		// Button array
		return I<CSceneItemPlayer>(
				new CSceneItemPlayerButtonArray((CSceneItemButtonArray&) sceneItem, sceneAppResourceManagementInfo,
						procs));
	else if (type == CSceneItemHotspot::mType)
		// Hotspot
		return I<CSceneItemPlayer>(new CSceneItemPlayerHotspot((CSceneItemHotspot&) sceneItem, procs));
	else if (type == CSceneItemText::mType)
		// Text
		return I<CSceneItemPlayer>(
				new CSceneItemPlayerText((CSceneItemText&) sceneItem, sceneAppResourceManagementInfo, procs));
	else if (type == CSceneItemVideo::mType)
		// Video
		return I<CSceneItemPlayer>(
				new CSceneItemPlayerVideo((CSceneItemVideo&) sceneItem, sceneAppResourceManagementInfo, procs));
	else {
		// No SceneItemPlayer
		CLogServices::logMessage(CString("No player created for ") + sceneItem.getType());
		AssertFailUnimplemented();

		return I<CSceneItemPlayer>(nil);
	}
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
	scenePlayer.start(mInternals->mGPU);

	return scenePlayer;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppPlayer::setCurrent(CSceneTransitionPlayer* sceneTransitionPlayer, CScene::Index sceneIndex)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->setCurrent(sceneTransitionPlayer, sceneIndex);
}

//----------------------------------------------------------------------------------------------------------------------
const SSceneAppResourceLoading& CSceneAppPlayer::getSceneAppResourceLoading() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mSceneAppResourceLoading;
}

//----------------------------------------------------------------------------------------------------------------------
CGPUTextureManager& CSceneAppPlayer::getGPUTextureManager() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mGPUTextureManager;
}

//----------------------------------------------------------------------------------------------------------------------
CSceneAppMediaEngine& CSceneAppPlayer::getSceneAppMediaEngine() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mSceneAppMediaEngine;
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
