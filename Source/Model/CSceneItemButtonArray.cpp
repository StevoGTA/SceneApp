//----------------------------------------------------------------------------------------------------------------------
//	CSceneItemButtonArray.cpp			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CSceneItemButtonArray.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: - Local data

// Button Array Keys
static	CString	sButtonsInfoKey("buttonsInfo");
static	CString	sImageResourceFilenameKey("imageFilename");
static	CString	sStartTimeIntervalKey("startTime");

// Individual Button Keys
static	CString	sActionArrayInfoKey("actionInfo");
static	CString	sDownImageRectKey("downImageRect");
static	CString	sScreenPositionPointKey("screenPositionPt");
static	CString	sUpImageRectKey("upImageRect");

CString	CSceneItemButtonArray::mType("buttonArray");

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneItemButtonArrayButtonInternals

class CSceneItemButtonArrayButtonInternals {
	public:
		CSceneItemButtonArrayButtonInternals() : mActionArray(nil) {}
		~CSceneItemButtonArrayButtonInternals()
			{
				DisposeOf(mActionArray);
			}

		CActionArray*	mActionArray;
		S2DRect32		mUpImageRect;
		S2DRect32		mDownImageRect;
		S2DPoint32		mScreenPositionPoint;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneItemButtonArrayInternals

class CSceneItemButtonArrayInternals {
	public:
		CSceneItemButtonArrayInternals() : mStartTimeInterval(0.0), mSceneButtonArrayButtonArray(true) {}
		~CSceneItemButtonArrayInternals() {}

		UniversalTimeInterval					mStartTimeInterval;
		CString									mImageResourceFilename;
		TPtrArray<CSceneItemButtonArrayButton*>	mSceneButtonArrayButtonArray;
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

	if (info.contains(sActionArrayInfoKey))
		mInternals->mActionArray = new CActionArray(info.getDictionary(sActionArrayInfoKey));
	mInternals->mUpImageRect = S2DRect32(info.getString(sUpImageRectKey));
	mInternals->mDownImageRect = S2DRect32(info.getString(sDownImageRectKey));
	mInternals->mScreenPositionPoint = S2DPoint32(info.getString(sScreenPositionPointKey));
}

//----------------------------------------------------------------------------------------------------------------------
CSceneItemButtonArrayButton::CSceneItemButtonArrayButton(const CSceneItemButtonArrayButton& other)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new CSceneItemButtonArrayButtonInternals();

	if (other.mInternals->mActionArray != nil)
		mInternals->mActionArray = new CActionArray(*other.mInternals->mActionArray);
	mInternals->mUpImageRect = other.mInternals->mUpImageRect;
	mInternals->mDownImageRect = other.mInternals->mDownImageRect;
	mInternals->mScreenPositionPoint = other.mInternals->mScreenPositionPoint;
}

//----------------------------------------------------------------------------------------------------------------------
CSceneItemButtonArrayButton::~CSceneItemButtonArrayButton()
//----------------------------------------------------------------------------------------------------------------------
{
	DisposeOf(mInternals);
}

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
TArray<CDictionary> CSceneItemButtonArrayButton::getProperties() const
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	TArray<CDictionary>	properties;

	// Add properties
	CDictionary	actionPropertyInfo;
	actionPropertyInfo.set(CSceneItem::mPropertyTitleKey, CString("Action Array"));
	actionPropertyInfo.set(CSceneItem::mPropertyNameKey, sActionArrayInfoKey);
	actionPropertyInfo.set(CSceneItem::mPropertyTypeKey, kSceneItemPropertyTypeActionArray);
	properties += actionPropertyInfo;

	CDictionary	upImageRectPropertyInfo;
	upImageRectPropertyInfo.set(CSceneItem::mPropertyTitleKey, CString("Up Image Rect"));
	upImageRectPropertyInfo.set(CSceneItem::mPropertyNameKey, sUpImageRectKey);
	upImageRectPropertyInfo.set(CSceneItem::mPropertyTypeKey, kSceneItemPropertyTypeScreenRect);
	properties += upImageRectPropertyInfo;

	CDictionary	downImageRectPropertyInfo;
	downImageRectPropertyInfo.set(CSceneItem::mPropertyTitleKey, CString("Down Image Rect"));
	downImageRectPropertyInfo.set(CSceneItem::mPropertyNameKey, sDownImageRectKey);
	downImageRectPropertyInfo.set(CSceneItem::mPropertyTypeKey, kSceneItemPropertyTypeScreenRect);
	properties += downImageRectPropertyInfo;

	CDictionary	resourceFilenamePropertyInfo;
	resourceFilenamePropertyInfo.set(CSceneItem::mPropertyTitleKey, CString("Screen Position Point"));
	resourceFilenamePropertyInfo.set(CSceneItem::mPropertyNameKey, sScreenPositionPointKey);
	resourceFilenamePropertyInfo.set(CSceneItem::mPropertyTypeKey, kSceneItemPropertyTypeScreenPoint);
	properties += resourceFilenamePropertyInfo;

	return properties;
}

//----------------------------------------------------------------------------------------------------------------------
CDictionary CSceneItemButtonArrayButton::getInfo()
//----------------------------------------------------------------------------------------------------------------------
{
	CDictionary	info;

	if (mInternals->mActionArray != nil)
		info.set(sActionArrayInfoKey, mInternals->mActionArray->getInfo());
	info.set(sUpImageRectKey, mInternals->mUpImageRect.asString());
	info.set(sDownImageRectKey, mInternals->mDownImageRect.asString());
	info.set(sScreenPositionPointKey, mInternals->mScreenPositionPoint.asString());

	return info;
}

//----------------------------------------------------------------------------------------------------------------------
OR<CActionArray> CSceneItemButtonArrayButton::getActionArray() const
//----------------------------------------------------------------------------------------------------------------------
{
	return (mInternals->mActionArray != nil) ? OR<CActionArray>(*mInternals->mActionArray) : OR<CActionArray>();
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemButtonArrayButton::setActionArray(const CActionArray& actionArray)
//----------------------------------------------------------------------------------------------------------------------
{
	DisposeOf(mInternals->mActionArray);
	mInternals->mActionArray = new CActionArray(actionArray);
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
	mInternals->mScreenPositionPoint = screenPositionPoint;
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneItemButtonArray

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

	mInternals->mStartTimeInterval = info.getFloat64(sStartTimeIntervalKey);
	mInternals->mImageResourceFilename = info.getString(sImageResourceFilenameKey);

	TArray<CDictionary>	buttonInfos = info.getArrayOfDictionaries(sButtonsInfoKey);
	for (CArrayItemIndex i = 0; i < buttonInfos.getCount(); i++)
		mInternals->mSceneButtonArrayButtonArray += new CSceneItemButtonArrayButton(buttonInfos[i]);
}

//----------------------------------------------------------------------------------------------------------------------
CSceneItemButtonArray::CSceneItemButtonArray(const CSceneItemButtonArray& other) : CSceneItem(other)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new CSceneItemButtonArrayInternals();

	mInternals->mStartTimeInterval = other.mInternals->mStartTimeInterval;
	mInternals->mImageResourceFilename = other.mInternals->mImageResourceFilename;

	for (CArrayItemIndex i = 0; i < other.mInternals->mSceneButtonArrayButtonArray.getCount(); i++)
		mInternals->mSceneButtonArrayButtonArray +=
				new CSceneItemButtonArrayButton(*other.mInternals->mSceneButtonArrayButtonArray[i]);
}

//----------------------------------------------------------------------------------------------------------------------
CSceneItemButtonArray::~CSceneItemButtonArray()
//----------------------------------------------------------------------------------------------------------------------
{
	DisposeOf(mInternals);
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
	startTimePropertyInfo.set(mPropertyTitleKey, CString("Start Time Interval"));
	startTimePropertyInfo.set(mPropertyNameKey, sStartTimeIntervalKey);
	startTimePropertyInfo.set(mPropertyTypeKey, kSceneItemPropertyTypeStartTimeInterval);
	properties += startTimePropertyInfo;

	CDictionary	resourceFilenamePropertyInfo;
	resourceFilenamePropertyInfo.set(mPropertyTitleKey, CString("Image Filename"));
	resourceFilenamePropertyInfo.set(mPropertyNameKey, sImageResourceFilenameKey);
	resourceFilenamePropertyInfo.set(mPropertyTypeKey, kSceneItemPropertyTypeFilename);
	properties += resourceFilenamePropertyInfo;

	CDictionary	buttonArrayPropertyInfo;
	buttonArrayPropertyInfo.set(mPropertyTitleKey, CString("Button Array"));
	buttonArrayPropertyInfo.set(mPropertyNameKey, sButtonsInfoKey);
	buttonArrayPropertyInfo.set(mPropertyTypeKey, kSceneItemButtonArrayPropertyTypeButtonArray);
	properties += buttonArrayPropertyInfo;

	return properties;
}

//----------------------------------------------------------------------------------------------------------------------
CDictionary CSceneItemButtonArray::getInfo()
//----------------------------------------------------------------------------------------------------------------------
{
	CDictionary	info = CSceneItem::getInfo();

	info.set(sStartTimeIntervalKey, mInternals->mStartTimeInterval);
	info.set(sImageResourceFilenameKey, mInternals->mImageResourceFilename);

	TArray<CDictionary>	buttonInfos;
	for (CArrayItemCount i = 0; i < mInternals->mSceneButtonArrayButtonArray.getCount(); i++)
		buttonInfos += mInternals->mSceneButtonArrayButtonArray[i]->getInfo();
	info.set(sButtonsInfoKey, buttonInfos);

	return info;
}

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
UniversalTimeInterval CSceneItemButtonArray::getStartTimeInterval() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mStartTimeInterval;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemButtonArray::setStartTimeInterval(UniversalTimeInterval startTimeInterval)
//----------------------------------------------------------------------------------------------------------------------
{
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
	mInternals->mImageResourceFilename = imageResourceFilename;
}

//----------------------------------------------------------------------------------------------------------------------
const TPtrArray<CSceneItemButtonArrayButton*>& CSceneItemButtonArray::getSceneButtonArrayButtonArray() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mSceneButtonArrayButtonArray;
}
