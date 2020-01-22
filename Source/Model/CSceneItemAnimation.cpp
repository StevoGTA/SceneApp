//----------------------------------------------------------------------------------------------------------------------
//	CSceneItemAnimation.cpp			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CSceneItemAnimation.h"

#include "CCelAnimationInfo.h"
#include "CKeyframeAnimationInfo.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: Local data

static	CString	sAudioInfoKey("audioInfo");
static	CString	sCelAnimationInfoKey("celAnimationInfo");
static	CString	sFinishedActionArrayInfoKey("finishedActionInfo");
static	CString	sKeyframeAnimationInfoKey("keyframeAnimationInfo");
static	CString	sLoopCountKey("loopCount");
static	CString	sStartedActionArrayInfoKey("startedActionInfo");
static	CString	sStartTimeIntervalKey("startTime");
static	CString	sTriggerTypeKey("triggerType");

CString	CSceneItemAnimation::mType("animation");

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneItemAnimationInternals

class CSceneItemAnimationInternals {
	public:
		CSceneItemAnimationInternals() :
			mStartedActionArray(nil), mFinishedActionArray(nil), mAudioInfo(nil), mCelAnimationInfo(nil),
					mKeyframeAnimationInfo(nil), mTriggerType(kSceneItemAnimationTriggerTypeTimed), mLoopCount(1),
					mStartTimeInterval(0.0)
			{}
		~CSceneItemAnimationInternals()
			{
				DisposeOf(mStartedActionArray);
				DisposeOf(mFinishedActionArray);
				DisposeOf(mAudioInfo);
				DisposeOf(mCelAnimationInfo);
				DisposeOf(mKeyframeAnimationInfo);
			}

		CActionArray*					mStartedActionArray;
		CActionArray*					mFinishedActionArray;
		CAudioInfo*						mAudioInfo;
		CCelAnimationInfo*				mCelAnimationInfo;
		CKeyframeAnimationInfo*			mKeyframeAnimationInfo;
		ESceneItemAnimationTriggerType	mTriggerType;
		UInt32							mLoopCount;
		UniversalTimeInterval			mStartTimeInterval;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneItemAnimation

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

	if (info.contains(sStartedActionArrayInfoKey))
		mInternals->mStartedActionArray = new CActionArray(info.getDictionary(sStartedActionArrayInfoKey));
	if (info.contains(sFinishedActionArrayInfoKey))
		mInternals->mFinishedActionArray = new CActionArray(info.getDictionary(sFinishedActionArrayInfoKey));
	if (info.contains(sAudioInfoKey))
		mInternals->mAudioInfo = new CAudioInfo(info.getDictionary(sAudioInfoKey));
	if (info.contains(sCelAnimationInfoKey))
		mInternals->mCelAnimationInfo = new CCelAnimationInfo(info.getDictionary(sCelAnimationInfoKey));
	if (info.contains(sKeyframeAnimationInfoKey))
		mInternals->mKeyframeAnimationInfo = new CKeyframeAnimationInfo(info.getDictionary(sKeyframeAnimationInfoKey));
	mInternals->mTriggerType =
			(ESceneItemAnimationTriggerType) info.getUInt8(sTriggerTypeKey, mInternals->mTriggerType);
	mInternals->mLoopCount = info.getUInt32(sLoopCountKey, mInternals->mLoopCount);
	mInternals->mStartTimeInterval = info.getFloat64(sStartTimeIntervalKey);
}

//----------------------------------------------------------------------------------------------------------------------
CSceneItemAnimation::CSceneItemAnimation(const CSceneItemAnimation& other) : CSceneItem(other)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new CSceneItemAnimationInternals();

	if (other.mInternals->mStartedActionArray != nil)
		mInternals->mStartedActionArray = new CActionArray(*other.mInternals->mStartedActionArray);
	if (other.mInternals->mFinishedActionArray != nil)
		mInternals->mFinishedActionArray = new CActionArray(*other.mInternals->mFinishedActionArray);
	if (other.mInternals->mAudioInfo != nil)
		mInternals->mAudioInfo = new CAudioInfo(*other.mInternals->mAudioInfo);
	if (other.mInternals->mCelAnimationInfo != nil)
		mInternals->mCelAnimationInfo = other.mInternals->mCelAnimationInfo->copy();
	if (other.mInternals->mKeyframeAnimationInfo != nil)
		mInternals->mKeyframeAnimationInfo = new CKeyframeAnimationInfo(*other.mInternals->mKeyframeAnimationInfo);
	mInternals->mTriggerType = other.mInternals->mTriggerType;
	mInternals->mLoopCount = other.mInternals->mLoopCount;
	mInternals->mStartTimeInterval = other.mInternals->mStartTimeInterval;
}

//----------------------------------------------------------------------------------------------------------------------
CSceneItemAnimation::~CSceneItemAnimation()
//----------------------------------------------------------------------------------------------------------------------
{
	DisposeOf(mInternals);
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
	startedActionPropertyInfo.set(mPropertyTitleKey, CString("Started Action Array"));
	startedActionPropertyInfo.set(mPropertyNameKey, sStartedActionArrayInfoKey);
	startedActionPropertyInfo.set(mPropertyTypeKey, kSceneItemPropertyTypeActionArray);
	properties += startedActionPropertyInfo;

	CDictionary	finishedActionPropertyInfo;
	finishedActionPropertyInfo.set(mPropertyTitleKey, CString("Finished Action Array"));
	finishedActionPropertyInfo.set(mPropertyNameKey, sFinishedActionArrayInfoKey);
	finishedActionPropertyInfo.set(mPropertyTypeKey, kSceneItemPropertyTypeActionArray);
	properties += finishedActionPropertyInfo;

	CDictionary	audioInfoPropertyInfo;
	audioInfoPropertyInfo.set(mPropertyTitleKey, CString("Audio"));
	audioInfoPropertyInfo.set(mPropertyNameKey, sAudioInfoKey);
	audioInfoPropertyInfo.set(mPropertyTypeKey, kSceneItemPropertyTypeAudioInfo);
	properties += audioInfoPropertyInfo;

	CDictionary	celAnimationInfoPropertyInfo;
	celAnimationInfoPropertyInfo.set(mPropertyTitleKey, CString("Cel Animation"));
	celAnimationInfoPropertyInfo.set(mPropertyNameKey, sCelAnimationInfoKey);
	celAnimationInfoPropertyInfo.set(mPropertyTypeKey, kSceneItemPropertyTypeCelAnimationInfo);
	properties += celAnimationInfoPropertyInfo;

	CDictionary	keyframeAnimationInfoPropertyInfo;
	keyframeAnimationInfoPropertyInfo.set(mPropertyTitleKey, CString("Keyframe Animation"));
	keyframeAnimationInfoPropertyInfo.set(mPropertyNameKey, sKeyframeAnimationInfoKey);
	keyframeAnimationInfoPropertyInfo.set(mPropertyTypeKey, kSceneItemPropertyTypeKeyframeAnimationInfo);
	properties += keyframeAnimationInfoPropertyInfo;

	CDictionary	loopCountPropertyInfo;
	loopCountPropertyInfo.set(mPropertyTitleKey, CString("Loop Count"));
	loopCountPropertyInfo.set(mPropertyNameKey, sLoopCountKey);
	loopCountPropertyInfo.set(mPropertyTypeKey, kSceneItemAnimationPropertyTypeLoopCount);
	properties += loopCountPropertyInfo;

	CDictionary	startTimeIntervalPropertyInfo;
	startTimeIntervalPropertyInfo.set(mPropertyTitleKey, CString("Start Time"));
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

	if (mInternals->mStartedActionArray != nil)
		info.set(sStartedActionArrayInfoKey, mInternals->mStartedActionArray->getInfo());
	if (mInternals->mFinishedActionArray != nil)
		info.set(sFinishedActionArrayInfoKey, mInternals->mFinishedActionArray->getInfo());
	if (mInternals->mAudioInfo != nil)
		info.set(sAudioInfoKey, mInternals->mAudioInfo->getInfo());
	if (mInternals->mCelAnimationInfo != nil)
		info.set(sCelAnimationInfoKey, mInternals->mCelAnimationInfo->getInfo());
	if (mInternals->mKeyframeAnimationInfo != nil)
		info.set(sKeyframeAnimationInfoKey, mInternals->mKeyframeAnimationInfo->getInfo());
	info.set(sTriggerTypeKey, (UInt8) mInternals->mTriggerType);
	info.set(sLoopCountKey, mInternals->mLoopCount);
	info.set(sStartTimeIntervalKey, mInternals->mStartTimeInterval);

	return info;
}

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
OR<CActionArray> CSceneItemAnimation::getStartedActionArray() const
//----------------------------------------------------------------------------------------------------------------------
{
	return (mInternals->mStartedActionArray != nil) ?
			OR<CActionArray>(*mInternals->mStartedActionArray) : OR<CActionArray>();
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemAnimation::setStartedActionArray(const CActionArray& actionArray)
//----------------------------------------------------------------------------------------------------------------------
{
	DisposeOf(mInternals->mStartedActionArray);
	mInternals->mStartedActionArray = new CActionArray(actionArray);
}

//----------------------------------------------------------------------------------------------------------------------
OR<CActionArray> CSceneItemAnimation::getFinishedActionArray() const
//----------------------------------------------------------------------------------------------------------------------
{
	return (mInternals->mFinishedActionArray != nil) ?
			OR<CActionArray>(*mInternals->mFinishedActionArray) : OR<CActionArray>();
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemAnimation::setFinishedActionArray(const CActionArray& actionArray)
//----------------------------------------------------------------------------------------------------------------------
{
	DisposeOf(mInternals->mFinishedActionArray);
	mInternals->mFinishedActionArray = new CActionArray(actionArray);
}

//----------------------------------------------------------------------------------------------------------------------
OR<CAudioInfo> CSceneItemAnimation::getAudioInfo() const
//----------------------------------------------------------------------------------------------------------------------
{
	return (mInternals->mAudioInfo != nil) ? OR<CAudioInfo>(*mInternals->mAudioInfo) : OR<CAudioInfo>();
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemAnimation::setAudioInfo(const CAudioInfo& audioInfo)
//----------------------------------------------------------------------------------------------------------------------
{
	DisposeOf(mInternals->mAudioInfo);
	mInternals->mAudioInfo = new CAudioInfo(audioInfo);
}

//----------------------------------------------------------------------------------------------------------------------
OR<CCelAnimationInfo> CSceneItemAnimation::getCelAnimationInfo() const
//----------------------------------------------------------------------------------------------------------------------
{
	return (mInternals->mCelAnimationInfo != nil) ?
			OR<CCelAnimationInfo>(*mInternals->mCelAnimationInfo) : OR<CCelAnimationInfo>();
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemAnimation::setCelAnimationInfo(const CCelAnimationInfo& celAnimationInfo)
//----------------------------------------------------------------------------------------------------------------------
{
	DisposeOf(mInternals->mCelAnimationInfo);
	DisposeOf(mInternals->mKeyframeAnimationInfo);
	mInternals->mCelAnimationInfo = celAnimationInfo.copy();
}

//----------------------------------------------------------------------------------------------------------------------
OR<CKeyframeAnimationInfo> CSceneItemAnimation::getKeyframeAnimationInfo() const
//----------------------------------------------------------------------------------------------------------------------
{
	return (mInternals->mKeyframeAnimationInfo != nil) ?
			OR<CKeyframeAnimationInfo>(*mInternals->mKeyframeAnimationInfo) : OR<CKeyframeAnimationInfo>();
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemAnimation::setKeyframeAnimationInfo(const CKeyframeAnimationInfo& keyframeAnimationInfo)
//----------------------------------------------------------------------------------------------------------------------
{
	DisposeOf(mInternals->mCelAnimationInfo);
	DisposeOf(mInternals->mKeyframeAnimationInfo);
	mInternals->mKeyframeAnimationInfo = new CKeyframeAnimationInfo(keyframeAnimationInfo);
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
	mInternals->mLoopCount = loopCount;
}

//----------------------------------------------------------------------------------------------------------------------
UniversalTimeInterval CSceneItemAnimation::getStartTimeInterval() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mStartTimeInterval;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemAnimation::setStartTimeInterval(UniversalTimeInterval startTimeInterval)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mStartTimeInterval = startTimeInterval;
}
