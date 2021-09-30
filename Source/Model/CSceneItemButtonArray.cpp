//----------------------------------------------------------------------------------------------------------------------
//	CSceneItemButtonArray.cpp			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CSceneItemButtonArray.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CSceneItemButtonArrayButtonInternals

class CSceneItemButtonArrayButtonInternals :
		public TCopyOnWriteReferenceCountable<CSceneItemButtonArrayButtonInternals> {
	public:
		CSceneItemButtonArrayButtonInternals() : TCopyOnWriteReferenceCountable() {}
		CSceneItemButtonArrayButtonInternals(const CSceneItemButtonArrayButtonInternals& other) :
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
CSceneItemButtonArrayButton::CSceneItemButtonArrayButton()
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new CSceneItemButtonArrayButtonInternals();
}

//----------------------------------------------------------------------------------------------------------------------
CSceneItemButtonArrayButton::CSceneItemButtonArrayButton(const CDictionary& info)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new CSceneItemButtonArrayButtonInternals();

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
	mInternals = mInternals->prepareForWrite();

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
	mInternals = mInternals->prepareForWrite();

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
	mInternals = mInternals->prepareForWrite();

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
	mInternals = mInternals->prepareForWrite();

	// Update
	mInternals->mScreenPositionPoint = screenPositionPoint;
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneItemButtonArrayInternals

class CSceneItemButtonArrayInternals : public TCopyOnWriteReferenceCountable<CSceneItemButtonArrayInternals> {
	public:
		CSceneItemButtonArrayInternals() : TCopyOnWriteReferenceCountable() {}
		CSceneItemButtonArrayInternals(const CSceneItemButtonArrayInternals& other) :
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

CString	CSceneItemButtonArray::mType(OSSTR("buttonArray"));

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
CSceneItemButtonArray::CSceneItemButtonArray() : CSceneItem()
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new CSceneItemButtonArrayInternals();
}

//----------------------------------------------------------------------------------------------------------------------
CSceneItemButtonArray::CSceneItemButtonArray(const CDictionary& info) : CSceneItem(info)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new CSceneItemButtonArrayInternals();

	if (info.contains(CString(OSSTR("startTime"))))
		mInternals->mStartTimeInterval = OV<UniversalTimeInterval>(info.getFloat64(CString(OSSTR("startTime"))));
	mInternals->mImageResourceFilename = info.getString(CString(OSSTR("imageFilename")));
	mInternals->mSceneItemButtonArrayButtons =
			TNArray<CSceneItemButtonArrayButton>(info.getArrayOfDictionaries(CString(OSSTR("buttonsInfo"))),
					(CSceneItemButtonArrayButton (*)(CArray::ItemRef item)) CSceneItemButtonArrayButton::makeFrom);
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
	startTimePropertyInfo.set(mPropertyNameKey, CString(OSSTR("startTime")));
	startTimePropertyInfo.set(mPropertyTypeKey, CSceneItem::kPropertyTypeStartTimeInterval);
	properties += startTimePropertyInfo;

	CDictionary	resourceFilenamePropertyInfo;
	resourceFilenamePropertyInfo.set(mPropertyTitleKey, CString(OSSTR("Image Filename")));
	resourceFilenamePropertyInfo.set(mPropertyNameKey, CString(OSSTR("imageFilename")));
	resourceFilenamePropertyInfo.set(mPropertyTypeKey, CSceneItem::kPropertyTypeFilename);
	properties += resourceFilenamePropertyInfo;

	CDictionary	buttonArrayPropertyInfo;
	buttonArrayPropertyInfo.set(mPropertyTitleKey, CString(OSSTR("Button Array")));
	buttonArrayPropertyInfo.set(mPropertyNameKey, CString(OSSTR("buttonsInfo")));
	buttonArrayPropertyInfo.set(mPropertyTypeKey, kPropertyTypeButtonArray);
	properties += buttonArrayPropertyInfo;

	return properties;
}

//----------------------------------------------------------------------------------------------------------------------
CDictionary CSceneItemButtonArray::getInfo() const
//----------------------------------------------------------------------------------------------------------------------
{
	CDictionary	info = CSceneItem::getInfo();

	if (mInternals->mStartTimeInterval.hasValue())
		info.set(CString(OSSTR("startTime")), *mInternals->mStartTimeInterval);
	info.set(CString(OSSTR("imageFilename")), mInternals->mImageResourceFilename);
	info.set(CString(OSSTR("buttonsInfo")),
			TNArray<CDictionary>(mInternals->mSceneItemButtonArrayButtons,
					(CDictionary (*)(CArray::ItemRef item)) CSceneItemButtonArrayButton::getInfoFrom));

	return info;
}

// MARK: Instance methods

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
	// Prepare to write
	mInternals = mInternals->prepareForWrite();

	// Update
	mInternals->mStartTimeInterval = startTimeInterval;
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
	// Prepare to write
	mInternals = mInternals->prepareForWrite();

	// Update
	mInternals->mImageResourceFilename = imageResourceFilename;
}

//----------------------------------------------------------------------------------------------------------------------
const TArray<CSceneItemButtonArrayButton>& CSceneItemButtonArray::getSceneItemButtonArrayButtons() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mSceneItemButtonArrayButtons;
}
