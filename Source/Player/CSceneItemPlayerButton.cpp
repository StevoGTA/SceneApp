//----------------------------------------------------------------------------------------------------------------------
//	CSceneItemPlayerButton.cpp			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CSceneItemPlayerButton.h"

#include "CKeyframeAnimationPlayer.h"
#include "CSceneAppMediaEngine.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CSceneItemPlayerButton::Internals

class CSceneItemPlayerButton::Internals {
	public:
						Internals(CSceneItemPlayerButton& sceneItemPlayerButton,
								const SSceneAppResourceManagementInfo& sceneAppResourceManagementInfo) :
							mSceneItemPlayerButton(sceneItemPlayerButton),
									mSceneAppMediaEngine(sceneAppResourceManagementInfo.mSceneAppMediaEngine),
									mIsEnabled(true),
									mUpKeyframeAnimationPlayer(nil), mDownKeyframeAnimationPlayer(nil),
									mDisabledKeyframeAnimationPlayer(nil), mCurrentKeyframeAnimationPlayer(nil),
									mKeyframeAnimationPlayerProcsInfo(
											(CKeyframeAnimationPlayer::Procs::ShouldLoopProc)
													keyframeAnimationPlayerShouldLoop,
											nil, nil,
											(CKeyframeAnimationPlayer::Procs::PerformActionsProc)
													keyframeAnimationPlayerPerformActions,
											this)
							{}
						~Internals()
							{
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

		static	bool	keyframeAnimationPlayerShouldLoop(CKeyframeAnimationPlayer& keyframeAnimationPlayer,
								UInt32 currentLoopCount, Internals* internals)
							{ return true; }
		static	void	keyframeAnimationPlayerPerformActions(CKeyframeAnimationPlayer& keyframeAnimationPlayer,
								const CActions& actions, Internals* internals)
							{ internals->mSceneItemPlayerButton.perform(actions); }

				CSceneItemPlayerButton&			mSceneItemPlayerButton;
				CSceneAppMediaEngine&			mSceneAppMediaEngine;

				bool							mIsEnabled;
				OI<CMediaPlayer>				mMediaPlayer;

				CKeyframeAnimationPlayer*		mUpKeyframeAnimationPlayer;
				CKeyframeAnimationPlayer*		mDownKeyframeAnimationPlayer;
				CKeyframeAnimationPlayer*		mDisabledKeyframeAnimationPlayer;
				CKeyframeAnimationPlayer*		mCurrentKeyframeAnimationPlayer;

				CKeyframeAnimationPlayer::Procs	mKeyframeAnimationPlayerProcsInfo;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneItemPlayerButton

// MARK: Properties

CString	CSceneItemPlayerButton::mPropertyNameEnabled(OSSTR("enabled"));

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
CSceneItemPlayerButton::CSceneItemPlayerButton(const CSceneItemButton& sceneItemButton,
		const SSceneAppResourceManagementInfo& sceneAppResourceManagementInfo, const Procs& procs) :
		CSceneItemPlayer(sceneItemButton, procs)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new Internals(*this, sceneAppResourceManagementInfo);

	OI<CKeyframeAnimationInfo>	upKeyframeAnimationInfo = sceneItemButton.getUpKeyframeAnimationInfo();
	if (upKeyframeAnimationInfo.hasInstance()) {
		mInternals->mUpKeyframeAnimationPlayer =
				new CKeyframeAnimationPlayer(*upKeyframeAnimationInfo, sceneAppResourceManagementInfo,
						mInternals->mKeyframeAnimationPlayerProcsInfo, sceneItemButton.getStartTimeInterval());
		mInternals->mCurrentKeyframeAnimationPlayer = mInternals->mUpKeyframeAnimationPlayer;
	}
	
	OI<CKeyframeAnimationInfo>	downKeyframeAnimationInfo = sceneItemButton.getDownKeyframeAnimationInfo();
	if (downKeyframeAnimationInfo.hasInstance())
		mInternals->mDownKeyframeAnimationPlayer =
				new CKeyframeAnimationPlayer(*downKeyframeAnimationInfo, sceneAppResourceManagementInfo,
						mInternals->mKeyframeAnimationPlayerProcsInfo, sceneItemButton.getStartTimeInterval());
	else
		mInternals->mDownKeyframeAnimationPlayer = mInternals->mUpKeyframeAnimationPlayer;

	OI<CKeyframeAnimationInfo>	disabledKeyframeAnimationInfo = sceneItemButton.getDisabledKeyframeAnimationInfo();
	if (disabledKeyframeAnimationInfo.hasInstance()) {
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
	OI<CActions>	actions = getSceneItemButton().getActions();

	return actions.hasInstance() ? *actions : CActions();
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerButton::load(CGPU& gpu)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	const	CSceneItemButton&	sceneItemButton = getSceneItemButton();
	
	// Load audio
	const	OI<CAudioInfo>&	audioInfo = sceneItemButton.getAudioInfo();
	if (audioInfo.hasInstance())
		// Setup CMediaPlayer
		mInternals->mMediaPlayer = mInternals->mSceneAppMediaEngine.getMediaPlayer(*audioInfo);

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
	// Unload audio
	mInternals->mMediaPlayer = OI<CMediaPlayer>();

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
	// Setup
	const	CSceneItemButton&	sceneItemButton = getSceneItemButton();
	
	// Reset audio
	if (mInternals->mMediaPlayer.hasInstance())
		// Reset
		mInternals->mMediaPlayer->reset();

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
void CSceneItemPlayerButton::render(CGPU& gpu, const CGPURenderObject::RenderInfo& renderInfo) const
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
	// Check enabled
	if (mInternals->mIsEnabled) {
		// Change animation
		mInternals->mCurrentKeyframeAnimationPlayer = mInternals->mDownKeyframeAnimationPlayer;
		
		// Check for audio
		if (mInternals->mMediaPlayer.hasInstance()) {
			// Start playback
			mInternals->mMediaPlayer->reset();
			mInternals->mMediaPlayer->play();
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerButton::touchOrMouseMovedFromPoint(const S2DPointF32& point1, const S2DPointF32& point2,
		const void* reference)
//----------------------------------------------------------------------------------------------------------------------
{
	// Check enabled
	if (mInternals->mIsEnabled) {
		// Check point
		if (mInternals->isPointInHitRect(point2)) {
			// Inside
			// Check if just moved into hit rect and have audio
			if ((mInternals->mCurrentKeyframeAnimationPlayer != mInternals->mDownKeyframeAnimationPlayer) &&
					mInternals->mMediaPlayer.hasInstance()) {
				// Restart playback
				mInternals->mMediaPlayer->reset();
				mInternals->mMediaPlayer->play();
			}

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
			OI<CActions>	actions = getSceneItemButton().getActions();
			if (actions.hasInstance())
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
void CSceneItemPlayerButton::setProperty(const CString& propertyName, const SValue& value)
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
