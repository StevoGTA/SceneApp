//----------------------------------------------------------------------------------------------------------------------
//	CSceneItemVideo.cpp			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CSceneItemVideo.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CSceneItemVideoInternals

class CSceneItemVideoInternals : public TCopyOnWriteReferenceCountable<CSceneItemVideoInternals> {
	public:
		CSceneItemVideoInternals() :
			mControlMode(CSceneItemVideo::kControlModeDefault)
			{}
		CSceneItemVideoInternals(const CSceneItemVideoInternals& other) :
			mControlMode(other.mControlMode), mResourceFilename(other.mResourceFilename),
					mFinishedActions(other.mFinishedActions), mScreenRect(other.mScreenRect),
					mStartTimeInterval(other.mStartTimeInterval)
			{}

		CSceneItemVideo::ControlMode	mControlMode;
		OI<CString>						mResourceFilename;
		OI<CActions>					mFinishedActions;
		S2DRectF32						mScreenRect;
		OV<UniversalTimeInterval>		mStartTimeInterval;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneItemVideo

// MARK: Properties

CString	CSceneItemVideo::mType(OSSTR("video"));

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
CSceneItemVideo::CSceneItemVideo() : CSceneItem()
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new CSceneItemVideoInternals();
}

//----------------------------------------------------------------------------------------------------------------------
CSceneItemVideo::CSceneItemVideo(const CDictionary& info) : CSceneItem(info)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new CSceneItemVideoInternals();

	mInternals->mControlMode = (ControlMode) info.getUInt32(CString(OSSTR("controlMode")), mInternals->mControlMode);
	if (info.contains(CString(OSSTR("filename"))))
		mInternals->mResourceFilename = OI<CString>(info.getString(CString(OSSTR("filename"))));
	if (info.contains(CString(OSSTR("finishedActionInfo"))))
		mInternals->mFinishedActions = OI<CActions>(CActions(info.getDictionary(CString(OSSTR("finishedActionInfo")))));
	mInternals->mScreenRect = S2DRectF32(info.getString(CString(OSSTR("screenRect"))));
	if (info.contains(CString(OSSTR("startTime"))))
		mInternals->mStartTimeInterval = info.getFloat64(CString(OSSTR("startTime")));
}

//----------------------------------------------------------------------------------------------------------------------
CSceneItemVideo::CSceneItemVideo(const CSceneItemVideo& other) : CSceneItem(other)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = other.mInternals->addReference();
}

//----------------------------------------------------------------------------------------------------------------------
CSceneItemVideo::~CSceneItemVideo()
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->removeReference();
}

// MARK: CSceneItem methods

//----------------------------------------------------------------------------------------------------------------------
TArray<CDictionary> CSceneItemVideo::getProperties() const
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	TArray<CDictionary>	properties = CSceneItem::getProperties();

	// Add properties
//	CDictionary	controlModePropertyInfo;
//	controlModePropertyInfo.set(mPropertyTitleKey, CString(OSSTR("Control Mode")));
//	controlModePropertyInfo.set(mPropertyNameKey, CString(OSSTR("controlMode")));
//	controlModePropertyInfo.set(mPropertyTypeKey, CSceneItem::kPropertyTypeControlMode);
//	properties += controlModePropertyInfo;

	CDictionary	resourceFilenamePropertyInfo;
	resourceFilenamePropertyInfo.set(mPropertyTitleKey, CString(OSSTR("Filename")));
	resourceFilenamePropertyInfo.set(mPropertyNameKey, CString(OSSTR("filename")));
	resourceFilenamePropertyInfo.set(mPropertyTypeKey, CSceneItem::kPropertyTypeFilename);
	properties += resourceFilenamePropertyInfo;

	CDictionary	finishedActionPropertyInfo;
	finishedActionPropertyInfo.set(mPropertyTitleKey, CString(OSSTR("Finished Actions")));
	finishedActionPropertyInfo.set(mPropertyNameKey, CString(OSSTR("finishedActionInfo")));
	finishedActionPropertyInfo.set(mPropertyTypeKey, CSceneItem::kPropertyTypeActions);
	properties += finishedActionPropertyInfo;

//	CDictionary	hotspotsArrayPropertyInfo;
//	hotspotsArrayPropertyInfo.set(mPropertyTitleKey, CString(OSSTR("Hotspots Array")));
//	hotspotsArrayPropertyInfo.set(mPropertyNameKey, CString(OSSTR("hotspots")));
//	hotspotsArrayPropertyInfo.set(mPropertyTypeKey, CSceneItem::kPropertyTypeScreenRectArray);
//	properties += hotspotsArrayPropertyInfo;

//	CDictionary	inImageResourceFilenamePropertyInfo;
//	inImageResourceFilenamePropertyInfo.set(mPropertyTitleKey, CString(OSSTR("In Image Filename")));
//	inImageResourceFilenamePropertyInfo.set(mPropertyNameKey, CString(OSSTR("inBackgroundFilename")));
//	inImageResourceFilenamePropertyInfo.set(mPropertyTypeKey, CSceneItem::kPropertyTypeFilename);
//	properties += inImageResourceFilenamePropertyInfo;

	CDictionary	screenRectPropertyInfo;
	screenRectPropertyInfo.set(mPropertyTitleKey, CString(OSSTR("Screen Rect")));
	screenRectPropertyInfo.set(mPropertyNameKey, CString(OSSTR("screenRect")));
	screenRectPropertyInfo.set(mPropertyTypeKey, CSceneItem::kPropertyTypeScreenRect);
	properties += screenRectPropertyInfo;

	CDictionary	startTimePropertyInfo;
	startTimePropertyInfo.set(mPropertyTitleKey, CString(OSSTR("Start Time Interval")));
	startTimePropertyInfo.set(mPropertyNameKey, CString(OSSTR("startTime")));
	startTimePropertyInfo.set(mPropertyTypeKey, CSceneItem::kPropertyTypeStartTimeInterval);
	properties += startTimePropertyInfo;

	return properties;
}

//----------------------------------------------------------------------------------------------------------------------
CDictionary CSceneItemVideo::getInfo() const
//----------------------------------------------------------------------------------------------------------------------
{
	CDictionary	info = CSceneItem::getInfo();

	info.set(CString(OSSTR("controlMode")), (UInt32) mInternals->mControlMode);
	if (mInternals->mResourceFilename.hasInstance())
		info.set(CString(OSSTR("filename")), *mInternals->mResourceFilename);
	if (mInternals->mFinishedActions.hasInstance())
		info.set(CString(OSSTR("finishedActionInfo")), mInternals->mFinishedActions->getInfo());
	info.set(CString(OSSTR("screenRect")), mInternals->mScreenRect.asString());
	if (mInternals->mStartTimeInterval.hasValue())
		info.set(CString(OSSTR("startTime")), *mInternals->mStartTimeInterval);

	return info;
}

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
CSceneItemVideo::ControlMode CSceneItemVideo::getControlMode() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mControlMode;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemVideo::setControlMode(ControlMode controlMode)
//----------------------------------------------------------------------------------------------------------------------
{
	// Prepare to write
	mInternals = mInternals->prepareForWrite();

	// Update
	mInternals->mControlMode = controlMode;
}

//----------------------------------------------------------------------------------------------------------------------
const OI<CString>& CSceneItemVideo::getResourceFilename() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mResourceFilename;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemVideo::setResourceFilename(const OI<CString>& resourceFilename)
//----------------------------------------------------------------------------------------------------------------------
{
	// Prepare to write
	mInternals = mInternals->prepareForWrite();

	// Update
	mInternals->mResourceFilename = resourceFilename;
}

//----------------------------------------------------------------------------------------------------------------------
const OI<CActions>& CSceneItemVideo::getFinishedActions() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mFinishedActions;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemVideo::setFinishedActions(const OI<CActions>& actions)
//----------------------------------------------------------------------------------------------------------------------
{
	// Prepare to write
	mInternals = mInternals->prepareForWrite();

	// Update
	mInternals->mFinishedActions = actions;
}

//----------------------------------------------------------------------------------------------------------------------
const S2DRectF32& CSceneItemVideo::getScreenRect() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mScreenRect;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemVideo::setScreenRect(const S2DRectF32& rect)
//----------------------------------------------------------------------------------------------------------------------
{
	// Prepare to write
	mInternals = mInternals->prepareForWrite();

	// Update
	mInternals->mScreenRect = rect;
}

//----------------------------------------------------------------------------------------------------------------------
const OV<UniversalTimeInterval>& CSceneItemVideo::getStartTimeInterval() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mStartTimeInterval;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemVideo::setStartTimeInterval(OV<UniversalTimeInterval> startTimeInterval)
//----------------------------------------------------------------------------------------------------------------------
{
	// Prepare to write
	mInternals = mInternals->prepareForWrite();

	// Update
	mInternals->mStartTimeInterval = startTimeInterval;
}
