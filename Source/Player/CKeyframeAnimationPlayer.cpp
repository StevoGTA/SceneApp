//----------------------------------------------------------------------------------------------------------------------
//	CKeyframeAnimationPlayer.cpp			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CKeyframeAnimationPlayer.h"

#include "CSceneAppX.h"

#include "CGPURenderObject2D.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: Local data

const	S2DPoint32	sAnchorPointDefault;
const	S2DPoint32	sScreenPositionPointDefault;
const	Float32		kAngleDegreesDefault = 0.0f;
const	Float32		kAlphaDefault = 1.0f;
const	Float32		kScaleDefault = 1.0f;

static	TPtrArray<CKeyframeAnimationPlayer*>	sDeadPlayerArray;

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CKeyframeAnimationPlayerKeyframe

class CKeyframeAnimationPlayerKeyframe {
	public:
											CKeyframeAnimationPlayerKeyframe(
													const CAnimationKeyframe& animationKeyframe,
													const CKeyframeAnimationPlayerKeyframe*
														previousKeyframeAnimationPlayerKeyframe);
											~CKeyframeAnimationPlayerKeyframe();

		OR<CActionArray>					getActionArray() const;
		UniversalTimeInterval				getDelay() const;
		EAnimationKeyframeTransitionType	getTransitionType() const;

		void								load(const SSceneAppResourceManagementInfo& sceneAppResourceManagementInfo);
		void								finishLoading();
		void								unload();
		bool								getIsFullyLoaded() const;

		void								setSpriteValuesToKeyframeValues();
		void								setSpriteValuesToInterpolatedValues(Float32 transitionFactor,
													const CKeyframeAnimationPlayerKeyframe& nextPlayerKeyframe);

		const	CAnimationKeyframe&	mAnimationKeyframe;

				S2DPoint32			mAnchorPoint;
				S2DPoint32			mScreenPositionPoint;
				Float32				mAngleDegrees;
				Float32				mAlpha;
				Float32				mScale;
				CString				mImageFilename;

				CGPURenderObject2D*	mGPURenderObject2D;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CKeyframeAnimationPlayerInternals

class CKeyframeAnimationPlayerInternals {
	public:
		CKeyframeAnimationPlayerInternals(const CKeyframeAnimationInfo& keyframeAnimationInfo,
				const SSceneAppResourceManagementInfo& sceneAppResourceManagementInfo,
				const SKeyframeAnimationPlayerProcsInfo& keyframeAnimationPlayerProcsInfo, bool makeCopy,
				UniversalTimeInterval startTimeInterval) :
			mPlayerKeyframesArray(true), mSceneAppResourceManagementInfo(sceneAppResourceManagementInfo),
					mKeyframeAnimationPlayerProcsInfo(keyframeAnimationPlayerProcsInfo)
			{
				if (makeCopy) {
					mDisposeKeyframeAnimationInfo = true;
					mKeyframeAnimationInfo = new CKeyframeAnimationInfo(keyframeAnimationInfo);
				} else {
					mDisposeKeyframeAnimationInfo = false;
					mKeyframeAnimationInfo = (CKeyframeAnimationInfo*) &keyframeAnimationInfo;
				}

				mIsStarted = false;
				mIsFinished = false;
				mNeedToAddToDeadPlayerArray = false;
				mCurrentTimeInterval = 0.0;
				mCurrentFrameTimeInterval = 0.0;
				mStartTimeInterval = startTimeInterval;
				mCurrentLoopCount = 0;

				mPreviousPlayerKeyframe = nil;
				mNextPlayerKeyframe = nil;
			}
		~CKeyframeAnimationPlayerInternals()
			{
				if (mDisposeKeyframeAnimationInfo) {
					// Dispose cel animation info
					DisposeOf(mKeyframeAnimationInfo);
				}
			}

				bool											mDisposeKeyframeAnimationInfo;
				CKeyframeAnimationInfo*							mKeyframeAnimationInfo;

				bool											mIsStarted;
				bool											mIsFinished;
				bool											mNeedToAddToDeadPlayerArray;
				UniversalTimeInterval							mCurrentTimeInterval;
				UniversalTimeInterval							mCurrentFrameTimeInterval;
				UniversalTimeInterval							mStartTimeInterval;
				UInt32											mCurrentLoopCount;

				CKeyframeAnimationPlayerKeyframe*				mPreviousPlayerKeyframe;
				CKeyframeAnimationPlayerKeyframe*				mNextPlayerKeyframe;
				TPtrArray<CKeyframeAnimationPlayerKeyframe*>	mPlayerKeyframesArray;

		const	SSceneAppResourceManagementInfo&				mSceneAppResourceManagementInfo;
		const	SKeyframeAnimationPlayerProcsInfo&				mKeyframeAnimationPlayerProcsInfo;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CKeyframeAnimationPlayerKeyframe

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
CKeyframeAnimationPlayerKeyframe::CKeyframeAnimationPlayerKeyframe(const CAnimationKeyframe& animationKeyframe,
		const CKeyframeAnimationPlayerKeyframe* previousKeyframeAnimationPlayerKeyframe) :
		mAnimationKeyframe(animationKeyframe)
//----------------------------------------------------------------------------------------------------------------------
{
	if (animationKeyframe.hasAnchorPoint())
		mAnchorPoint = animationKeyframe.getAnchorPoint();
	else if (previousKeyframeAnimationPlayerKeyframe != nil)
		mAnchorPoint = previousKeyframeAnimationPlayerKeyframe->mAnchorPoint;
	else
		mAnchorPoint = sAnchorPointDefault;
	
	// Screen Position Point
	if (animationKeyframe.hasScreenPositionPoint())
		mScreenPositionPoint = animationKeyframe.getScreenPositionPoint();
	else if (previousKeyframeAnimationPlayerKeyframe != nil)
		mScreenPositionPoint = previousKeyframeAnimationPlayerKeyframe->mScreenPositionPoint;
	else
		mScreenPositionPoint = sScreenPositionPointDefault;
	
	// Angle
	if (animationKeyframe.hasAngleDegrees())
		mAngleDegrees = animationKeyframe.getAngleDegrees();
	else if (previousKeyframeAnimationPlayerKeyframe != nil)
		mAngleDegrees = previousKeyframeAnimationPlayerKeyframe->mAngleDegrees;
	else
		mAngleDegrees = kAngleDegreesDefault;
	
	// Alpha
	if (animationKeyframe.hasAlpha())
		mAlpha = animationKeyframe.getAlpha();
	else if (previousKeyframeAnimationPlayerKeyframe != nil)
		mAlpha = previousKeyframeAnimationPlayerKeyframe->mAlpha;
	else
		mAlpha = kAlphaDefault;
	
	// Scale
	if (animationKeyframe.hasScale())
		mScale = animationKeyframe.getScale();
	else if (previousKeyframeAnimationPlayerKeyframe != nil)
		mScale = previousKeyframeAnimationPlayerKeyframe->mScale;
	else
		mScale = kScaleDefault;

	// Image Filename
	mImageFilename = animationKeyframe.getImageResourceFilename();
	if (mImageFilename.getLength() == 0)
		mImageFilename = previousKeyframeAnimationPlayerKeyframe->mImageFilename;

	// GPU Render Object 2D
	mGPURenderObject2D = nil;
}

//----------------------------------------------------------------------------------------------------------------------
CKeyframeAnimationPlayerKeyframe::~CKeyframeAnimationPlayerKeyframe()
//----------------------------------------------------------------------------------------------------------------------
{
	unload();
}

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
OR<CActionArray> CKeyframeAnimationPlayerKeyframe::getActionArray() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mAnimationKeyframe.getActionArray();
}

//----------------------------------------------------------------------------------------------------------------------
UniversalTimeInterval CKeyframeAnimationPlayerKeyframe::getDelay() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mAnimationKeyframe.getDelay();
}

//----------------------------------------------------------------------------------------------------------------------
EAnimationKeyframeTransitionType CKeyframeAnimationPlayerKeyframe::getTransitionType() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mAnimationKeyframe.getTransitionType();
}

//----------------------------------------------------------------------------------------------------------------------
void CKeyframeAnimationPlayerKeyframe::load(const SSceneAppResourceManagementInfo& sceneAppResourceManagementInfo)
//----------------------------------------------------------------------------------------------------------------------
{
	mGPURenderObject2D =
			new CGPURenderObject2D(
					sceneAppResourceManagementInfo.mGPUTextureManager.gpuTextureReference(
							sceneAppResourceManagementInfo.mCreateByteParcellerProc(mImageFilename), mImageFilename));
}

//----------------------------------------------------------------------------------------------------------------------
void CKeyframeAnimationPlayerKeyframe::finishLoading()
//----------------------------------------------------------------------------------------------------------------------
{
	mGPURenderObject2D->getGPUTextureReference().load();
}

//----------------------------------------------------------------------------------------------------------------------
void CKeyframeAnimationPlayerKeyframe::unload()
//----------------------------------------------------------------------------------------------------------------------
{
	DisposeOf(mGPURenderObject2D);
}

//----------------------------------------------------------------------------------------------------------------------
bool CKeyframeAnimationPlayerKeyframe::getIsFullyLoaded() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mGPURenderObject2D->getGPUTextureReference().getIsLoaded();
}

//----------------------------------------------------------------------------------------------------------------------
void CKeyframeAnimationPlayerKeyframe::setSpriteValuesToKeyframeValues()
//----------------------------------------------------------------------------------------------------------------------
{
	mGPURenderObject2D->setAnchorPoint(mAnchorPoint);
	mGPURenderObject2D->setScreenPositionPoint(mScreenPositionPoint);
	mGPURenderObject2D->setAngleAsDegrees(mAngleDegrees);
	mGPURenderObject2D->setAlpha(mAlpha);
	mGPURenderObject2D->setScale(mScale);
}

//----------------------------------------------------------------------------------------------------------------------
void CKeyframeAnimationPlayerKeyframe::setSpriteValuesToInterpolatedValues(Float32 transitionFactor,
		const CKeyframeAnimationPlayerKeyframe& nextPlayerKeyframe)
//----------------------------------------------------------------------------------------------------------------------
{
	Float32	invertTransitionFactor = 1.0 - transitionFactor, x, y;
	
	// Anchor Point
	x = invertTransitionFactor * mAnchorPoint.mX + transitionFactor * nextPlayerKeyframe.mAnchorPoint.mX;
	y = invertTransitionFactor * mAnchorPoint.mY + transitionFactor * nextPlayerKeyframe.mAnchorPoint.mY;
	mGPURenderObject2D->setAnchorPoint(S2DPoint32(x, y));
	
	// Screen Position Point
	x =
			invertTransitionFactor * mScreenPositionPoint.mX +
					transitionFactor * nextPlayerKeyframe.mScreenPositionPoint.mX;
	y =
			invertTransitionFactor * mScreenPositionPoint.mY +
					transitionFactor * nextPlayerKeyframe.mScreenPositionPoint.mY;
	mGPURenderObject2D->setScreenPositionPoint(S2DPoint32(x, y));
	
	// Angle
	mGPURenderObject2D->setAngleAsDegrees(invertTransitionFactor * mAngleDegrees +
			transitionFactor * nextPlayerKeyframe.mAngleDegrees);
	
	// Alpha
	mGPURenderObject2D->setAlpha(invertTransitionFactor * mAlpha + transitionFactor * nextPlayerKeyframe.mAlpha);
	
	// Scale
	mGPURenderObject2D->setScale(invertTransitionFactor * mScale + transitionFactor * nextPlayerKeyframe.mScale);
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CKeyframeAnimationPlayer

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
CKeyframeAnimationPlayer::CKeyframeAnimationPlayer(const CKeyframeAnimationInfo& keyframeAnimationInfo,
		const SSceneAppResourceManagementInfo& sceneAppResourceManagementInfo,
		const SKeyframeAnimationPlayerProcsInfo& keyframeAnimationPlayerProcsInfo, bool makeCopy,
		UniversalTimeInterval startTimeInterval)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals =
			new CKeyframeAnimationPlayerInternals(keyframeAnimationInfo, sceneAppResourceManagementInfo,
					keyframeAnimationPlayerProcsInfo, makeCopy, startTimeInterval);

	const	TPtrArray<CAnimationKeyframe*>&	array = mInternals->mKeyframeAnimationInfo->getAnimationKeyframesArray();
	CKeyframeAnimationPlayerKeyframe*		previousPlayerKeyframe = nil;
	for (CArrayItemIndex i = 0; i < array.getCount(); i++) {
		CKeyframeAnimationPlayerKeyframe*	playerKeyframe =
													new CKeyframeAnimationPlayerKeyframe(*array[i],
															previousPlayerKeyframe);
		mInternals->mPlayerKeyframesArray += playerKeyframe;
		previousPlayerKeyframe = playerKeyframe;

		if (i == 0)
			mInternals->mPreviousPlayerKeyframe = playerKeyframe;
		else if (i == 1)
			mInternals->mNextPlayerKeyframe = playerKeyframe;
	}
}

//----------------------------------------------------------------------------------------------------------------------
CKeyframeAnimationPlayer::~CKeyframeAnimationPlayer()
//----------------------------------------------------------------------------------------------------------------------
{
	if (mInternals->mNeedToAddToDeadPlayerArray)
		sDeadPlayerArray += this;
		
	DisposeOf(mInternals);
}

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
S2DRect32 CKeyframeAnimationPlayer::getScreenRect()
//----------------------------------------------------------------------------------------------------------------------
{
	// Check if have previous player key frame
	if (mInternals->mPreviousPlayerKeyframe != nil) {
		//
		SGPUTextureSize	gpuTextureSize =
								mInternals->mPreviousPlayerKeyframe->mGPURenderObject2D->getGPUTextureReference().
										getGPUTextureInfo().mGPUTextureSize;

		S2DRect32	rect;
		rect.mOrigin.mX =
				mInternals->mPreviousPlayerKeyframe->mScreenPositionPoint.mX -
						mInternals->mPreviousPlayerKeyframe->mAnchorPoint.mX;
		rect.mOrigin.mY =
				mInternals->mPreviousPlayerKeyframe->mScreenPositionPoint.mY -
						mInternals->mPreviousPlayerKeyframe->mAnchorPoint.mY;
		rect.mSize = S2DSize32(gpuTextureSize.mWidth, gpuTextureSize.mHeight);

		return rect;
	} else
		return S2DRect32();
}

//----------------------------------------------------------------------------------------------------------------------
CActionArray CKeyframeAnimationPlayer::getAllActions() const
//----------------------------------------------------------------------------------------------------------------------
{
	CActionArray	allActionArray;
	for (CArrayItemIndex i = 0; i < mInternals->mPlayerKeyframesArray.getCount(); i++) {
		OR<CActionArray>	actionArray = mInternals->mPlayerKeyframesArray[i]->mAnimationKeyframe.getActionArray();
		if (actionArray.hasReference())
			allActionArray.addFrom(*actionArray);
	}

	return allActionArray;
}

//----------------------------------------------------------------------------------------------------------------------
void CKeyframeAnimationPlayer::load(bool start)
//----------------------------------------------------------------------------------------------------------------------
{
	for (CArrayItemIndex i = 0; i < mInternals->mPlayerKeyframesArray.getCount(); i++)
		mInternals->mPlayerKeyframesArray[i]->load(mInternals->mSceneAppResourceManagementInfo);

	reset(start);
}

//----------------------------------------------------------------------------------------------------------------------
void CKeyframeAnimationPlayer::finishLoading()
//----------------------------------------------------------------------------------------------------------------------
{
	if (mInternals->mPreviousPlayerKeyframe != nil)
		mInternals->mPreviousPlayerKeyframe->finishLoading();
}

//----------------------------------------------------------------------------------------------------------------------
void CKeyframeAnimationPlayer::unload()
//----------------------------------------------------------------------------------------------------------------------
{
	for (CArrayItemIndex i = 0; i < mInternals->mPlayerKeyframesArray.getCount(); i++)
		mInternals->mPlayerKeyframesArray[i]->unload();
}

//----------------------------------------------------------------------------------------------------------------------
bool CKeyframeAnimationPlayer::getIsFullyLoaded() const
//----------------------------------------------------------------------------------------------------------------------
{
	return (mInternals->mPreviousPlayerKeyframe != nil) ?
			mInternals->mPreviousPlayerKeyframe->getIsFullyLoaded() : true;
}

//----------------------------------------------------------------------------------------------------------------------
void CKeyframeAnimationPlayer::reset(bool start)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mIsStarted = false;
	mInternals->mIsFinished = false;
	mInternals->mCurrentTimeInterval = 0.0;
	mInternals->mCurrentFrameTimeInterval = 0.0;
	mInternals->mCurrentLoopCount = 0;

	TPtrArray<CKeyframeAnimationPlayerKeyframe*>&	playerKeyframesArray = mInternals->mPlayerKeyframesArray;
	mInternals->mPreviousPlayerKeyframe = (playerKeyframesArray.getCount() > 0) ? playerKeyframesArray[0] : nil;
	mInternals->mNextPlayerKeyframe = (playerKeyframesArray.getCount() > 1) ? playerKeyframesArray[1] : nil;

	if (start)
		update(0.0, false);
}

//----------------------------------------------------------------------------------------------------------------------
void CKeyframeAnimationPlayer::update(UniversalTimeInterval deltaTimeInterval, bool isRunning)
//----------------------------------------------------------------------------------------------------------------------
{
	if (!mInternals->mIsStarted) {
		mInternals->mCurrentTimeInterval += deltaTimeInterval;
		if (mInternals->mCurrentTimeInterval >= mInternals->mStartTimeInterval)
			mInternals->mIsStarted = true;
	}
	
	if (mInternals->mIsStarted && !mInternals->mIsFinished) {
		if (isRunning || ((mInternals->mPreviousPlayerKeyframe != nil) &&
				(mInternals->mPreviousPlayerKeyframe->getTransitionType() !=
						kAnimationKeyframeTransitionTypeHoldUntilStart)))
			// Update our current time
			mInternals->mCurrentFrameTimeInterval += deltaTimeInterval;

		const	SKeyframeAnimationPlayerProcsInfo&				procsInfo =
																		mInternals->mKeyframeAnimationPlayerProcsInfo;
				TPtrArray<CKeyframeAnimationPlayerKeyframe*>&	playerKeyframesArray =
																		mInternals->mPlayerKeyframesArray;
		if (mInternals->mPreviousPlayerKeyframe != nil) {
			// Get on the correct keyframe
			while (
					// Running or transition is not hold until start
					(isRunning ||
							(mInternals->mPreviousPlayerKeyframe->getTransitionType() !=
									kAnimationKeyframeTransitionTypeHoldUntilStart)) &&
					
					// Delay is not delay forever
					(mInternals->mPreviousPlayerKeyframe->getDelay() != kAnimationKeyframeDelayForever) &&
					
					// Current frame time is after the current frame delay
					(mInternals->mCurrentFrameTimeInterval > mInternals->mPreviousPlayerKeyframe->getDelay())) {
				// Update frame time
				mInternals->mCurrentFrameTimeInterval -= mInternals->mPreviousPlayerKeyframe->getDelay();
				
				// Update keyframe pointers
				mInternals->mPreviousPlayerKeyframe = mInternals->mNextPlayerKeyframe;
				
				// We have an action for this keyframe?
				mInternals->mNeedToAddToDeadPlayerArray = true;
				if (mInternals->mPreviousPlayerKeyframe != nil) {
					OR<CActionArray>	actionArray = mInternals->mPreviousPlayerKeyframe->getActionArray();
					if (actionArray.hasReference() && (procsInfo.mActionArrayHandlerProc != nil))
						procsInfo.mActionArrayHandlerProc(*this, *actionArray, procsInfo.mUserData);
				}

				// Did we die?
				if (sDeadPlayerArray.contains(this)) {
					// Yes
					sDeadPlayerArray -= this;
					
					return;
				}
				mInternals->mNeedToAddToDeadPlayerArray = false;
				
				// We done?
				if (mInternals->mPreviousPlayerKeyframe == nil)
					// Yes					
					return;

				OV<CArrayItemIndex>	index = playerKeyframesArray.getIndexOf(mInternals->mNextPlayerKeyframe);
				if (*index < (playerKeyframesArray.getCount() - 1))
					// Next keyframe please!
					mInternals->mNextPlayerKeyframe = playerKeyframesArray[*index + 1];
				else if ((procsInfo.mShouldLoopProc != nil) &&
						procsInfo.mShouldLoopProc(*this, ++mInternals->mCurrentLoopCount, procsInfo.mUserData)) {
					// Looping
					mInternals->mNextPlayerKeyframe = playerKeyframesArray[0];

// May not be the correct location (untested)
					if (procsInfo.mLoopingProc != nil)
						procsInfo.mLoopingProc(*this, procsInfo.mUserData);
				} else {
					// Just about done
					mInternals->mNextPlayerKeyframe = nil;
					
// May not be the correct location (untested)
					if (procsInfo.mFinishedProc != nil)
						procsInfo.mFinishedProc(*this, procsInfo.mUserData);
				}
			}
			
			// Now what to do!
			Float32	percent, transitionFactor;
			if ((mInternals->mNextPlayerKeyframe != nil) &&
					(mInternals->mPreviousPlayerKeyframe->getTransitionType() ==
							kAnimationKeyframeTransitionTypeLinear)) {
				// Linear between keyframes
				transitionFactor =
						mInternals->mCurrentFrameTimeInterval / mInternals->mPreviousPlayerKeyframe->getDelay();
				mInternals->mPreviousPlayerKeyframe->setSpriteValuesToInterpolatedValues(transitionFactor,
						*mInternals->mNextPlayerKeyframe);
			} else if ((mInternals->mNextPlayerKeyframe != nil) &&
					(mInternals->mPreviousPlayerKeyframe->getTransitionType() ==
							kAnimationKeyframeTransitionTypeEaseIn)) {
				// Linear, then ease in
				percent = mInternals->mCurrentFrameTimeInterval / mInternals->mPreviousPlayerKeyframe->getDelay();
				if (percent <= 0.5f)
					transitionFactor = percent;
				else
					transitionFactor = 3.0f * percent * percent - 2.0f * percent * percent * percent;
				mInternals->mPreviousPlayerKeyframe->setSpriteValuesToInterpolatedValues(transitionFactor,
						*mInternals->mNextPlayerKeyframe);
			} else if ((mInternals->mNextPlayerKeyframe != nil) &&
					(mInternals->mPreviousPlayerKeyframe->getTransitionType() ==
							kAnimationKeyframeTransitionTypeEaseOut)) {
				// Ease out, then linear
				percent = mInternals->mCurrentFrameTimeInterval / mInternals->mPreviousPlayerKeyframe->getDelay();
				if (percent <= 0.5f)
					transitionFactor = 3.0f * percent * percent - 2.0f * percent * percent * percent;
				else
					transitionFactor = percent;
				mInternals->mPreviousPlayerKeyframe->setSpriteValuesToInterpolatedValues(transitionFactor,
						*mInternals->mNextPlayerKeyframe);
			} else if ((mInternals->mNextPlayerKeyframe != nil) &&
					(mInternals->mPreviousPlayerKeyframe->getTransitionType() ==
							kAnimationKeyframeTransitionTypeEaseInEaseOut)) {
				// Ease out, then ease in
				percent = mInternals->mCurrentFrameTimeInterval / mInternals->mPreviousPlayerKeyframe->getDelay();
				transitionFactor = 3.0f * percent * percent - 2.0f * percent * percent * percent;
				mInternals->mPreviousPlayerKeyframe->setSpriteValuesToInterpolatedValues(transitionFactor,
						*mInternals->mNextPlayerKeyframe);
			} else {
				// No next keyframe or no transition or jump between frames.
				// Copy keyframe values to sprite values
				mInternals->mPreviousPlayerKeyframe->setSpriteValuesToKeyframeValues();
			}
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------
void CKeyframeAnimationPlayer::render(CGPU& gpu, const S2DPoint32& offset)
//----------------------------------------------------------------------------------------------------------------------
{
	if (mInternals->mIsStarted && !mInternals->mIsFinished && (mInternals->mPreviousPlayerKeyframe != nil))
		// Draw
		mInternals->mPreviousPlayerKeyframe->mGPURenderObject2D->render(gpu, offset);
}

//----------------------------------------------------------------------------------------------------------------------
bool CKeyframeAnimationPlayer::getIsFinished() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mIsFinished;
}
