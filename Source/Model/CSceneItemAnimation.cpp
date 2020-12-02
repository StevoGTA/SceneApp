//----------------------------------------------------------------------------------------------------------------------
//	CSceneItemAnimation.cpp			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CSceneItemAnimation.h"

//#include "CCelAnimationInfo.h"
#include "CKeyframeAnimationInfo.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: Local data

static	CString	sAudioInfoKey(OSSTR("audioInfo"));
static	CString	sCelAnimationInfoKey(OSSTR("celAnimationInfo"));
static	CString	sFinishedActionsInfoKey(OSSTR("finishedActionInfo"));
static	CString	sKeyframeAnimationInfoKey(OSSTR("keyframeAnimationInfo"));
static	CString	sLoopCountKey(OSSTR("loopCount"));
static	CString	sStartedActionsInfoKey(OSSTR("startedActionInfo"));
static	CString	sStartTimeIntervalKey(OSSTR("startTime"));

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneItemAnimationInternals

class CSceneItemAnimationInternals : public TCopyOnWriteReferenceCountable<CSceneItemAnimationInternals> {
	public:
		CSceneItemAnimationInternals() : TCopyOnWriteReferenceCountable(), mLoopCount(1) {}
		CSceneItemAnimationInternals(const CSceneItemAnimationInternals& other) :
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
	mInternals = new CSceneItemAnimationInternals();
}

//----------------------------------------------------------------------------------------------------------------------
CSceneItemAnimation::CSceneItemAnimation(const CDictionary& info) : CSceneItem(info)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new CSceneItemAnimationInternals();

	if (info.contains(sStartedActionsInfoKey))
		mInternals->mStartedActions = OI<CActions>(CActions(info.getDictionary(sStartedActionsInfoKey)));
	if (info.contains(sFinishedActionsInfoKey))
		mInternals->mFinishedActions = OI<CActions>(CActions(info.getDictionary(sFinishedActionsInfoKey)));
	if (info.contains(sAudioInfoKey))
		mInternals->mAudioInfo = OI<CAudioInfo>(CAudioInfo(info.getDictionary(sAudioInfoKey)));
//	if (info.contains(sCelAnimationInfoKey))
//		mInternals->mCelAnimationInfo =
//				OI<CCelAnimationInfo>(CCelAnimationInfo(info.getDictionary(sCelAnimationInfoKey)));
	if (info.contains(sKeyframeAnimationInfoKey))
		mInternals->mKeyframeAnimationInfo =
				OI<CKeyframeAnimationInfo>(CKeyframeAnimationInfo(info.getDictionary(sKeyframeAnimationInfoKey)));
	mInternals->mLoopCount = info.getUInt32(sLoopCountKey, 1);
	if (info.contains(sStartTimeIntervalKey))
		mInternals->mStartTimeInterval = OV<UniversalTimeInterval>(info.getFloat64(sStartTimeIntervalKey));
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
TArray<CDictionary> CSceneItemAnimation::getProperties() const
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	TArray<CDictionary>	properties = CSceneItem::getProperties();

	// Add properties
	CDictionary	startedActionPropertyInfo;
	startedActionPropertyInfo.set(mPropertyTitleKey, CString(OSSTR("Started Actions")));
	startedActionPropertyInfo.set(mPropertyNameKey, sStartedActionsInfoKey);
	startedActionPropertyInfo.set(mPropertyTypeKey, kSceneItemPropertyTypeActions);
	properties += startedActionPropertyInfo;

	CDictionary	finishedActionPropertyInfo;
	finishedActionPropertyInfo.set(mPropertyTitleKey, CString(OSSTR("Finished Actions")));
	finishedActionPropertyInfo.set(mPropertyNameKey, sFinishedActionsInfoKey);
	finishedActionPropertyInfo.set(mPropertyTypeKey, kSceneItemPropertyTypeActions);
	properties += finishedActionPropertyInfo;

	CDictionary	audioInfoPropertyInfo;
	audioInfoPropertyInfo.set(mPropertyTitleKey, CString(OSSTR("Audio")));
	audioInfoPropertyInfo.set(mPropertyNameKey, sAudioInfoKey);
	audioInfoPropertyInfo.set(mPropertyTypeKey, kSceneItemPropertyTypeAudioInfo);
	properties += audioInfoPropertyInfo;

//	CDictionary	celAnimationInfoPropertyInfo;
//	celAnimationInfoPropertyInfo.set(mPropertyTitleKey, CString(OSSTR("Cel Animation")));
//	celAnimationInfoPropertyInfo.set(mPropertyNameKey, sCelAnimationInfoKey);
//	celAnimationInfoPropertyInfo.set(mPropertyTypeKey, kSceneItemPropertyTypeCelAnimationInfo);
//	properties += celAnimationInfoPropertyInfo;

	CDictionary	keyframeAnimationInfoPropertyInfo;
	keyframeAnimationInfoPropertyInfo.set(mPropertyTitleKey, CString(OSSTR("Keyframe Animation")));
	keyframeAnimationInfoPropertyInfo.set(mPropertyNameKey, sKeyframeAnimationInfoKey);
	keyframeAnimationInfoPropertyInfo.set(mPropertyTypeKey, kSceneItemPropertyTypeKeyframeAnimationInfo);
	properties += keyframeAnimationInfoPropertyInfo;

	CDictionary	loopCountPropertyInfo;
	loopCountPropertyInfo.set(mPropertyTitleKey, CString(OSSTR("Loop Count")));
	loopCountPropertyInfo.set(mPropertyNameKey, sLoopCountKey);
	loopCountPropertyInfo.set(mPropertyTypeKey, kSceneItemAnimationPropertyTypeLoopCount);
	properties += loopCountPropertyInfo;

	CDictionary	startTimeIntervalPropertyInfo;
	startTimeIntervalPropertyInfo.set(mPropertyTitleKey, CString(OSSTR("Start Time")));
	startTimeIntervalPropertyInfo.set(mPropertyNameKey, sStartTimeIntervalKey);
	startTimeIntervalPropertyInfo.set(mPropertyTypeKey, kSceneItemPropertyTypeStartTimeInterval);
	properties += startTimeIntervalPropertyInfo;

	return properties;
}

//----------------------------------------------------------------------------------------------------------------------
CDictionary CSceneItemAnimation::getInfo() const
//----------------------------------------------------------------------------------------------------------------------
{
	CDictionary	info = CSceneItem::getInfo();

	if (mInternals->mStartedActions.hasInstance())
		info.set(sStartedActionsInfoKey, mInternals->mStartedActions->getInfo());
	if (mInternals->mFinishedActions.hasInstance())
		info.set(sFinishedActionsInfoKey, mInternals->mFinishedActions->getInfo());
	if (mInternals->mAudioInfo.hasInstance())
		info.set(sAudioInfoKey, mInternals->mAudioInfo->getInfo());
//	if (mInternals->mCelAnimationInfo.hasInstance())
//		info.set(sCelAnimationInfoKey, mInternals->mCelAnimationInfo->getInfo());
	if (mInternals->mKeyframeAnimationInfo.hasInstance())
		info.set(sKeyframeAnimationInfoKey, mInternals->mKeyframeAnimationInfo->getInfo());
	info.set(sLoopCountKey, mInternals->mLoopCount);
	if (mInternals->mStartTimeInterval.hasValue())
		info.set(sStartTimeIntervalKey, *mInternals->mStartTimeInterval);

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
	mInternals = mInternals->prepareForWrite();

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
	mInternals = mInternals->prepareForWrite();

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
	mInternals = mInternals->prepareForWrite();

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
//	mInternals = mInternals->prepareForWrite();
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
	mInternals = mInternals->prepareForWrite();

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
	mInternals = mInternals->prepareForWrite();

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
	mInternals = mInternals->prepareForWrite();

	// Update
	mInternals->mStartTimeInterval = startTimeInterval;
}
