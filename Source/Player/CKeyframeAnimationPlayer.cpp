//----------------------------------------------------------------------------------------------------------------------
//	CKeyframeAnimationPlayer.cpp			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CKeyframeAnimationPlayer.h"

#include "CGPURenderObject2D.h"
#include "CImage.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: Local data

const	S2DPointF32	sAnchorPointDefault;
const	S2DPointF32	sScreenPositionPointDefault;
const	Float32		kAngleDegreesDefault = 0.0f;
const	Float32		kAlphaDefault = 1.0f;
const	Float32		kScaleDefault = 1.0f;

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CKeyframeAnimationPlayerKeyframe

class CKeyframeAnimationPlayerKeyframe {
	public:
														CKeyframeAnimationPlayerKeyframe(
																const CAnimationKeyframe& animationKeyframe,
																const OR<CKeyframeAnimationPlayerKeyframe>
																		previousKeyframeAnimationPlayerKeyframe);
														~CKeyframeAnimationPlayerKeyframe();

		const	OI<CActions>&							getActions() const;
		const	OV<UniversalTimeInterval>&				getDelay() const;
		const	OV<EAnimationKeyframeTransitionType>&	getTransitionType() const;

				void									load(CGPU& gpu,
																const SSceneAppResourceManagementInfo&
																		sceneAppResourceManagementInfo);
				void									finishLoading();
				void									unload();

				void									setSpriteValuesToKeyframeValues();
				void									setSpriteValuesToInterpolatedValues(Float32 transitionFactor,
																const CKeyframeAnimationPlayerKeyframe&
																		nextPlayerKeyframe);

		const	CAnimationKeyframe&	mAnimationKeyframe;

				S2DPointF32			mAnchorPoint;
				S2DPointF32			mScreenPositionPoint;
				Float32				mAngleDegrees;
				Float32				mAlpha;
				Float32				mScale;
				CString				mImageFilename;

				CGPURenderObject2D*	mGPURenderObject2D;
};

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
CKeyframeAnimationPlayerKeyframe::CKeyframeAnimationPlayerKeyframe(const CAnimationKeyframe& animationKeyframe,
		const OR<CKeyframeAnimationPlayerKeyframe> previousKeyframeAnimationPlayerKeyframe) :
		mAnimationKeyframe(animationKeyframe), mGPURenderObject2D(nil)
//----------------------------------------------------------------------------------------------------------------------
{
	// Anchor point
	const	OV<S2DPointF32>&	anchorPoint = animationKeyframe.getAnchorPoint();
	if (anchorPoint.hasValue())
		mAnchorPoint = *anchorPoint;
	else if (previousKeyframeAnimationPlayerKeyframe.hasReference())
		mAnchorPoint = previousKeyframeAnimationPlayerKeyframe->mAnchorPoint;
	else
		mAnchorPoint = sAnchorPointDefault;

	// Screen Position Point
	const	OV<S2DPointF32>&	screenPositionPoint = animationKeyframe.getScreenPositionPoint();
	if (screenPositionPoint.hasValue())
		mScreenPositionPoint = *screenPositionPoint;
	else if (previousKeyframeAnimationPlayerKeyframe.hasReference())
		mScreenPositionPoint = previousKeyframeAnimationPlayerKeyframe->mScreenPositionPoint;
	else
		mScreenPositionPoint = sScreenPositionPointDefault;

	// Angle
	const	OV<Float32>&	angleDegrees = animationKeyframe.getAngleDegrees();
	if (angleDegrees.hasValue())
		mAngleDegrees = *angleDegrees;
	else if (previousKeyframeAnimationPlayerKeyframe.hasReference())
		mAngleDegrees = previousKeyframeAnimationPlayerKeyframe->mAngleDegrees;
	else
		mAngleDegrees = kAngleDegreesDefault;

	// Alpha
	const	OV<Float32>&	alpha = animationKeyframe.getAlpha();
	if (alpha.hasValue())
		mAlpha = *alpha;
	else if (previousKeyframeAnimationPlayerKeyframe.hasReference())
		mAlpha = previousKeyframeAnimationPlayerKeyframe->mAlpha;
	else
		mAlpha = kAlphaDefault;

	// Scale
	const	OV<Float32>&	scale = animationKeyframe.getScale();
	if (scale.hasValue())
		mScale = *scale;
	else if (previousKeyframeAnimationPlayerKeyframe.hasReference())
		mScale = previousKeyframeAnimationPlayerKeyframe->mScale;
	else
		mScale = kScaleDefault;

	// Image Filename
	const	OV<CString>&	imageResourceFilename = animationKeyframe.getImageResourceFilename();
	if (imageResourceFilename.hasValue())
		mImageFilename = *imageResourceFilename;
	else
		mImageFilename = previousKeyframeAnimationPlayerKeyframe->mImageFilename;
}

//----------------------------------------------------------------------------------------------------------------------
CKeyframeAnimationPlayerKeyframe::~CKeyframeAnimationPlayerKeyframe()
//----------------------------------------------------------------------------------------------------------------------
{
	unload();
}

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
const OI<CActions>& CKeyframeAnimationPlayerKeyframe::getActions() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mAnimationKeyframe.getActions();
}

//----------------------------------------------------------------------------------------------------------------------
const OV<UniversalTimeInterval>& CKeyframeAnimationPlayerKeyframe::getDelay() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mAnimationKeyframe.getDelay();
}

//----------------------------------------------------------------------------------------------------------------------
const OV<EAnimationKeyframeTransitionType>& CKeyframeAnimationPlayerKeyframe::getTransitionType() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mAnimationKeyframe.getTransitionType();
}

//----------------------------------------------------------------------------------------------------------------------
void CKeyframeAnimationPlayerKeyframe::load(CGPU& gpu,
		const SSceneAppResourceManagementInfo& sceneAppResourceManagementInfo)
//----------------------------------------------------------------------------------------------------------------------
{
	CGPUTextureReference	gpuTextureReference =
									sceneAppResourceManagementInfo.mGPUTextureManager.gpuTextureReference(
											sceneAppResourceManagementInfo.createByteParceller(mImageFilename),
											CImage::getBitmap, mImageFilename);
	gpuTextureReference.finishLoading();

	S2DSizeU16	size = gpuTextureReference.getGPUTexture().getUsedSize();
	S2DRectF32	rect(0.0, 0.0, size.mWidth, size.mHeight);
	mGPURenderObject2D = new CGPURenderObject2D(gpu, rect, rect, gpuTextureReference);
}

//----------------------------------------------------------------------------------------------------------------------
void CKeyframeAnimationPlayerKeyframe::finishLoading()
//----------------------------------------------------------------------------------------------------------------------
{
	mGPURenderObject2D->finishLoading();
}

//----------------------------------------------------------------------------------------------------------------------
void CKeyframeAnimationPlayerKeyframe::unload()
//----------------------------------------------------------------------------------------------------------------------
{
	Delete(mGPURenderObject2D);
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
	Float32	invertTransitionFactor = 1.0f - transitionFactor, x, y;

	// Anchor Point
	x = invertTransitionFactor * mAnchorPoint.mX + transitionFactor * nextPlayerKeyframe.mAnchorPoint.mX;
	y = invertTransitionFactor * mAnchorPoint.mY + transitionFactor * nextPlayerKeyframe.mAnchorPoint.mY;
	mGPURenderObject2D->setAnchorPoint(S2DPointF32(x, y));

	// Screen Position Point
	x =
			invertTransitionFactor * mScreenPositionPoint.mX +
					transitionFactor * nextPlayerKeyframe.mScreenPositionPoint.mX;
	y =
			invertTransitionFactor * mScreenPositionPoint.mY +
					transitionFactor * nextPlayerKeyframe.mScreenPositionPoint.mY;
	mGPURenderObject2D->setScreenPositionPoint(S2DPointF32(x, y));

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
// MARK: - CKeyframeAnimationPlayerInternals

class CKeyframeAnimationPlayerInternals {
	public:
		CKeyframeAnimationPlayerInternals(const CKeyframeAnimationInfo& keyframeAnimationInfo,
				const SSceneAppResourceManagementInfo& sceneAppResourceManagementInfo,
				const SKeyframeAnimationPlayerProcsInfo& keyframeAnimationPlayerProcsInfo,
				const OV<UniversalTimeInterval>& startTimeInterval) :
			mKeyframeAnimationInfo(keyframeAnimationInfo), mIsStarted(false), mIsFinished(false),
					//mNeedToAddToDeadPlayers(false),
					mCurrentTimeInterval(0.0), mCurrentFrameTimeInterval(0.0),
					mStartTimeInterval(startTimeInterval), mCurrentLoopCount(0),
			mSceneAppResourceManagementInfo(sceneAppResourceManagementInfo),
					mKeyframeAnimationPlayerProcsInfo(keyframeAnimationPlayerProcsInfo)
			{}

		const	CKeyframeAnimationInfo&						mKeyframeAnimationInfo;

				bool										mIsStarted;
				bool										mIsFinished;
				UniversalTimeInterval						mCurrentTimeInterval;
				UniversalTimeInterval						mCurrentFrameTimeInterval;
				OV<UniversalTimeInterval>					mStartTimeInterval;
				UInt32										mCurrentLoopCount;

				OR<CKeyframeAnimationPlayerKeyframe>		mPreviousPlayerKeyframe;
				OR<CKeyframeAnimationPlayerKeyframe>		mNextPlayerKeyframe;
				TIArray<CKeyframeAnimationPlayerKeyframe>	mKeyframeAnimationPlayerKeyframes;

				OR<bool>									mHasBeenDeleted;

		const	SSceneAppResourceManagementInfo&			mSceneAppResourceManagementInfo;
		const	SKeyframeAnimationPlayerProcsInfo&			mKeyframeAnimationPlayerProcsInfo;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CKeyframeAnimationPlayer

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
CKeyframeAnimationPlayer::CKeyframeAnimationPlayer(const CKeyframeAnimationInfo& keyframeAnimationInfo,
		const SSceneAppResourceManagementInfo& sceneAppResourceManagementInfo,
		const SKeyframeAnimationPlayerProcsInfo& keyframeAnimationPlayerProcsInfo,
		const OV<UniversalTimeInterval>& startTimeInterval)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals =
			new CKeyframeAnimationPlayerInternals(keyframeAnimationInfo, sceneAppResourceManagementInfo,
					keyframeAnimationPlayerProcsInfo, startTimeInterval);

	const	TArray<CAnimationKeyframe>&				array =
															mInternals->mKeyframeAnimationInfo.
																	getAnimationKeyframesArray();
			OR<CKeyframeAnimationPlayerKeyframe>	previousPlayerKeyframe;
	for (CArray::ItemIndex i = 0; i < array.getCount(); i++) {
		CKeyframeAnimationPlayerKeyframe*	playerKeyframe =
													new CKeyframeAnimationPlayerKeyframe(array[i],
															previousPlayerKeyframe);
		mInternals->mKeyframeAnimationPlayerKeyframes += playerKeyframe;
		previousPlayerKeyframe = OR<CKeyframeAnimationPlayerKeyframe>(*playerKeyframe);

		if (i == 0)
			mInternals->mPreviousPlayerKeyframe = previousPlayerKeyframe;
		else if (i == 1)
			mInternals->mNextPlayerKeyframe = previousPlayerKeyframe;
	}
}

//----------------------------------------------------------------------------------------------------------------------
CKeyframeAnimationPlayer::~CKeyframeAnimationPlayer()
//----------------------------------------------------------------------------------------------------------------------
{
	// Check if need to indicate we are being deleted
	if (mInternals->mHasBeenDeleted.hasReference())
		// Need to indicate
		*mInternals->mHasBeenDeleted = true;

	Delete(mInternals);
}

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
S2DRectF32 CKeyframeAnimationPlayer::getScreenRect()
//----------------------------------------------------------------------------------------------------------------------
{
	// Check if have previous player key frame
	if (mInternals->mPreviousPlayerKeyframe.hasReference()) {
		// Get texture size
		const	S2DSizeU16&	gpuTextureSize =
									mInternals->mPreviousPlayerKeyframe->mGPURenderObject2D->getGPUTextureReferences().
											getFirst().getGPUTexture().getUsedSize();

		S2DRectF32	rect;
		rect.mOrigin.mX =
				mInternals->mPreviousPlayerKeyframe->mScreenPositionPoint.mX -
						mInternals->mPreviousPlayerKeyframe->mAnchorPoint.mX;
		rect.mOrigin.mY =
				mInternals->mPreviousPlayerKeyframe->mScreenPositionPoint.mY -
						mInternals->mPreviousPlayerKeyframe->mAnchorPoint.mY;
		rect.mSize = S2DSizeF32(gpuTextureSize.mWidth, gpuTextureSize.mHeight);

		return rect;
	} else
		return S2DRectF32();
}

//----------------------------------------------------------------------------------------------------------------------
CActions CKeyframeAnimationPlayer::getAllActions() const
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	CActions	allActions;

	// Iterate all keyframes
	for (TIteratorD<CKeyframeAnimationPlayerKeyframe> iterator =
					mInternals->mKeyframeAnimationPlayerKeyframes.getIterator();
			iterator.hasValue(); iterator.advance()) {
		// Get actions
		const	OI<CActions>&	actions = iterator.getValue().mAnimationKeyframe.getActions();

		// Check if have actions
		if (actions.hasInstance())
			// Add to total list
			allActions.addFrom(*actions);
	}

	return allActions;
}

//----------------------------------------------------------------------------------------------------------------------
void CKeyframeAnimationPlayer::load(CGPU& gpu, bool start)
//----------------------------------------------------------------------------------------------------------------------
{
	// Iterate all keyframes
	for (TIteratorD<CKeyframeAnimationPlayerKeyframe> iterator =
					mInternals->mKeyframeAnimationPlayerKeyframes.getIterator();
			iterator.hasValue(); iterator.advance())
		// Load
		iterator.getValue().load(gpu, mInternals->mSceneAppResourceManagementInfo);

	// Reset
	reset(start);
}

//----------------------------------------------------------------------------------------------------------------------
void CKeyframeAnimationPlayer::finishLoading()
//----------------------------------------------------------------------------------------------------------------------
{
	// Check if have reference
	if (mInternals->mPreviousPlayerKeyframe.hasReference())
		// Finish loading
		mInternals->mPreviousPlayerKeyframe->finishLoading();
}

//----------------------------------------------------------------------------------------------------------------------
void CKeyframeAnimationPlayer::unload()
//----------------------------------------------------------------------------------------------------------------------
{
	// Iterate all keyframes
	for (TIteratorD<CKeyframeAnimationPlayerKeyframe> iterator =
					mInternals->mKeyframeAnimationPlayerKeyframes.getIterator();
			iterator.hasValue(); iterator.advance())
		// Unload
		iterator.getValue().unload();
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

	TIArray<CKeyframeAnimationPlayerKeyframe>&	keyframeAnimationPlayerKeyframes =
														mInternals->mKeyframeAnimationPlayerKeyframes;
	mInternals->mPreviousPlayerKeyframe =
			(keyframeAnimationPlayerKeyframes.getCount() > 0) ?
					OR<CKeyframeAnimationPlayerKeyframe>(keyframeAnimationPlayerKeyframes[0]) :
					OR<CKeyframeAnimationPlayerKeyframe>();
	mInternals->mNextPlayerKeyframe =
			(keyframeAnimationPlayerKeyframes.getCount() > 1) ?
					OR<CKeyframeAnimationPlayerKeyframe>(keyframeAnimationPlayerKeyframes[1]) :
					OR<CKeyframeAnimationPlayerKeyframe>();

	if (start)
		update(0.0, false);
}

//----------------------------------------------------------------------------------------------------------------------
void CKeyframeAnimationPlayer::update(UniversalTimeInterval deltaTimeInterval, bool isRunning)
//----------------------------------------------------------------------------------------------------------------------
{
	if (!mInternals->mIsStarted) {
		mInternals->mCurrentTimeInterval += deltaTimeInterval;
		if (mInternals->mCurrentTimeInterval >= mInternals->mStartTimeInterval.getValue(0.0))
			mInternals->mIsStarted = true;
	}
	
	if (mInternals->mIsStarted && !mInternals->mIsFinished && (mInternals->mPreviousPlayerKeyframe.hasReference())) {
		// Ensure is running or transition is not hold until start
		if (isRunning ||
				(mInternals->mPreviousPlayerKeyframe->getTransitionType().hasValue() &&
						(*mInternals->mPreviousPlayerKeyframe->getTransitionType() !=
								kAnimationKeyframeTransitionTypeHoldUntilStart)))
			// Update our current time
			mInternals->mCurrentFrameTimeInterval += deltaTimeInterval;

		// Setup
		const	SKeyframeAnimationPlayerProcsInfo&			procsInfo = mInternals->mKeyframeAnimationPlayerProcsInfo;
				TIArray<CKeyframeAnimationPlayerKeyframe>&	keyframeAnimationPlayerKeyframes =
																	mInternals->mKeyframeAnimationPlayerKeyframes;

		// Get on the correct keyframe
		while (
				// Running or transition is not hold until start
				(isRunning ||
						(mInternals->mPreviousPlayerKeyframe->getTransitionType().hasValue() &&
								(*mInternals->mPreviousPlayerKeyframe->getTransitionType() !=
										kAnimationKeyframeTransitionTypeHoldUntilStart))) &&

				// Have delay
				mInternals->mPreviousPlayerKeyframe->getDelay().hasValue() &&

				// Current frame time is after the current frame delay
				(mInternals->mCurrentFrameTimeInterval > *mInternals->mPreviousPlayerKeyframe->getDelay())) {
			// Update frame time
			mInternals->mCurrentFrameTimeInterval -= *mInternals->mPreviousPlayerKeyframe->getDelay();

			// Update keyframe pointers
			mInternals->mPreviousPlayerKeyframe = mInternals->mNextPlayerKeyframe;
			mInternals->mNextPlayerKeyframe = OR<CKeyframeAnimationPlayerKeyframe>();

			// We done?
			if (!mInternals->mPreviousPlayerKeyframe.hasReference())
				// Yes
				return;

			// Process actions - this may cause us to go away
			bool	hasBeenDeleted = false;
			mInternals->mHasBeenDeleted = OR<bool>(hasBeenDeleted);

			const	OI<CActions>&	actions = mInternals->mPreviousPlayerKeyframe->getActions();
			if (actions.hasInstance())
				// Handle actions
				procsInfo.performActions(*this, *actions);

			// Check if deleted
			if (hasBeenDeleted)
				return;

			// Reset
			mInternals->mHasBeenDeleted = OR<bool>();

			// Update next keyframe
			OV<CArray::ItemIndex>	index =
											keyframeAnimationPlayerKeyframes.getIndexOf(
													*mInternals->mPreviousPlayerKeyframe);
			if (*index < (keyframeAnimationPlayerKeyframes.getCount() - 1))
				// Next keyframe please!
				mInternals->mNextPlayerKeyframe =
						OR<CKeyframeAnimationPlayerKeyframe>(keyframeAnimationPlayerKeyframes[*index + 1]);
			else if (procsInfo.canPerformShouldLoop() && procsInfo.shouldLoop(*this, ++mInternals->mCurrentLoopCount)) {
				// Looping
				mInternals->mNextPlayerKeyframe =
						OR<CKeyframeAnimationPlayerKeyframe>(keyframeAnimationPlayerKeyframes[0]);

				// Call proc
				procsInfo.didLoop(*this);
			} else if (mInternals->mPreviousPlayerKeyframe->getDelay().hasValue()) {
				// Finshed
				mInternals->mIsFinished = true;

				// Call proc
				procsInfo.didFinish(*this);
			}
		}

		// Now what to do!
				Float32									percent, transitionFactor;
		const	OV<EAnimationKeyframeTransitionType>&	transitionType =
																mInternals->mPreviousPlayerKeyframe->
																		getTransitionType();
		if ((mInternals->mNextPlayerKeyframe.hasReference()) &&
				transitionType.hasValue() && (*transitionType == kAnimationKeyframeTransitionTypeLinear)) {
			// Linear between keyframes
			transitionFactor =
					(Float32) (mInternals->mCurrentFrameTimeInterval /
							*mInternals->mPreviousPlayerKeyframe->getDelay());
			mInternals->mPreviousPlayerKeyframe->setSpriteValuesToInterpolatedValues(transitionFactor,
					*mInternals->mNextPlayerKeyframe);
		} else if ((mInternals->mNextPlayerKeyframe.hasReference()) &&
				transitionType.hasValue() && (*transitionType == kAnimationKeyframeTransitionTypeEaseIn)) {
			// Linear, then ease in
			percent =
					(Float32) (mInternals->mCurrentFrameTimeInterval /
							*mInternals->mPreviousPlayerKeyframe->getDelay());
			if (percent <= 0.5f)
				transitionFactor = percent;
			else
				transitionFactor = 3.0f * percent * percent - 2.0f * percent * percent * percent;
			mInternals->mPreviousPlayerKeyframe->setSpriteValuesToInterpolatedValues(transitionFactor,
					*mInternals->mNextPlayerKeyframe);
		} else if ((mInternals->mNextPlayerKeyframe.hasReference()) &&
				transitionType.hasValue() && (*transitionType == kAnimationKeyframeTransitionTypeEaseOut)) {
			// Ease out, then linear
			percent =
					(Float32) (mInternals->mCurrentFrameTimeInterval /
							*mInternals->mPreviousPlayerKeyframe->getDelay());
			if (percent <= 0.5f)
				transitionFactor = 3.0f * percent * percent - 2.0f * percent * percent * percent;
			else
				transitionFactor = percent;
			mInternals->mPreviousPlayerKeyframe->setSpriteValuesToInterpolatedValues(transitionFactor,
					*mInternals->mNextPlayerKeyframe);
		} else if ((mInternals->mNextPlayerKeyframe.hasReference()) &&
				transitionType.hasValue() && (*transitionType == kAnimationKeyframeTransitionTypeEaseInEaseOut)) {
			// Ease out, then ease in
			percent =
					(Float32) (mInternals->mCurrentFrameTimeInterval /
							*mInternals->mPreviousPlayerKeyframe->getDelay());
			transitionFactor = 3.0f * percent * percent - 2.0f * percent * percent * percent;
			mInternals->mPreviousPlayerKeyframe->setSpriteValuesToInterpolatedValues(transitionFactor,
					*mInternals->mNextPlayerKeyframe);
		} else {
			// No next keyframe or no transition or jump between frames.
			// Copy keyframe values to sprite values
			mInternals->mPreviousPlayerKeyframe->setSpriteValuesToKeyframeValues();
		}

		// Make sure to finish loading
		mInternals->mPreviousPlayerKeyframe->finishLoading();
	}
}

//----------------------------------------------------------------------------------------------------------------------
void CKeyframeAnimationPlayer::render(CGPU& gpu, const CGPURenderObject::RenderInfo& renderInfo) const
//----------------------------------------------------------------------------------------------------------------------
{
	if (mInternals->mIsStarted && !mInternals->mIsFinished && (mInternals->mPreviousPlayerKeyframe.hasReference()))
		// Draw
		mInternals->mPreviousPlayerKeyframe->mGPURenderObject2D->render(CGPURenderObject2D::Indexes::forIndex(0),
				renderInfo);
}

//----------------------------------------------------------------------------------------------------------------------
bool CKeyframeAnimationPlayer::getIsFinished() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mIsFinished;
}
