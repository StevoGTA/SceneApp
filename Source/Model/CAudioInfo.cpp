//----------------------------------------------------------------------------------------------------------------------
//	CAudioInfo.cpp			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CAudioInfo.h"

#include "CSceneItem.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: Local data

static	CString	sResourceFilenameKey(OSSTR("filename"));
static	CString	sGainKey(OSSTR("gain"));
static	CString	sLoopCountKey(OSSTR("loopCount"));
static	CString	sOptionsKey(OSSTR("options"));

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CAudioInfoInternals

class CAudioInfoInternals : public TCopyOnWriteReferenceCountable<CAudioInfoInternals> {
	public:
		CAudioInfoInternals(Float32 gain, const CString& resourceFilename, OV<UInt32> loopCount,
				EAudioInfoOptions options) :
			TCopyOnWriteReferenceCountable(),
					mGain(gain), mResourceFilename(resourceFilename), mLoopCount(loopCount), mOptions(options)
			{}
		CAudioInfoInternals(const CAudioInfoInternals& other) :
			TCopyOnWriteReferenceCountable(),
					mGain(other.mGain), mResourceFilename(other.mResourceFilename), mLoopCount(other.mLoopCount),
					mOptions(other.mOptions)
			{}

		Float32				mGain;
		CString				mResourceFilename;
		OV<UInt32>			mLoopCount;
		EAudioInfoOptions	mOptions;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CAudioInfo

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
CAudioInfo::CAudioInfo(const CString& resourceFilename)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new CAudioInfoInternals(1.0, resourceFilename, 1, kAudioInfoOptionsNone);
}

//----------------------------------------------------------------------------------------------------------------------
CAudioInfo::CAudioInfo(const CDictionary& info)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	const	CString&	resourceFilename = info.getString(sResourceFilenameKey);
	AssertFailIf(resourceFilename.isEmpty())

	mInternals =
			new CAudioInfoInternals(info.getFloat32(sGainKey, 1.0), info.getString(sResourceFilenameKey),
					info.contains(sLoopCountKey) ? OV<UInt32>(info.getUInt32(sLoopCountKey)) : OV<UInt32>(),
					(EAudioInfoOptions) info.getUInt32(sOptionsKey, kAudioInfoOptionsNone));
}

//----------------------------------------------------------------------------------------------------------------------
CAudioInfo::CAudioInfo(const CAudioInfo& other)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = other.mInternals->addReference();
}

//----------------------------------------------------------------------------------------------------------------------
CAudioInfo::~CAudioInfo()
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->removeReference();
}

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
TArray<CDictionary> CAudioInfo::getProperties() const
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	TNArray<CDictionary>	properties;

	// Add properties
	CDictionary	gainPropertyInfo;
	gainPropertyInfo.set(CSceneItem::mPropertyTitleKey, CString(OSSTR("Gain")));
	gainPropertyInfo.set(CSceneItem::mPropertyNameKey, sGainKey);
	gainPropertyInfo.set(CSceneItem::mPropertyTypeKey, kAudioInfoPropertyTypeGain);
	properties += gainPropertyInfo;

	CDictionary	resourceFilenamePropertyInfo;
	resourceFilenamePropertyInfo.set(CSceneItem::mPropertyTitleKey, CString(OSSTR("Resource Filename")));
	resourceFilenamePropertyInfo.set(CSceneItem::mPropertyNameKey, sResourceFilenameKey);
	resourceFilenamePropertyInfo.set(CSceneItem::mPropertyTypeKey, kSceneItemPropertyTypeFilename);
	properties += resourceFilenamePropertyInfo;

	CDictionary	loopCountPropertyInfo;
	loopCountPropertyInfo.set(CSceneItem::mPropertyTitleKey, CString(OSSTR("Loop Count")));
	loopCountPropertyInfo.set(CSceneItem::mPropertyNameKey, sLoopCountKey);
	loopCountPropertyInfo.set(CSceneItem::mPropertyTypeKey, kAudioInfoPropertyTypeLoopCount);
	properties += loopCountPropertyInfo;

	CDictionary	optionsPropertyInfo;
	optionsPropertyInfo.set(CSceneItem::mPropertyTitleKey, CString(OSSTR("Options")));
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
	if (mInternals->mLoopCount.hasValue())
		info.set(sLoopCountKey, *mInternals->mLoopCount);
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
	// Prepare to write
	mInternals = mInternals->prepareForWrite();

	// Update
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
	// Prepare to write
	mInternals = mInternals->prepareForWrite();

	// Update
	mInternals->mResourceFilename = resourceFilename;
}

//----------------------------------------------------------------------------------------------------------------------
OV<UInt32> CAudioInfo::getLoopCount() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mLoopCount;
}

//----------------------------------------------------------------------------------------------------------------------
void CAudioInfo::setLoopCount(OV<UInt32> loopCount)
//----------------------------------------------------------------------------------------------------------------------
{
	// Prepare to write
	mInternals = mInternals->prepareForWrite();

	// Update
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
	// Prepare to write
	mInternals = mInternals->prepareForWrite();

	// Update
	mInternals->mOptions = options;
}
