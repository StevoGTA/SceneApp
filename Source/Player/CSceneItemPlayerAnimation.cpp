//----------------------------------------------------------------------------------------------------------------------
//	CSceneItemPlayerAnimation.cpp			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CSceneItemPlayerAnimation.h"

//#include "CCelAnimationPlayer.h"
#include "CKeyframeAnimationPlayer.h"
#include "CSceneAppMediaEngine.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CSceneItemPlayerAnimation::Internals

class CSceneItemPlayerAnimation::Internals {
	public:
		Internals(CSceneItemPlayerAnimation& sceneItemPlayerAnimation,
				const SSceneAppResourceManagementInfo& sceneAppResourceManagementInfo) :
			mSceneItemPlayerAnimation(sceneItemPlayerAnimation),
					mSceneAppMediaEngine(sceneAppResourceManagementInfo.mSceneAppMediaEngine),
					mKeyframeAnimationPlayer(nil),
					mIsStarted(false), mCurrentTimeInterval(0.0), mSceneItemPlayerAnimationProcs(nil),
//					mCelAnimationPlayerProcsInfo(celAnimationPlayerGetFinishedAction, nil,
//							celAnimationPlayerFinished, nil, this),
					mKeyframeAnimationPlayerProcsInfo(
							(CKeyframeAnimationPlayer::Procs::ShouldLoopProc) keyframeAnimationPlayerShouldLoop,
							nil,
							(CKeyframeAnimationPlayer::Procs::DidFinishProc) keyframeAnimationPlayerDidFinish,
							(CKeyframeAnimationPlayer::Procs::PerformActionsProc) keyframeAnimationPlayerPerformActions,
							this)
			{}
		~Internals()
			{
//				Delete(mCelAnimationPlayer);
				Delete(mKeyframeAnimationPlayer);
			}

//		static	ECelAnimationPlayerFinishedAction	celAnimationPlayerGetFinishedAction(
//															CCelAnimationPlayer& celAnimationPlayer,
//															UInt32 currentLoopCount, Internals* internals)
//														{
//															if ((internals->mSceneItemPlayerAnimationProcs != nil) &&
//																	(internals->mSceneItemPlayerAnimationProcs->mShouldLoopProc != nil))
//																// Ask
//																return internals->mSceneItemPlayerAnimationProcs->mShouldLoopProc(internals->mSceneItemPlayerAnimation,
//																		currentLoopCount,
//																		internals->mSceneItemPlayerAnimationProcs->mUserData) ?
//																				kCelAnimationPlayerFinishedActionLoop : kCelAnimationPlayerFinishedActionFinish;
//															else {
//																// Figure
//																const	CSceneItemAnimation&	sceneItemAnimation =
//																										internals->mSceneItemPlayerAnimation.getSceneItemAnimation();
//																if (sceneItemAnimation.getLoopCount() == kSceneItemAnimationLoopForever)
//																	// Loop forever
//																	return kCelAnimationPlayerFinishedActionLoop;
//																else if (sceneItemAnimation.getLoopCount() == kSceneItemAnimationLoopHoldLastFrame)
//																	// Hold last frame
//																	return kCelAnimationPlayerFinishedActionHoldLastFrame;
//																else
//																	// Process regular loop count
//																	return (currentLoopCount < sceneItemAnimation.getLoopCount()) ?
//																			kCelAnimationPlayerFinishedActionLoop : kCelAnimationPlayerFinishedActionFinish;
//															}
//														}
//		static	void								celAnimationPlayerFinished(
//															CCelAnimationPlayer& celAnimationPlayer,
//															Internals* internals)
//														{
//														}

		static	bool								keyframeAnimationPlayerShouldLoop(
															CKeyframeAnimationPlayer& keyframeAnimationPlayer,
															UInt32 currentLoopCount, Internals* internals)
														{
															if ((internals->mSceneItemPlayerAnimationProcs != nil) &&
																	internals->mSceneItemPlayerAnimationProcs->
																			canPerformShouldLoop())
																// Ask
																return internals->mSceneItemPlayerAnimationProcs->
																		shouldLoop(internals->mSceneItemPlayerAnimation,
																				currentLoopCount);
															else {
																// Figure
																const	CSceneItemAnimation&	sceneItemAnimation =
																										internals->
																												mSceneItemPlayerAnimation
																														.getSceneItemAnimation();
																return (sceneItemAnimation.getLoopCount() ==
																				CSceneItemAnimation::kLoopForever) ||
																		(currentLoopCount <
																				sceneItemAnimation.getLoopCount());
															}
														}
		static	void								keyframeAnimationPlayerDidFinish(
															CKeyframeAnimationPlayer& keyframeAnimationPlayer,
															Internals* internals)
														{
															const	OI<CActions>&	actions =
																							internals->
																									mSceneItemPlayerAnimation
																											.getSceneItemAnimation()
																											.getFinishedActions();
															if (actions.hasInstance())
																internals->mSceneItemPlayerAnimation.perform(*actions);
														}
		static	void								keyframeAnimationPlayerPerformActions(
															CKeyframeAnimationPlayer& keyframeAnimationPlayer,
															const CActions& actions, Internals* internals)
														{ internals->mSceneItemPlayerAnimation.perform(actions); }

				CSceneItemPlayerAnimation&			mSceneItemPlayerAnimation;
				CSceneAppMediaEngine&				mSceneAppMediaEngine;

//				CCelAnimationPlayer*				mCelAnimationPlayer;
				CKeyframeAnimationPlayer*			mKeyframeAnimationPlayer;
				OI<CMediaPlayer>					mMediaPlayer;

				bool								mIsStarted;
				UniversalTimeInterval				mCurrentTimeInterval;

		const	CSceneItemPlayerAnimation::Procs*	mSceneItemPlayerAnimationProcs;
//				SCelAnimationPlayerProcsInfo		mCelAnimationPlayerProcsInfo;
				CKeyframeAnimationPlayer::Procs		mKeyframeAnimationPlayerProcsInfo;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneItemPlayerAnimation

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
CSceneItemPlayerAnimation::CSceneItemPlayerAnimation(const CSceneItemAnimation& sceneItemAnimation,
		const SSceneAppResourceManagementInfo& sceneAppResourceManagementInfo,
		const CSceneItemPlayer::Procs& sceneItemPlayerProcs) :
		CSceneItemPlayer(sceneItemAnimation, sceneItemPlayerProcs)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new Internals(*this, sceneAppResourceManagementInfo);

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

	// Check audio info
	const	OI<CAudioInfo>&	audioInfo = sceneItemAnimation.getAudioInfo();
	if (audioInfo.hasInstance())
		// Setup media player
		mInternals->mMediaPlayer = mInternals->mSceneAppMediaEngine.getMediaPlayer(*audioInfo);

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
	
	// Unload media player
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

	// Check for media player
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
	// Check if started
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
void CSceneItemPlayerAnimation::render(CGPU& gpu, const CGPURenderObject::RenderInfo& renderInfo) const
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
void CSceneItemPlayerAnimation::setSceneItemPlayerAnimationProcs(const Procs& procs)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mSceneItemPlayerAnimationProcs = &procs;
}
