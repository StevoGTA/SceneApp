//----------------------------------------------------------------------------------------------------------------------
//	CSceneItemButtonArray.cpp			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CSceneItemButtonArray.h"

#include "CReferenceCountable.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CSceneItemButtonArrayButton::Internals

class CSceneItemButtonArrayButton::Internals : public TCopyOnWriteReferenceCountable<Internals> {
	public:
		Internals() : TCopyOnWriteReferenceCountable() {}
		Internals(const Internals& other) :
			TCopyOnWriteReferenceCountable(),
			mActions(other.mActions), mUpImageRect(other.mUpImageRect), mDownImageRect(other.mDownImageRect),
					mScreenPositionPoint(other.mScreenPositionPoint)
			{}

		OI<CActions>	mActions;
		S2DRectF32		mUpImageRect;
		S2DRectF32		mDownImageRect;
		S2DPointF32		mScreenPositionPoint;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneItemButtonArrayButton

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
CSceneItemButtonArrayButton::CSceneItemButtonArrayButton(const CDictionary& info)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new Internals();

	if (info.contains(CString(OSSTR("actionInfo"))))
		mInternals->mActions = OI<CActions>(CActions(info.getDictionary(CString(OSSTR("actionInfo")))));
	mInternals->mUpImageRect = S2DRectF32(info.getString(CString(OSSTR("upImageRect"))));
	mInternals->mDownImageRect = S2DRectF32(info.getString(CString(OSSTR("downImageRect"))));
	mInternals->mScreenPositionPoint = S2DPointF32(info.getString(CString(OSSTR("screenPositionPt"))));
}

//----------------------------------------------------------------------------------------------------------------------
CSceneItemButtonArrayButton::CSceneItemButtonArrayButton(const CSceneItemButtonArrayButton& other)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = other.mInternals->addReference();
}

//----------------------------------------------------------------------------------------------------------------------
CSceneItemButtonArrayButton::~CSceneItemButtonArrayButton()
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->removeReference();
}

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
TArray<CDictionary> CSceneItemButtonArrayButton::getProperties() const
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	TNArray<CDictionary>	properties;

	// Add properties
	CDictionary	actionPropertyInfo;
	actionPropertyInfo.set(CSceneItem::mPropertyTitleKey, CString(OSSTR("Actions")));
	actionPropertyInfo.set(CSceneItem::mPropertyNameKey, CString(OSSTR("actionInfo")));
	actionPropertyInfo.set(CSceneItem::mPropertyTypeKey, CSceneItem::kPropertyTypeActions);
	properties += actionPropertyInfo;

	CDictionary	upImageRectPropertyInfo;
	upImageRectPropertyInfo.set(CSceneItem::mPropertyTitleKey, CString(OSSTR("Up Image Rect")));
	upImageRectPropertyInfo.set(CSceneItem::mPropertyNameKey, CString(OSSTR("upImageRect")));
	upImageRectPropertyInfo.set(CSceneItem::mPropertyTypeKey, CSceneItem::kPropertyTypeScreenRect);
	properties += upImageRectPropertyInfo;

	CDictionary	downImageRectPropertyInfo;
	downImageRectPropertyInfo.set(CSceneItem::mPropertyTitleKey, CString(OSSTR("Down Image Rect")));
	downImageRectPropertyInfo.set(CSceneItem::mPropertyNameKey, CString(OSSTR("downImageRect")));
	downImageRectPropertyInfo.set(CSceneItem::mPropertyTypeKey, CSceneItem::kPropertyTypeScreenRect);
	properties += downImageRectPropertyInfo;

	CDictionary	resourceFilenamePropertyInfo;
	resourceFilenamePropertyInfo.set(CSceneItem::mPropertyTitleKey, CString(OSSTR("Screen Position Point")));
	resourceFilenamePropertyInfo.set(CSceneItem::mPropertyNameKey, CString(OSSTR("screenPositionPt")));
	resourceFilenamePropertyInfo.set(CSceneItem::mPropertyTypeKey, CSceneItem::kPropertyTypeScreenPoint);
	properties += resourceFilenamePropertyInfo;

	return properties;
}

//----------------------------------------------------------------------------------------------------------------------
CDictionary CSceneItemButtonArrayButton::getInfo() const
//----------------------------------------------------------------------------------------------------------------------
{
	CDictionary	info;

	if (mInternals->mActions.hasInstance())
		info.set(CString(OSSTR("actionInfo")), mInternals->mActions->getInfo());
	info.set(CString(OSSTR("upImageRect")), mInternals->mUpImageRect.asString());
	info.set(CString(OSSTR("downImageRect")), mInternals->mDownImageRect.asString());
	info.set(CString(OSSTR("screenPositionPt")), mInternals->mScreenPositionPoint.asString());

	return info;
}

//----------------------------------------------------------------------------------------------------------------------
const OI<CActions>& CSceneItemButtonArrayButton::getActions() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mActions;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemButtonArrayButton::setActions(const OI<CActions>& actions)
//----------------------------------------------------------------------------------------------------------------------
{
	// Prepare to write
	Internals::prepareForWrite(&mInternals);

	// Update
	mInternals->mActions = actions;
}

//----------------------------------------------------------------------------------------------------------------------
const S2DRectF32& CSceneItemButtonArrayButton::getUpImageRect() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mUpImageRect;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemButtonArrayButton::setUpImageRect(const S2DRectF32& upImageRect)
//----------------------------------------------------------------------------------------------------------------------
{
	// Prepare to write
	Internals::prepareForWrite(&mInternals);

	// Update
	mInternals->mUpImageRect = upImageRect;
}

//----------------------------------------------------------------------------------------------------------------------
const S2DRectF32& CSceneItemButtonArrayButton::getDownImageRect() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mDownImageRect;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemButtonArrayButton::setDownImageRect(const S2DRectF32& downImageRect)
//----------------------------------------------------------------------------------------------------------------------
{
	// Prepare to write
	Internals::prepareForWrite(&mInternals);

	// Update
	mInternals->mDownImageRect = downImageRect;
}

//----------------------------------------------------------------------------------------------------------------------
const S2DPointF32& CSceneItemButtonArrayButton::getScreenPositionPoint() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mScreenPositionPoint;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemButtonArrayButton::setScreenPositionPoint(const S2DPointF32& screenPositionPoint)
//----------------------------------------------------------------------------------------------------------------------
{
	// Prepare to write
	Internals::prepareForWrite(&mInternals);

	// Update
	mInternals->mScreenPositionPoint = screenPositionPoint;
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneItemButtonArray::Internals

class CSceneItemButtonArray::Internals : public TCopyOnWriteReferenceCountable<Internals> {
	public:
		Internals() : TCopyOnWriteReferenceCountable() {}
		Internals(const Internals& other) :
			TCopyOnWriteReferenceCountable(),
					mStartTimeInterval(other.mStartTimeInterval), mImageResourceFilename(other.mImageResourceFilename),
					mSceneItemButtonArrayButtons(other.mSceneItemButtonArrayButtons)
			{}

		OV<UniversalTimeInterval>				mStartTimeInterval;
		CString									mImageResourceFilename;
		TNArray<CSceneItemButtonArrayButton>	mSceneItemButtonArrayButtons;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneItemButtonArray

// MARK: Properties

const	CString	CSceneItemButtonArray::mType(OSSTR("buttonArray"));

const	CString	CSceneItemButtonArray::mPropertyNameButtons(OSSTR("buttonsInfo"));
const	CString	CSceneItemButtonArray::mPropertyNameImageFilename(OSSTR("imageFilename"));
const	CString	CSceneItemButtonArray::mPropertyNameStartTimeInterval(OSSTR("startTime"));

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
CSceneItemButtonArray::CSceneItemButtonArray(const CDictionary& info) : CSceneItem(info)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new Internals();

	mInternals->mSceneItemButtonArrayButtons =
			TNArray<CSceneItemButtonArrayButton>(info.getArrayOfDictionaries(mPropertyNameButtons),
					(TNArray<CSceneItemButtonArrayButton>::MapProc) CSceneItemButtonArrayButton::makeFrom);
	mInternals->mImageResourceFilename = info.getString(mPropertyNameImageFilename);
	mInternals->mStartTimeInterval = info.getOVFloat64(mPropertyNameStartTimeInterval);
}

//----------------------------------------------------------------------------------------------------------------------
CSceneItemButtonArray::CSceneItemButtonArray(const CSceneItemButtonArray& other) : CSceneItem(other)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = other.mInternals->addReference();
}

//----------------------------------------------------------------------------------------------------------------------
CSceneItemButtonArray::~CSceneItemButtonArray()
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->removeReference();
}

// MARK: CSceneItem methods

//----------------------------------------------------------------------------------------------------------------------
TMArray<CDictionary> CSceneItemButtonArray::getProperties() const
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	TMArray<CDictionary>	properties = CSceneItem::getProperties();

	// Add properties
	CDictionary	startTimePropertyInfo;
	startTimePropertyInfo.set(mPropertyTitleKey, CString(OSSTR("Start Time Interval")));
	startTimePropertyInfo.set(mPropertyNameKey, mPropertyNameStartTimeInterval);
	startTimePropertyInfo.set(mPropertyTypeKey, CSceneItem::kPropertyTypeStartTimeInterval);
	properties += startTimePropertyInfo;

	CDictionary	resourceFilenamePropertyInfo;
	resourceFilenamePropertyInfo.set(mPropertyTitleKey, CString(OSSTR("Image Filename")));
	resourceFilenamePropertyInfo.set(mPropertyNameKey, mPropertyNameImageFilename);
	resourceFilenamePropertyInfo.set(mPropertyTypeKey, CSceneItem::kPropertyTypeFilename);
	properties += resourceFilenamePropertyInfo;

	CDictionary	buttonArrayPropertyInfo;
	buttonArrayPropertyInfo.set(mPropertyTitleKey, CString(OSSTR("Button Array")));
	buttonArrayPropertyInfo.set(mPropertyNameKey, mPropertyNameButtons);
	buttonArrayPropertyInfo.set(mPropertyTypeKey, kPropertyTypeButtonArray);
	properties += buttonArrayPropertyInfo;

	return properties;
}

//----------------------------------------------------------------------------------------------------------------------
CDictionary CSceneItemButtonArray::getInfo() const
//----------------------------------------------------------------------------------------------------------------------
{
	CDictionary	info = CSceneItem::getInfo();

	info.set(mPropertyNameButtons,
			TNArray<CDictionary>(mInternals->mSceneItemButtonArrayButtons,
					(TNArray<CDictionary>::MapProc) CSceneItemButtonArrayButton::getInfoFrom));
	info.set(mPropertyNameImageFilename, mInternals->mImageResourceFilename);
	info.set(mPropertyNameStartTimeInterval, mInternals->mStartTimeInterval);

	return info;
}

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
const TArray<CSceneItemButtonArrayButton>& CSceneItemButtonArray::getSceneItemButtonArrayButtons() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mSceneItemButtonArrayButtons;
}

//----------------------------------------------------------------------------------------------------------------------
const CString& CSceneItemButtonArray::getImageResourceFilename() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mImageResourceFilename;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemButtonArray::setImageResourceFilename(const CString& imageResourceFilename)
//----------------------------------------------------------------------------------------------------------------------
{
	// Check if changed
	if (imageResourceFilename != mInternals->mImageResourceFilename) {
		// Prepare to write
		Internals::prepareForWrite(&mInternals);

		// Update
		mInternals->mImageResourceFilename = imageResourceFilename;

		// Note updated
		noteUpdated(mPropertyNameImageFilename);
	}
}

//----------------------------------------------------------------------------------------------------------------------
const OV<UniversalTimeInterval>& CSceneItemButtonArray::getStartTimeInterval() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mStartTimeInterval;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemButtonArray::setStartTimeInterval(const OV<UniversalTimeInterval>& startTimeInterval)
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
