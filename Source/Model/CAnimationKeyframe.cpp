//----------------------------------------------------------------------------------------------------------------------
//	CAnimationKeyframe.cpp			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CAnimationKeyframe.h"

#include "CSceneItem.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: Local data
static	const	UniversalTimeInterval	kDelayDefault = 0.0;

static	CString	sActionArrayInfoKey("actionInfo");
static	CString	sAnchorPointKey("anchorPt");
static	CString	sScreenPositionPointKey("screenPositionPt");
static	CString	sAngleDegreesKey("angle");
static	CString	sAlphaKey("alpha");
static	CString	sScaleKey("scale");
static	CString	sDelayKey("delay");
static	CString	sTransitionTypeKey("transitionType");
static	CString	sImageResourceFilenameKey("imageFilename");
static	CString	sOptionsKey("options");

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CAnimationKeyframeInternals

class CAnimationKeyframeInternals {
	public:
		CAnimationKeyframeInternals() :
			mActionArray(nil), mAnchorPoint(kAnimationKeyframeAnchorPointUseCurrent),
					mScreenPositionPoint(kAnimationKeyframeScreenPositionPointUseCurrent),
					mAngleDegrees(kAnimationKeyframeAngleDegreesUseCurrent), mAlpha(kAnimationKeyframeAlphaUseCurrent),
					mScale(kAnimationKeyframeScaleUseCurrent), mDelay(kDelayDefault),
					mTransitionType(kAnimationKeyframeTransitionTypeNone), mOptions(kAnimationKeyframeOptionsNone)
			{}
		~CAnimationKeyframeInternals()
			{
				DisposeOf(mActionArray);
			}

		CActionArray*						mActionArray;
		S2DPoint32							mAnchorPoint;			// Point relative to lower left of image of frame anchor point
		S2DPoint32							mScreenPositionPoint;	// Point on screen of anchor point
		Float32								mAngleDegrees;			// Degrees of image rotation around anchor point
		Float32								mAlpha;					// 0.0f fully transparent - 1.0f fully opaque
		Float32								mScale;					// 1.0f is original size
		UniversalTimeInterval				mDelay;					// Time delay between this keyframe and the next keyframe
		EAnimationKeyframeTransitionType	mTransitionType;
		CString								mImageResourceFilename;
		EAnimationKeyframeOptions			mOptions;
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

	if (info.contains(sActionArrayInfoKey))
		mInternals->mActionArray = new CActionArray(info.getDictionary(sActionArrayInfoKey));
	if (info.contains(sAnchorPointKey))
		mInternals->mAnchorPoint = S2DPoint32(info.getString(sAnchorPointKey));
	if (info.contains(sScreenPositionPointKey))
		mInternals->mScreenPositionPoint = S2DPoint32(info.getString(sScreenPositionPointKey));
	mInternals->mAngleDegrees = info.getFloat32(sAngleDegreesKey, mInternals->mAngleDegrees);
	mInternals->mAlpha = info.getFloat32(sAlphaKey, mInternals->mAlpha);
	mInternals->mScale = info.getFloat32(sScaleKey, mInternals->mScale);
	mInternals->mDelay = info.getFloat32(sDelayKey, mInternals->mDelay);
	mInternals->mTransitionType =
			(EAnimationKeyframeTransitionType) info.getUInt32(sTransitionTypeKey, mInternals->mTransitionType);
	mInternals->mImageResourceFilename = info.getString(sImageResourceFilenameKey);
	mInternals->mOptions = (EAnimationKeyframeOptions) info.getUInt32(sOptionsKey, mInternals->mOptions);

	// Force some things
	if (mInternals->mDelay == kAnimationKeyframeDelayForever)
		mInternals->mTransitionType = kAnimationKeyframeTransitionTypeJump;
}

//----------------------------------------------------------------------------------------------------------------------
CAnimationKeyframe::CAnimationKeyframe(const CAnimationKeyframe& other)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new CAnimationKeyframeInternals();

	if (other.mInternals->mActionArray != nil)
		mInternals->mActionArray = new CActionArray(*other.mInternals->mActionArray);
	mInternals->mAnchorPoint = other.mInternals->mAnchorPoint;
	mInternals->mScreenPositionPoint = other.mInternals->mScreenPositionPoint;
	mInternals->mAngleDegrees = other.mInternals->mAngleDegrees;
	mInternals->mAlpha = other.mInternals->mAlpha;
	mInternals->mScale = other.mInternals->mScale;
	mInternals->mDelay = other.mInternals->mDelay;
	mInternals->mTransitionType = other.mInternals->mTransitionType;
	mInternals->mImageResourceFilename = other.mInternals->mImageResourceFilename;
	mInternals->mOptions = other.mInternals->mOptions;
}

//----------------------------------------------------------------------------------------------------------------------
CAnimationKeyframe::~CAnimationKeyframe()
//----------------------------------------------------------------------------------------------------------------------
{
	DisposeOf(mInternals);
}

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
TArray<CDictionary> CAnimationKeyframe::getProperties() const
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	TArray<CDictionary>	properties;

	// Add properties
	CDictionary	actionPropertyInfo;
	actionPropertyInfo.set(CSceneItem::mPropertyTitleKey, CString("Action Array"));
	actionPropertyInfo.set(CSceneItem::mPropertyNameKey, sActionArrayInfoKey);
	actionPropertyInfo.set(CSceneItem::mPropertyTypeKey, kSceneItemPropertyTypeActionArray);
	properties += actionPropertyInfo;

	CDictionary	anchorPointPropertyInfo;
	anchorPointPropertyInfo.set(CSceneItem::mPropertyTitleKey, CString("Anchor Point"));
	anchorPointPropertyInfo.set(CSceneItem::mPropertyNameKey, sAnchorPointKey);
	anchorPointPropertyInfo.set(CSceneItem::mPropertyTypeKey, kAnimationKeyframePropertyTypeAnchorPoint);
	properties += anchorPointPropertyInfo;

	CDictionary	screenPositionPointPropertyInfo;
	screenPositionPointPropertyInfo.set(CSceneItem::mPropertyTitleKey, CString("Screen Position Point"));
	screenPositionPointPropertyInfo.set(CSceneItem::mPropertyNameKey, sScreenPositionPointKey);
	screenPositionPointPropertyInfo.set(CSceneItem::mPropertyTypeKey,
			kAnimationKeyframePropertyTypeScreenPositionPoint);
	properties += screenPositionPointPropertyInfo;

	CDictionary	anglePropertyInfo;
	anglePropertyInfo.set(CSceneItem::mPropertyTitleKey, CString("Angle in Degrees"));
	anglePropertyInfo.set(CSceneItem::mPropertyNameKey, sAngleDegreesKey);
	anglePropertyInfo.set(CSceneItem::mPropertyTypeKey, kAnimationKeyframePropertyTypeAngle);
	properties += anglePropertyInfo;

	CDictionary	alphaPropertyInfo;
	alphaPropertyInfo.set(CSceneItem::mPropertyTitleKey, CString("Alpha"));
	alphaPropertyInfo.set(CSceneItem::mPropertyNameKey, sAlphaKey);
	alphaPropertyInfo.set(CSceneItem::mPropertyTypeKey, kAnimationKeyframePropertyTypeAlpha);
	properties += alphaPropertyInfo;

	CDictionary	scalePropertyInfo;
	scalePropertyInfo.set(CSceneItem::mPropertyTitleKey, CString("Scale"));
	scalePropertyInfo.set(CSceneItem::mPropertyNameKey, sScaleKey);
	scalePropertyInfo.set(CSceneItem::mPropertyTypeKey, kAnimationKeyframePropertyTypeScale);
	properties += scalePropertyInfo;

	CDictionary	delayPropertyInfo;
	delayPropertyInfo.set(CSceneItem::mPropertyTitleKey, CString("Delay"));
	delayPropertyInfo.set(CSceneItem::mPropertyNameKey, sDelayKey);
	delayPropertyInfo.set(CSceneItem::mPropertyTypeKey, kAnimationKeyframePropertyTypeDelay);
	properties += delayPropertyInfo;

	CDictionary	transitionTypePropertyInfo;
	transitionTypePropertyInfo.set(CSceneItem::mPropertyTitleKey, CString("Transition Type"));
	transitionTypePropertyInfo.set(CSceneItem::mPropertyNameKey, sTransitionTypeKey);
	transitionTypePropertyInfo.set(CSceneItem::mPropertyTypeKey, kAnimationKeyframePropertyTypeTransitionType);
	properties += transitionTypePropertyInfo;

	CDictionary	imageResourceFilenamePropertyInfo;
	imageResourceFilenamePropertyInfo.set(CSceneItem::mPropertyTitleKey, CString("Image Resource Filename"));
	imageResourceFilenamePropertyInfo.set(CSceneItem::mPropertyNameKey, sImageResourceFilenameKey);
	imageResourceFilenamePropertyInfo.set(CSceneItem::mPropertyTypeKey, kAnimationKeyframePropertyTypeImageFilename);
	properties += imageResourceFilenamePropertyInfo;

	CDictionary	optionsPropertyInfo;
	optionsPropertyInfo.set(CSceneItem::mPropertyTitleKey, CString("Options"));
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

	if (mInternals->mActionArray != nil)
		info.set(sActionArrayInfoKey, mInternals->mActionArray->getInfo());

	if (hasAnchorPoint())
		info.set(sAnchorPointKey, mInternals->mAnchorPoint.asString());

	if (hasScreenPositionPoint())
		info.set(sScreenPositionPointKey, mInternals->mScreenPositionPoint.asString());

	if (hasAngleDegrees())
		info.set(sAngleDegreesKey, mInternals->mAngleDegrees);

	if (hasAlpha())
		info.set(sAlphaKey, mInternals->mAlpha);

	if (hasScale())
		info.set(sScaleKey, mInternals->mScale);

	info.set(sDelayKey, mInternals->mDelay);

	if (hasTransitionType())
		info.set(sTransitionTypeKey, (UInt32) mInternals->mTransitionType);

	if (hasImageResourceFilename())
		info.set(sImageResourceFilenameKey, mInternals->mImageResourceFilename);

	info.set(sOptionsKey, (UInt32) mInternals->mOptions);

	return info;
}

//----------------------------------------------------------------------------------------------------------------------
OR<CActionArray> CAnimationKeyframe::getActionArray() const
//----------------------------------------------------------------------------------------------------------------------
{
	return (mInternals->mActionArray != nil) ? OR<CActionArray>(*mInternals->mActionArray) : OR<CActionArray>();
}

//----------------------------------------------------------------------------------------------------------------------
void CAnimationKeyframe::setActionArray(OR<CActionArray> actionArray)
//----------------------------------------------------------------------------------------------------------------------
{
	DisposeOf(mInternals->mActionArray);
	if (actionArray.hasReference())
		mInternals->mActionArray = new CActionArray(*actionArray);
}

//----------------------------------------------------------------------------------------------------------------------
bool CAnimationKeyframe::hasAnchorPoint() const
//----------------------------------------------------------------------------------------------------------------------
{
	return (mInternals->mAnchorPoint.mX != kAnimationKeyframeAnchorPointUseCurrent.mX) ||
			(mInternals->mAnchorPoint.mY != kAnimationKeyframeAnchorPointUseCurrent.mY);
}

//----------------------------------------------------------------------------------------------------------------------
const S2DPoint32& CAnimationKeyframe::getAnchorPoint() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mAnchorPoint;
}

//----------------------------------------------------------------------------------------------------------------------
void CAnimationKeyframe::setAnchorPoint(const S2DPoint32& anchorPoint)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mAnchorPoint = anchorPoint;
}

//----------------------------------------------------------------------------------------------------------------------
bool CAnimationKeyframe::hasScreenPositionPoint() const
//----------------------------------------------------------------------------------------------------------------------
{
	return (mInternals->mScreenPositionPoint.mX != kAnimationKeyframeScreenPositionPointUseCurrent.mX) ||
			(mInternals->mScreenPositionPoint.mY != kAnimationKeyframeScreenPositionPointUseCurrent.mY);
}

//----------------------------------------------------------------------------------------------------------------------
const S2DPoint32& CAnimationKeyframe::getScreenPositionPoint() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mScreenPositionPoint;
}

//----------------------------------------------------------------------------------------------------------------------
void CAnimationKeyframe::setScreenPositionPoint(const S2DPoint32& screenPositionPoint)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mScreenPositionPoint = screenPositionPoint;
}

//----------------------------------------------------------------------------------------------------------------------
bool CAnimationKeyframe::hasAngleDegrees() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mAngleDegrees != kAnimationKeyframeAngleDegreesUseCurrent;
}

//----------------------------------------------------------------------------------------------------------------------
Float32 CAnimationKeyframe::getAngleDegrees() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mAngleDegrees;
}

//----------------------------------------------------------------------------------------------------------------------
void CAnimationKeyframe::setAngleDegrees(Float32 angleDegrees)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mAngleDegrees = angleDegrees;
}

//----------------------------------------------------------------------------------------------------------------------
bool CAnimationKeyframe::hasAlpha() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mAlpha != kAnimationKeyframeAlphaUseCurrent;
}

//----------------------------------------------------------------------------------------------------------------------
Float32 CAnimationKeyframe::getAlpha() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mAlpha;
}

//----------------------------------------------------------------------------------------------------------------------
void CAnimationKeyframe::setAlpha(Float32 alpha)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mAlpha = alpha;
}

//----------------------------------------------------------------------------------------------------------------------
bool CAnimationKeyframe::hasScale() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mScale != kAnimationKeyframeScaleUseCurrent;
}

//----------------------------------------------------------------------------------------------------------------------
Float32 CAnimationKeyframe::getScale() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mScale;
}

//----------------------------------------------------------------------------------------------------------------------
void CAnimationKeyframe::setScale(Float32 scale)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mScale = scale;
}

//----------------------------------------------------------------------------------------------------------------------
UniversalTimeInterval CAnimationKeyframe::getDelay() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mDelay;
}

//----------------------------------------------------------------------------------------------------------------------
void CAnimationKeyframe::setDelay(UniversalTimeInterval delay)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mDelay = delay;
}

//----------------------------------------------------------------------------------------------------------------------
bool CAnimationKeyframe::hasTransitionType() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mTransitionType != kAnimationKeyframeTransitionTypeNone;
}

//----------------------------------------------------------------------------------------------------------------------
EAnimationKeyframeTransitionType CAnimationKeyframe::getTransitionType() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mTransitionType;
}

//----------------------------------------------------------------------------------------------------------------------
void CAnimationKeyframe::setTransitionType(EAnimationKeyframeTransitionType transitionType)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mTransitionType = transitionType;
}

//----------------------------------------------------------------------------------------------------------------------
bool CAnimationKeyframe::hasImageResourceFilename() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mImageResourceFilename.getLength() > 0;
}

//----------------------------------------------------------------------------------------------------------------------
const CString& CAnimationKeyframe::getImageResourceFilename() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mImageResourceFilename;
}

//----------------------------------------------------------------------------------------------------------------------
void CAnimationKeyframe::setImageResourceFilename(const CString& imageResourceFilename)
//----------------------------------------------------------------------------------------------------------------------
{
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
	mInternals->mOptions = options;
}
