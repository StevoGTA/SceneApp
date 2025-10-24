//----------------------------------------------------------------------------------------------------------------------
//	CSceneItemButton.cpp			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CSceneItemButton.h"

#include "CKeyframeAnimationInfo.h"
#include "CReferenceCountable.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CSceneItemButton::Internals

class CSceneItemButton::Internals : public TCopyOnWriteReferenceCountable<Internals> {
	public:
		Internals() : TCopyOnWriteReferenceCountable(), mHitRadius(0.0) {}
		Internals(const Internals& other) :
			TCopyOnWriteReferenceCountable(),
					mActions(other.mActions), mAudioInfo(other.mAudioInfo),
							mUpKeyframeAnimationInfo(other.mUpKeyframeAnimationInfo),
							mDownKeyframeAnimationInfo(other.mDownKeyframeAnimationInfo),
							mDisabledKeyframeAnimationInfo(other.mDisabledKeyframeAnimationInfo),
							mHitRadius(other.mHitRadius), mStartTimeInterval(other.mStartTimeInterval)
			{}

		OV<CActions>				mActions;
		OV<CAudioInfo>				mAudioInfo;
		OV<CKeyframeAnimationInfo>	mUpKeyframeAnimationInfo;
		OV<CKeyframeAnimationInfo>	mDownKeyframeAnimationInfo;
		OV<CKeyframeAnimationInfo>	mDisabledKeyframeAnimationInfo;
		Float32						mHitRadius;
		OV<UniversalTimeInterval>	mStartTimeInterval;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneItemButton

// MARK: Properties

const	CString	CSceneItemButton::mType(OSSTR("button"));

const	CString	CSceneItemButton::mPropertyNameActions(OSSTR("actionInfo"));
const	CString	CSceneItemButton::mPropertyNameAudio(OSSTR("audioInfo"));
const	CString	CSceneItemButton::mPropertyNameUpAnimation(OSSTR("upAnimationInfo"));
const	CString	CSceneItemButton::mPropertyNameDownAnimation(OSSTR("downAnimationInfo"));
const	CString	CSceneItemButton::mPropertyNameDisabledAnimation(OSSTR("disabledAnimationInfo"));
const	CString	CSceneItemButton::mPropertyNameStartTime(OSSTR("startTime"));
const	CString	CSceneItemButton::mPropertyNameHitRadius(OSSTR("hitRadius"));

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
CSceneItemButton::CSceneItemButton(const CDictionary& info) : CSceneItem(info)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new Internals();

	if (info.contains(mPropertyNameActions))
		mInternals->mActions.setValue(CActions(info.getDictionary(mPropertyNameActions)));
	if (info.contains(mPropertyNameAudio))
		mInternals->mAudioInfo.setValue(CAudioInfo(info.getDictionary(mPropertyNameAudio)));
	mInternals->mUpKeyframeAnimationInfo.setValue(CKeyframeAnimationInfo(info.getDictionary(mPropertyNameUpAnimation)));
	if (info.contains(mPropertyNameDownAnimation))
		mInternals->mDownKeyframeAnimationInfo.setValue(
				CKeyframeAnimationInfo(info.getDictionary(mPropertyNameDownAnimation)));
	if (info.contains(mPropertyNameDisabledAnimation))
		mInternals->mDisabledKeyframeAnimationInfo.setValue(
				CKeyframeAnimationInfo(info.getDictionary(mPropertyNameDisabledAnimation)));
	mInternals->mStartTimeInterval = info.getOVFloat64(mPropertyNameStartTime);
	mInternals->mHitRadius = info.getFloat32(mPropertyNameHitRadius);
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
	actionPropertyInfo.set(mPropertyNameKey, mPropertyNameActions);
	actionPropertyInfo.set(mPropertyTypeKey, CSceneItem::kPropertyTypeActions);
	properties += actionPropertyInfo;

	CDictionary	audioInfoPropertyInfo;
	audioInfoPropertyInfo.set(mPropertyTitleKey, CString(OSSTR("Audio")));
	audioInfoPropertyInfo.set(mPropertyNameKey, mPropertyNameAudio);
	audioInfoPropertyInfo.set(mPropertyTypeKey, CAudioInfo::kPropertyTypeAudioInfo);
	properties += audioInfoPropertyInfo;

	CDictionary	upKeyframeAnimationInfoPropertyInfo;
	upKeyframeAnimationInfoPropertyInfo.set(mPropertyTitleKey, CString(OSSTR("Up Keyframe Animation")));
	upKeyframeAnimationInfoPropertyInfo.set(mPropertyNameKey, mPropertyNameUpAnimation);
	upKeyframeAnimationInfoPropertyInfo.set(mPropertyTypeKey,
			CKeyframeAnimationInfo::kPropertyTypeKeyframeAnimationInfo);
	properties += upKeyframeAnimationInfoPropertyInfo;

	CDictionary	downKeyframeAnimationInfoPropertyInfo;
	downKeyframeAnimationInfoPropertyInfo.set(mPropertyTitleKey, CString(OSSTR("Down Keyframe Animation")));
	downKeyframeAnimationInfoPropertyInfo.set(mPropertyNameKey, mPropertyNameDownAnimation);
	downKeyframeAnimationInfoPropertyInfo.set(mPropertyTypeKey,
			CKeyframeAnimationInfo::kPropertyTypeKeyframeAnimationInfo);
	properties += downKeyframeAnimationInfoPropertyInfo;

	CDictionary	disabledKeyframeAnimationInfoPropertyInfo;
	disabledKeyframeAnimationInfoPropertyInfo.set(mPropertyTitleKey, CString(OSSTR("Disabled Keyframe Animation")));
	disabledKeyframeAnimationInfoPropertyInfo.set(mPropertyNameKey, mPropertyNameDisabledAnimation);
	disabledKeyframeAnimationInfoPropertyInfo.set(mPropertyTypeKey,
			CKeyframeAnimationInfo::kPropertyTypeKeyframeAnimationInfo);
	properties += disabledKeyframeAnimationInfoPropertyInfo;

	CDictionary	startTimeIntervalPropertyInfo;
	startTimeIntervalPropertyInfo.set(mPropertyTitleKey, CString(OSSTR("Start Time Interval")));
	startTimeIntervalPropertyInfo.set(mPropertyNameKey, mPropertyNameStartTime);
	startTimeIntervalPropertyInfo.set(mPropertyTypeKey, CSceneItem::kPropertyTypeStartTimeInterval);
	properties += startTimeIntervalPropertyInfo;

	CDictionary	hitRadiusPropertyInfo;
	hitRadiusPropertyInfo.set(mPropertyTitleKey, CString(OSSTR("Hit Radius")));
	hitRadiusPropertyInfo.set(mPropertyNameKey, mPropertyNameHitRadius);
	hitRadiusPropertyInfo.set(mPropertyTypeKey, CSceneItem::kPropertyTypePixelWidth);
	properties += hitRadiusPropertyInfo;

	return properties;
}

//----------------------------------------------------------------------------------------------------------------------
CDictionary CSceneItemButton::getInfo() const
//----------------------------------------------------------------------------------------------------------------------
{
	CDictionary	info = CSceneItem::getInfo();

	if (mInternals->mActions.hasValue())
		info.set(mPropertyNameActions, mInternals->mActions->getInfo());
	if (mInternals->mAudioInfo.hasValue())
		info.set(mPropertyNameAudio, mInternals->mAudioInfo->getInfo());
	if (mInternals->mUpKeyframeAnimationInfo.hasValue())
		info.set(mPropertyNameUpAnimation, mInternals->mUpKeyframeAnimationInfo->getInfo());
	if (mInternals->mDownKeyframeAnimationInfo.hasValue())
		info.set(mPropertyNameDownAnimation, mInternals->mDownKeyframeAnimationInfo->getInfo());
	if (mInternals->mDisabledKeyframeAnimationInfo.hasValue())
		info.set(mPropertyNameDisabledAnimation, mInternals->mDisabledKeyframeAnimationInfo->getInfo());
	info.set(mPropertyNameStartTime, mInternals->mStartTimeInterval);
	info.set(mPropertyNameHitRadius, mInternals->mHitRadius);

	return info;
}

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
const OV<CActions>& CSceneItemButton::getActions() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mActions;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemButton::setActions(const OV<CActions>& actions)
//----------------------------------------------------------------------------------------------------------------------
{
	// Check if changed
	if (actions != mInternals->mActions) {
		// Prepare to write
		Internals::prepareForWrite(&mInternals);

		// Update
		mInternals->mActions = actions;

		// Note updated
		noteUpdated(mPropertyNameActions);
	}
}

//----------------------------------------------------------------------------------------------------------------------
const OV<CAudioInfo>& CSceneItemButton::getAudioInfo() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mAudioInfo;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemButton::setAudioInfo(const OV<CAudioInfo>& audioInfo)
//----------------------------------------------------------------------------------------------------------------------
{
	// Check if changed
	if (audioInfo != mInternals->mAudioInfo) {
		// Prepare to write
		Internals::prepareForWrite(&mInternals);

		// Update
		mInternals->mAudioInfo = audioInfo;

		// Note updated
		noteUpdated(mPropertyNameAudio);
	}
}

//----------------------------------------------------------------------------------------------------------------------
const OV<CKeyframeAnimationInfo>& CSceneItemButton::getUpKeyframeAnimationInfo() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mUpKeyframeAnimationInfo;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemButton::setUpKeyframeAnimationInfo(const OV<CKeyframeAnimationInfo>& keyframeAnimationInfo)
//----------------------------------------------------------------------------------------------------------------------
{
	// Check if changed
	if (keyframeAnimationInfo != mInternals->mUpKeyframeAnimationInfo) {
		// Prepare to write
		Internals::prepareForWrite(&mInternals);

		// Update
		mInternals->mUpKeyframeAnimationInfo = keyframeAnimationInfo;

		// Note updated
		noteUpdated(mPropertyNameUpAnimation);
	}
}

//----------------------------------------------------------------------------------------------------------------------
const OV<CKeyframeAnimationInfo>& CSceneItemButton::getDownKeyframeAnimationInfo() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mDownKeyframeAnimationInfo;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemButton::setDownKeyframeAnimationInfo(const OV<CKeyframeAnimationInfo>& keyframeAnimationInfo)
//----------------------------------------------------------------------------------------------------------------------
{
	// Check if changed
	if (keyframeAnimationInfo != mInternals->mDownKeyframeAnimationInfo) {
		// Prepare to write
		Internals::prepareForWrite(&mInternals);

		// Update
		mInternals->mDownKeyframeAnimationInfo = keyframeAnimationInfo;

		// Note updated
		noteUpdated(mPropertyNameDownAnimation);
	}
}

//----------------------------------------------------------------------------------------------------------------------
const OV<CKeyframeAnimationInfo>& CSceneItemButton::getDisabledKeyframeAnimationInfo() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mDisabledKeyframeAnimationInfo;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemButton::setDisabledKeyframeAnimationInfo(const OV<CKeyframeAnimationInfo>& keyframeAnimationInfo)
//----------------------------------------------------------------------------------------------------------------------
{
	// Check if changed
	if (keyframeAnimationInfo != mInternals->mDisabledKeyframeAnimationInfo) {
		// Prepare to write
		Internals::prepareForWrite(&mInternals);

		// Update
		mInternals->mDisabledKeyframeAnimationInfo = keyframeAnimationInfo;

		// Note updated
		noteUpdated(mPropertyNameDisabledAnimation);
	}
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
	// Check if changed
	if (startTimeInterval != mInternals->mStartTimeInterval) {
		// Prepare to write
		Internals::prepareForWrite(&mInternals);

		// Update
		mInternals->mStartTimeInterval = startTimeInterval;

		// Note updated
		noteUpdated(mPropertyNameStartTime);
	}
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
	// Check if changed
	if (hitRadius != mInternals->mHitRadius) {
		// Prepare to write
		Internals::prepareForWrite(&mInternals);

		// Update
		mInternals->mHitRadius = hitRadius;

		// Note updated
		noteUpdated(mPropertyNameHitRadius);
	}
}
