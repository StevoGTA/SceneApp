//
//  CSceneAppPlayer.cpp
//  StevoBrock-Core
//
//  Created by Stevo on 10/4/19.
//

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
#include "CSceneTransitionPlayer.h"

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
// MARK: - CSceneAppPlayerInternals

class CSceneAppPlayerInternals {
	public:
										CSceneAppPlayerInternals(CGPU& gpu,
												const SSceneAppPlayerProcsInfo& sceneAppPlayerProcsInfo);

				void					handleTouchesBegan();
				void					handleTouchesMoved();
				void					handleTouchesEnded();
				void					handleTouchesCancelled();
				void					cancelAllSceneTouches();
				void					cancelAllSceneTransitionTouches();

				OV<SceneIndex>			getSceneIndex(const CString& actionName, const CDictionary& actioninfo);
				void					setCurrent(CScenePlayer& scenePlayer, SceneIndex sceneIndex);

		static	bool					compareReference(const STouchHandlerInfo& touchHandlerInfo, void* reference)
											{ return touchHandlerInfo.mReference == reference; }
		static	S2DSize32				scenePlayerGetViewportSizeProc(void* userData);
		static	OV<CSceneItemPlayer*>	scenePlayerCreateSceneItemPlayerProc(const CSceneItem& sceneItem,
												const SSceneAppResourceManagementInfo& sceneAppResourceManagementInfo,
												const SSceneItemPlayerProcsInfo& sceneItemPlayerProcsInfo,
												void* userData);
		static	void					scenePlayerActionArrayPerformProc(const CActionArray& actionArray,
												const S2DPoint32& point, void* userData);
//		static	CScenePlayer*			scenePlayer(CArrayItemRef item)
//											{ return (CScenePlayer*) item; }

		ESceneAppPlayerOptions								mOptions;

		CGPU&												mGPU;
		CGPUTextureManager									mGPUTextureManager;

		SSceneAppPlayerProcsInfo							mSceneAppPlayerProcsInfo;
		SSceneAppResourceManagementInfo						mSceneAppResourceManagementInfo;
		SScenePlayerProcsInfo								mScenePlayerProcsInfo;

		CScenePackage										mScenePackage;

		TArray<CScenePlayer*>								mScenePlayers;
		CScenePlayer*										mCurrentScenePlayer;
		OR<CSceneTransitionPlayer>							mCurrentSceneTransitionPlayer;

		TLockingArray<SSceneAppPlayerTouchBeganInfo>		mTouchBeganInfos;
		TLockingArray<SSceneAppPlayerTouchMovedInfo>		mTouchMovedInfos;
		TLockingArray<SSceneAppPlayerTouchEndedInfo>		mTouchEndedInfos;
		TLockingArray<SSceneAppPlayerTouchCancelledInfo>	mTouchCancelledInfos;
		TArray<STouchHandlerInfo>							mSceneTouchHandlerInfos;
		TArray<STouchHandlerInfo>							mSceneTransitionTouchHandlerInfos;

		UniversalTime										mStopTime;
		UniversalTime										mLastPeriodicOutputTime;
};

//----------------------------------------------------------------------------------------------------------------------
CSceneAppPlayerInternals::CSceneAppPlayerInternals(CGPU& gpu, const SSceneAppPlayerProcsInfo& sceneAppPlayerProcsInfo) :
	mOptions(kSceneAppPlayerOptionsDefault), mGPU(gpu), mGPUTextureManager(gpu),
			mSceneAppPlayerProcsInfo(sceneAppPlayerProcsInfo),
			mSceneAppResourceManagementInfo(mSceneAppPlayerProcsInfo.mCreateByteParcellerProc, mGPUTextureManager),
			mScenePlayerProcsInfo(scenePlayerGetViewportSizeProc, scenePlayerCreateSceneItemPlayerProc,
					scenePlayerActionArrayPerformProc, this),
			mCurrentScenePlayer(nil), mStopTime(0.0), mLastPeriodicOutputTime(0.0)
//----------------------------------------------------------------------------------------------------------------------
{}

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppPlayerInternals::handleTouchesBegan()
//----------------------------------------------------------------------------------------------------------------------
{
	// Check for info
	if (mTouchBeganInfos.isEmpty())
		return;

	// Collect infos to handle
	TArray<SSceneAppPlayerTouchBeganInfo>	touchBeganInfos;
	while (!mTouchBeganInfos.isEmpty()) {
		// Get the next info
		SSceneAppPlayerTouchBeganInfo	touchBeganInfo = mTouchBeganInfos.popFirst();

		// Check if handling multitouch
		if (
				(mCurrentSceneTransitionPlayer.hasReference() &&
						!mCurrentSceneTransitionPlayer->supportsMultitouch()) ||
				((mOptions & kSceneAppPlayerOptionsTouchHandlingMask) == kSceneAppPlayerOptionsLastSingleTouchOnly))
			// Last single touch only
			touchBeganInfos.removeAll();

		// Add
		touchBeganInfos += touchBeganInfo;
	}

	// Check if need to cancel current touches
	if (mCurrentSceneTransitionPlayer.hasReference() ||
			((mOptions & kSceneAppPlayerOptionsTouchHandlingMask) == kSceneAppPlayerOptionsLastSingleTouchOnly))
		// Cancel current scene touches
		cancelAllSceneTouches();

	// Handle
	while (!touchBeganInfos.isEmpty()) {
		// Pop first info
		SSceneAppPlayerTouchBeganInfo	touchBeganInfo = touchBeganInfos.popFirst();

		// Check for scene transition player
		if (!mCurrentSceneTransitionPlayer.hasReference()) {
			// Pass to scene player
			STouchHandlerInfo	touchHandlerInfo(touchBeganInfo.mReference);
			mSceneTouchHandlerInfos += touchHandlerInfo;

			mCurrentScenePlayer->touchBeganOrMouseDownAtPoint(touchBeganInfo.mViewportPoint, touchBeganInfo.mTapCount,
					touchBeganInfo.mReference);
		} else {
			// Pass to scene transition player
			STouchHandlerInfo	touchHandlerInfo(touchBeganInfo.mReference);
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
		if (!mCurrentSceneTransitionPlayer.hasReference()) {
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
		if (!mCurrentSceneTransitionPlayer.hasReference()) {
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
		if (!mCurrentSceneTransitionPlayer.hasReference()) {
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
	if (!mCurrentSceneTransitionPlayer.hasReference())
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
OV<SceneIndex> CSceneAppPlayerInternals::getSceneIndex(const CString& actionName, const CDictionary& actionInfo)
//----------------------------------------------------------------------------------------------------------------------
{
	// Check action name
	if ((actionName != CAction::mNameSceneCover) &&
			(actionName != CAction::mNameSceneCut) &&
			(actionName != CAction::mNameScenePush) &&
			(actionName != CAction::mNameSceneUncover))
		// Does not have a scene index
		return OV<SceneIndex>();

	// Try to find scene with matching name
	const	CString&	sceneName = actionInfo.getString(CAction::mInfoSceneNameKey);
	if (!sceneName.isEmpty()) {
		// Iterate all scenes to find scene with matching name
		for (UInt32 sceneIndex = 0; sceneIndex < mScenePackage.getScenesCount(); sceneIndex++) {
			// Check this scene
			if (mScenePackage.getSceneAtIndex(sceneIndex).getName() == sceneName)
				// Found match
				return OV<SceneIndex>(sceneIndex);
		}
	}

	// Try to decode and retrieve scene index
	const	CString&	loadSceneIndexFrom = actionInfo.getString(CAction::mInfoLoadSceneIndexFromKey);
	if (!loadSceneIndexFrom.isEmpty()) {
		// Retrieve from prefs
		SSInt32Pref	pref(loadSceneIndexFrom.getCString(), 0);

		return CPreferences::hasValue(pref) ?OV<SceneIndex>(CPreferences::getSInt32(pref)) : OV<SceneIndex>();
	}

	// Use scene index
	return OV<SceneIndex>(actionInfo.getUInt32(CAction::mInfoSceneIndexKey));
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppPlayerInternals::setCurrent(CScenePlayer& scenePlayer, SceneIndex sceneIndex)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	TPtrArray<CScenePlayer*>	toLoadSceneItemPlayers;

//	TPtrArray<CScenePlayer*>	toUnloadSceneItemPlayers(mScenePlayers, scenePlayer);
//	toUnloadSceneItemPlayers -= &scenePlayer;
	TPtrArray<CScenePlayer*>	toUnloadSceneItemPlayers;
	for (TIteratorD<CScenePlayer*> iterator = mScenePlayers.getIterator(); iterator.hasValue(); iterator.advance()) {
		//
		if (iterator.getValue() != &scenePlayer)
			//
			toUnloadSceneItemPlayers += iterator.getValue();
	}

	// Iterate all actions for this scene player
	CActionArray	actionArray = scenePlayer.getAllActions();
	for (TIteratorS<CAction*> iterator = actionArray.getIterator(); iterator.hasValue(); iterator.advance()) {
		// Setup
		const	CAction&	action = *iterator.getValue();

		// Query linked scene index
		OV<SceneIndex>	sceneIndex = getSceneIndex(action.getName(), action.getInfo());
		if (!sceneIndex.hasValue())
			// No linked scene index
			continue;

		// Get linked scene player
		CScenePlayer&	linkedScenePlayer = *mScenePlayers[*sceneIndex];
		if (!(action.getOptions() & kActionOptionsDontPreload)) {
			// Preload
			toLoadSceneItemPlayers += &linkedScenePlayer;
			toUnloadSceneItemPlayers -= &linkedScenePlayer;
		}
	}

	// Load new scene
	scenePlayer.load();

	// Unload scenes no longer linked
//	for (TIteratorS<CScenePlayer*> iterator = toUnloadSceneItemPlayers.getIterator(); iterator.hasValue();
//			iterator.advance())
//		// Unload
//		iterator.getValue()->unload();

	// Load linked scenes
	for (TIteratorS<CScenePlayer*> iterator = toLoadSceneItemPlayers.getIterator(); iterator.hasValue();
			iterator.advance())
		// Load
		iterator.getValue()->load();

	// Reset current scene player
//	mCurrentScenePlayer->reset();

	// Update
	mCurrentScenePlayer = &scenePlayer;

	const	CScene&	scene = mCurrentScenePlayer->getScene();
	if (!scene.getStoreSceneIndexAsString().isEmpty()) {
		// Yes
		SSInt32Pref	pref(scene.getStoreSceneIndexAsString().getCString(), 0);
		CPreferences::set(pref, sceneIndex);
	}

	cancelAllSceneTouches();
	cancelAllSceneTransitionTouches();

	// Start
	mCurrentScenePlayer->start();
}

//----------------------------------------------------------------------------------------------------------------------
S2DSize32 CSceneAppPlayerInternals::scenePlayerGetViewportSizeProc(void* userData)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	CSceneAppPlayerInternals*	internals = (CSceneAppPlayerInternals*) userData;

	return internals->mSceneAppPlayerProcsInfo.mGetViewportSizeProc(
			internals->mSceneAppPlayerProcsInfo.mUserData);
}

//----------------------------------------------------------------------------------------------------------------------
OV<CSceneItemPlayer*> CSceneAppPlayerInternals::scenePlayerCreateSceneItemPlayerProc(const CSceneItem& sceneItem,
		const SSceneAppResourceManagementInfo& sceneAppResourceManagementInfo,
		const SSceneItemPlayerProcsInfo& sceneItemPlayerProcsInfo, void* userData)
//----------------------------------------------------------------------------------------------------------------------
{
//	// Setup
//	CSceneAppPlayerInternals&	internals = *((CSceneAppPlayerInternals*) userData);
//
	// Check type
	const	CString&	type = sceneItem.getType();
	if (type == CSceneItemAnimation::mType)
		// Animation
		return OV<CSceneItemPlayer*>(
				new CSceneItemPlayerAnimation((const CSceneItemAnimation&) sceneItem, sceneAppResourceManagementInfo,
						sceneItemPlayerProcsInfo));
	else if (type == CSceneItemButton::mType)
		// Button
		return OV<CSceneItemPlayer*>(
				new CSceneItemPlayerButton((const CSceneItemButton&) sceneItem, sceneAppResourceManagementInfo,
						sceneItemPlayerProcsInfo));
	else if (type == CSceneItemButtonArray::mType)
		// Button array
		return OV<CSceneItemPlayer*>(
				new CSceneItemPlayerButtonArray((const CSceneItemButtonArray&) sceneItem,
						sceneAppResourceManagementInfo, sceneItemPlayerProcsInfo));
	else if (type == CSceneItemHotspot::mType)
		// Hotspot
		return OV<CSceneItemPlayer*>(
				new CSceneItemPlayerHotspot((const CSceneItemHotspot&) sceneItem,sceneAppResourceManagementInfo,
						sceneItemPlayerProcsInfo));
	else
		// Unknown
		return OV<CSceneItemPlayer*>();
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppPlayerInternals::scenePlayerActionArrayPerformProc(const CActionArray& actionArray,
	const S2DPoint32& point, void* userData)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	CSceneAppPlayerInternals&	internals = *((CSceneAppPlayerInternals*) userData);

	// Iterate all actions
	for (TIteratorS<CAction*> iterator = actionArray.getIterator(); iterator.hasValue(); iterator.advance()) {
		// Setup
		const	CAction&		action = *iterator.getValue();
		const	CString&		actionName = action.getName();
		const	CDictionary&	actionInfo = action.getActionInfo();

		// Check action name
		if (actionName == CAction::mNameOpenURL) {
			// Open URL
			if (internals.mSceneAppPlayerProcsInfo.mOpenURLProc != nil) {
				// Inform
				CURL	url(actionInfo.getString(CAction::mInfoURLKey));
				internals.mSceneAppPlayerProcsInfo.mOpenURLProc(url, actionInfo.contains(CAction::mInfoUseWebView),
						internals.mSceneAppPlayerProcsInfo.mUserData);
			}
		} else if (actionName == CAction::mNamePlayAudio) {
			// Play Audio
//			DisposeOf(internals.mAudioOutputTrackReference);
//
//			internals.mAudioOutputTrackReference =
//					CAudioOutputTrackCache::newAudioOutputTrackReferenceFromURL(
//							eGetURLForResourceFilename(actionInfo.getString(mInfoAudioFilenameKey)));
//			internals.mAudioOutputTrackReference->play();
		} else if (actionName == CAction::mNamePlayMovie) {
			// Play movie
//			CString	movieFilename = actionInfo.getString(CAction::mInfoMovieFilenameKey);
//			CURL	movieURL = eGetURLForResourceFilename(movieFilename);
//			internals.mSceneAppMoviePlayer = new CSceneAppMoviePlayer(movieURL);
//			internals.mSceneAppMoviePlayer->setControlMode(
//					(ESceneAppMoviePlayerControlMode) actionInfo.getUInt32(mInfoControlModeKey,
//							kSceneAppMoviePlayerControlModeDefault));
//			internals.mSceneAppMoviePlayer->play();
		} else if (actionName == CAction::mNameSceneCover) {
			// Cover to scene
//			SceneIndex	sceneIndex = getSceneIndexForAction(internals, actionName, actionInfo).getValue();
//			CScene&		scene = internals.mScenes->getSceneAtIndex(sceneIndex);
//			for (i = 0; i < internals.mScenePlayersArray.getCount(); i++) {
//				CScenePlayer*	scenePlayer = internals.mScenePlayersArray[i];
//				if (&scenePlayer->getScene() == &scene) {
//					// Load scene (if not already loaded)
//					scenePlayer->load();
//
//					// Create transition
//					setCurrentTransition(new CSceneTransitionPlayerSlide(*internals.mCurrentScenePlayer, *scenePlayer,
//							actionInfo, point));
//
//					break;
//				}
//			}
		} else if (actionName == CAction::mNameSceneCut) {
			// Cut to scene
			SceneIndex	sceneIndex = internals.getSceneIndex(actionName, actionInfo).getValue();
			internals.setCurrent(*internals.mScenePlayers[sceneIndex], sceneIndex);
		} else if (actionName == CAction::mNameScenePush) {
			// Push to scene
//			SceneIndex	sceneIndex = getSceneIndexForAction(internals, actionName, actionInfo).getValue();
//			CScene&		scene = internals.mScenes->getSceneAtIndex(sceneIndex);
//			for (i = 0; i < internals.mScenePlayersArray.getCount(); i++) {
//				CScenePlayer*	scenePlayer = internals.mScenePlayersArray[i];
//				if (&scenePlayer->getScene() == &scene) {
//					// Load scene (if not already loaded)
//					scenePlayer->load();
//
//					// Create transition
//					setCurrentTransition(new CSceneTransitionPlayerPush(*internals.mCurrentScenePlayer, *scenePlayer,
//							actionInfo, point));
//
//					break;
//				}
//			}
		} else if (actionName == CAction::mNameSceneUncover) {
			// Uncover to scene
//			SceneIndex	sceneIndex = getSceneIndexForAction(internals, actionName, actionInfo).getValue();
//			CScene&		scene = internals.mScenes->getSceneAtIndex(sceneIndex);
//			for (i = 0; i < internals.mScenePlayersArray.getCount(); i++) {
//				CScenePlayer*	scenePlayer = internals.mScenePlayersArray[i];
//				if (&scenePlayer->getScene() == &scene) {
//					// Load scene (if not already loaded)
//					scenePlayer->load();
//
//					// Create transition
//					setCurrentTransition(new CSceneTransitionPlayerSlide(*internals.mCurrentScenePlayer, *scenePlayer,
//							actionInfo, point));
//
//					break;
//				}
//			}
		} else if (actionName == CAction::mNameSetItemNameValue) {
			// Set Item Name/Value
			internals.mCurrentScenePlayer->setItemPlayerProperty(actionInfo.getString(CAction::mInfoItemNameKey),
					actionInfo.getString(CAction::mInfoPropertyNameKey),
					actionInfo.getValue(CAction::mInfoPropertyValueKey));
		} else if (actionName == CAction::mNameSendItemCommand) {
			// Send Item Command
			internals.mCurrentScenePlayer->handleItemPlayerCommand(actionInfo.getString(CAction::mInfoItemNameKey),
					actionInfo.getString(CAction::mInfoCommandKey), actionInfo, point);
		} else if (actionName == CAction::mNameSendAppCommand) {
			// Send App Command
			if (internals.mSceneAppPlayerProcsInfo.mHandleCommandProc != nil) {
				// Inform
				internals.mSceneAppPlayerProcsInfo.mHandleCommandProc(actionInfo.getString(CAction::mInfoCommandKey),
						actionInfo, internals.mSceneAppPlayerProcsInfo.mUserData);
			}
		}
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
	mInternals = new CSceneAppPlayerInternals(gpu, sceneAppPlayerProcsInfo);
}

//----------------------------------------------------------------------------------------------------------------------
CSceneAppPlayer::~CSceneAppPlayer()
//----------------------------------------------------------------------------------------------------------------------
{
	DisposeOf(mInternals);
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
	CLogServices::logMessage(CString("Loading scenes from ") + scenePackageInfo.mFilename);
#endif

	UError	error;
	CDictionary	info =
						CBinaryPropertyList::dictionaryFrom(
								mInternals->mSceneAppPlayerProcsInfo.mCreateByteParcellerProc(
										scenePackageInfo.mFilename),
								error);
	ReturnIfError(error);

	mInternals->mOptions = (ESceneAppPlayerOptions) info.getUInt64(CString("options"), kSceneAppPlayerOptionsDefault);
	mInternals->mScenePackage = CScenePackage(info);

//	// Setup viewport parameters
//	switch (mInternals->mScenes->getViewportZoom()) {
//		case kSceneAppViewportZoomCenter: {
//			// Center scenes in viewport
//			mInternals->mViewportOffset =
//					S2DPoint32((viewportPixelSize.mWidth - eSceneAppViewportPixelSize.mWidth) * 0.5,
//							(viewportPixelSize.mHeight - eSceneAppViewportPixelSize.mHeight) * 0.5);
//			mInternals->mViewportScale = S2DPoint32(1.0, 1.0);
//			} break;
//
//		case kSceneAppViewportZoomScale:
//			// Scale scenes to fill viewport completely.
//			mInternals->mViewportOffset = S2DPoint32::mZero;
//			mInternals->mViewportScale =
//					S2DPoint32(viewportPixelSize.mWidth / eSceneAppViewportPixelSize.mWidth,
//							viewportPixelSize.mHeight / eSceneAppViewportPixelSize.mHeight);
//			break;
//
//		case kSceneAppViewportZoomScaleAspectFit: {
//			// Scale scenes to be as large as possible and still fit all content
//			Float32	scaleX = viewportPixelSize.mWidth / eSceneAppViewportPixelSize.mWidth;
//			Float32	scaleY = viewportPixelSize.mHeight / eSceneAppViewportPixelSize.mHeight;
//			if (scaleX < scaleY) {
//				// Fill width, offset height
//				mInternals->mViewportOffset.mY =
//						(viewportPixelSize.mHeight - eSceneAppViewportPixelSize.mHeight * scaleX) * 0.5;
//				mInternals->mViewportScale = S2DPoint32(scaleX, scaleX);
//			} else {
//				// Fill height, offset width
//				mInternals->mViewportOffset.mX =
//						(viewportPixelSize.mWidth - eSceneAppViewportPixelSize.mWidth * scaleY) * 0.5;
//				mInternals->mViewportScale = S2DPoint32(scaleY, scaleY);
//			}
//			} break;
//
//		case kSceneAppViewportZoomScaleAspectFill: {
//			// Scale scenes to be as small as possible and fill every pixel
//			Float32	scaleX = viewportPixelSize.mWidth / eSceneAppViewportPixelSize.mWidth;
//			Float32	scaleY = viewportPixelSize.mHeight / eSceneAppViewportPixelSize.mHeight;
//			if (scaleX < scaleY) {
//				// Extend and offset width, fill height
//				mInternals->mViewportOffset.mX =
//						(viewportPixelSize.mWidth - eSceneAppViewportPixelSize.mWidth * scaleY) * 0.5;
//				mInternals->mViewportScale = S2DPoint32(scaleY, scaleY);
//			} else {
//				// Extend and offset height, fill width
//				mInternals->mViewportOffset.mY =
//						(viewportPixelSize.mHeight - eSceneAppViewportPixelSize.mHeight * scaleX) * 0.5;
//				mInternals->mViewportScale = S2DPoint32(scaleX, scaleX);
//			}
//			} break;
//	}

	// Create scene players
	mInternals->mScenePlayers.removeAll();

	CScene&	initialScene = mInternals->mScenePackage.getInitialScene();
	for (UInt32 i = 0; i < mInternals->mScenePackage.getScenesCount(); i++) {
		// Create scene player
		CScene&			scene = mInternals->mScenePackage.getSceneAtIndex(i);
		CScenePlayer*	scenePlayer =
								new CScenePlayer(scene, mInternals->mSceneAppResourceManagementInfo,
										mInternals->mScenePlayerProcsInfo);
		mInternals->mScenePlayers += scenePlayer;

		// Handle initial scene
		if (scene == initialScene) {
			// Is initial scene
			mInternals->mCurrentScenePlayer = scenePlayer;
			mInternals->mCurrentScenePlayer->load();
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppPlayer::start()
//----------------------------------------------------------------------------------------------------------------------
{
#if defined(DEBUG)
	CLogServices::logMessage(CString("CSceneAppPlayer - start"));
#endif

//	mInternals->mAreUpdatesDisabled = false;

	mInternals->mCurrentScenePlayer->start();
//	sUpdateLinkedScenes(*mInternals);

	if ((mInternals->mLastPeriodicOutputTime != 0.0) && (mInternals->mStopTime != 0.0))
		// Advance last periodic time to ignore time between stop and start
		mInternals->mLastPeriodicOutputTime += CTimeInfo::getCurrentUniversalTime() - mInternals->mStopTime;
//	updateAndDraw();

	mInternals->mSceneAppPlayerProcsInfo.mInstallPeriodicProc(mInternals->mSceneAppPlayerProcsInfo.mUserData);
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppPlayer::stop()
//----------------------------------------------------------------------------------------------------------------------
{
#if defined(DEBUG)
	CLogServices::logMessage(CString("CSceneAppPlayer - stop"));
#endif

	mInternals->mSceneAppPlayerProcsInfo.mRemovePeriodicProc(mInternals->mSceneAppPlayerProcsInfo.mUserData);
	mInternals->mStopTime = CTimeInfo::getCurrentUniversalTime();
	mInternals->mSceneTouchHandlerInfos.removeAll();
	mInternals->mSceneTransitionTouchHandlerInfos.removeAll();
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
	if (deltaTimeInterval > 0.0)
		// Update
		mInternals->mCurrentScenePlayer->update(deltaTimeInterval);

	// Render
	mInternals->mGPU.renderStart();
	mInternals->mCurrentScenePlayer->render(mInternals->mGPU, S2DPoint32());
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
	if (!mInternals->mCurrentSceneTransitionPlayer.hasReference())
		// Pass to scene player
		mInternals->mCurrentScenePlayer->shakeBegan();
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppPlayer::shakeEnded()
//----------------------------------------------------------------------------------------------------------------------
{
	// Check for scene transition player
	if (!mInternals->mCurrentSceneTransitionPlayer.hasReference())
		// Pass to scene player
		mInternals->mCurrentScenePlayer->shakeEnded();
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppPlayer::shakeCancelled()
//----------------------------------------------------------------------------------------------------------------------
{
	// Check for scene transition player
	if (!mInternals->mCurrentSceneTransitionPlayer.hasReference())
		// Pass to scene player
		mInternals->mCurrentScenePlayer->shakeCancelled();
}
