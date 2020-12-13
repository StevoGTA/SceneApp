//----------------------------------------------------------------------------------------------------------------------
//	CAnimationKeyframe.cpp			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CAnimationKeyframe.h"

#include "CSceneItem.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CAnimationKeyframeInternals

class CAnimationKeyframeInternals : public TCopyOnWriteReferenceCountable<CAnimationKeyframeInternals> {
	public:
		CAnimationKeyframeInternals() : TCopyOnWriteReferenceCountable(), mOptions(CAnimationKeyframe::kOptionsNone) {}
		CAnimationKeyframeInternals(const CAnimationKeyframeInternals& other) :
			TCopyOnWriteReferenceCountable(),
					mActions(other.mActions), mAnchorPoint(other.mAnchorPoint),
					mScreenPositionPoint(other.mScreenPositionPoint), mAngleDegrees(other.mAngleDegrees),
					mAlpha(other.mAlpha), mScale(other.mScale), mDelay(other.mDelay),
					mTransitionType(other.mTransitionType), mImageResourceFilename(other.mImageResourceFilename),
					mOptions(other.mOptions)
			{}

		OI<CActions>							mActions;
		OV<S2DPointF32>							mAnchorPoint;
		OV<S2DPointF32>							mScreenPositionPoint;
		OV<Float32>								mAngleDegrees;
		OV<Float32>								mAlpha;
		OV<Float32>								mScale;
		OV<UniversalTimeInterval>				mDelay;
		OV<CAnimationKeyframe::TransitionType>	mTransitionType;
		OV<CString>								mImageResourceFilename;
		CAnimationKeyframe::Options				mOptions;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CAnimationKeyframe

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
CAnimationKeyframe::CAnimationKeyframe()
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new CAnimationKeyframeInternals();
}

//----------------------------------------------------------------------------------------------------------------------
CAnimationKeyframe::CAnimationKeyframe(const CDictionary& info)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new CAnimationKeyframeInternals();

	if (info.contains(CString(OSSTR("actionInfo"))))
		mInternals->mActions = OI<CActions>(CActions(info.getDictionary(CString(OSSTR("actionInfo")))));
	if (info.contains(CString(OSSTR("anchorPt"))))
		mInternals->mAnchorPoint = OV<S2DPointF32>(info.getString(CString(OSSTR("anchorPt"))));
	if (info.contains(CString(OSSTR("screenPositionPt"))))
		mInternals->mScreenPositionPoint = OV<S2DPointF32>(info.getString(CString(OSSTR("screenPositionPt"))));
	if (info.contains(CString(OSSTR("angle"))))
		mInternals->mAngleDegrees = OV<Float32>(info.getFloat32(CString(OSSTR("angle"))));
	if (info.contains(CString(OSSTR("alpha"))))
		mInternals->mAlpha = OV<Float32>(info.getFloat32(CString(OSSTR("alpha"))));
	if (info.contains(CString(OSSTR("scale"))))
		mInternals->mScale = OV<Float32>(info.getFloat32(CString(OSSTR("scale"))));
	if (info.contains(CString(OSSTR("delay"))))
		mInternals->mDelay = info.getFloat32(CString(OSSTR("delay")));
	if (info.contains(CString(OSSTR("transitionType"))))
		mInternals->mTransitionType =
			OV<TransitionType>((TransitionType) info.getUInt32(CString(OSSTR("transitionType"))));
	if (info.contains(CString(OSSTR("imageFilename"))))
		mInternals->mImageResourceFilename = OV<CString>(info.getString(CString(OSSTR("imageFilename"))));
	mInternals->mOptions = (Options) info.getUInt32(CString(OSSTR("options")), mInternals->mOptions);

	// Force some things
	if (!mInternals->mDelay.hasValue() && !mInternals->mTransitionType.hasValue())
		// No delay means we end on this frame, so make sure the transition type is set
		mInternals->mTransitionType = OV<TransitionType>(kTransitionTypeJump);
}

//----------------------------------------------------------------------------------------------------------------------
CAnimationKeyframe::CAnimationKeyframe(const CAnimationKeyframe& other)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = other.mInternals->addReference();
}

//----------------------------------------------------------------------------------------------------------------------
CAnimationKeyframe::~CAnimationKeyframe()
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->removeReference();
}

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
TArray<CDictionary> CAnimationKeyframe::getProperties() const
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	TNArray<CDictionary>	properties;

	// Add properties
	CDictionary	actionPropertyInfo;
	actionPropertyInfo.set(CSceneItem::mPropertyTitleKey, CString(OSSTR("Actions")));
	actionPropertyInfo.set(CSceneItem::mPropertyNameKey, CString(OSSTR("actionInfo")));
	actionPropertyInfo.set(CSceneItem::mPropertyTypeKey, CSceneItem::kPropertyTypeActions);
	properties += actionPropertyInfo;

	CDictionary	anchorPointPropertyInfo;
	anchorPointPropertyInfo.set(CSceneItem::mPropertyTitleKey, CString(OSSTR("Anchor Point")));
	anchorPointPropertyInfo.set(CSceneItem::mPropertyNameKey, CString(OSSTR("anchorPt")));
	anchorPointPropertyInfo.set(CSceneItem::mPropertyTypeKey, kPropertyTypeAnchorPoint);
	properties += anchorPointPropertyInfo;

	CDictionary	screenPositionPointPropertyInfo;
	screenPositionPointPropertyInfo.set(CSceneItem::mPropertyTitleKey, CString(OSSTR("Screen Position Point")));
	screenPositionPointPropertyInfo.set(CSceneItem::mPropertyNameKey, CString(OSSTR("screenPositionPt")));
	screenPositionPointPropertyInfo.set(CSceneItem::mPropertyTypeKey, kPropertyTypeScreenPositionPoint);
	properties += screenPositionPointPropertyInfo;

	CDictionary	anglePropertyInfo;
	anglePropertyInfo.set(CSceneItem::mPropertyTitleKey, CString(OSSTR("Angle in Degrees")));
	anglePropertyInfo.set(CSceneItem::mPropertyNameKey, CString(OSSTR("angle")));
	anglePropertyInfo.set(CSceneItem::mPropertyTypeKey, kPropertyTypeAngle);
	properties += anglePropertyInfo;

	CDictionary	alphaPropertyInfo;
	alphaPropertyInfo.set(CSceneItem::mPropertyTitleKey, CString(OSSTR("Alpha")));
	alphaPropertyInfo.set(CSceneItem::mPropertyNameKey, CString(OSSTR("alpha")));
	alphaPropertyInfo.set(CSceneItem::mPropertyTypeKey, kPropertyTypeAlpha);
	properties += alphaPropertyInfo;

	CDictionary	scalePropertyInfo;
	scalePropertyInfo.set(CSceneItem::mPropertyTitleKey, CString(OSSTR("Scale")));
	scalePropertyInfo.set(CSceneItem::mPropertyNameKey, CString(OSSTR("scale")));
	scalePropertyInfo.set(CSceneItem::mPropertyTypeKey, kPropertyTypeScale);
	properties += scalePropertyInfo;

	CDictionary	delayPropertyInfo;
	delayPropertyInfo.set(CSceneItem::mPropertyTitleKey, CString(OSSTR("Delay")));
	delayPropertyInfo.set(CSceneItem::mPropertyNameKey, CString(OSSTR("delay")));
	delayPropertyInfo.set(CSceneItem::mPropertyTypeKey, kPropertyTypeDelay);
	properties += delayPropertyInfo;

	CDictionary	transitionTypePropertyInfo;
	transitionTypePropertyInfo.set(CSceneItem::mPropertyTitleKey, CString(OSSTR("Transition Type")));
	transitionTypePropertyInfo.set(CSceneItem::mPropertyNameKey, CString(OSSTR("transitionType")));
	transitionTypePropertyInfo.set(CSceneItem::mPropertyTypeKey, kPropertyTypeTransitionType);
	properties += transitionTypePropertyInfo;

	CDictionary	imageResourceFilenamePropertyInfo;
	imageResourceFilenamePropertyInfo.set(CSceneItem::mPropertyTitleKey, CString(OSSTR("Image Resource Filename")));
	imageResourceFilenamePropertyInfo.set(CSceneItem::mPropertyNameKey, CString(OSSTR("imageFilename")));
	imageResourceFilenamePropertyInfo.set(CSceneItem::mPropertyTypeKey, kPropertyTypeImageFilename);
	properties += imageResourceFilenamePropertyInfo;

	CDictionary	optionsPropertyInfo;
	optionsPropertyInfo.set(CSceneItem::mPropertyTitleKey, CString(OSSTR("Options")));
	optionsPropertyInfo.set(CSceneItem::mPropertyNameKey, CString(OSSTR("options")));
	optionsPropertyInfo.set(CSceneItem::mPropertyTypeKey, kPropertyTypeOptions);
	properties += optionsPropertyInfo;

	return properties;
}

//----------------------------------------------------------------------------------------------------------------------
CDictionary CAnimationKeyframe::getInfo() const
//----------------------------------------------------------------------------------------------------------------------
{
	CDictionary	info;

	if (mInternals->mActions.hasInstance())
		info.set(CString(OSSTR("actionInfo")), mInternals->mActions->getInfo());

	if (mInternals->mAnchorPoint.hasValue())
		info.set(CString(OSSTR("anchorPt")), (*mInternals->mAnchorPoint).asString());

	if (mInternals->mScreenPositionPoint.hasValue())
		info.set(CString(OSSTR("screenPositionPt")), (*mInternals->mScreenPositionPoint).asString());

	if (mInternals->mAngleDegrees.hasValue())
		info.set(CString(OSSTR("angle")), *mInternals->mAngleDegrees);

	if (mInternals->mAlpha.hasValue())
		info.set(CString(OSSTR("alpha")), *mInternals->mAlpha);

	if (mInternals->mScale.hasValue())
		info.set(CString(OSSTR("scale")), *mInternals->mScale);

	if (mInternals->mDelay.hasValue())
		info.set(CString(OSSTR("delay")), *mInternals->mDelay);

	if (mInternals->mTransitionType.hasValue())
		info.set(CString(OSSTR("transitionType")), (UInt32) *mInternals->mTransitionType);

	if (!mInternals->mImageResourceFilename.hasValue())
		info.set(CString(OSSTR("imageFilename")), *mInternals->mImageResourceFilename);

	info.set(CString(OSSTR("options")), (UInt32) mInternals->mOptions);

	return info;
}

//----------------------------------------------------------------------------------------------------------------------
const OI<CActions>& CAnimationKeyframe::getActions() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mActions;
}

//----------------------------------------------------------------------------------------------------------------------
void CAnimationKeyframe::setActions(const OI<CActions>& actions)
//----------------------------------------------------------------------------------------------------------------------
{
	// Prepare to write
	mInternals = mInternals->prepareForWrite();

	// Update
	mInternals->mActions = actions;
}

//----------------------------------------------------------------------------------------------------------------------
const OV<S2DPointF32>& CAnimationKeyframe::getAnchorPoint() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mAnchorPoint;
}

//----------------------------------------------------------------------------------------------------------------------
void CAnimationKeyframe::setAnchorPoint(const OV<S2DPointF32>& anchorPoint)
//----------------------------------------------------------------------------------------------------------------------
{
	// Prepare to write
	mInternals = mInternals->prepareForWrite();

	// Update
	mInternals->mAnchorPoint = anchorPoint;
}

//----------------------------------------------------------------------------------------------------------------------
const OV<S2DPointF32>& CAnimationKeyframe::getScreenPositionPoint() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mScreenPositionPoint;
}

//----------------------------------------------------------------------------------------------------------------------
void CAnimationKeyframe::setScreenPositionPoint(const OV<S2DPointF32>& screenPositionPoint)
//----------------------------------------------------------------------------------------------------------------------
{
	// Prepare to write
	mInternals = mInternals->prepareForWrite();

	// Update
	mInternals->mScreenPositionPoint = screenPositionPoint;
}

//----------------------------------------------------------------------------------------------------------------------
const OV<Float32>& CAnimationKeyframe::getAngleDegrees() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mAngleDegrees;
}

//----------------------------------------------------------------------------------------------------------------------
void CAnimationKeyframe::setAngleDegrees(const OV<Float32>& angleDegrees)
//----------------------------------------------------------------------------------------------------------------------
{
	// Prepare to write
	mInternals = mInternals->prepareForWrite();

	// Update
	mInternals->mAngleDegrees = angleDegrees;
}

//----------------------------------------------------------------------------------------------------------------------
const OV<Float32>& CAnimationKeyframe::getAlpha() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mAlpha;
}

//----------------------------------------------------------------------------------------------------------------------
void CAnimationKeyframe::setAlpha(const OV<Float32>& alpha)
//----------------------------------------------------------------------------------------------------------------------
{
	// Prepare to write
	mInternals = mInternals->prepareForWrite();

	// Update
	mInternals->mAlpha = alpha;
}

//----------------------------------------------------------------------------------------------------------------------
const OV<Float32>& CAnimationKeyframe::getScale() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mScale;
}

//----------------------------------------------------------------------------------------------------------------------
void CAnimationKeyframe::setScale(const OV<Float32>& scale)
//----------------------------------------------------------------------------------------------------------------------
{
	// Prepare to write
	mInternals = mInternals->prepareForWrite();

	// Update
	mInternals->mScale = scale;
}

//----------------------------------------------------------------------------------------------------------------------
const OV<UniversalTimeInterval>& CAnimationKeyframe::getDelay() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mDelay;
}

//----------------------------------------------------------------------------------------------------------------------
void CAnimationKeyframe::setDelay(const OV<UniversalTimeInterval>& delay)
//----------------------------------------------------------------------------------------------------------------------
{
	// Prepare to write
	mInternals = mInternals->prepareForWrite();

	// Update
	mInternals->mDelay = delay;
}

//----------------------------------------------------------------------------------------------------------------------
const OV<CAnimationKeyframe::TransitionType>& CAnimationKeyframe::getTransitionType() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mTransitionType;
}

//----------------------------------------------------------------------------------------------------------------------
void CAnimationKeyframe::setTransitionType(const OV<TransitionType>& transitionType)
//----------------------------------------------------------------------------------------------------------------------
{
	// Prepare to write
	mInternals = mInternals->prepareForWrite();

	// Update
	mInternals->mTransitionType = transitionType;
}

//----------------------------------------------------------------------------------------------------------------------
const OV<CString>& CAnimationKeyframe::getImageResourceFilename() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mImageResourceFilename;
}

//----------------------------------------------------------------------------------------------------------------------
void CAnimationKeyframe::setImageResourceFilename(const OV<CString>& imageResourceFilename)
//----------------------------------------------------------------------------------------------------------------------
{
	// Prepare to write
	mInternals = mInternals->prepareForWrite();

	// Update
	mInternals->mImageResourceFilename = imageResourceFilename;
}

//----------------------------------------------------------------------------------------------------------------------
CAnimationKeyframe::Options CAnimationKeyframe::getOptions() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mOptions;
}

//----------------------------------------------------------------------------------------------------------------------
void CAnimationKeyframe::setOptions(Options options)
//----------------------------------------------------------------------------------------------------------------------
{
	// Prepare to write
	mInternals = mInternals->prepareForWrite();

	// Update
	mInternals->mOptions = options;
}
