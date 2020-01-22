//----------------------------------------------------------------------------------------------------------------------
//	CSceneItemPlayerAnimation.cpp			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CSceneItemPlayerAnimation.h"

//#include "CCelAnimationPlayer.h"
#include "CKeyframeAnimationPlayer.h"

#include "CAudioOutputTrackCache.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CSceneItemPlayerAnimationInternals

class CSceneItemPlayerAnimationInternals {
	public:
		CSceneItemPlayerAnimationInternals(const SSceneItemPlayerProcsInfo& sceneItemPlayerProcsInfo,
				CSceneItemPlayerAnimation& sceneItemPlayerAnimation);
		~CSceneItemPlayerAnimationInternals();

		CSceneItemPlayerAnimation&					mSceneItemPlayerAnimation;

		CAudioOutputTrackReference*					mAudioOutputTrackReference;
//		CCelAnimationPlayer*						mCelAnimationPlayer;
		CKeyframeAnimationPlayer*					mKeyframeAnimationPlayer;

		bool										mIsStarted;
		bool										mIsAudioPlaying;
		UniversalTimeInterval						mCurrentTimeInterval;

		const	SSceneItemPlayerAnimationProcsInfo*	mSceneItemPlayerAnimationProcsInfo;
//				SCelAnimationPlayerProcsInfo		mCelAnimationPlayerProcsInfo;
				SKeyframeAnimationPlayerProcsInfo	mKeyframeAnimationPlayerProcsInfo;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - Local proc declarations

//static	CSceneItemPlayer*					sCreateSceneItemPlayer(const CSceneItem& sceneItem,
//													const SSceneAppResourceManagementInfo&
//															sceneAppResourceManagementInfo,
//													const SSceneItemPlayerProcsInfo& sceneItemPlayerProcsInfo,
//													bool makeCopy);

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
static	void								sKeyframeAnimationPlayerActionArrayHandlerProc(
													CKeyframeAnimationPlayer& keyframeAnimationPlayer,
													const CActionArray& actionArray, void* userData);

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - Register Scene Item Player

//REGISTER_SCENE_ITEM_PLAYER(CSceneItemPlayerAnimation, sCreateSceneItemPlayer, CSceneItemAnimation::mType);

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneItemPlayerAnimationInternals

//----------------------------------------------------------------------------------------------------------------------
CSceneItemPlayerAnimationInternals::CSceneItemPlayerAnimationInternals(
		const SSceneItemPlayerProcsInfo& sceneItemPlayerProcsInfo, CSceneItemPlayerAnimation& sceneItemPlayerAnimation)
		: mSceneItemPlayerAnimation(sceneItemPlayerAnimation),
//		mCelAnimationPlayerProcsInfo(sCelAnimationPlayerGetFinishedActionProc, nil, sCelAnimationPlayerFinishedProc,
//				nil, this),
		mKeyframeAnimationPlayerProcsInfo(sKeyframeAnimationPlayerShouldLoopProc, nil,
				sKeyframeAnimationPlayerFinishedProc, sKeyframeAnimationPlayerActionArrayHandlerProc, this)
//----------------------------------------------------------------------------------------------------------------------
{
	mAudioOutputTrackReference = nil;
//	mCelAnimationPlayer = nil;
	mKeyframeAnimationPlayer = nil;

	mIsStarted = false;
	mIsAudioPlaying = false;
	mCurrentTimeInterval = 0.0;

	mSceneItemPlayerAnimationProcsInfo = nil;
}

//----------------------------------------------------------------------------------------------------------------------
CSceneItemPlayerAnimationInternals::~CSceneItemPlayerAnimationInternals()
//----------------------------------------------------------------------------------------------------------------------
{
	DisposeOf(mAudioOutputTrackReference);
//	DisposeOf(mCelAnimationPlayer);
	DisposeOf(mKeyframeAnimationPlayer);
}

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
	mInternals = new CSceneItemPlayerAnimationInternals(sceneItemPlayerProcsInfo, *this);

//	if (sceneItemAnimation.getCelAnimationInfoOrNil() != nil)
//		mInternals->mCelAnimationPlayer =
//				new CCelAnimationPlayer(*getSceneItemAnimation().getCelAnimationInfoOrNil(),
//						sceneAppResourceManagementInfo, mInternals->mCelAnimationPlayerProcsInfo, 0.0);
	OR<CKeyframeAnimationInfo>	keyframeAnimationInfo = sceneItemAnimation.getKeyframeAnimationInfo();
	if (mInternals == nil) (void) mInternals;
	else if (keyframeAnimationInfo.hasReference())
		mInternals->mKeyframeAnimationPlayer =
				new CKeyframeAnimationPlayer(*keyframeAnimationInfo, sceneAppResourceManagementInfo,
						mInternals->mKeyframeAnimationPlayerProcsInfo, 0.0);
}

//----------------------------------------------------------------------------------------------------------------------
CSceneItemPlayerAnimation::~CSceneItemPlayerAnimation()
//----------------------------------------------------------------------------------------------------------------------
{
	DisposeOf(mInternals);
}

// MARK: CSceneItemPlayer methods

//----------------------------------------------------------------------------------------------------------------------
S2DRect32 CSceneItemPlayerAnimation::getCurrentScreenRect() const
//----------------------------------------------------------------------------------------------------------------------
{
//	if (mInternals->mCelAnimationPlayer != nil)
//#warning finish
//		return S2DRect32::mZero;
//	else
		return mInternals->mKeyframeAnimationPlayer->getScreenRect();
}

//----------------------------------------------------------------------------------------------------------------------
CActionArray CSceneItemPlayerAnimation::getAllActions() const
//----------------------------------------------------------------------------------------------------------------------
{
//	if (mInternals->mCelAnimationPlayer != nil)
//		return CActionArray();
	if (mInternals == nil) return CActionArray();
	else if (mInternals->mKeyframeAnimationPlayer != nil)
		return mInternals->mKeyframeAnimationPlayer->getAllActions();
	else
		return CActionArray();
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerAnimation::load()
//----------------------------------------------------------------------------------------------------------------------
{
	// Load animation
//	if (mInternals->mCelAnimationPlayer != nil)
//		mInternals->mCelAnimationPlayer->load(
//				getSceneItemAnimation().getStartTimeInterval() == kSceneItemStartTimeStartAtLoad);
	if (mInternals == nil) (void) mInternals;
	else if (mInternals->mKeyframeAnimationPlayer != nil)
		mInternals->mKeyframeAnimationPlayer->load(
				getSceneItemAnimation().getStartTimeInterval() == kSceneItemStartTimeStartAtLoad);
	
	// Load audio
	OR<CAudioInfo>	audioInfo = getSceneItemAnimation().getAudioInfo();
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
//						eGetURLForResourceFilename(audioInfo->getResourceFilename()),
//								audioInfo->getLoopCount(), options);
//		mInternals->mAudioOutputTrackReference->setGain(audioInfo->getGain());
	}

	if (getSceneItemAnimation().getStartTimeInterval() == kSceneItemStartTimeStartAtLoad) {
		// Start
		mInternals->mIsStarted = true;

		// Start audio
//		if (mInternals->mAudioOutputTrackReference != nil) {
//			mInternals->mAudioOutputTrackReference->setLoopCount(audioInfo->getLoopCount());
//			mInternals->mAudioOutputTrackReference->play();
//			mInternals->mIsAudioPlaying = true;
//		}

		// Started action
		OR<CActionArray>	startedActionArray = getSceneItemAnimation().getStartedActionArray();
		if (startedActionArray.hasReference())
			perform(*startedActionArray);
	}
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerAnimation::finishLoading()
//----------------------------------------------------------------------------------------------------------------------
{
//	if (mInternals->mCelAnimationPlayer != nil)
//		mInternals->mCelAnimationPlayer->finishLoading();
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
	if (mInternals->mIsAudioPlaying)
		mInternals->mAudioOutputTrackReference->reset();
	DisposeOf(mInternals->mAudioOutputTrackReference);
}

//----------------------------------------------------------------------------------------------------------------------
bool CSceneItemPlayerAnimation::getIsFullyLoaded() const
//----------------------------------------------------------------------------------------------------------------------
{
//	if (mInternals->mCelAnimationPlayer != nil)
//		return mInternals->mCelAnimationPlayer->getIsFullyLoaded();
	if (mInternals == nil) return true;
	else if (mInternals->mKeyframeAnimationPlayer != nil)
		return mInternals->mKeyframeAnimationPlayer->getIsFullyLoaded();
	else
		return true;
}

//----------------------------------------------------------------------------------------------------------------------
UniversalTimeInterval CSceneItemPlayerAnimation::getStartTimeInterval() const
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
//		mInternals->mCelAnimationPlayer->reset(
//				getSceneItemAnimation().getStartTimeInterval() == kSceneItemStartTimeStartAtLoad);
	if (mInternals == nil) (void) mInternals;
	else if (mInternals->mKeyframeAnimationPlayer != nil)
		mInternals->mKeyframeAnimationPlayer->reset(
				getSceneItemAnimation().getStartTimeInterval() == kSceneItemStartTimeStartAtLoad);
	
	// Reset audio
	if (mInternals->mAudioOutputTrackReference != nil)
		mInternals->mAudioOutputTrackReference->reset();
	mInternals->mIsAudioPlaying = false;

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
		if (mInternals->mCurrentTimeInterval >= getSceneItemAnimation().getStartTimeInterval()) {
			// Start
			mInternals->mIsStarted = true;
			
			// Finish loading
//			if (mInternals->mCelAnimationPlayer != nil)
//				mInternals->mCelAnimationPlayer->finishLoading();
			if (mInternals == nil) (void) mInternals;
			else if (mInternals->mKeyframeAnimationPlayer != nil)
				mInternals->mKeyframeAnimationPlayer->finishLoading();
			
			// Start audio
			if (mInternals->mAudioOutputTrackReference != nil) {
				mInternals->mAudioOutputTrackReference->setLoopCount(
						getSceneItemAnimation().getAudioInfo()->getLoopCount());
				if (getSceneItemAnimation().getAudioInfo()->getOptions() & kAudioInfoOptionsStopBeforePlay)
					mInternals->mAudioOutputTrackReference->reset();
				mInternals->mAudioOutputTrackReference->play();
				mInternals->mIsAudioPlaying = true;
			}

			// Started action
			OR<CActionArray>	startedActionArray = getSceneItemAnimation().getStartedActionArray();
			if (startedActionArray.hasReference())
				perform(*startedActionArray);
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
void CSceneItemPlayerAnimation::render(CGPU& gpu, const S2DPoint32& offset)
//----------------------------------------------------------------------------------------------------------------------
{
//	if (mInternals->mCelAnimationPlayer != nil)
//		mInternals->mCelAnimationPlayer->render(gpu, offset);
	if (mInternals == nil) (void) mInternals;
	else if (mInternals->mKeyframeAnimationPlayer != nil)
		mInternals->mKeyframeAnimationPlayer->render(gpu, offset);
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
	else if (mInternals->mAudioOutputTrackReference != nil)
		return !mInternals->mAudioOutputTrackReference->isPlaying();
	else
		return true;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerAnimation::setAudioGain(Float32 gain)
//----------------------------------------------------------------------------------------------------------------------
{
	if (mInternals->mAudioOutputTrackReference != nil)
		mInternals->mAudioOutputTrackReference->setGain(gain);
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerAnimation::resetAudioGain()
//----------------------------------------------------------------------------------------------------------------------
{
	if (mInternals->mAudioOutputTrackReference != nil)
		mInternals->mAudioOutputTrackReference->setGain(getSceneItemAnimation().getAudioInfo()->getGain());
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
//CSceneItemPlayer* sCreateSceneItemPlayer(const CSceneItem& sceneItem,
//		const SSceneAppResourceManagementInfo& sceneAppResourceManagementInfo,
//		const SSceneItemPlayerProcsInfo& sceneItemPlayerProcsInfo, bool makeCopy)
////----------------------------------------------------------------------------------------------------------------------
//{
//	return new CSceneItemPlayerAnimation(*((CSceneItemAnimation*) &sceneItem), sceneAppResourceManagementInfo,
//			sceneItemPlayerProcsInfo, makeCopy);
//}

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
//	CActionArray*	actionArray =
//							internals->mSceneItemPlayerAnimation.getSceneItemAnimation().getFinishedActionArrayOrNil();
//	if (actionArray != nil)
//		internals->mSceneItemPlayerAnimation.perform(*actionArray);
//}

//----------------------------------------------------------------------------------------------------------------------
bool sKeyframeAnimationPlayerShouldLoopProc(CKeyframeAnimationPlayer& keyframeAnimationPlayer, UInt32 currentLoopCount,
		void* userData)
//----------------------------------------------------------------------------------------------------------------------
{
	CSceneItemPlayerAnimationInternals*	internals = (CSceneItemPlayerAnimationInternals*) userData;

	if ((internals->mSceneItemPlayerAnimationProcsInfo != nil) &&
			(internals->mSceneItemPlayerAnimationProcsInfo->mShouldLoopProc != nil))
		// Ask
		return internals->mSceneItemPlayerAnimationProcsInfo->mShouldLoopProc(internals->mSceneItemPlayerAnimation,
				currentLoopCount, internals->mSceneItemPlayerAnimationProcsInfo->mUserData);
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

	OR<CActionArray>	actionArray =
								internals->mSceneItemPlayerAnimation.getSceneItemAnimation().getFinishedActionArray();
	if (actionArray.hasReference())
		internals->mSceneItemPlayerAnimation.perform(*actionArray);
}

//----------------------------------------------------------------------------------------------------------------------
void sKeyframeAnimationPlayerActionArrayHandlerProc(CKeyframeAnimationPlayer& keyframeAnimationPlayer,
		const CActionArray& actionArray, void* userData)
//----------------------------------------------------------------------------------------------------------------------
{
	CSceneItemPlayerAnimationInternals*	internals = (CSceneItemPlayerAnimationInternals*) userData;

	internals->mSceneItemPlayerAnimation.perform(actionArray);
}
