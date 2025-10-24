//----------------------------------------------------------------------------------------------------------------------
//	CSceneItemPlayerAnimation.cpp			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CSceneItemPlayerAnimation.h"

#include "CKeyframeAnimationPlayer.h"
#include "CSceneAppMediaEngine.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CSceneItemPlayerAnimation::Internals

class CSceneItemPlayerAnimation::Internals {
	public:
													Internals(CSceneItemPlayerAnimation& sceneItemPlayerAnimation,
															const SSceneAppResourceManagementInfo&
																	sceneAppResourceManagementInfo) :
														mSceneItemPlayerAnimation(sceneItemPlayerAnimation),
																mSceneAppMediaEngine(
																		sceneAppResourceManagementInfo
																				.mSceneAppMediaEngine),
																mKeyframeAnimationPlayer(nil),
																mIsStarted(false), mCurrentTimeInterval(0.0),
																mSceneItemPlayerAnimationProcs(nil),
																mKeyframeAnimationPlayerProcsInfo(
																		(CKeyframeAnimationPlayer::Procs::ShouldLoopProc)
																				keyframeAnimationPlayerShouldLoop,
																		nil,
																		(CKeyframeAnimationPlayer::Procs::DidFinishProc)
																				keyframeAnimationPlayerDidFinish,
																		(CKeyframeAnimationPlayer::Procs::PerformActionsProc)
																				keyframeAnimationPlayerPerformActions,
																		this)
														{}
													~Internals()
														{
															Delete(mKeyframeAnimationPlayer);
														}


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
															const	OV<CActions>&	actions =
																							internals->
																									mSceneItemPlayerAnimation
																											.getSceneItemAnimation()
																											.getFinishedActions();
															if (actions.hasValue())
																internals->mSceneItemPlayerAnimation.perform(*actions);
														}
		static	void								keyframeAnimationPlayerPerformActions(
															CKeyframeAnimationPlayer& keyframeAnimationPlayer,
															const CActions& actions, Internals* internals)
														{ internals->mSceneItemPlayerAnimation.perform(actions); }

				CSceneItemPlayerAnimation&			mSceneItemPlayerAnimation;
				CSceneAppMediaEngine&				mSceneAppMediaEngine;

				CKeyframeAnimationPlayer*			mKeyframeAnimationPlayer;
				OI<CMediaPlayer>					mMediaPlayer;

				bool								mIsStarted;
				UniversalTimeInterval				mCurrentTimeInterval;

		const	CSceneItemPlayerAnimation::Procs*	mSceneItemPlayerAnimationProcs;
				CKeyframeAnimationPlayer::Procs		mKeyframeAnimationPlayerProcsInfo;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneItemPlayerAnimation

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
CSceneItemPlayerAnimation::CSceneItemPlayerAnimation(CSceneItemAnimation& sceneItemAnimation,
		const SSceneAppResourceManagementInfo& sceneAppResourceManagementInfo,
		const CSceneItemPlayer::Procs& sceneItemPlayerProcs) :
		CSceneItemPlayer(sceneItemAnimation, sceneItemPlayerProcs)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new Internals(*this, sceneAppResourceManagementInfo);

	if (mInternals == nil) (void) mInternals;
	else if (sceneItemAnimation.getKeyframeAnimationInfo().hasValue())
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
	if (mInternals == nil) (void) mInternals;
	else if (mInternals->mKeyframeAnimationPlayer != nil)
		mInternals->mKeyframeAnimationPlayer->load(gpu, !sceneItemAnimation.getStartTimeInterval().hasValue());

	// Check audio info
	const	OV<CAudioInfo>&	audioInfo = sceneItemAnimation.getAudioInfo();
	if (audioInfo.hasValue())
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
		const	OV<CActions>&	startedActions = sceneItemAnimation.getStartedActions();
		if (startedActions.hasValue())
			// Perform
			perform(*startedActions);
	}
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerAnimation::unload()
//----------------------------------------------------------------------------------------------------------------------
{
	// Unload animation
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
void CSceneItemPlayerAnimation::update(CGPU& gpu, UniversalTimeInterval deltaTimeInterval, bool isRunning)
//----------------------------------------------------------------------------------------------------------------------
{
	// Check if started
	if (!mInternals->mIsStarted) {
		mInternals->mCurrentTimeInterval += deltaTimeInterval;
		if (mInternals->mCurrentTimeInterval >= getSceneItemAnimation().getStartTimeInterval().getValue(0.0)) {
			// Start
			mInternals->mIsStarted = true;
			
			// Finish loading
			if (mInternals == nil) (void) mInternals;
			else if (mInternals->mKeyframeAnimationPlayer != nil)
				mInternals->mKeyframeAnimationPlayer->finishLoading();

			// Check for audio
			if (mInternals->mMediaPlayer.hasInstance())
				// Start
				mInternals->mMediaPlayer->play();

			// Started action
			const	OV<CActions>&	startedActions = getSceneItemAnimation().getStartedActions();
			if (startedActions.hasValue())
				perform(*startedActions);
		}
	}
	
	if (mInternals->mIsStarted) {
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
	if (mInternals == nil) (void) mInternals;
	else if (mInternals->mKeyframeAnimationPlayer != nil)
		mInternals->mKeyframeAnimationPlayer->render(gpu, renderInfo);
}

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
bool CSceneItemPlayerAnimation::getIsFinished() const
//----------------------------------------------------------------------------------------------------------------------
{
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
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerAnimation::setSceneItemPlayerAnimationProcs(const Procs& procs)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mSceneItemPlayerAnimationProcs = &procs;
}
