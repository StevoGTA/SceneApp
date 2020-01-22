//----------------------------------------------------------------------------------------------------------------------
//	CSceneItemButton.cpp			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CSceneItemButton.h"

#include "CKeyframeAnimationInfo.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: Local data

static	CString	sActionArrayInfoKey("actionInfo");
static	CString	sAudioInfoKey("audioInfo");
static	CString	sHitRadiusKey("hitRadius");
static	CString	sDisabledKeyframeAnimationInfoKey("disabledAnimationInfo");
static	CString	sDownKeyframeAnimationInfoKey("downAnimationInfo");
static	CString	sStartTimeIntervalKey("startTime");
static	CString	sUpKeyframeAnimationInfoKey("upAnimationInfo");

CString	CSceneItemButton::mType("button");

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneItemButtonInternals

class CSceneItemButtonInternals {
	public:
		CSceneItemButtonInternals() :
			mActionArray(nil), mAudioInfo(nil), mUpKeyframeAnimationInfo(nil), mDownKeyframeAnimationInfo(nil),
					mDisabledKeyframeAnimationInfo(nil), mHitRadius(0.0), mStartTimeInterval(0.0)
			{}
		~CSceneItemButtonInternals()
			{
				DisposeOf(mActionArray);
				DisposeOf(mAudioInfo);
				DisposeOf(mUpKeyframeAnimationInfo);
				DisposeOf(mDownKeyframeAnimationInfo);
				DisposeOf(mDisabledKeyframeAnimationInfo);
			}

		CActionArray*			mActionArray;
		CAudioInfo*				mAudioInfo;
		CKeyframeAnimationInfo*	mUpKeyframeAnimationInfo;
		CKeyframeAnimationInfo*	mDownKeyframeAnimationInfo;
		CKeyframeAnimationInfo*	mDisabledKeyframeAnimationInfo;
		Float32					mHitRadius;
		UniversalTimeInterval	mStartTimeInterval;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
CSceneItemButton::CSceneItemButton() : CSceneItem()
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new CSceneItemButtonInternals();
}

//----------------------------------------------------------------------------------------------------------------------
CSceneItemButton::CSceneItemButton(const CDictionary& info) : CSceneItem(info)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new CSceneItemButtonInternals();

	if (info.contains(sActionArrayInfoKey))
		mInternals->mActionArray = new CActionArray(info.getDictionary(sActionArrayInfoKey));

	if (info.contains(sAudioInfoKey))
		mInternals->mAudioInfo = new CAudioInfo(info.getDictionary(sAudioInfoKey));

	mInternals->mUpKeyframeAnimationInfo = new CKeyframeAnimationInfo(info.getDictionary(sUpKeyframeAnimationInfoKey));

	if (info.contains(sDownKeyframeAnimationInfoKey))
		mInternals->mDownKeyframeAnimationInfo =
				new CKeyframeAnimationInfo(info.getDictionary(sDownKeyframeAnimationInfoKey));

	if (info.contains(sDisabledKeyframeAnimationInfoKey))
		mInternals->mDisabledKeyframeAnimationInfo =
				new CKeyframeAnimationInfo(info.getDictionary(sDisabledKeyframeAnimationInfoKey));

	mInternals->mStartTimeInterval = info.getFloat64(sStartTimeIntervalKey);
	mInternals->mHitRadius = info.getFloat32(sHitRadiusKey);
}

//----------------------------------------------------------------------------------------------------------------------
CSceneItemButton::CSceneItemButton(const CSceneItemButton& other) : CSceneItem(other)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new CSceneItemButtonInternals();

	if (other.mInternals->mActionArray != nil)
		mInternals->mActionArray = new CActionArray(*other.mInternals->mActionArray);
	if (other.mInternals->mAudioInfo != nil)
		mInternals->mAudioInfo = new CAudioInfo(*other.mInternals->mAudioInfo);
	if (other.mInternals->mUpKeyframeAnimationInfo != nil)
		mInternals->mUpKeyframeAnimationInfo = new CKeyframeAnimationInfo(*other.mInternals->mUpKeyframeAnimationInfo);
	if (other.mInternals->mDownKeyframeAnimationInfo != nil)
		mInternals->mDownKeyframeAnimationInfo =
				new CKeyframeAnimationInfo(*other.mInternals->mDownKeyframeAnimationInfo);
	if (other.mInternals->mDisabledKeyframeAnimationInfo != nil)
		mInternals->mDisabledKeyframeAnimationInfo =
				new CKeyframeAnimationInfo(*other.mInternals->mDisabledKeyframeAnimationInfo);
	mInternals->mStartTimeInterval = other.mInternals->mStartTimeInterval;
	mInternals->mHitRadius = other.mInternals->mHitRadius;
}

//----------------------------------------------------------------------------------------------------------------------
CSceneItemButton::~CSceneItemButton()
//----------------------------------------------------------------------------------------------------------------------
{
	DisposeOf(mInternals);
}

// MARK: CSceneItem methods

//----------------------------------------------------------------------------------------------------------------------
TArray<CDictionary> CSceneItemButton::getProperties() const
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	TArray<CDictionary>	properties = CSceneItem::getProperties();

	// Add properties
	CDictionary	actionPropertyInfo;
	actionPropertyInfo.set(mPropertyTitleKey, CString("Action Array"));
	actionPropertyInfo.set(mPropertyNameKey, sActionArrayInfoKey);
	actionPropertyInfo.set(mPropertyTypeKey, kSceneItemPropertyTypeActionArray);
	properties += actionPropertyInfo;

	CDictionary	audioInfoPropertyInfo;
	audioInfoPropertyInfo.set(mPropertyTitleKey, CString("Audio"));
	audioInfoPropertyInfo.set(mPropertyNameKey, sAudioInfoKey);
	audioInfoPropertyInfo.set(mPropertyTypeKey, kSceneItemPropertyTypeAudioInfo);
	properties += audioInfoPropertyInfo;

	CDictionary	upKeyframeAnimationInfoPropertyInfo;
	upKeyframeAnimationInfoPropertyInfo.set(mPropertyTitleKey, CString("Up Keyframe Animation"));
	upKeyframeAnimationInfoPropertyInfo.set(mPropertyNameKey, sUpKeyframeAnimationInfoKey);
	upKeyframeAnimationInfoPropertyInfo.set(mPropertyTypeKey, kSceneItemPropertyTypeKeyframeAnimationInfo);
	properties += upKeyframeAnimationInfoPropertyInfo;

	CDictionary	downKeyframeAnimationInfoPropertyInfo;
	downKeyframeAnimationInfoPropertyInfo.set(mPropertyTitleKey, CString("Down Keyframe Animation"));
	downKeyframeAnimationInfoPropertyInfo.set(mPropertyNameKey, sDownKeyframeAnimationInfoKey);
	downKeyframeAnimationInfoPropertyInfo.set(mPropertyTypeKey, kSceneItemPropertyTypeKeyframeAnimationInfo);
	properties += downKeyframeAnimationInfoPropertyInfo;

	CDictionary	disabledKeyframeAnimationInfoPropertyInfo;
	disabledKeyframeAnimationInfoPropertyInfo.set(mPropertyTitleKey, CString("Disabled Keyframe Animation"));
	disabledKeyframeAnimationInfoPropertyInfo.set(mPropertyNameKey, sDisabledKeyframeAnimationInfoKey);
	disabledKeyframeAnimationInfoPropertyInfo.set(mPropertyTypeKey, kSceneItemPropertyTypeKeyframeAnimationInfo);
	properties += disabledKeyframeAnimationInfoPropertyInfo;

	CDictionary	startTimeIntervalPropertyInfo;
	startTimeIntervalPropertyInfo.set(mPropertyTitleKey, CString("Start Time Interval"));
	startTimeIntervalPropertyInfo.set(mPropertyNameKey, sStartTimeIntervalKey);
	startTimeIntervalPropertyInfo.set(mPropertyTypeKey, kSceneItemPropertyTypeStartTimeInterval);
	properties += startTimeIntervalPropertyInfo;

	CDictionary	hitRadiusPropertyInfo;
	hitRadiusPropertyInfo.set(mPropertyTitleKey, CString("Hit Radius"));
	hitRadiusPropertyInfo.set(mPropertyNameKey, sHitRadiusKey);
	hitRadiusPropertyInfo.set(mPropertyTypeKey, kSceneItemPropertyTypePixelWidth);
	properties += hitRadiusPropertyInfo;

	return properties;
}

//----------------------------------------------------------------------------------------------------------------------
CDictionary CSceneItemButton::getInfo() const
//----------------------------------------------------------------------------------------------------------------------
{
	CDictionary	info = CSceneItem::getInfo();

	if (mInternals->mActionArray != nil)
		info.set(sActionArrayInfoKey, mInternals->mActionArray->getInfo());
	if (mInternals->mAudioInfo != nil)
		info.set(sAudioInfoKey, mInternals->mAudioInfo->getInfo());
	if (mInternals->mUpKeyframeAnimationInfo != nil)
		info.set(sUpKeyframeAnimationInfoKey, mInternals->mUpKeyframeAnimationInfo->getInfo());
	if (mInternals->mDownKeyframeAnimationInfo != nil)
		info.set(sDownKeyframeAnimationInfoKey, mInternals->mDownKeyframeAnimationInfo->getInfo());
	if (mInternals->mDisabledKeyframeAnimationInfo != nil)
		info.set(sDisabledKeyframeAnimationInfoKey, mInternals->mDisabledKeyframeAnimationInfo->getInfo());
	info.set(sStartTimeIntervalKey, mInternals->mStartTimeInterval);
	info.set(sHitRadiusKey, mInternals->mHitRadius);

	return info;
}

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
OR<CActionArray> CSceneItemButton::getActionArray() const
//----------------------------------------------------------------------------------------------------------------------
{
	return (mInternals->mActionArray != nil) ? OR<CActionArray>(*mInternals->mActionArray) : OR<CActionArray>();
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemButton::setActionArray(const CActionArray& actionArray)
//----------------------------------------------------------------------------------------------------------------------
{
	DisposeOf(mInternals->mActionArray);
	mInternals->mActionArray = new CActionArray(actionArray);
}

//----------------------------------------------------------------------------------------------------------------------
OR<CAudioInfo> CSceneItemButton::getAudioInfo() const
//----------------------------------------------------------------------------------------------------------------------
{
	return (mInternals->mAudioInfo != nil) ? OR<CAudioInfo>(*mInternals->mAudioInfo) : OR<CAudioInfo>();
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemButton::setAudioInfo(const CAudioInfo& audioInfo)
//----------------------------------------------------------------------------------------------------------------------
{
	DisposeOf(mInternals->mAudioInfo);
	mInternals->mAudioInfo = new CAudioInfo(audioInfo);
}

//----------------------------------------------------------------------------------------------------------------------
OR<CKeyframeAnimationInfo> CSceneItemButton::getUpKeyframeAnimationInfo() const
//----------------------------------------------------------------------------------------------------------------------
{
	return (mInternals->mUpKeyframeAnimationInfo != nil) ?
			OR<CKeyframeAnimationInfo>(*mInternals->mUpKeyframeAnimationInfo) : OR<CKeyframeAnimationInfo>();
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemButton::setUpKeyframeAnimationInfo(const CKeyframeAnimationInfo& keyframeAnimationInfo)
//----------------------------------------------------------------------------------------------------------------------
{
	DisposeOf(mInternals->mUpKeyframeAnimationInfo);
	mInternals->mUpKeyframeAnimationInfo = new CKeyframeAnimationInfo(keyframeAnimationInfo);
}

//----------------------------------------------------------------------------------------------------------------------
OR<CKeyframeAnimationInfo> CSceneItemButton::getDownKeyframeAnimationInfo() const
//----------------------------------------------------------------------------------------------------------------------
{
	return (mInternals->mDownKeyframeAnimationInfo != nil) ?
			OR<CKeyframeAnimationInfo>(*mInternals->mDownKeyframeAnimationInfo) : OR<CKeyframeAnimationInfo>();
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemButton::setDownKeyframeAnimationInfo(const CKeyframeAnimationInfo& keyframeAnimationInfo)
//----------------------------------------------------------------------------------------------------------------------
{
	DisposeOf(mInternals->mDownKeyframeAnimationInfo);
	mInternals->mDownKeyframeAnimationInfo = new CKeyframeAnimationInfo(keyframeAnimationInfo);
}

//----------------------------------------------------------------------------------------------------------------------
OR<CKeyframeAnimationInfo> CSceneItemButton::getDisabledKeyframeAnimationInfo() const
//----------------------------------------------------------------------------------------------------------------------
{
	return (mInternals->mDisabledKeyframeAnimationInfo != nil) ?
			OR<CKeyframeAnimationInfo>(*mInternals->mDisabledKeyframeAnimationInfo) : OR<CKeyframeAnimationInfo>();
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemButton::setDisabledKeyframeAnimationInfo(const CKeyframeAnimationInfo& keyframeAnimationInfo)
//----------------------------------------------------------------------------------------------------------------------
{
	DisposeOf(mInternals->mDisabledKeyframeAnimationInfo);
	mInternals->mDisabledKeyframeAnimationInfo = new CKeyframeAnimationInfo(keyframeAnimationInfo);
}

//----------------------------------------------------------------------------------------------------------------------
UniversalTimeInterval CSceneItemButton::getStartTimeInterval() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mStartTimeInterval;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemButton::setStartTimeInterval(UniversalTimeInterval startTimeInterval)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mStartTimeInterval = startTimeInterval;
}

//----------------------------------------------------------------------------------------------------------------------
Float32 CSceneItemButton::getHitRadius() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mHitRadius;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemButton::setHitRadius(Float32 hitRadius)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mHitRadius = hitRadius;
}
