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

		OV<CActions>				mStartedActions;
		OV<CActions>				mFinishedActions;
		OV<CAudioInfo>				mAudioInfo;
		OV<CKeyframeAnimationInfo>	mKeyframeAnimationInfo;
		UInt32						mLoopCount;
		OV<UniversalTimeInterval>	mStartTimeInterval;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneItemAnimation

// MARK: Properties

const	CString	CSceneItemAnimation::mType(OSSTR("animation"));

const	CString	CSceneItemAnimation::mPropertyNameStartedActions(OSSTR("startedActionInfo"));
const	CString	CSceneItemAnimation::mPropertyNameFinishedActions(OSSTR("finishedActionInfo"));
const	CString	CSceneItemAnimation::mPropertyNameAudio(OSSTR("audioInfo"));
const	CString	CSceneItemAnimation::mPropertyNameCelAnimations(OSSTR("celAnimationInfo"));
const	CString	CSceneItemAnimation::mPropertyNameKeyframeAnimations(OSSTR("keyframeAnimationInfo"));
const	CString	CSceneItemAnimation::mPropertyNameLoopCount(OSSTR("loopCount"));
const	CString	CSceneItemAnimation::mPropertyNameStartTime(OSSTR("startTime"));

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
CSceneItemAnimation::CSceneItemAnimation(const CKeyframeAnimationInfo& keyframeAnimationInfo) : CSceneItem()
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	mInternals = new Internals();
	mInternals->mKeyframeAnimationInfo.setValue(keyframeAnimationInfo);
}

//----------------------------------------------------------------------------------------------------------------------
CSceneItemAnimation::CSceneItemAnimation(const CDictionary& info) : CSceneItem(info)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new Internals();

	if (info.contains(mPropertyNameStartedActions))
		mInternals->mStartedActions.setValue(CActions(info.getDictionary(mPropertyNameStartedActions)));
	if (info.contains(mPropertyNameFinishedActions))
		mInternals->mFinishedActions.setValue(CActions(info.getDictionary(mPropertyNameFinishedActions)));
	if (info.contains(mPropertyNameAudio))
		mInternals->mAudioInfo.setValue(CAudioInfo(info.getDictionary(mPropertyNameAudio)));
	if (info.contains(mPropertyNameKeyframeAnimations))
		mInternals->mKeyframeAnimationInfo.setValue(
				CKeyframeAnimationInfo(info.getDictionary(mPropertyNameKeyframeAnimations)));
	mInternals->mLoopCount = info.getUInt32(mPropertyNameLoopCount, 1);
	mInternals->mStartTimeInterval = info.getOVFloat64(mPropertyNameStartTime);
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
	startedActionPropertyInfo.set(mPropertyNameKey, mPropertyNameStartedActions);
	startedActionPropertyInfo.set(mPropertyTypeKey, CSceneItem::kPropertyTypeActions);
	properties += startedActionPropertyInfo;

	CDictionary	finishedActionPropertyInfo;
	finishedActionPropertyInfo.set(mPropertyTitleKey, CString(OSSTR("Finished Actions")));
	finishedActionPropertyInfo.set(mPropertyNameKey, mPropertyNameFinishedActions);
	finishedActionPropertyInfo.set(mPropertyTypeKey, CSceneItem::kPropertyTypeActions);
	properties += finishedActionPropertyInfo;

	CDictionary	audioInfoPropertyInfo;
	audioInfoPropertyInfo.set(mPropertyTitleKey, CString(OSSTR("Audio")));
	audioInfoPropertyInfo.set(mPropertyNameKey, mPropertyNameAudio);
	audioInfoPropertyInfo.set(mPropertyTypeKey, CAudioInfo::kPropertyTypeAudioInfo);
	properties += audioInfoPropertyInfo;

	CDictionary	keyframeAnimationInfoPropertyInfo;
	keyframeAnimationInfoPropertyInfo.set(mPropertyTitleKey, CString(OSSTR("Keyframe Animation")));
	keyframeAnimationInfoPropertyInfo.set(mPropertyNameKey, mPropertyNameKeyframeAnimations);
	keyframeAnimationInfoPropertyInfo.set(mPropertyTypeKey, CKeyframeAnimationInfo::kPropertyTypeKeyframeAnimationInfo);
	properties += keyframeAnimationInfoPropertyInfo;

	CDictionary	loopCountPropertyInfo;
	loopCountPropertyInfo.set(mPropertyTitleKey, CString(OSSTR("Loop Count")));
	loopCountPropertyInfo.set(mPropertyNameKey, mPropertyNameLoopCount);
	loopCountPropertyInfo.set(mPropertyTypeKey, kPropertyTypeLoopCount);
	properties += loopCountPropertyInfo;

	CDictionary	startTimeIntervalPropertyInfo;
	startTimeIntervalPropertyInfo.set(mPropertyTitleKey, CString(OSSTR("Start Time")));
	startTimeIntervalPropertyInfo.set(mPropertyNameKey, mPropertyNameStartTime);
	startTimeIntervalPropertyInfo.set(mPropertyTypeKey, CSceneItem::kPropertyTypeStartTimeInterval);
	properties += startTimeIntervalPropertyInfo;

	return properties;
}

//----------------------------------------------------------------------------------------------------------------------
CDictionary CSceneItemAnimation::getInfo() const
//----------------------------------------------------------------------------------------------------------------------
{
	CDictionary	info = CSceneItem::getInfo();

	if (mInternals->mStartedActions.hasValue())
		info.set(mPropertyNameStartedActions, mInternals->mStartedActions->getInfo());
	if (mInternals->mFinishedActions.hasValue())
		info.set(mPropertyNameFinishedActions, mInternals->mFinishedActions->getInfo());
	if (mInternals->mAudioInfo.hasValue())
		info.set(mPropertyNameAudio, mInternals->mAudioInfo->getInfo());
	if (mInternals->mKeyframeAnimationInfo.hasValue())
		info.set(mPropertyNameKeyframeAnimations, mInternals->mKeyframeAnimationInfo->getInfo());
	info.set(mPropertyNameLoopCount, mInternals->mLoopCount);
	info.set(mPropertyNameStartTime, mInternals->mStartTimeInterval);

	return info;
}

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
const OV<CActions>& CSceneItemAnimation::getStartedActions() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mStartedActions;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemAnimation::setStartedActions(const OV<CActions>& actions)
//----------------------------------------------------------------------------------------------------------------------
{
	// Check if changed
	if (actions != mInternals->mStartedActions) {
		// Prepare to write
		Internals::prepareForWrite(&mInternals);

		// Update
		mInternals->mStartedActions = actions;

		// Note updated
		noteUpdated(mPropertyNameStartedActions);
	}
}

//----------------------------------------------------------------------------------------------------------------------
const OV<CActions>& CSceneItemAnimation::getFinishedActions() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mFinishedActions;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemAnimation::setFinishedActions(const OV<CActions>& actions)
//----------------------------------------------------------------------------------------------------------------------
{
	// Check if changed
	if (actions != mInternals->mFinishedActions) {
		// Prepare to write
		Internals::prepareForWrite(&mInternals);

		// Update
		mInternals->mFinishedActions = actions;

		// Note updated
		noteUpdated(mPropertyNameFinishedActions);
	}
}

//----------------------------------------------------------------------------------------------------------------------
const OV<CAudioInfo>& CSceneItemAnimation::getAudioInfo() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mAudioInfo;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemAnimation::setAudioInfo(const OV<CAudioInfo>& audioInfo)
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
const OV<CKeyframeAnimationInfo>& CSceneItemAnimation::getKeyframeAnimationInfo() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mKeyframeAnimationInfo;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemAnimation::setAnimationInfo(const CKeyframeAnimationInfo& keyframeAnimationInfo)
//----------------------------------------------------------------------------------------------------------------------
{
	// Check if changed
	if (!mInternals->mKeyframeAnimationInfo.hasValue() ||
			(keyframeAnimationInfo != *mInternals->mKeyframeAnimationInfo)) {
		// Prepare to write
		Internals::prepareForWrite(&mInternals);

		// Update
		mInternals->mKeyframeAnimationInfo.setValue(keyframeAnimationInfo);

		// Note updated
		noteUpdated(mPropertyNameKeyframeAnimations);
	}
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
	// Check if changed
	if (loopCount != mInternals->mLoopCount) {
		// Prepare to write
		Internals::prepareForWrite(&mInternals);

		// Update
		mInternals->mLoopCount = loopCount;

		// Note updated
		noteUpdated(mPropertyNameLoopCount);
	}
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
