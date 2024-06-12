//----------------------------------------------------------------------------------------------------------------------
//	CSceneItemAnimation.cpp			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CSceneItemAnimation.h"

//#include "CCelAnimationInfo.h"
#include "CKeyframeAnimationInfo.h"
#include "CReferenceCountable.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CSceneItemAnimation::Internals

class CSceneItemAnimation::Internals : public TCopyOnWriteReferenceCountable<Internals> {
	public:
		Internals() : TCopyOnWriteReferenceCountable(), mLoopCount(1) {}
		Internals(const Internals& other) :
			TCopyOnWriteReferenceCountable(),
					mStartedActions(other.mStartedActions), mFinishedActions(other.mFinishedActions),
					mAudioInfo(other.mAudioInfo), /* mCelAnimationInfo(other.mCelAnimationInfo), */
					mKeyframeAnimationInfo(other.mKeyframeAnimationInfo), mLoopCount(other.mLoopCount),
					mStartTimeInterval(other.mStartTimeInterval)
			{}

		OI<CActions>				mStartedActions;
		OI<CActions>				mFinishedActions;
		OI<CAudioInfo>				mAudioInfo;
//		OI<CCelAnimationInfo>		mCelAnimationInfo;
		OI<CKeyframeAnimationInfo>	mKeyframeAnimationInfo;
		UInt32						mLoopCount;
		OV<UniversalTimeInterval>	mStartTimeInterval;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneItemAnimation

// MARK: Properties

CString	CSceneItemAnimation::mType(OSSTR("animation"));

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
CSceneItemAnimation::CSceneItemAnimation() : CSceneItem()
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new Internals();
}

//----------------------------------------------------------------------------------------------------------------------
CSceneItemAnimation::CSceneItemAnimation(const CDictionary& info) : CSceneItem(info)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new Internals();

	if (info.contains(CString(OSSTR("startedActionInfo"))))
		mInternals->mStartedActions = OI<CActions>(CActions(info.getDictionary(CString(OSSTR("startedActionInfo")))));
	if (info.contains(CString(OSSTR("finishedActionInfo"))))
		mInternals->mFinishedActions = OI<CActions>(CActions(info.getDictionary(CString(OSSTR("finishedActionInfo")))));
	if (info.contains(CString(OSSTR("audioInfo"))))
		mInternals->mAudioInfo = OI<CAudioInfo>(CAudioInfo(info.getDictionary(CString(OSSTR("audioInfo")))));
//	if (info.contains(CString(OSSTR("celAnimationInfo"))))
//		mInternals->mCelAnimationInfo =
//				OI<CCelAnimationInfo>(CCelAnimationInfo(info.getDictionary(CString(OSSTR("celAnimationInfo")))));
	if (info.contains(CString(OSSTR("keyframeAnimationInfo"))))
		mInternals->mKeyframeAnimationInfo =
				OI<CKeyframeAnimationInfo>(CKeyframeAnimationInfo(
						info.getDictionary(CString(OSSTR("keyframeAnimationInfo")))));
	mInternals->mLoopCount = info.getUInt32(CString(OSSTR("loopCount")), 1);
	if (info.contains(CString(OSSTR("startTime"))))
		mInternals->mStartTimeInterval = OV<UniversalTimeInterval>(info.getFloat64(CString(OSSTR("startTime"))));
}

//----------------------------------------------------------------------------------------------------------------------
CSceneItemAnimation::CSceneItemAnimation(const CSceneItemAnimation& other) : CSceneItem(other)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = other.mInternals->addReference();
}

//----------------------------------------------------------------------------------------------------------------------
CSceneItemAnimation::~CSceneItemAnimation()
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->removeReference();
}

// MARK: CSceneItem methods

//----------------------------------------------------------------------------------------------------------------------
TMArray<CDictionary> CSceneItemAnimation::getProperties() const
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	TMArray<CDictionary>	properties = CSceneItem::getProperties();

	// Add properties
	CDictionary	startedActionPropertyInfo;
	startedActionPropertyInfo.set(mPropertyTitleKey, CString(OSSTR("Started Actions")));
	startedActionPropertyInfo.set(mPropertyNameKey, CString(OSSTR("startedActionInfo")));
	startedActionPropertyInfo.set(mPropertyTypeKey, CSceneItem::kPropertyTypeActions);
	properties += startedActionPropertyInfo;

	CDictionary	finishedActionPropertyInfo;
	finishedActionPropertyInfo.set(mPropertyTitleKey, CString(OSSTR("Finished Actions")));
	finishedActionPropertyInfo.set(mPropertyNameKey, CString(OSSTR("finishedActionInfo")));
	finishedActionPropertyInfo.set(mPropertyTypeKey, CSceneItem::kPropertyTypeActions);
	properties += finishedActionPropertyInfo;

	CDictionary	audioInfoPropertyInfo;
	audioInfoPropertyInfo.set(mPropertyTitleKey, CString(OSSTR("Audio")));
	audioInfoPropertyInfo.set(mPropertyNameKey, CString(OSSTR("audioInfo")));
	audioInfoPropertyInfo.set(mPropertyTypeKey, CAudioInfo::kPropertyTypeAudioInfo);
	properties += audioInfoPropertyInfo;

//	CDictionary	celAnimationInfoPropertyInfo;
//	celAnimationInfoPropertyInfo.set(mPropertyTitleKey, CString(OSSTR("Cel Animation")));
//	celAnimationInfoPropertyInfo.set(mPropertyNameKey, CString(OSSTR("celAnimationInfo")));
//	celAnimationInfoPropertyInfo.set(mPropertyTypeKey, kSceneItemPropertyTypeCelAnimationInfo);
//	properties += celAnimationInfoPropertyInfo;

	CDictionary	keyframeAnimationInfoPropertyInfo;
	keyframeAnimationInfoPropertyInfo.set(mPropertyTitleKey, CString(OSSTR("Keyframe Animation")));
	keyframeAnimationInfoPropertyInfo.set(mPropertyNameKey, CString(OSSTR("keyframeAnimationInfo")));
	keyframeAnimationInfoPropertyInfo.set(mPropertyTypeKey, CKeyframeAnimationInfo::kPropertyTypeKeyframeAnimationInfo);
	properties += keyframeAnimationInfoPropertyInfo;

	CDictionary	loopCountPropertyInfo;
	loopCountPropertyInfo.set(mPropertyTitleKey, CString(OSSTR("Loop Count")));
	loopCountPropertyInfo.set(mPropertyNameKey, CString(OSSTR("loopCount")));
	loopCountPropertyInfo.set(mPropertyTypeKey, kPropertyTypeLoopCount);
	properties += loopCountPropertyInfo;

	CDictionary	startTimeIntervalPropertyInfo;
	startTimeIntervalPropertyInfo.set(mPropertyTitleKey, CString(OSSTR("Start Time")));
	startTimeIntervalPropertyInfo.set(mPropertyNameKey, CString(OSSTR("startTime")));
	startTimeIntervalPropertyInfo.set(mPropertyTypeKey, CSceneItem::kPropertyTypeStartTimeInterval);
	properties += startTimeIntervalPropertyInfo;

	return properties;
}

//----------------------------------------------------------------------------------------------------------------------
CDictionary CSceneItemAnimation::getInfo() const
//----------------------------------------------------------------------------------------------------------------------
{
	CDictionary	info = CSceneItem::getInfo();

	if (mInternals->mStartedActions.hasInstance())
		info.set(CString(OSSTR("startedActionInfo")), mInternals->mStartedActions->getInfo());
	if (mInternals->mFinishedActions.hasInstance())
		info.set(CString(OSSTR("finishedActionInfo")), mInternals->mFinishedActions->getInfo());
	if (mInternals->mAudioInfo.hasInstance())
		info.set(CString(OSSTR("audioInfo")), mInternals->mAudioInfo->getInfo());
//	if (mInternals->mCelAnimationInfo.hasInstance())
//		info.set(CString(OSSTR("celAnimationInfo")), mInternals->mCelAnimationInfo->getInfo());
	if (mInternals->mKeyframeAnimationInfo.hasInstance())
		info.set(CString(OSSTR("keyframeAnimationInfo")), mInternals->mKeyframeAnimationInfo->getInfo());
	info.set(CString(OSSTR("loopCount")), mInternals->mLoopCount);
	if (mInternals->mStartTimeInterval.hasValue())
		info.set(CString(OSSTR("startTime")), *mInternals->mStartTimeInterval);

	return info;
}

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
const OI<CActions>& CSceneItemAnimation::getStartedActions() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mStartedActions;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemAnimation::setStartedActions(const OI<CActions>& actions)
//----------------------------------------------------------------------------------------------------------------------
{
	// Prepare to write
	Internals::prepareForWrite(&mInternals);

	// Update
	mInternals->mStartedActions = actions;
}

//----------------------------------------------------------------------------------------------------------------------
const OI<CActions>& CSceneItemAnimation::getFinishedActions() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mFinishedActions;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemAnimation::setFinishedActions(const OI<CActions>& actions)
//----------------------------------------------------------------------------------------------------------------------
{
	// Prepare to write
	Internals::prepareForWrite(&mInternals);

	// Update
	mInternals->mFinishedActions = actions;
}

//----------------------------------------------------------------------------------------------------------------------
const OI<CAudioInfo>& CSceneItemAnimation::getAudioInfo() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mAudioInfo;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemAnimation::setAudioInfo(const OI<CAudioInfo>& audioInfo)
//----------------------------------------------------------------------------------------------------------------------
{
	// Prepare to write
	Internals::prepareForWrite(&mInternals);

	// Update
	mInternals->mAudioInfo = audioInfo;
}

////----------------------------------------------------------------------------------------------------------------------
//const OI<CCelAnimationInfo>& CSceneItemAnimation::getCelAnimationInfo() const
////----------------------------------------------------------------------------------------------------------------------
//{
//	return mInternals->mCelAnimationInfo;
//}

////----------------------------------------------------------------------------------------------------------------------
//void CSceneItemAnimation::setCelAnimationInfo(const OI<CCelAnimationInfo>& celAnimationInfo)
////----------------------------------------------------------------------------------------------------------------------
//{
//	// Prepare to write
//	Internals::prepareForWrite(&mInternals);
//
//	// Update
//	mInternals->mCelAnimationInfo = celAnimationInfo;
//	mInternals->mKeyframeAnimationInfo = OI<CKeyframeAnimationInfo>();
//}

//----------------------------------------------------------------------------------------------------------------------
const OI<CKeyframeAnimationInfo>& CSceneItemAnimation::getKeyframeAnimationInfo() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mKeyframeAnimationInfo;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemAnimation::setKeyframeAnimationInfo(const OI<CKeyframeAnimationInfo>& keyframeAnimationInfo)
//----------------------------------------------------------------------------------------------------------------------
{
	// Prepare to write
	Internals::prepareForWrite(&mInternals);

	// Update
//	mInternals->mCelAnimationInfo = mInternals->mCelAnimationInfo();
	mInternals->mKeyframeAnimationInfo = keyframeAnimationInfo;
}

//----------------------------------------------------------------------------------------------------------------------
UInt32 CSceneItemAnimation::getLoopCount() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mLoopCount;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemAnimation::setLoopCount(UInt32 loopCount)
//----------------------------------------------------------------------------------------------------------------------
{
	// Prepare to write
	Internals::prepareForWrite(&mInternals);

	// Update
	mInternals->mLoopCount = loopCount;
}

//----------------------------------------------------------------------------------------------------------------------
const OV<UniversalTimeInterval>& CSceneItemAnimation::getStartTimeInterval() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mStartTimeInterval;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemAnimation::setStartTimeInterval(const OV<UniversalTimeInterval>& startTimeInterval)
//----------------------------------------------------------------------------------------------------------------------
{
	// Prepare to write
	Internals::prepareForWrite(&mInternals);

	// Update
	mInternals->mStartTimeInterval = startTimeInterval;
}
