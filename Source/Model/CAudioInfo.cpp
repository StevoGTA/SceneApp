//----------------------------------------------------------------------------------------------------------------------
//	CAudioInfo.cpp			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CAudioInfo.h"

#include "CReferenceCountable.h"
#include "CSceneItem.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CAudioInfo::Internals

class CAudioInfo::Internals : public TCopyOnWriteReferenceCountable<Internals> {
	public:
		Internals(Float32 gain, const CString& resourceFilename, OV<UInt32> loopCount, CAudioInfo::Options options) :
			TCopyOnWriteReferenceCountable(),
					mGain(gain), mResourceFilename(resourceFilename), mLoopCount(loopCount), mOptions(options)
			{}
		Internals(const Internals& other) :
			TCopyOnWriteReferenceCountable(),
					mGain(other.mGain), mResourceFilename(other.mResourceFilename), mLoopCount(other.mLoopCount),
					mOptions(other.mOptions)
			{}

		Float32				mGain;
		CString				mResourceFilename;
		OV<UInt32>			mLoopCount;
		CAudioInfo::Options	mOptions;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CAudioInfo

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
CAudioInfo::CAudioInfo(const CString& resourceFilename)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new Internals(1.0, resourceFilename, 1, kOptionsNone);
}

//----------------------------------------------------------------------------------------------------------------------
CAudioInfo::CAudioInfo(const CDictionary& info)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	const	CString&	resourceFilename = info.getString(CString(OSSTR("filename")));
	AssertFailIf(resourceFilename.isEmpty())

	mInternals =
			new Internals(info.getFloat32(CString(OSSTR("gain")), 1.0), info.getString(CString(OSSTR("filename"))),
					info.contains(CString(OSSTR("loopCount"))) ?
							OV<UInt32>(info.getUInt32(CString(OSSTR("loopCount")))) : OV<UInt32>(),
					(Options) info.getUInt32(CString(OSSTR("options")), kOptionsNone));
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
	gainPropertyInfo.set(CSceneItem::mPropertyNameKey, CString(OSSTR("gain")));
	gainPropertyInfo.set(CSceneItem::mPropertyTypeKey, kPropertyTypeGain);
	properties += gainPropertyInfo;

	CDictionary	resourceFilenamePropertyInfo;
	resourceFilenamePropertyInfo.set(CSceneItem::mPropertyTitleKey, CString(OSSTR("Resource Filename")));
	resourceFilenamePropertyInfo.set(CSceneItem::mPropertyNameKey, CString(OSSTR("filename")));
	resourceFilenamePropertyInfo.set(CSceneItem::mPropertyTypeKey, CSceneItem::kPropertyTypeFilename);
	properties += resourceFilenamePropertyInfo;

	CDictionary	loopCountPropertyInfo;
	loopCountPropertyInfo.set(CSceneItem::mPropertyTitleKey, CString(OSSTR("Loop Count")));
	loopCountPropertyInfo.set(CSceneItem::mPropertyNameKey, CString(OSSTR("loopCount")));
	loopCountPropertyInfo.set(CSceneItem::mPropertyTypeKey, kPropertyTypeLoopCount);
	properties += loopCountPropertyInfo;

	CDictionary	optionsPropertyInfo;
	optionsPropertyInfo.set(CSceneItem::mPropertyTitleKey, CString(OSSTR("Options")));
	optionsPropertyInfo.set(CSceneItem::mPropertyNameKey, CString(OSSTR("options")));
	optionsPropertyInfo.set(CSceneItem::mPropertyTypeKey, kPropertyTypeOptions);
	properties += optionsPropertyInfo;

	return properties;
}

//----------------------------------------------------------------------------------------------------------------------
CDictionary CAudioInfo::getInfo() const
//----------------------------------------------------------------------------------------------------------------------
{
	CDictionary	info;

	info.set(CString(OSSTR("gain")), mInternals->mGain);
	info.set(CString(OSSTR("filename")), mInternals->mResourceFilename);
	if (mInternals->mLoopCount.hasValue())
		info.set(CString(OSSTR("loopCount")), *mInternals->mLoopCount);
	info.set(CString(OSSTR("options")), (UInt32) mInternals->mOptions);

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
	Internals::prepareForWrite(&mInternals);

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
	Internals::prepareForWrite(&mInternals);

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
	Internals::prepareForWrite(&mInternals);

	// Update
	mInternals->mLoopCount = loopCount;
}

//----------------------------------------------------------------------------------------------------------------------
CAudioInfo::Options CAudioInfo::getOptions() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mOptions;
}

//----------------------------------------------------------------------------------------------------------------------
void CAudioInfo::setOptions(Options options)
//----------------------------------------------------------------------------------------------------------------------
{
	// Prepare to write
	Internals::prepareForWrite(&mInternals);

	// Update
	mInternals->mOptions = options;
}
