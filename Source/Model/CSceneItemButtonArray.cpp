//----------------------------------------------------------------------------------------------------------------------
//	CSceneItemButtonArray.cpp			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CSceneItemButtonArray.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: Local data

// Button Array Keys
static	CString	sButtonsInfoKey(OSSTR("buttonsInfo"));
static	CString	sImageResourceFilenameKey(OSSTR("imageFilename"));
static	CString	sStartTimeIntervalKey(OSSTR("startTime"));

// Individual Button Keys
static	CString	sActionsInfoKey(OSSTR("actionInfo"));
static	CString	sDownImageRectKey(OSSTR("downImageRect"));
static	CString	sScreenPositionPointKey(OSSTR("screenPositionPt"));
static	CString	sUpImageRectKey(OSSTR("upImageRect"));

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneItemButtonArrayButtonInternals

class CSceneItemButtonArrayButtonInternals :
		public TCopyOnWriteReferenceCountable<CSceneItemButtonArrayButtonInternals> {
	public:
		CSceneItemButtonArrayButtonInternals() : TCopyOnWriteReferenceCountable() {}
		CSceneItemButtonArrayButtonInternals(const CSceneItemButtonArrayButtonInternals& other) :
			TCopyOnWriteReferenceCountable(),
			mActions(other.mActions), mUpImageRect(other.mUpImageRect), mDownImageRect(other.mDownImageRect),
					mScreenPositionPoint(other.mScreenPositionPoint)
			{}

		OO<CActions>	mActions;
		S2DRect32		mUpImageRect;
		S2DRect32		mDownImageRect;
		S2DPoint32		mScreenPositionPoint;
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

	if (info.contains(sActionsInfoKey))
		mInternals->mActions = OO<CActions>(CActions(info.getDictionary(sActionsInfoKey)));
	mInternals->mUpImageRect = S2DRect32(info.getString(sUpImageRectKey));
	mInternals->mDownImageRect = S2DRect32(info.getString(sDownImageRectKey));
	mInternals->mScreenPositionPoint = S2DPoint32(info.getString(sScreenPositionPointKey));
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
	actionPropertyInfo.set(CSceneItem::mPropertyNameKey, sActionsInfoKey);
	actionPropertyInfo.set(CSceneItem::mPropertyTypeKey, kSceneItemPropertyTypeActions);
	properties += actionPropertyInfo;

	CDictionary	upImageRectPropertyInfo;
	upImageRectPropertyInfo.set(CSceneItem::mPropertyTitleKey, CString(OSSTR("Up Image Rect")));
	upImageRectPropertyInfo.set(CSceneItem::mPropertyNameKey, sUpImageRectKey);
	upImageRectPropertyInfo.set(CSceneItem::mPropertyTypeKey, kSceneItemPropertyTypeScreenRect);
	properties += upImageRectPropertyInfo;

	CDictionary	downImageRectPropertyInfo;
	downImageRectPropertyInfo.set(CSceneItem::mPropertyTitleKey, CString(OSSTR("Down Image Rect")));
	downImageRectPropertyInfo.set(CSceneItem::mPropertyNameKey, sDownImageRectKey);
	downImageRectPropertyInfo.set(CSceneItem::mPropertyTypeKey, kSceneItemPropertyTypeScreenRect);
	properties += downImageRectPropertyInfo;

	CDictionary	resourceFilenamePropertyInfo;
	resourceFilenamePropertyInfo.set(CSceneItem::mPropertyTitleKey, CString(OSSTR("Screen Position Point")));
	resourceFilenamePropertyInfo.set(CSceneItem::mPropertyNameKey, sScreenPositionPointKey);
	resourceFilenamePropertyInfo.set(CSceneItem::mPropertyTypeKey, kSceneItemPropertyTypeScreenPoint);
	properties += resourceFilenamePropertyInfo;

	return properties;
}

//----------------------------------------------------------------------------------------------------------------------
CDictionary CSceneItemButtonArrayButton::getInfo() const
//----------------------------------------------------------------------------------------------------------------------
{
	CDictionary	info;

	if (mInternals->mActions.hasObject())
		info.set(sActionsInfoKey, mInternals->mActions->getInfo());
	info.set(sUpImageRectKey, mInternals->mUpImageRect.asString());
	info.set(sDownImageRectKey, mInternals->mDownImageRect.asString());
	info.set(sScreenPositionPointKey, mInternals->mScreenPositionPoint.asString());

	return info;
}

//----------------------------------------------------------------------------------------------------------------------
const OO<CActions>& CSceneItemButtonArrayButton::getActions() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mActions;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemButtonArrayButton::setActions(const OO<CActions>& actions)
//----------------------------------------------------------------------------------------------------------------------
{
	// Prepare to write
	mInternals->prepareForWrite();

	// Update
	mInternals->mActions = actions;
}

//----------------------------------------------------------------------------------------------------------------------
const S2DRect32& CSceneItemButtonArrayButton::getUpImageRect() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mUpImageRect;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemButtonArrayButton::setUpImageRect(const S2DRect32& upImageRect)
//----------------------------------------------------------------------------------------------------------------------
{
	// Prepare to write
	mInternals->prepareForWrite();

	// Update
	mInternals->mUpImageRect = upImageRect;
}

//----------------------------------------------------------------------------------------------------------------------
const S2DRect32& CSceneItemButtonArrayButton::getDownImageRect() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mDownImageRect;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemButtonArrayButton::setDownImageRect(const S2DRect32& downImageRect)
//----------------------------------------------------------------------------------------------------------------------
{
	// Prepare to write
	mInternals->prepareForWrite();

	// Update
	mInternals->mDownImageRect = downImageRect;
}

//----------------------------------------------------------------------------------------------------------------------
const S2DPoint32& CSceneItemButtonArrayButton::getScreenPositionPoint() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mScreenPositionPoint;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemButtonArrayButton::setScreenPositionPoint(const S2DPoint32& screenPositionPoint)
//----------------------------------------------------------------------------------------------------------------------
{
	// Prepare to write
	mInternals->prepareForWrite();

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

	if (info.contains(sStartTimeIntervalKey))
		mInternals->mStartTimeInterval = OV<UniversalTimeInterval>(info.getFloat64(sStartTimeIntervalKey));
	mInternals->mImageResourceFilename = info.getString(sImageResourceFilenameKey);
	mInternals->mSceneItemButtonArrayButtons =
			TNArray<CSceneItemButtonArrayButton>(info.getArrayOfDictionaries(sButtonsInfoKey),
					(CSceneItemButtonArrayButton (*)(CArrayItemRef item)) CSceneItemButtonArrayButton::makeFrom);
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
TArray<CDictionary> CSceneItemButtonArray::getProperties() const
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	TArray<CDictionary>	properties = CSceneItem::getProperties();

	// Add properties
	CDictionary	startTimePropertyInfo;
	startTimePropertyInfo.set(mPropertyTitleKey, CString(OSSTR("Start Time Interval")));
	startTimePropertyInfo.set(mPropertyNameKey, sStartTimeIntervalKey);
	startTimePropertyInfo.set(mPropertyTypeKey, kSceneItemPropertyTypeStartTimeInterval);
	properties += startTimePropertyInfo;

	CDictionary	resourceFilenamePropertyInfo;
	resourceFilenamePropertyInfo.set(mPropertyTitleKey, CString(OSSTR("Image Filename")));
	resourceFilenamePropertyInfo.set(mPropertyNameKey, sImageResourceFilenameKey);
	resourceFilenamePropertyInfo.set(mPropertyTypeKey, kSceneItemPropertyTypeFilename);
	properties += resourceFilenamePropertyInfo;

	CDictionary	buttonArrayPropertyInfo;
	buttonArrayPropertyInfo.set(mPropertyTitleKey, CString(OSSTR("Button Array")));
	buttonArrayPropertyInfo.set(mPropertyNameKey, sButtonsInfoKey);
	buttonArrayPropertyInfo.set(mPropertyTypeKey, kSceneItemButtonArrayPropertyTypeButtonArray);
	properties += buttonArrayPropertyInfo;

	return properties;
}

//----------------------------------------------------------------------------------------------------------------------
CDictionary CSceneItemButtonArray::getInfo() const
//----------------------------------------------------------------------------------------------------------------------
{
	CDictionary	info = CSceneItem::getInfo();

	if (mInternals->mStartTimeInterval.hasValue())
		info.set(sStartTimeIntervalKey, *mInternals->mStartTimeInterval);
	info.set(sImageResourceFilenameKey, mInternals->mImageResourceFilename);
	info.set(sButtonsInfoKey,
			TNArray<CDictionary>(mInternals->mSceneItemButtonArrayButtons,
					(CDictionary (*)(CArrayItemRef item)) CSceneItemButtonArrayButton::getInfoFrom));

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
	mInternals->prepareForWrite();

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
	mInternals->prepareForWrite();

	// Update
	mInternals->mImageResourceFilename = imageResourceFilename;
}

//----------------------------------------------------------------------------------------------------------------------
const TNArray<CSceneItemButtonArrayButton>& CSceneItemButtonArray::getSceneItemButtonArrayButtons() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mSceneItemButtonArrayButtons;
}
