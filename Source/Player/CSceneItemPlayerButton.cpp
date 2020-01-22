//----------------------------------------------------------------------------------------------------------------------
//	CSceneItemPlayerButton.cpp			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CSceneItemPlayerButton.h"

#include "CKeyframeAnimationPlayer.h"

#include "CAudioOutputTrackCache.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CSceneItemPlayerButtonInternals

class CSceneItemPlayerButtonInternals {
	public:
				CSceneItemPlayerButtonInternals(const SSceneItemPlayerProcsInfo& sceneItemPlayerProcsInfo,
						CSceneItemPlayerButton& sceneItemPlayerButton);
				~CSceneItemPlayerButtonInternals();

		bool	isPointInHitRect(const S2DPoint32& point);

		CSceneItemPlayerButton&				mSceneItemPlayerButton;

		bool								mIsAudioPlaying;
		bool								mIsEnabled;
		CAudioOutputTrackReference*			mAudioOutputTrackReference;

		CKeyframeAnimationPlayer*			mUpKeyframeAnimationPlayer;
		CKeyframeAnimationPlayer*			mDownKeyframeAnimationPlayer;
		CKeyframeAnimationPlayer*			mDisabledKeyframeAnimationPlayer;
		CKeyframeAnimationPlayer*			mCurrentKeyframeAnimationPlayer;

		SKeyframeAnimationPlayerProcsInfo	mKeyframeAnimationPlayerProcsInfo;
};

CString	CSceneItemPlayerButton::mPropertyNameEnabled("enabled");

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - Local proc declarations

//static	CSceneItemPlayer*	sCreateSceneItemPlayer(const CSceneItem& sceneItem,
//									const SSceneAppResourceManagementInfo& sceneAppResourceManagementInfo,
//									const SSceneItemPlayerProcsInfo& sceneItemPlayerProcsInfo, bool makeCopy);

static	bool				sKeyframeAnimationPlayerShouldLoopProc(CKeyframeAnimationPlayer& keyframeAnimationPlayer,
									UInt32 currentLoopCount, void* userData);
static	void				sKeyframeAnimationPlayerActionArrayHandlerProc(
									CKeyframeAnimationPlayer& keyframeAnimationPlayer, const CActionArray& actionArray,
									void* userData);

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: Register Scene Item Player

//REGISTER_SCENE_ITEM_PLAYER(CSceneItemPlayerButton, sCreateSceneItemPlayer, CSceneItemButton::mType);

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneItemPlayerButtonInternals

//----------------------------------------------------------------------------------------------------------------------
CSceneItemPlayerButtonInternals::CSceneItemPlayerButtonInternals(
		const SSceneItemPlayerProcsInfo& sceneItemPlayerProcsInfo, CSceneItemPlayerButton& sceneItemPlayerButton) :
		mSceneItemPlayerButton(sceneItemPlayerButton),
		mKeyframeAnimationPlayerProcsInfo(sKeyframeAnimationPlayerShouldLoopProc, nil, nil,
				sKeyframeAnimationPlayerActionArrayHandlerProc, this)
//----------------------------------------------------------------------------------------------------------------------
{
	mIsAudioPlaying = false;
	mIsEnabled = true;
	mAudioOutputTrackReference = nil;

	mUpKeyframeAnimationPlayer = nil;
	mDownKeyframeAnimationPlayer = nil;
	mDisabledKeyframeAnimationPlayer = nil;
	mCurrentKeyframeAnimationPlayer = nil;
}

//----------------------------------------------------------------------------------------------------------------------
CSceneItemPlayerButtonInternals::~CSceneItemPlayerButtonInternals()
//----------------------------------------------------------------------------------------------------------------------
{
	DisposeOf(mAudioOutputTrackReference);

	if (mDownKeyframeAnimationPlayer != mUpKeyframeAnimationPlayer) {
		DisposeOf(mUpKeyframeAnimationPlayer);
		DisposeOf(mDownKeyframeAnimationPlayer);
	} else
		DisposeOf(mUpKeyframeAnimationPlayer);
	
	DisposeOf(mDisabledKeyframeAnimationPlayer);
}

//----------------------------------------------------------------------------------------------------------------------
bool CSceneItemPlayerButtonInternals::isPointInHitRect(const S2DPoint32& point)
//----------------------------------------------------------------------------------------------------------------------
{
	Float32	hitRadius = mSceneItemPlayerButton.getSceneItemButton().getHitRadius();
	if (hitRadius == 0.0)
		// Hit Radius 0 means use artwork
		return mUpKeyframeAnimationPlayer->getScreenRect().contains(point);
	else {
		// Calculate hit test rect based on radius from screen center
		S2DRect32	screenRect = mUpKeyframeAnimationPlayer->getScreenRect();
		Float32		centerX = screenRect.getMidX();
		Float32		centerY = screenRect.getMidY();
		S2DRect32	hitTestRect = S2DRect32(centerX - hitRadius, centerY - hitRadius, hitRadius * 2.0, hitRadius * 2.0);

		return hitTestRect.contains(point);
	}
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneItemPlayerButton

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
CSceneItemPlayerButton::CSceneItemPlayerButton(const CSceneItemButton& sceneItemButton,
		const SSceneAppResourceManagementInfo& sceneAppResourceManagementInfo,
		const SSceneItemPlayerProcsInfo& sceneItemPlayerProcsInfo) :
		CSceneItemPlayer(sceneItemButton, sceneItemPlayerProcsInfo)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new CSceneItemPlayerButtonInternals(sceneItemPlayerProcsInfo, *this);

	OR<CKeyframeAnimationInfo>	upKeyframeAnimationInfo = sceneItemButton.getUpKeyframeAnimationInfo();
	if (upKeyframeAnimationInfo.hasReference()) {
		mInternals->mUpKeyframeAnimationPlayer =
				new CKeyframeAnimationPlayer(*upKeyframeAnimationInfo, sceneAppResourceManagementInfo,
						mInternals->mKeyframeAnimationPlayerProcsInfo, sceneItemButton.getStartTimeInterval());
		mInternals->mCurrentKeyframeAnimationPlayer = mInternals->mUpKeyframeAnimationPlayer;
	}
	
	OR<CKeyframeAnimationInfo>	downKeyframeAnimationInfo = sceneItemButton.getDownKeyframeAnimationInfo();
	if (downKeyframeAnimationInfo.hasReference())
		mInternals->mDownKeyframeAnimationPlayer =
				new CKeyframeAnimationPlayer(*downKeyframeAnimationInfo, sceneAppResourceManagementInfo,
						mInternals->mKeyframeAnimationPlayerProcsInfo, sceneItemButton.getStartTimeInterval());
	else
		mInternals->mDownKeyframeAnimationPlayer = mInternals->mUpKeyframeAnimationPlayer;

	OR<CKeyframeAnimationInfo>	disabledKeyframeAnimationInfo = sceneItemButton.getDisabledKeyframeAnimationInfo();
	if (disabledKeyframeAnimationInfo.hasReference()) {
		mInternals->mDisabledKeyframeAnimationPlayer =
				new CKeyframeAnimationPlayer(*disabledKeyframeAnimationInfo, sceneAppResourceManagementInfo,
						mInternals->mKeyframeAnimationPlayerProcsInfo, sceneItemButton.getStartTimeInterval());
	}
}

//----------------------------------------------------------------------------------------------------------------------
CSceneItemPlayerButton::~CSceneItemPlayerButton()
//----------------------------------------------------------------------------------------------------------------------
{
	DisposeOf(mInternals);
}

// MARK: CSceneItemPlayer methods

//----------------------------------------------------------------------------------------------------------------------
S2DRect32 CSceneItemPlayerButton::getCurrentScreenRect() const
//----------------------------------------------------------------------------------------------------------------------
{
	if (mInternals->mUpKeyframeAnimationPlayer != nil)
		return mInternals->mUpKeyframeAnimationPlayer->getScreenRect();
	else
		return S2DRect32();
}

//----------------------------------------------------------------------------------------------------------------------
CActionArray CSceneItemPlayerButton::getAllActions() const
//----------------------------------------------------------------------------------------------------------------------
{
	CActionArray	actionsArray;

	OR<CActionArray>	actionArray = getSceneItemButton().getActionArray();
	if (actionArray.hasReference())
		actionsArray.addFrom(*actionArray);

	return actionsArray;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerButton::load()
//----------------------------------------------------------------------------------------------------------------------
{
	const	CSceneItemButton&	sceneItemButton = getSceneItemButton();
	
	// Load audio
	OR<CAudioInfo>	audioInfo = sceneItemButton.getAudioInfo();
	if ((mInternals->mAudioOutputTrackReference == nil) && audioInfo.hasReference() &&
			(audioInfo->getResourceFilename().getLength() > 0)) {
		EAudioOutputTrackReferenceOptions	options = kAudioOutputTrackReferenceOptionsNone;
		if (audioInfo->getOptions() & kAudioInfoOptionsUseDefinedLoopInFile)
			options =
					(EAudioOutputTrackReferenceOptions)
							(options | kAudioOutputTrackReferenceOptionsUseDefinedLoopInFile);
		if (audioInfo->getOptions() & kAudioInfoOptionsLoadFileIntoMemory)
			options = (EAudioOutputTrackReferenceOptions) (options | kAudioOutputTrackReferenceOptionsLoadIntoMemory);
//		mInternals->mAudioOutputTrackReference =
//				CAudioOutputTrackCache::newAudioOutputTrackReferenceFromURL(
//						eGetURLForResourceFilename(sceneItemButton.getAudioInfoOrNil()->getResourceFilename()),
//						sceneItemButton.getAudioInfoOrNil()->getLoopCount(), options);
	}

	// Load animations
	if (mInternals->mUpKeyframeAnimationPlayer != nil)
		mInternals->mUpKeyframeAnimationPlayer->load(
				sceneItemButton.getStartTimeInterval() == kSceneItemStartTimeStartAtLoad);
	if (mInternals->mDownKeyframeAnimationPlayer != nil)
		mInternals->mDownKeyframeAnimationPlayer->load(
				sceneItemButton.getStartTimeInterval() == kSceneItemStartTimeStartAtLoad);
	if (mInternals->mDisabledKeyframeAnimationPlayer != nil)
		mInternals->mDisabledKeyframeAnimationPlayer->load(
				sceneItemButton.getStartTimeInterval() == kSceneItemStartTimeStartAtLoad);
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerButton::finishLoading()
//----------------------------------------------------------------------------------------------------------------------
{
	if (mInternals->mUpKeyframeAnimationPlayer != nil)
		mInternals->mUpKeyframeAnimationPlayer->finishLoading();
	if (mInternals->mDownKeyframeAnimationPlayer != nil)
		mInternals->mDownKeyframeAnimationPlayer->finishLoading();
	if (mInternals->mDisabledKeyframeAnimationPlayer != nil)
		mInternals->mDisabledKeyframeAnimationPlayer->finishLoading();
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerButton::unload()
//----------------------------------------------------------------------------------------------------------------------
{
	// Unload audio
	if (mInternals->mIsAudioPlaying)
		mInternals->mAudioOutputTrackReference->reset();
	DisposeOf(mInternals->mAudioOutputTrackReference);

	// Unload animations
	if (mInternals->mUpKeyframeAnimationPlayer != nil)
		mInternals->mUpKeyframeAnimationPlayer->unload();
	if (mInternals->mDownKeyframeAnimationPlayer != nil)
		mInternals->mDownKeyframeAnimationPlayer->unload();
	if (mInternals->mDisabledKeyframeAnimationPlayer != nil)
		mInternals->mDisabledKeyframeAnimationPlayer->unload();
}

//----------------------------------------------------------------------------------------------------------------------
bool CSceneItemPlayerButton::getIsFullyLoaded() const
//----------------------------------------------------------------------------------------------------------------------
{
	bool	isFullyLoaded = true;

	if (mInternals->mUpKeyframeAnimationPlayer != nil)
		isFullyLoaded &= mInternals->mUpKeyframeAnimationPlayer->getIsFullyLoaded();
	if (mInternals->mDownKeyframeAnimationPlayer != nil)
		isFullyLoaded &= mInternals->mDownKeyframeAnimationPlayer->getIsFullyLoaded();
	if (mInternals->mDisabledKeyframeAnimationPlayer != nil)
		isFullyLoaded &= mInternals->mDisabledKeyframeAnimationPlayer->getIsFullyLoaded();

	return isFullyLoaded;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerButton::reset()
//----------------------------------------------------------------------------------------------------------------------
{
	const	CSceneItemButton&	sceneItemButton = getSceneItemButton();
	
	// Reset audio
	if (mInternals->mAudioOutputTrackReference != nil)
		mInternals->mAudioOutputTrackReference->reset();
	mInternals->mIsAudioPlaying = false;

	// Reset animation
	if (mInternals->mUpKeyframeAnimationPlayer != nil)
		mInternals->mUpKeyframeAnimationPlayer->reset(
				sceneItemButton.getStartTimeInterval() == kSceneItemStartTimeStartAtLoad);
	if (mInternals->mDownKeyframeAnimationPlayer != nil)
		mInternals->mDownKeyframeAnimationPlayer->reset(
				sceneItemButton.getStartTimeInterval() == kSceneItemStartTimeStartAtLoad);
	if (mInternals->mDisabledKeyframeAnimationPlayer != nil)
		mInternals->mDisabledKeyframeAnimationPlayer->reset(
				sceneItemButton.getStartTimeInterval() == kSceneItemStartTimeStartAtLoad);

	// Super
	CSceneItemPlayer::reset();
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerButton::update(UniversalTimeInterval deltaTimeInterval, bool isRunning)
//----------------------------------------------------------------------------------------------------------------------
{
	if (mInternals->mUpKeyframeAnimationPlayer != nil)
		mInternals->mUpKeyframeAnimationPlayer->update(deltaTimeInterval, isRunning);
	if (mInternals->mDownKeyframeAnimationPlayer != nil)
		mInternals->mDownKeyframeAnimationPlayer->update(deltaTimeInterval, isRunning);
	if (mInternals->mDisabledKeyframeAnimationPlayer != nil)
		mInternals->mDisabledKeyframeAnimationPlayer->update(deltaTimeInterval, isRunning);
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerButton::render(CGPU& gpu, const S2DPoint32& offset)
//----------------------------------------------------------------------------------------------------------------------
{
	if (mInternals->mIsEnabled) {
		if (mInternals->mCurrentKeyframeAnimationPlayer != nil)
			mInternals->mCurrentKeyframeAnimationPlayer->render(gpu, offset);
	} else if (mInternals->mDisabledKeyframeAnimationPlayer != nil)
		mInternals->mDisabledKeyframeAnimationPlayer->render(gpu, offset);
}

//----------------------------------------------------------------------------------------------------------------------
bool CSceneItemPlayerButton::handlesTouchOrMouseAtPoint(const S2DPoint32& point)
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mIsEnabled && (mInternals->mUpKeyframeAnimationPlayer != nil) &&
			mInternals->isPointInHitRect(point);
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerButton::touchBeganOrMouseDownAtPoint(const S2DPoint32& point, UInt32 tapOrClickCount,
		const void* reference)
//----------------------------------------------------------------------------------------------------------------------
{
	if (mInternals->mIsEnabled) {
		// Change animation
		mInternals->mCurrentKeyframeAnimationPlayer = mInternals->mDownKeyframeAnimationPlayer;
		
//		// Start audio
//		if (mInternals->mAudioOutputTrackReference != nil) {
//			CSceneItemButton&	sceneItemButton = getSceneItemButton();
//	
//			mInternals->mAudioOutputTrackReference->setGain(sceneItemButton.getAudioInfoOrNil()->getGain());
//			mInternals->mAudioOutputTrackReference->setLoopCount(sceneItemButton.getAudioInfoOrNil()->getLoopCount());
//			mInternals->mAudioOutputTrackReference->reset();
//			mInternals->mAudioOutputTrackReference->play();
//			mInternals->mIsAudioPlaying = true;
//		}
	}
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerButton::touchOrMouseMovedFromPoint(const S2DPoint32& point1, const S2DPoint32& point2,
		const void* reference)
//----------------------------------------------------------------------------------------------------------------------
{
	if (mInternals->mIsEnabled) {
		if (mInternals->isPointInHitRect(point2)) {
			// Inside
			// Start audio
//			if ((mInternals->mCurrentKeyframeAnimationPlayer != mInternals->mDownKeyframeAnimationPlayer) &&
//					(mInternals->mAudioOutputTrackReference != nil)) {
//				CSceneItemButton&	sceneItemButton = getSceneItemButton();
//	
//				mInternals->mAudioOutputTrackReference->setGain(sceneItemButton.getAudioInfoOrNil()->getGain());
//				mInternals->mAudioOutputTrackReference->setLoopCount(
//						sceneItemButton.getAudioInfoOrNil()->getLoopCount());
//				mInternals->mAudioOutputTrackReference->reset();
//				mInternals->mAudioOutputTrackReference->play();
//				mInternals->mIsAudioPlaying = true;
//			}

			// Update animation
			mInternals->mCurrentKeyframeAnimationPlayer = mInternals->mDownKeyframeAnimationPlayer;
		} else
			// Outside
			mInternals->mCurrentKeyframeAnimationPlayer = mInternals->mUpKeyframeAnimationPlayer;
	}
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerButton::touchEndedOrMouseUpAtPoint(const S2DPoint32& point, const void* reference)
//----------------------------------------------------------------------------------------------------------------------
{
	if (mInternals->mIsEnabled) {
		mInternals->mCurrentKeyframeAnimationPlayer = mInternals->mUpKeyframeAnimationPlayer;

		if (mInternals->isPointInHitRect(point)) {
			OR<CActionArray>	actionArray = getSceneItemButton().getActionArray();
			if (actionArray.hasReference())
				perform(*actionArray, point);
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerButton::touchOrMouseCancelled(const void* reference)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mCurrentKeyframeAnimationPlayer = mInternals->mUpKeyframeAnimationPlayer;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerButton::setProperty(const CString& propertyName, const SDictionaryValue& value)
//----------------------------------------------------------------------------------------------------------------------
{
	if (propertyName == mPropertyNameEnabled) {
		// number is 0 or 1
		mInternals->mIsEnabled = value.getBool();
		if (!mInternals->mIsEnabled)
			mInternals->mCurrentKeyframeAnimationPlayer = mInternals->mUpKeyframeAnimationPlayer;
	} else
		CSceneItemPlayer::setProperty(propertyName, value);
}

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerButton::setIsEnabled(bool isEnabled)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mIsEnabled = isEnabled;
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - Local proc definitions

////----------------------------------------------------------------------------------------------------------------------
//CSceneItemPlayer* sCreateSceneItemPlayer(const CSceneItem& sceneItem,
//		const SSceneAppResourceManagementInfo& sceneAppResourceManagementInfo,
//		const SSceneItemPlayerProcsInfo& sceneItemPlayerProcsInfo, bool makeCopy)
////----------------------------------------------------------------------------------------------------------------------
//{
//	return new CSceneItemPlayerButton(*((CSceneItemButton*) &sceneItem), sceneAppResourceManagementInfo,
//			sceneItemPlayerProcsInfo, makeCopy);
//}
//
//----------------------------------------------------------------------------------------------------------------------
bool sKeyframeAnimationPlayerShouldLoopProc(CKeyframeAnimationPlayer& keyframeAnimationPlayer, UInt32 currentLoopCount,
		void* userData)
//----------------------------------------------------------------------------------------------------------------------
{
	return true;
}

//----------------------------------------------------------------------------------------------------------------------
void sKeyframeAnimationPlayerActionArrayHandlerProc(CKeyframeAnimationPlayer& keyframeAnimationPlayer,
		const CActionArray& actionArray, void* userData)
//----------------------------------------------------------------------------------------------------------------------
{
	CSceneItemPlayerButtonInternals*	internals = (CSceneItemPlayerButtonInternals*) userData;

	internals->mSceneItemPlayerButton.perform(actionArray);
}
