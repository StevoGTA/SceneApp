//----------------------------------------------------------------------------------------------------------------------
//	CAnimationKeyframe.cpp			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CAnimationKeyframe.h"

#include "CSceneItem.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: Local data

static	CString	sActionsInfoKey(OSSTR("actionInfo"));
static	CString	sAnchorPointKey(OSSTR("anchorPt"));
static	CString	sScreenPositionPointKey(OSSTR("screenPositionPt"));
static	CString	sAngleDegreesKey(OSSTR("angle"));
static	CString	sAlphaKey(OSSTR("alpha"));
static	CString	sScaleKey(OSSTR("scale"));
static	CString	sDelayKey(OSSTR("delay"));
static	CString	sTransitionTypeKey(OSSTR("transitionType"));
static	CString	sImageResourceFilenameKey(OSSTR("imageFilename"));
static	CString	sOptionsKey(OSSTR("options"));

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CAnimationKeyframeInternals

class CAnimationKeyframeInternals : public TCopyOnWriteReferenceCountable<CAnimationKeyframeInternals> {
	public:
		CAnimationKeyframeInternals() : TCopyOnWriteReferenceCountable(), mOptions(kAnimationKeyframeOptionsNone) {}
		CAnimationKeyframeInternals(const CAnimationKeyframeInternals& other) :
			TCopyOnWriteReferenceCountable(),
					mActions(other.mActions), mAnchorPoint(other.mAnchorPoint),
					mScreenPositionPoint(other.mScreenPositionPoint), mAngleDegrees(other.mAngleDegrees),
					mAlpha(other.mAlpha), mScale(other.mScale), mDelay(other.mDelay),
					mTransitionType(other.mTransitionType), mImageResourceFilename(other.mImageResourceFilename),
					mOptions(other.mOptions)
			{}

		OO<CActions>							mActions;
		OV<S2DPointF32>							mAnchorPoint;
		OV<S2DPointF32>							mScreenPositionPoint;
		OV<Float32>								mAngleDegrees;
		OV<Float32>								mAlpha;
		OV<Float32>								mScale;
		OV<UniversalTimeInterval>				mDelay;
		OV<EAnimationKeyframeTransitionType>	mTransitionType;
		OV<CString>								mImageResourceFilename;
		EAnimationKeyframeOptions				mOptions;
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

	if (info.contains(sActionsInfoKey))
		mInternals->mActions = OO<CActions>(CActions(info.getDictionary(sActionsInfoKey)));
	if (info.contains(sAnchorPointKey))
		mInternals->mAnchorPoint = OV<S2DPointF32>(info.getString(sAnchorPointKey));
	if (info.contains(sScreenPositionPointKey))
		mInternals->mScreenPositionPoint = OV<S2DPointF32>(info.getString(sScreenPositionPointKey));
	if (info.contains(sAngleDegreesKey))
		mInternals->mAngleDegrees = OV<Float32>(info.getFloat32(sAngleDegreesKey));
	if (info.contains(sAlphaKey))
		mInternals->mAlpha = OV<Float32>(info.getFloat32(sAlphaKey));
	if (info.contains(sScaleKey))
		mInternals->mScale = OV<Float32>(info.getFloat32(sScaleKey));
	if (info.contains(sDelayKey))
		mInternals->mDelay = info.getFloat32(sDelayKey);
	if (info.contains(sTransitionTypeKey))
		mInternals->mTransitionType =
			OV<EAnimationKeyframeTransitionType>((EAnimationKeyframeTransitionType) info.getUInt32(sTransitionTypeKey));
	if (info.contains(sImageResourceFilenameKey))
		mInternals->mImageResourceFilename = OV<CString>(info.getString(sImageResourceFilenameKey));
	mInternals->mOptions = (EAnimationKeyframeOptions) info.getUInt32(sOptionsKey, mInternals->mOptions);

	// Force some things
	if (!mInternals->mDelay.hasValue() && !mInternals->mTransitionType.hasValue())
		// No delay means we end on this frame, so make sure the transition type is set
		mInternals->mTransitionType = OV<EAnimationKeyframeTransitionType>(kAnimationKeyframeTransitionTypeJump);
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
	actionPropertyInfo.set(CSceneItem::mPropertyNameKey, sActionsInfoKey);
	actionPropertyInfo.set(CSceneItem::mPropertyTypeKey, kSceneItemPropertyTypeActions);
	properties += actionPropertyInfo;

	CDictionary	anchorPointPropertyInfo;
	anchorPointPropertyInfo.set(CSceneItem::mPropertyTitleKey, CString(OSSTR("Anchor Point")));
	anchorPointPropertyInfo.set(CSceneItem::mPropertyNameKey, sAnchorPointKey);
	anchorPointPropertyInfo.set(CSceneItem::mPropertyTypeKey, kAnimationKeyframePropertyTypeAnchorPoint);
	properties += anchorPointPropertyInfo;

	CDictionary	screenPositionPointPropertyInfo;
	screenPositionPointPropertyInfo.set(CSceneItem::mPropertyTitleKey, CString(OSSTR("Screen Position Point")));
	screenPositionPointPropertyInfo.set(CSceneItem::mPropertyNameKey, sScreenPositionPointKey);
	screenPositionPointPropertyInfo.set(CSceneItem::mPropertyTypeKey,
			kAnimationKeyframePropertyTypeScreenPositionPoint);
	properties += screenPositionPointPropertyInfo;

	CDictionary	anglePropertyInfo;
	anglePropertyInfo.set(CSceneItem::mPropertyTitleKey, CString(OSSTR("Angle in Degrees")));
	anglePropertyInfo.set(CSceneItem::mPropertyNameKey, sAngleDegreesKey);
	anglePropertyInfo.set(CSceneItem::mPropertyTypeKey, kAnimationKeyframePropertyTypeAngle);
	properties += anglePropertyInfo;

	CDictionary	alphaPropertyInfo;
	alphaPropertyInfo.set(CSceneItem::mPropertyTitleKey, CString(OSSTR("Alpha")));
	alphaPropertyInfo.set(CSceneItem::mPropertyNameKey, sAlphaKey);
	alphaPropertyInfo.set(CSceneItem::mPropertyTypeKey, kAnimationKeyframePropertyTypeAlpha);
	properties += alphaPropertyInfo;

	CDictionary	scalePropertyInfo;
	scalePropertyInfo.set(CSceneItem::mPropertyTitleKey, CString(OSSTR("Scale")));
	scalePropertyInfo.set(CSceneItem::mPropertyNameKey, sScaleKey);
	scalePropertyInfo.set(CSceneItem::mPropertyTypeKey, kAnimationKeyframePropertyTypeScale);
	properties += scalePropertyInfo;

	CDictionary	delayPropertyInfo;
	delayPropertyInfo.set(CSceneItem::mPropertyTitleKey, CString(OSSTR("Delay")));
	delayPropertyInfo.set(CSceneItem::mPropertyNameKey, sDelayKey);
	delayPropertyInfo.set(CSceneItem::mPropertyTypeKey, kAnimationKeyframePropertyTypeDelay);
	properties += delayPropertyInfo;

	CDictionary	transitionTypePropertyInfo;
	transitionTypePropertyInfo.set(CSceneItem::mPropertyTitleKey, CString(OSSTR("Transition Type")));
	transitionTypePropertyInfo.set(CSceneItem::mPropertyNameKey, sTransitionTypeKey);
	transitionTypePropertyInfo.set(CSceneItem::mPropertyTypeKey, kAnimationKeyframePropertyTypeTransitionType);
	properties += transitionTypePropertyInfo;

	CDictionary	imageResourceFilenamePropertyInfo;
	imageResourceFilenamePropertyInfo.set(CSceneItem::mPropertyTitleKey, CString(OSSTR("Image Resource Filename")));
	imageResourceFilenamePropertyInfo.set(CSceneItem::mPropertyNameKey, sImageResourceFilenameKey);
	imageResourceFilenamePropertyInfo.set(CSceneItem::mPropertyTypeKey, kAnimationKeyframePropertyTypeImageFilename);
	properties += imageResourceFilenamePropertyInfo;

	CDictionary	optionsPropertyInfo;
	optionsPropertyInfo.set(CSceneItem::mPropertyTitleKey, CString(OSSTR("Options")));
	optionsPropertyInfo.set(CSceneItem::mPropertyNameKey, sOptionsKey);
	optionsPropertyInfo.set(CSceneItem::mPropertyTypeKey, kAnimationKeyframePropertyTypeOptions);
	properties += optionsPropertyInfo;

	return properties;
}

//----------------------------------------------------------------------------------------------------------------------
CDictionary CAnimationKeyframe::getInfo() const
//----------------------------------------------------------------------------------------------------------------------
{
	CDictionary	info;

	if (mInternals->mActions.hasObject())
		info.set(sActionsInfoKey, mInternals->mActions->getInfo());

	if (mInternals->mAnchorPoint.hasValue())
		info.set(sAnchorPointKey, (*mInternals->mAnchorPoint).asString());

	if (mInternals->mScreenPositionPoint.hasValue())
		info.set(sScreenPositionPointKey, (*mInternals->mScreenPositionPoint).asString());

	if (mInternals->mAngleDegrees.hasValue())
		info.set(sAngleDegreesKey, *mInternals->mAngleDegrees);

	if (mInternals->mAlpha.hasValue())
		info.set(sAlphaKey, *mInternals->mAlpha);

	if (mInternals->mScale.hasValue())
		info.set(sScaleKey, *mInternals->mScale);

	if (mInternals->mDelay.hasValue())
		info.set(sDelayKey, *mInternals->mDelay);

	if (mInternals->mTransitionType.hasValue())
		info.set(sTransitionTypeKey, (UInt32) *mInternals->mTransitionType);

	if (!mInternals->mImageResourceFilename.hasValue())
		info.set(sImageResourceFilenameKey, *mInternals->mImageResourceFilename);

	info.set(sOptionsKey, (UInt32) mInternals->mOptions);

	return info;
}

//----------------------------------------------------------------------------------------------------------------------
const OO<CActions>& CAnimationKeyframe::getActions() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mActions;
}

//----------------------------------------------------------------------------------------------------------------------
void CAnimationKeyframe::setActions(const OO<CActions>& actions)
//----------------------------------------------------------------------------------------------------------------------
{
	// Prepare to write
	mInternals->prepareForWrite();

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
	mInternals->prepareForWrite();

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
	mInternals->prepareForWrite();

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
	mInternals->prepareForWrite();

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
	mInternals->prepareForWrite();

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
	mInternals->prepareForWrite();

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
	mInternals->prepareForWrite();

	// Update
	mInternals->mDelay = delay;
}

//----------------------------------------------------------------------------------------------------------------------
const OV<EAnimationKeyframeTransitionType>& CAnimationKeyframe::getTransitionType() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mTransitionType;
}

//----------------------------------------------------------------------------------------------------------------------
void CAnimationKeyframe::setTransitionType(const OV<EAnimationKeyframeTransitionType>& transitionType)
//----------------------------------------------------------------------------------------------------------------------
{
	// Prepare to write
	mInternals->prepareForWrite();

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
	mInternals->prepareForWrite();

	// Update
	mInternals->mImageResourceFilename = imageResourceFilename;
}

//----------------------------------------------------------------------------------------------------------------------
EAnimationKeyframeOptions CAnimationKeyframe::getOptions() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mOptions;
}

//----------------------------------------------------------------------------------------------------------------------
void CAnimationKeyframe::setOptions(EAnimationKeyframeOptions options)
//----------------------------------------------------------------------------------------------------------------------
{
	// Prepare to write
	mInternals->prepareForWrite();

	// Update
	mInternals->mOptions = options;
}
