//----------------------------------------------------------------------------------------------------------------------
//	CSceneItemButton.cpp			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CSceneItemButton.h"

#include "CKeyframeAnimationInfo.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CSceneItemButtonInternals

class CSceneItemButtonInternals : public TCopyOnWriteReferenceCountable<CSceneItemButtonInternals> {
	public:
		CSceneItemButtonInternals() : TCopyOnWriteReferenceCountable(), mHitRadius(0.0) {}
		CSceneItemButtonInternals(const CSceneItemButtonInternals& other) :
			TCopyOnWriteReferenceCountable(),
					mActions(other.mActions), mAudioInfo(other.mAudioInfo),
							mUpKeyframeAnimationInfo(other.mUpKeyframeAnimationInfo),
							mDownKeyframeAnimationInfo(other.mDownKeyframeAnimationInfo),
							mDisabledKeyframeAnimationInfo(other.mDisabledKeyframeAnimationInfo),
							mHitRadius(other.mHitRadius), mStartTimeInterval(other.mStartTimeInterval)
			{}

		OI<CActions>				mActions;
		OI<CAudioInfo>				mAudioInfo;
		OI<CKeyframeAnimationInfo>	mUpKeyframeAnimationInfo;
		OI<CKeyframeAnimationInfo>	mDownKeyframeAnimationInfo;
		OI<CKeyframeAnimationInfo>	mDisabledKeyframeAnimationInfo;
		Float32						mHitRadius;
		OV<UniversalTimeInterval>	mStartTimeInterval;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneItemButton

// MARK: Properties

CString	CSceneItemButton::mType(OSSTR("button"));

// MARK: Lifecycle methods

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

	if (info.contains(CString(OSSTR("actionInfo"))))
		mInternals->mActions = OI<CActions>(CActions(info.getDictionary(CString(OSSTR("actionInfo")))));
	if (info.contains(CString(OSSTR("audioInfo"))))
		mInternals->mAudioInfo = OI<CAudioInfo>(CAudioInfo(info.getDictionary(CString(OSSTR("audioInfo")))));
	mInternals->mUpKeyframeAnimationInfo =
			OI<CKeyframeAnimationInfo>(CKeyframeAnimationInfo(info.getDictionary(CString(OSSTR("upAnimationInfo")))));
	if (info.contains(CString(OSSTR("downAnimationInfo"))))
		mInternals->mDownKeyframeAnimationInfo =
				OI<CKeyframeAnimationInfo>(CKeyframeAnimationInfo(
						info.getDictionary(CString(OSSTR("downAnimationInfo")))));
	if (info.contains(CString(OSSTR("disabledAnimationInfo"))))
		mInternals->mDisabledKeyframeAnimationInfo =
				OI<CKeyframeAnimationInfo>(
						CKeyframeAnimationInfo(info.getDictionary(CString(OSSTR("disabledAnimationInfo")))));
	if (info.contains(CString(OSSTR("startTime"))))
		mInternals->mStartTimeInterval = OV<UniversalTimeInterval>(info.getFloat64(CString(OSSTR("startTime"))));
	mInternals->mHitRadius = info.getFloat32(CString(OSSTR("hitRadius")));
}

//----------------------------------------------------------------------------------------------------------------------
CSceneItemButton::CSceneItemButton(const CSceneItemButton& other) : CSceneItem(other)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = other.mInternals->addReference();
}

//----------------------------------------------------------------------------------------------------------------------
CSceneItemButton::~CSceneItemButton()
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->removeReference();
}

// MARK: CSceneItem methods

//----------------------------------------------------------------------------------------------------------------------
TMArray<CDictionary> CSceneItemButton::getProperties() const
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	TMArray<CDictionary>	properties = CSceneItem::getProperties();

	// Add properties
	CDictionary	actionPropertyInfo;
	actionPropertyInfo.set(mPropertyTitleKey, CString(OSSTR("Actions")));
	actionPropertyInfo.set(mPropertyNameKey, CString(OSSTR("actionInfo")));
	actionPropertyInfo.set(mPropertyTypeKey, CSceneItem::kPropertyTypeActions);
	properties += actionPropertyInfo;

	CDictionary	audioInfoPropertyInfo;
	audioInfoPropertyInfo.set(mPropertyTitleKey, CString(OSSTR("Audio")));
	audioInfoPropertyInfo.set(mPropertyNameKey, CString(OSSTR("audioInfo")));
	audioInfoPropertyInfo.set(mPropertyTypeKey, CAudioInfo::kPropertyTypeAudioInfo);
	properties += audioInfoPropertyInfo;

	CDictionary	upKeyframeAnimationInfoPropertyInfo;
	upKeyframeAnimationInfoPropertyInfo.set(mPropertyTitleKey, CString(OSSTR("Up Keyframe Animation")));
	upKeyframeAnimationInfoPropertyInfo.set(mPropertyNameKey, CString(OSSTR("upAnimationInfo")));
	upKeyframeAnimationInfoPropertyInfo.set(mPropertyTypeKey,
			CKeyframeAnimationInfo::kPropertyTypeKeyframeAnimationInfo);
	properties += upKeyframeAnimationInfoPropertyInfo;

	CDictionary	downKeyframeAnimationInfoPropertyInfo;
	downKeyframeAnimationInfoPropertyInfo.set(mPropertyTitleKey, CString(OSSTR("Down Keyframe Animation")));
	downKeyframeAnimationInfoPropertyInfo.set(mPropertyNameKey, CString(OSSTR("downAnimationInfo")));
	downKeyframeAnimationInfoPropertyInfo.set(mPropertyTypeKey,
			CKeyframeAnimationInfo::kPropertyTypeKeyframeAnimationInfo);
	properties += downKeyframeAnimationInfoPropertyInfo;

	CDictionary	disabledKeyframeAnimationInfoPropertyInfo;
	disabledKeyframeAnimationInfoPropertyInfo.set(mPropertyTitleKey, CString(OSSTR("Disabled Keyframe Animation")));
	disabledKeyframeAnimationInfoPropertyInfo.set(mPropertyNameKey, CString(OSSTR("disabledAnimationInfo")));
	disabledKeyframeAnimationInfoPropertyInfo.set(mPropertyTypeKey,
			CKeyframeAnimationInfo::kPropertyTypeKeyframeAnimationInfo);
	properties += disabledKeyframeAnimationInfoPropertyInfo;

	CDictionary	startTimeIntervalPropertyInfo;
	startTimeIntervalPropertyInfo.set(mPropertyTitleKey, CString(OSSTR("Start Time Interval")));
	startTimeIntervalPropertyInfo.set(mPropertyNameKey, CString(OSSTR("startTime")));
	startTimeIntervalPropertyInfo.set(mPropertyTypeKey, CSceneItem::kPropertyTypeStartTimeInterval);
	properties += startTimeIntervalPropertyInfo;

	CDictionary	hitRadiusPropertyInfo;
	hitRadiusPropertyInfo.set(mPropertyTitleKey, CString(OSSTR("Hit Radius")));
	hitRadiusPropertyInfo.set(mPropertyNameKey, CString(OSSTR("hitRadius")));
	hitRadiusPropertyInfo.set(mPropertyTypeKey, CSceneItem::kPropertyTypePixelWidth);
	properties += hitRadiusPropertyInfo;

	return properties;
}

//----------------------------------------------------------------------------------------------------------------------
CDictionary CSceneItemButton::getInfo() const
//----------------------------------------------------------------------------------------------------------------------
{
	CDictionary	info = CSceneItem::getInfo();

	if (mInternals->mActions.hasInstance())
		info.set(CString(OSSTR("actionInfo")), mInternals->mActions->getInfo());
	if (mInternals->mAudioInfo.hasInstance())
		info.set(CString(OSSTR("audioInfo")), mInternals->mAudioInfo->getInfo());
	if (mInternals->mUpKeyframeAnimationInfo.hasInstance())
		info.set(CString(OSSTR("upAnimationInfo")), mInternals->mUpKeyframeAnimationInfo->getInfo());
	if (mInternals->mDownKeyframeAnimationInfo.hasInstance())
		info.set(CString(OSSTR("downAnimationInfo")), mInternals->mDownKeyframeAnimationInfo->getInfo());
	if (mInternals->mDisabledKeyframeAnimationInfo.hasInstance())
		info.set(CString(OSSTR("disabledAnimationInfo")), mInternals->mDisabledKeyframeAnimationInfo->getInfo());
	if (mInternals->mStartTimeInterval.hasValue())
		info.set(CString(OSSTR("startTime")), *mInternals->mStartTimeInterval);
	info.set(CString(OSSTR("hitRadius")), mInternals->mHitRadius);

	return info;
}

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
const OI<CActions>& CSceneItemButton::getActions() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mActions;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemButton::setActions(const OI<CActions>& actions)
//----------------------------------------------------------------------------------------------------------------------
{
	// Prepare to write
	mInternals = mInternals->prepareForWrite();

	// Update
	mInternals->mActions = actions;
}

//----------------------------------------------------------------------------------------------------------------------
const OI<CAudioInfo>& CSceneItemButton::getAudioInfo() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mAudioInfo;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemButton::setAudioInfo(const OI<CAudioInfo>& audioInfo)
//----------------------------------------------------------------------------------------------------------------------
{
	// Prepare to write
	mInternals = mInternals->prepareForWrite();

	// Update
	mInternals->mAudioInfo = audioInfo;
}

//----------------------------------------------------------------------------------------------------------------------
const OI<CKeyframeAnimationInfo>& CSceneItemButton::getUpKeyframeAnimationInfo() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mUpKeyframeAnimationInfo;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemButton::setUpKeyframeAnimationInfo(const OI<CKeyframeAnimationInfo>& keyframeAnimationInfo)
//----------------------------------------------------------------------------------------------------------------------
{
	// Prepare to write
	mInternals = mInternals->prepareForWrite();

	// Update
	mInternals->mUpKeyframeAnimationInfo = keyframeAnimationInfo;
}

//----------------------------------------------------------------------------------------------------------------------
const OI<CKeyframeAnimationInfo>& CSceneItemButton::getDownKeyframeAnimationInfo() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mDownKeyframeAnimationInfo;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemButton::setDownKeyframeAnimationInfo(const OI<CKeyframeAnimationInfo>& keyframeAnimationInfo)
//----------------------------------------------------------------------------------------------------------------------
{
	// Prepare to write
	mInternals = mInternals->prepareForWrite();

	// Update
	mInternals->mDownKeyframeAnimationInfo = keyframeAnimationInfo;
}

//----------------------------------------------------------------------------------------------------------------------
const OI<CKeyframeAnimationInfo>& CSceneItemButton::getDisabledKeyframeAnimationInfo() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mDisabledKeyframeAnimationInfo;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemButton::setDisabledKeyframeAnimationInfo(const OI<CKeyframeAnimationInfo>& keyframeAnimationInfo)
//----------------------------------------------------------------------------------------------------------------------
{
	// Prepare to write
	mInternals = mInternals->prepareForWrite();

	// Update
	mInternals->mDisabledKeyframeAnimationInfo = keyframeAnimationInfo;
}

//----------------------------------------------------------------------------------------------------------------------
const OV<UniversalTimeInterval>& CSceneItemButton::getStartTimeInterval() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mStartTimeInterval;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemButton::setStartTimeInterval(const OV<UniversalTimeInterval>& startTimeInterval)
//----------------------------------------------------------------------------------------------------------------------
{
	// Prepare to write
	mInternals = mInternals->prepareForWrite();

	// Update
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
	// Prepare to write
	mInternals = mInternals->prepareForWrite();

	// Update
	mInternals->mHitRadius = hitRadius;
}
