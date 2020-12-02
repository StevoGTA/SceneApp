//----------------------------------------------------------------------------------------------------------------------
//	CSceneItemPlayerAnimation.cpp			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CSceneItemPlayerAnimation.h"

//#include "CCelAnimationPlayer.h"
#include "CKeyframeAnimationPlayer.h"
#include "CSceneAppMediaEngine.h"

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: Local proc declarations

//static	ECelAnimationPlayerFinishedAction	sCelAnimationPlayerGetFinishedActionProc(
//													CCelAnimationPlayer& celAnimationPlayer, UInt32 currentLoopCount,
//													void* userData);
//static	void								sCelAnimationPlayerFinishedProc(CCelAnimationPlayer& celAnimationPlayer,
//													void* userData);

static	bool								sKeyframeAnimationPlayerShouldLoopProc(
													CKeyframeAnimationPlayer& keyframeAnimationPlayer,
													UInt32 currentLoopCount, void* userData);
static	void								sKeyframeAnimationPlayerFinishedProc(
													CKeyframeAnimationPlayer& keyframeAnimationPlayer, void* userData);
static	void								sKeyframeAnimationPlayerActionsHandlerProc(
													CKeyframeAnimationPlayer& keyframeAnimationPlayer,
													const CActions& actions, void* userData);

//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneItemPlayerAnimationInternals

class CSceneItemPlayerAnimationInternals {
	public:
		CSceneItemPlayerAnimationInternals(CSceneItemPlayerAnimation& sceneItemPlayerAnimation,
				const SSceneAppResourceManagementInfo& sceneAppResourceManagementInfo,
				const SSceneItemPlayerProcsInfo& sceneItemPlayerProcsInfo) :
			mSceneItemPlayerAnimation(sceneItemPlayerAnimation),
					mSceneAppResourceManagementInfo(sceneAppResourceManagementInfo), mKeyframeAnimationPlayer(nil),
					mIsStarted(false), mCurrentTimeInterval(0.0), mSceneItemPlayerAnimationProcsInfo(nil),
//					mCelAnimationPlayerProcsInfo(sCelAnimationPlayerGetFinishedActionProc, nil,
//							sCelAnimationPlayerFinishedProc, nil, this),
					mKeyframeAnimationPlayerProcsInfo(sKeyframeAnimationPlayerShouldLoopProc, nil,
							sKeyframeAnimationPlayerFinishedProc, sKeyframeAnimationPlayerActionsHandlerProc, this)
			{}
		~CSceneItemPlayerAnimationInternals()
			{
//				Delete(mCelAnimationPlayer);
				Delete(mKeyframeAnimationPlayer);
			}

				CSceneItemPlayerAnimation&			mSceneItemPlayerAnimation;
		const	SSceneAppResourceManagementInfo&	mSceneAppResourceManagementInfo;

//				CCelAnimationPlayer*				mCelAnimationPlayer;
				CKeyframeAnimationPlayer*			mKeyframeAnimationPlayer;
				OI<CMediaPlayer>					mMediaPlayer;

				bool								mIsStarted;
				UniversalTimeInterval				mCurrentTimeInterval;

		const	SSceneItemPlayerAnimationProcsInfo*	mSceneItemPlayerAnimationProcsInfo;
//				SCelAnimationPlayerProcsInfo		mCelAnimationPlayerProcsInfo;
				SKeyframeAnimationPlayerProcsInfo	mKeyframeAnimationPlayerProcsInfo;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneItemPlayerAnimation

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
CSceneItemPlayerAnimation::CSceneItemPlayerAnimation(const CSceneItemAnimation& sceneItemAnimation,
		const SSceneAppResourceManagementInfo& sceneAppResourceManagementInfo,
		const SSceneItemPlayerProcsInfo& sceneItemPlayerProcsInfo) :
		CSceneItemPlayer(sceneItemAnimation, sceneItemPlayerProcsInfo)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals =
			new CSceneItemPlayerAnimationInternals(*this, sceneAppResourceManagementInfo, sceneItemPlayerProcsInfo);

//	if (sceneItemAnimation.getCelAnimationInfoOrNil() != nil)
//		mInternals->mCelAnimationPlayer =
//				new CCelAnimationPlayer(*getSceneItemAnimation().getCelAnimationInfoOrNil(),
//						sceneAppResourceManagementInfo, mInternals->mCelAnimationPlayerProcsInfo, 0.0);
	if (mInternals == nil) (void) mInternals;
	else if (sceneItemAnimation.getKeyframeAnimationInfo().hasInstance())
		mInternals->mKeyframeAnimationPlayer =
				new CKeyframeAnimationPlayer(*sceneItemAnimation.getKeyframeAnimationInfo(),
						sceneAppResourceManagementInfo, mInternals->mKeyframeAnimationPlayerProcsInfo, 0.0);
}

//----------------------------------------------------------------------------------------------------------------------
CSceneItemPlayerAnimation::~CSceneItemPlayerAnimation()
//----------------------------------------------------------------------------------------------------------------------
{
	Delete(mInternals);
}

// MARK: CSceneItemPlayer methods

//----------------------------------------------------------------------------------------------------------------------
CActions CSceneItemPlayerAnimation::getAllActions() const
//----------------------------------------------------------------------------------------------------------------------
{
//	if (mInternals->mCelAnimationPlayer != nil)
//		return CActions();
	if (mInternals == nil) return CActions();
	else if (mInternals->mKeyframeAnimationPlayer != nil)
		return mInternals->mKeyframeAnimationPlayer->getAllActions();
	else
		return CActions();
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerAnimation::load(CGPU& gpu)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	const	CSceneItemAnimation&	sceneItemAnimation = getSceneItemAnimation();
	
	// Load animation
//	if (mInternals->mCelAnimationPlayer != nil)
//		mInternals->mCelAnimationPlayer->load(!sceneItemAnimation.getStartTimeInterval().hasValue());
	if (mInternals == nil) (void) mInternals;
	else if (mInternals->mKeyframeAnimationPlayer != nil)
		mInternals->mKeyframeAnimationPlayer->load(gpu, !sceneItemAnimation.getStartTimeInterval().hasValue());

	// Load audio
	const	OI<CAudioInfo>&	audioInfo = sceneItemAnimation.getAudioInfo();
	if (audioInfo.hasInstance())
		// Setup CMediaPlayer
		mInternals->mMediaPlayer =
				mInternals->mSceneAppResourceManagementInfo.mSceneAppMediaEngine.getMediaPlayer(*audioInfo);

	// Do super
	CSceneItemPlayer::load(gpu);

	if (!sceneItemAnimation.getStartTimeInterval().hasValue()) {
		// Start
		mInternals->mIsStarted = true;

		// Check for audio
		if (mInternals->mMediaPlayer.hasInstance())
			// Start
			mInternals->mMediaPlayer->play();

		// Check for started actions
		const	OI<CActions>&	startedActions = sceneItemAnimation.getStartedActions();
		if (startedActions.hasInstance())
			// Perform
			perform(*startedActions);
	}
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerAnimation::unload()
//----------------------------------------------------------------------------------------------------------------------
{
	// Unload animation
//	if (mInternals->mCelAnimationPlayer != nil)
//		mInternals->mCelAnimationPlayer->unload();
	if (mInternals == nil) (void) mInternals;
	else if (mInternals->mKeyframeAnimationPlayer != nil)
		mInternals->mKeyframeAnimationPlayer->unload();
	
	// Unload audio
	mInternals->mMediaPlayer = OI<CMediaPlayer>();
}

//----------------------------------------------------------------------------------------------------------------------
const OV<UniversalTimeInterval>& CSceneItemPlayerAnimation::getStartTimeInterval() const
//----------------------------------------------------------------------------------------------------------------------
{
	return getSceneItemAnimation().getStartTimeInterval();
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerAnimation::reset()
//----------------------------------------------------------------------------------------------------------------------
{
	// Reset animation
//	if (mInternals->mCelAnimationPlayer != nil)
//		mInternals->mCelAnimationPlayer->reset(!getSceneItemAnimation().getStartTimeInterval().hasValue());
	if (mInternals == nil) (void) mInternals;
	else if (mInternals->mKeyframeAnimationPlayer != nil)
		mInternals->mKeyframeAnimationPlayer->reset(!getSceneItemAnimation().getStartTimeInterval().hasValue());

	// Check for audio
	if (mInternals->mMediaPlayer.hasInstance())
		// Reset
		mInternals->mMediaPlayer->reset();

	// Reset the rest
	mInternals->mIsStarted = false;
	mInternals->mCurrentTimeInterval = 0.0;

	// Super
	CSceneItemPlayer::reset();
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerAnimation::update(UniversalTimeInterval deltaTimeInterval, bool isRunning)
//----------------------------------------------------------------------------------------------------------------------
{
	if (!mInternals->mIsStarted) {
		mInternals->mCurrentTimeInterval += deltaTimeInterval;
		if (mInternals->mCurrentTimeInterval >= getSceneItemAnimation().getStartTimeInterval().getValue(0.0)) {
			// Start
			mInternals->mIsStarted = true;
			
			// Finish loading
//			if (mInternals->mCelAnimationPlayer != nil)
//				mInternals->mCelAnimationPlayer->finishLoading();
			if (mInternals == nil) (void) mInternals;
			else if (mInternals->mKeyframeAnimationPlayer != nil)
				mInternals->mKeyframeAnimationPlayer->finishLoading();

			// Check for audio
			if (mInternals->mMediaPlayer.hasInstance())
				// Start
				mInternals->mMediaPlayer->play();

			// Started action
			const	OI<CActions>&	startedActions = getSceneItemAnimation().getStartedActions();
			if (startedActions.hasInstance())
				perform(*startedActions);
		}
	}
	
	if (mInternals->mIsStarted) {
//		if (mInternals->mCelAnimationPlayer != nil)
//			// Update
//			mInternals->mCelAnimationPlayer->update(deltaTimeInterval, isRunning);
		if (mInternals == nil) (void) mInternals;
		else if (mInternals->mKeyframeAnimationPlayer != nil)
			// Update
			mInternals->mKeyframeAnimationPlayer->update(deltaTimeInterval, isRunning);
	}
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerAnimation::render(CGPU& gpu, const SGPURenderObjectRenderInfo& renderInfo) const
//----------------------------------------------------------------------------------------------------------------------
{
//	if (mInternals->mCelAnimationPlayer != nil)
//		mInternals->mCelAnimationPlayer->render(gpu, offset);
	if (mInternals == nil) (void) mInternals;
	else if (mInternals->mKeyframeAnimationPlayer != nil)
		mInternals->mKeyframeAnimationPlayer->render(gpu, renderInfo);
}

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
bool CSceneItemPlayerAnimation::getIsFinished() const
//----------------------------------------------------------------------------------------------------------------------
{
//	if (mInternals->mCelAnimationPlayer != nil)
//		return mInternals->mCelAnimationPlayer->getIsFinished();
	if (mInternals == nil) return true;
	else if (mInternals->mKeyframeAnimationPlayer != nil)
		return mInternals->mKeyframeAnimationPlayer->getIsFinished();
	else if (mInternals->mMediaPlayer.hasInstance())
		return mInternals->mMediaPlayer->isPlaying();
	else
		return true;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerAnimation::setAudioGain(Float32 gain)
//----------------------------------------------------------------------------------------------------------------------
{
	// Check for audio
	if (mInternals->mMediaPlayer.hasInstance())
		// Set gain
		mInternals->mMediaPlayer->setAudioGain(gain);
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerAnimation::resetAudioGain()
//----------------------------------------------------------------------------------------------------------------------
{
	// Check for audio
	if (mInternals->mMediaPlayer.hasInstance())
		// Reset gain
		mInternals->mMediaPlayer->setAudioGain(getSceneItemAnimation().getAudioInfo()->getGain());
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerAnimation::setCommonTexturesSceneItemPlayerAnimation(
		const CSceneItemPlayerAnimation& commonTexturesSceneItemPlayerAnimation)
//----------------------------------------------------------------------------------------------------------------------
{
//	if (mInternals->mCelAnimationPlayer != nil)
//		mInternals->mCelAnimationPlayer->setCommonTexturesCelAnimationPlayer(
//				*commonTexturesSceneItemPlayerAnimation.mInternals->mCelAnimationPlayer);
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerAnimation::setSceneItemPlayerAnimationProcsInfo(
		const SSceneItemPlayerAnimationProcsInfo& sceneItemPlayerAnimationProcsInfo)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mSceneItemPlayerAnimationProcsInfo = &sceneItemPlayerAnimationProcsInfo;
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - Local proc definitions

////----------------------------------------------------------------------------------------------------------------------
//ECelAnimationPlayerFinishedAction sCelAnimationPlayerGetFinishedActionProc(CCelAnimationPlayer& celAnimationPlayer,
//		UInt32 currentLoopCount, void* userData)
////----------------------------------------------------------------------------------------------------------------------
//{
//	CSceneItemPlayerAnimationInternals*	internals = (CSceneItemPlayerAnimationInternals*) userData;
//
//	if ((internals->mSceneItemPlayerAnimationProcsInfo != nil) &&
//			(internals->mSceneItemPlayerAnimationProcsInfo->mShouldLoopProc != nil))
//		// Ask
//		return internals->mSceneItemPlayerAnimationProcsInfo->mShouldLoopProc(internals->mSceneItemPlayerAnimation,
//				currentLoopCount,
//				internals->mSceneItemPlayerAnimationProcsInfo->mUserData) ?
//						kCelAnimationPlayerFinishedActionLoop : kCelAnimationPlayerFinishedActionFinish;
//	else {
//		// Figure
//		const	CSceneItemAnimation&	sceneItemAnimation =
//												internals->mSceneItemPlayerAnimation.getSceneItemAnimation();
//		if (sceneItemAnimation.getLoopCount() == kSceneItemAnimationLoopForever)
//			// Loop forever
//			return kCelAnimationPlayerFinishedActionLoop;
//		else if (sceneItemAnimation.getLoopCount() == kSceneItemAnimationLoopHoldLastFrame)
//			// Hold last frame
//			return kCelAnimationPlayerFinishedActionHoldLastFrame;
//		else
//			// Process regular loop count
//			return (currentLoopCount < sceneItemAnimation.getLoopCount()) ?
//					kCelAnimationPlayerFinishedActionLoop : kCelAnimationPlayerFinishedActionFinish;
//	}
//}

////----------------------------------------------------------------------------------------------------------------------
//void sCelAnimationPlayerFinishedProc(CCelAnimationPlayer& celAnimationPlayer, void* userData)
////----------------------------------------------------------------------------------------------------------------------
//{
//	CSceneItemPlayerAnimationInternals*	internals = (CSceneItemPlayerAnimationInternals*) userData;
//
//	CActions*	actions =
//							internals->mSceneItemPlayerAnimation.getSceneItemAnimation().getFinishedActionsOrNil();
//	if (actions != nil)
//		internals->mSceneItemPlayerAnimation.perform(*actions);
//}

//----------------------------------------------------------------------------------------------------------------------
bool sKeyframeAnimationPlayerShouldLoopProc(CKeyframeAnimationPlayer& keyframeAnimationPlayer, UInt32 currentLoopCount,
		void* userData)
//----------------------------------------------------------------------------------------------------------------------
{
	CSceneItemPlayerAnimationInternals*	internals = (CSceneItemPlayerAnimationInternals*) userData;

	if ((internals->mSceneItemPlayerAnimationProcsInfo != nil) &&
			internals->mSceneItemPlayerAnimationProcsInfo->canPerformShouldLoop())
		// Ask
		return internals->mSceneItemPlayerAnimationProcsInfo->shouldLoop(internals->mSceneItemPlayerAnimation,
				currentLoopCount);
	else {
		// Figure
		const	CSceneItemAnimation&	sceneItemAnimation =
												internals->mSceneItemPlayerAnimation.getSceneItemAnimation();
		return (sceneItemAnimation.getLoopCount() == kSceneItemAnimationLoopForever) ||
				(currentLoopCount < sceneItemAnimation.getLoopCount());
	}
}

//----------------------------------------------------------------------------------------------------------------------
void sKeyframeAnimationPlayerFinishedProc(CKeyframeAnimationPlayer& keyframeAnimationPlayer, void* userData)
//----------------------------------------------------------------------------------------------------------------------
{
	CSceneItemPlayerAnimationInternals*	internals = (CSceneItemPlayerAnimationInternals*) userData;

	const	OI<CActions>&	actions = internals->mSceneItemPlayerAnimation.getSceneItemAnimation().getFinishedActions();
	if (actions.hasInstance())
		internals->mSceneItemPlayerAnimation.perform(*actions);
}

//----------------------------------------------------------------------------------------------------------------------
void sKeyframeAnimationPlayerActionsHandlerProc(CKeyframeAnimationPlayer& keyframeAnimationPlayer,
		const CActions& actions, void* userData)
//----------------------------------------------------------------------------------------------------------------------
{
	CSceneItemPlayerAnimationInternals*	internals = (CSceneItemPlayerAnimationInternals*) userData;

	internals->mSceneItemPlayerAnimation.perform(actions);
}
