//----------------------------------------------------------------------------------------------------------------------
//	CAudioInfo.cpp			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CAudioInfo.h"

#include "CSceneItem.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: Local data

static	CString	sResourceFilenameKey("filename");
static	CString	sGainKey("gain");
static	CString	sLoopCountKey("loopCount");
static	CString	sOptionsKey("options");

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CAudioInfoInternals

class CAudioInfoInternals {
	public:
		CAudioInfoInternals() : mGain(1.0), mLoopCount(1), mOptions(kAudioInfoOptionsNone) {}
		~CAudioInfoInternals() {}

		Float32				mGain;				// 0.0f is silent, 1.0f is original gain
		CString				mResourceFilename;
		UInt32				mLoopCount;			// 0 is repeat forever
		EAudioInfoOptions	mOptions;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CAudioInfo

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
CAudioInfo::CAudioInfo()
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new CAudioInfoInternals();
}

//----------------------------------------------------------------------------------------------------------------------
CAudioInfo::CAudioInfo(const CDictionary& info)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new CAudioInfoInternals();

	mInternals->mGain = info.getFloat32(sGainKey, mInternals->mGain);
	mInternals->mResourceFilename = info.getString(sResourceFilenameKey);
	mInternals->mLoopCount = info.getUInt32(sLoopCountKey, mInternals->mLoopCount);
	mInternals->mOptions = (EAudioInfoOptions) info.getUInt32(sOptionsKey, mInternals->mOptions);
}

//----------------------------------------------------------------------------------------------------------------------
CAudioInfo::CAudioInfo(const CAudioInfo& other)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new CAudioInfoInternals();

	mInternals->mGain = other.mInternals->mGain;
	mInternals->mResourceFilename = other.mInternals->mResourceFilename;
	mInternals->mLoopCount = other.mInternals->mLoopCount;
	mInternals->mOptions = other.mInternals->mOptions;
}

//----------------------------------------------------------------------------------------------------------------------
CAudioInfo::~CAudioInfo()
//----------------------------------------------------------------------------------------------------------------------
{
	DisposeOf(mInternals);
}

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
TArray<CDictionary> CAudioInfo::getProperties() const
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	TArray<CDictionary>	properties;

	// Add properties
	CDictionary	gainPropertyInfo;
	gainPropertyInfo.set(CSceneItem::mPropertyTitleKey, CString("Gain"));
	gainPropertyInfo.set(CSceneItem::mPropertyNameKey, sGainKey);
	gainPropertyInfo.set(CSceneItem::mPropertyTypeKey, kAudioInfoPropertyTypeGain);
	properties += gainPropertyInfo;

	CDictionary	resourceFilenamePropertyInfo;
	resourceFilenamePropertyInfo.set(CSceneItem::mPropertyTitleKey, CString("Resource Filename"));
	resourceFilenamePropertyInfo.set(CSceneItem::mPropertyNameKey, sResourceFilenameKey);
	resourceFilenamePropertyInfo.set(CSceneItem::mPropertyTypeKey, kSceneItemPropertyTypeFilename);
	properties += resourceFilenamePropertyInfo;

	CDictionary	loopCountPropertyInfo;
	loopCountPropertyInfo.set(CSceneItem::mPropertyTitleKey, CString("Loop Count"));
	loopCountPropertyInfo.set(CSceneItem::mPropertyNameKey, sLoopCountKey);
	loopCountPropertyInfo.set(CSceneItem::mPropertyTypeKey, kAudioInfoPropertyTypeLoopCount);
	properties += loopCountPropertyInfo;

	CDictionary	optionsPropertyInfo;
	optionsPropertyInfo.set(CSceneItem::mPropertyTitleKey, CString("Options"));
	optionsPropertyInfo.set(CSceneItem::mPropertyNameKey, sOptionsKey);
	optionsPropertyInfo.set(CSceneItem::mPropertyTypeKey, kAudioInfoPropertyTypeOptions);
	properties += optionsPropertyInfo;

	return properties;
}

//----------------------------------------------------------------------------------------------------------------------
CDictionary CAudioInfo::getInfo() const
//----------------------------------------------------------------------------------------------------------------------
{
	CDictionary	info;

	info.set(sGainKey, mInternals->mGain);
	info.set(sResourceFilenameKey, mInternals->mResourceFilename);
	info.set(sLoopCountKey, mInternals->mLoopCount);
	info.set(sOptionsKey, (UInt32) mInternals->mOptions);

	return info;
}

//----------------------------------------------------------------------------------------------------------------------
Float32 CAudioInfo::getGain() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mGain;
}

//----------------------------------------------------------------------------------------------------------------------
void CAudioInfo::setGain(Float32 gain)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mGain = gain;
}

//----------------------------------------------------------------------------------------------------------------------
const CString& CAudioInfo::getResourceFilename() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mResourceFilename;
}

//----------------------------------------------------------------------------------------------------------------------
void CAudioInfo::setResourceFilename(const CString& resourceFilename)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mResourceFilename = resourceFilename;
}

//----------------------------------------------------------------------------------------------------------------------
UInt32 CAudioInfo::getLoopCount() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mLoopCount;
}

//----------------------------------------------------------------------------------------------------------------------
void CAudioInfo::setLoopCount(UInt32 loopCount)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mLoopCount = loopCount;
}

//----------------------------------------------------------------------------------------------------------------------
EAudioInfoOptions CAudioInfo::getOptions() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mOptions;
}

//----------------------------------------------------------------------------------------------------------------------
void CAudioInfo::setOptions(EAudioInfoOptions options)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mOptions = options;
}
