//----------------------------------------------------------------------------------------------------------------------
//	CSceneItemVideo.cpp			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CSceneItemVideo.h"

#include "CReferenceCountable.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CSceneItemVideo::Internals

class CSceneItemVideo::Internals : public TCopyOnWriteReferenceCountable<Internals> {
	public:
		Internals() : TCopyOnWriteReferenceCountable(), mControlMode(CSceneItemVideo::kControlModeDefault) {}
		Internals(const Internals& other) :
			TCopyOnWriteReferenceCountable(),
					mControlMode(other.mControlMode), mResourceFilename(other.mResourceFilename),
					mFinishedActions(other.mFinishedActions), mScreenRect(other.mScreenRect),
					mStartTimeInterval(other.mStartTimeInterval)
			{}

		CSceneItemVideo::ControlMode	mControlMode;
		OV<CString>						mResourceFilename;
		OV<CActions>					mFinishedActions;
		S2DRectF32						mScreenRect;
		OV<UniversalTimeInterval>		mStartTimeInterval;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneItemVideo

// MARK: Properties

const	CString	CSceneItemVideo::mType(OSSTR("video"));

const	CString	CSceneItemVideo::mPropertyNameControlMode(OSSTR("controlMode"));
const	CString	CSceneItemVideo::mPropertyNameFilename(OSSTR("filename"));
const	CString	CSceneItemVideo::mPropertyNameFinishedActions(OSSTR("finishedActionInfo"));
const	CString	CSceneItemVideo::mPropertyNameHotspots(OSSTR("hotspots"));
const	CString	CSceneItemVideo::mPropertyNameInBackgroundImageFilename(OSSTR("inBackgroundFilename"));
const	CString	CSceneItemVideo::mPropertyNameScreenRect(OSSTR("screenRect"));
const	CString	CSceneItemVideo::mPropertyNameStartTimeInterval(OSSTR("startTime"));

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
CSceneItemVideo::CSceneItemVideo(const CDictionary& info) : CSceneItem(info)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new Internals();

	mInternals->mControlMode = (ControlMode) info.getUInt32(mPropertyNameControlMode, mInternals->mControlMode);
	if (info.contains(mPropertyNameFilename))
		mInternals->mResourceFilename.setValue(info.getString(mPropertyNameFilename));
	if (info.contains(mPropertyNameFinishedActions))
		mInternals->mFinishedActions.setValue(CActions(info.getDictionary(mPropertyNameFinishedActions)));
	mInternals->mScreenRect = S2DRectF32(info.getString(mPropertyNameScreenRect));
	mInternals->mStartTimeInterval = info.getOVFloat64(mPropertyNameStartTimeInterval);
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
TMArray<CDictionary> CSceneItemVideo::getProperties() const
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	TMArray<CDictionary>	properties = CSceneItem::getProperties();

	// Add properties
	CDictionary	resourceFilenamePropertyInfo;
	resourceFilenamePropertyInfo.set(mPropertyTitleKey, CString(OSSTR("Filename")));
	resourceFilenamePropertyInfo.set(mPropertyNameKey, mPropertyNameFilename);
	resourceFilenamePropertyInfo.set(mPropertyTypeKey, CSceneItem::kPropertyTypeFilename);
	properties += resourceFilenamePropertyInfo;

	CDictionary	finishedActionPropertyInfo;
	finishedActionPropertyInfo.set(mPropertyTitleKey, CString(OSSTR("Finished Actions")));
	finishedActionPropertyInfo.set(mPropertyNameKey, mPropertyNameFinishedActions);
	finishedActionPropertyInfo.set(mPropertyTypeKey, CSceneItem::kPropertyTypeActions);
	properties += finishedActionPropertyInfo;

	CDictionary	screenRectPropertyInfo;
	screenRectPropertyInfo.set(mPropertyTitleKey, CString(OSSTR("Screen Rect")));
	screenRectPropertyInfo.set(mPropertyNameKey, mPropertyNameScreenRect);
	screenRectPropertyInfo.set(mPropertyTypeKey, CSceneItem::kPropertyTypeScreenRect);
	properties += screenRectPropertyInfo;

	CDictionary	startTimePropertyInfo;
	startTimePropertyInfo.set(mPropertyTitleKey, CString(OSSTR("Start Time Interval")));
	startTimePropertyInfo.set(mPropertyNameKey, mPropertyNameStartTimeInterval);
	startTimePropertyInfo.set(mPropertyTypeKey, CSceneItem::kPropertyTypeStartTimeInterval);
	properties += startTimePropertyInfo;

	return properties;
}

//----------------------------------------------------------------------------------------------------------------------
CDictionary CSceneItemVideo::getInfo() const
//----------------------------------------------------------------------------------------------------------------------
{
	CDictionary	info = CSceneItem::getInfo();

	info.set(mPropertyNameControlMode, (UInt32) mInternals->mControlMode);
	info.set(mPropertyNameFilename, mInternals->mResourceFilename);
	if (mInternals->mFinishedActions.hasValue())
		info.set(mPropertyNameFinishedActions, mInternals->mFinishedActions->getInfo());
	info.set(mPropertyNameScreenRect, mInternals->mScreenRect.asString());
	info.set(mPropertyNameStartTimeInterval, mInternals->mStartTimeInterval);

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
	// Check if changed
	if (controlMode != mInternals->mControlMode) {
		// Prepare to write
		Internals::prepareForWrite(&mInternals);

		// Update
		mInternals->mControlMode = controlMode;

		// Note updated
		noteUpdated(mPropertyNameControlMode);
	}
}

//----------------------------------------------------------------------------------------------------------------------
const OV<CString>& CSceneItemVideo::getResourceFilename() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mResourceFilename;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemVideo::setResourceFilename(const OV<CString>& resourceFilename)
//----------------------------------------------------------------------------------------------------------------------
{
	// Check if changed
	if (resourceFilename != mInternals->mResourceFilename) {
		// Prepare to write
		Internals::prepareForWrite(&mInternals);

		// Update
		mInternals->mResourceFilename = resourceFilename;

		// Note updated
		noteUpdated(mPropertyNameFilename);
	}
}

//----------------------------------------------------------------------------------------------------------------------
const OV<CActions>& CSceneItemVideo::getFinishedActions() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mFinishedActions;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemVideo::setFinishedActions(const OV<CActions>& actions)
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
const S2DRectF32& CSceneItemVideo::getScreenRect() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mScreenRect;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemVideo::setScreenRect(const S2DRectF32& rect)
//----------------------------------------------------------------------------------------------------------------------
{
	// Check if changed
	if (rect != mInternals->mScreenRect) {
		// Prepare to write
		Internals::prepareForWrite(&mInternals);

		// Update
		mInternals->mScreenRect = rect;

		// Note updated
		noteUpdated(mPropertyNameScreenRect);
	}
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
	// Check if changed
	if (startTimeInterval != mInternals->mStartTimeInterval) {
		// Prepare to write
		Internals::prepareForWrite(&mInternals);

		// Update
		mInternals->mStartTimeInterval = startTimeInterval;

		// Note updated
		noteUpdated(mPropertyNameStartTimeInterval);
	}
}
