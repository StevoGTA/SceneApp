//----------------------------------------------------------------------------------------------------------------------
//	CSceneItemPlayerButton.cpp			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CSceneItemPlayerButton.h"

#include "CKeyframeAnimationPlayer.h"

//#include "CAudioOutputTrackCache.h"

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: Local proc declarations

static	bool	sKeyframeAnimationPlayerShouldLoopProc(CKeyframeAnimationPlayer& keyframeAnimationPlayer,
						UInt32 currentLoopCount, void* userData);
static	void	sKeyframeAnimationPlayerActionsHandlerProc(CKeyframeAnimationPlayer& keyframeAnimationPlayer,
						const CActions& actions, void* userData);

//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneItemPlayerButtonInternals

class CSceneItemPlayerButtonInternals {
	public:
				CSceneItemPlayerButtonInternals(const SSceneItemPlayerProcsInfo& sceneItemPlayerProcsInfo,
						CSceneItemPlayerButton& sceneItemPlayerButton) :
					mSceneItemPlayerButton(sceneItemPlayerButton), mIsAudioPlaying(false), mIsEnabled(true),
							mUpKeyframeAnimationPlayer(nil), mDownKeyframeAnimationPlayer(nil),
							mDisabledKeyframeAnimationPlayer(nil), mCurrentKeyframeAnimationPlayer(nil),
							mKeyframeAnimationPlayerProcsInfo(sKeyframeAnimationPlayerShouldLoopProc, nil, nil,
									sKeyframeAnimationPlayerActionsHandlerProc, this)
					{}
				~CSceneItemPlayerButtonInternals()
					{
//						Delete(mAudioOutputTrackReference);

						if (mDownKeyframeAnimationPlayer != mUpKeyframeAnimationPlayer) {
							Delete(mUpKeyframeAnimationPlayer);
							Delete(mDownKeyframeAnimationPlayer);
						} else
							Delete(mUpKeyframeAnimationPlayer);

						Delete(mDisabledKeyframeAnimationPlayer);
					}

		bool	isPointInHitRect(const S2DPointF32& point)
					{
						Float32	hitRadius = mSceneItemPlayerButton.getSceneItemButton().getHitRadius();
						if (hitRadius == 0.0)
							// Hit Radius 0 means use artwork
							return mUpKeyframeAnimationPlayer->getScreenRect().contains(point);
						else {
							// Calculate hit test rect based on radius from screen center
							S2DRectF32	screenRect = mUpKeyframeAnimationPlayer->getScreenRect();
							Float32		centerX = screenRect.getMidX();
							Float32		centerY = screenRect.getMidY();
							S2DRectF32	hitTestRect(centerX - hitRadius, centerY - hitRadius, hitRadius * 2.0f,
												hitRadius * 2.0f);

							return hitTestRect.contains(point);
						}
					}

		CSceneItemPlayerButton&				mSceneItemPlayerButton;

		bool								mIsAudioPlaying;
		bool								mIsEnabled;
//		CAudioOutputTrackReference*			mAudioOutputTrackReference;

		CKeyframeAnimationPlayer*			mUpKeyframeAnimationPlayer;
		CKeyframeAnimationPlayer*			mDownKeyframeAnimationPlayer;
		CKeyframeAnimationPlayer*			mDisabledKeyframeAnimationPlayer;
		CKeyframeAnimationPlayer*			mCurrentKeyframeAnimationPlayer;

		SKeyframeAnimationPlayerProcsInfo	mKeyframeAnimationPlayerProcsInfo;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneItemPlayerButton

// MARK: Properties

CString	CSceneItemPlayerButton::mPropertyNameEnabled(OSSTR("enabled"));

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
CSceneItemPlayerButton::CSceneItemPlayerButton(const CSceneItemButton& sceneItemButton,
		const SSceneAppResourceManagementInfo& sceneAppResourceManagementInfo,
		const SSceneItemPlayerProcsInfo& sceneItemPlayerProcsInfo) :
		CSceneItemPlayer(sceneItemButton, sceneItemPlayerProcsInfo)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new CSceneItemPlayerButtonInternals(sceneItemPlayerProcsInfo, *this);

	OO<CKeyframeAnimationInfo>	upKeyframeAnimationInfo = sceneItemButton.getUpKeyframeAnimationInfo();
	if (upKeyframeAnimationInfo.hasObject()) {
		mInternals->mUpKeyframeAnimationPlayer =
				new CKeyframeAnimationPlayer(*upKeyframeAnimationInfo, sceneAppResourceManagementInfo,
						mInternals->mKeyframeAnimationPlayerProcsInfo, sceneItemButton.getStartTimeInterval());
		mInternals->mCurrentKeyframeAnimationPlayer = mInternals->mUpKeyframeAnimationPlayer;
	}
	
	OO<CKeyframeAnimationInfo>	downKeyframeAnimationInfo = sceneItemButton.getDownKeyframeAnimationInfo();
	if (downKeyframeAnimationInfo.hasObject())
		mInternals->mDownKeyframeAnimationPlayer =
				new CKeyframeAnimationPlayer(*downKeyframeAnimationInfo, sceneAppResourceManagementInfo,
						mInternals->mKeyframeAnimationPlayerProcsInfo, sceneItemButton.getStartTimeInterval());
	else
		mInternals->mDownKeyframeAnimationPlayer = mInternals->mUpKeyframeAnimationPlayer;

	OO<CKeyframeAnimationInfo>	disabledKeyframeAnimationInfo = sceneItemButton.getDisabledKeyframeAnimationInfo();
	if (disabledKeyframeAnimationInfo.hasObject()) {
		mInternals->mDisabledKeyframeAnimationPlayer =
				new CKeyframeAnimationPlayer(*disabledKeyframeAnimationInfo, sceneAppResourceManagementInfo,
						mInternals->mKeyframeAnimationPlayerProcsInfo, sceneItemButton.getStartTimeInterval());
	}
}

//----------------------------------------------------------------------------------------------------------------------
CSceneItemPlayerButton::~CSceneItemPlayerButton()
//----------------------------------------------------------------------------------------------------------------------
{
	Delete(mInternals);
}

// MARK: CSceneItemPlayer methods

//----------------------------------------------------------------------------------------------------------------------
CActions CSceneItemPlayerButton::getAllActions() const
//----------------------------------------------------------------------------------------------------------------------
{
	OO<CActions>	actions = getSceneItemButton().getActions();

	return actions.hasObject() ? *actions : CActions();
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerButton::load(CGPU& gpu)
//----------------------------------------------------------------------------------------------------------------------
{
	const	CSceneItemButton&	sceneItemButton = getSceneItemButton();
	
//	// Load audio
//	OR<CAudioInfo>	audioInfo = sceneItemButton.getAudioInfo();
//	if ((mInternals->mAudioOutputTrackReference == nil) && audioInfo.hasReference() &&
//			(audioInfo->getResourceFilename().getLength() > 0)) {
//		EAudioOutputTrackReferenceOptions	options = kAudioOutputTrackReferenceOptionsNone;
//		if (audioInfo->getOptions() & kAudioInfoOptionsUseDefinedLoopInFile)
//			options =
//					(EAudioOutputTrackReferenceOptions)
//							(options | kAudioOutputTrackReferenceOptionsUseDefinedLoopInFile);
//		if (audioInfo->getOptions() & kAudioInfoOptionsLoadFileIntoMemory)
//			options = (EAudioOutputTrackReferenceOptions) (options | kAudioOutputTrackReferenceOptionsLoadIntoMemory);
//		mInternals->mAudioOutputTrackReference =
//				CAudioOutputTrackCache::newAudioOutputTrackReferenceFromURL(
//						eGetURLForResourceFilename(sceneItemButton.getAudioInfoOrNil()->getResourceFilename()),
//						sceneItemButton.getAudioInfoOrNil()->getLoopCount(), options);
//	}

	// Load animations
	if (mInternals->mUpKeyframeAnimationPlayer != nil)
		mInternals->mUpKeyframeAnimationPlayer->load(gpu, !sceneItemButton.getStartTimeInterval().hasValue());
	if (mInternals->mDownKeyframeAnimationPlayer != nil)
		mInternals->mDownKeyframeAnimationPlayer->load(gpu, !sceneItemButton.getStartTimeInterval().hasValue());
	if (mInternals->mDisabledKeyframeAnimationPlayer != nil)
		mInternals->mDisabledKeyframeAnimationPlayer->load(gpu, !sceneItemButton.getStartTimeInterval().hasValue());

	// Do super
	CSceneItemPlayer::load(gpu);
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerButton::unload()
//----------------------------------------------------------------------------------------------------------------------
{
//	// Unload audio
//	if (mInternals->mIsAudioPlaying)
//		mInternals->mAudioOutputTrackReference->reset();
//	Delete(mInternals->mAudioOutputTrackReference);

	// Unload animations
	if (mInternals->mUpKeyframeAnimationPlayer != nil)
		mInternals->mUpKeyframeAnimationPlayer->unload();
	if (mInternals->mDownKeyframeAnimationPlayer != nil)
		mInternals->mDownKeyframeAnimationPlayer->unload();
	if (mInternals->mDisabledKeyframeAnimationPlayer != nil)
		mInternals->mDisabledKeyframeAnimationPlayer->unload();
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerButton::reset()
//----------------------------------------------------------------------------------------------------------------------
{
	const	CSceneItemButton&	sceneItemButton = getSceneItemButton();
	
	// Reset audio
//	if (mInternals->mAudioOutputTrackReference != nil)
//		mInternals->mAudioOutputTrackReference->reset();
	mInternals->mIsAudioPlaying = false;

	// Reset animation
	if (mInternals->mUpKeyframeAnimationPlayer != nil)
		mInternals->mUpKeyframeAnimationPlayer->reset(!sceneItemButton.getStartTimeInterval().hasValue());
	if (mInternals->mDownKeyframeAnimationPlayer != nil)
		mInternals->mDownKeyframeAnimationPlayer->reset(!sceneItemButton.getStartTimeInterval().hasValue());
	if (mInternals->mDisabledKeyframeAnimationPlayer != nil)
		mInternals->mDisabledKeyframeAnimationPlayer->reset(!sceneItemButton.getStartTimeInterval().hasValue());

	// Super
	CSceneItemPlayer::reset();
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerButton::update(UniversalTimeInterval deltaTimeInterval, bool isRunning)
//----------------------------------------------------------------------------------------------------------------------
{
	if (mInternals->mIsEnabled) {
		if (mInternals->mCurrentKeyframeAnimationPlayer != nil)
			mInternals->mCurrentKeyframeAnimationPlayer->finishLoading();
	} else if (mInternals->mDisabledKeyframeAnimationPlayer != nil)
		mInternals->mDisabledKeyframeAnimationPlayer->finishLoading();

	if (mInternals->mUpKeyframeAnimationPlayer != nil)
		mInternals->mUpKeyframeAnimationPlayer->update(deltaTimeInterval, isRunning);
	if (mInternals->mDownKeyframeAnimationPlayer != nil)
		mInternals->mDownKeyframeAnimationPlayer->update(deltaTimeInterval, isRunning);
	if (mInternals->mDisabledKeyframeAnimationPlayer != nil)
		mInternals->mDisabledKeyframeAnimationPlayer->update(deltaTimeInterval, isRunning);
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerButton::render(CGPU& gpu, const SGPURenderObjectRenderInfo& renderInfo) const
//----------------------------------------------------------------------------------------------------------------------
{
	if (mInternals->mIsEnabled) {
		if (mInternals->mCurrentKeyframeAnimationPlayer != nil)
			mInternals->mCurrentKeyframeAnimationPlayer->render(gpu, renderInfo);
	} else if (mInternals->mDisabledKeyframeAnimationPlayer != nil)
		mInternals->mDisabledKeyframeAnimationPlayer->render(gpu, renderInfo);
}

//----------------------------------------------------------------------------------------------------------------------
bool CSceneItemPlayerButton::handlesTouchOrMouseAtPoint(const S2DPointF32& point) const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mIsEnabled && (mInternals->mUpKeyframeAnimationPlayer != nil) &&
			mInternals->isPointInHitRect(point);
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerButton::touchBeganOrMouseDownAtPoint(const S2DPointF32& point, UInt32 tapOrClickCount,
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
void CSceneItemPlayerButton::touchOrMouseMovedFromPoint(const S2DPointF32& point1, const S2DPointF32& point2,
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
void CSceneItemPlayerButton::touchEndedOrMouseUpAtPoint(const S2DPointF32& point, const void* reference)
//----------------------------------------------------------------------------------------------------------------------
{
	if (mInternals->mIsEnabled) {
		mInternals->mCurrentKeyframeAnimationPlayer = mInternals->mUpKeyframeAnimationPlayer;

		if (mInternals->isPointInHitRect(point)) {
			OO<CActions>	actions = getSceneItemButton().getActions();
			if (actions.hasObject())
				perform(*actions, point);
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

//----------------------------------------------------------------------------------------------------------------------
bool sKeyframeAnimationPlayerShouldLoopProc(CKeyframeAnimationPlayer& keyframeAnimationPlayer, UInt32 currentLoopCount,
		void* userData)
//----------------------------------------------------------------------------------------------------------------------
{
	return true;
}

//----------------------------------------------------------------------------------------------------------------------
void sKeyframeAnimationPlayerActionsHandlerProc(CKeyframeAnimationPlayer& keyframeAnimationPlayer,
		const CActions& actions, void* userData)
//----------------------------------------------------------------------------------------------------------------------
{
	CSceneItemPlayerButtonInternals*	internals = (CSceneItemPlayerButtonInternals*) userData;

	internals->mSceneItemPlayerButton.perform(actions);
}
