//----------------------------------------------------------------------------------------------------------------------
//	CSceneItem.cpp			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CSceneItem.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: Local data

static	CString	sNameKey("name");
static	CString	sOptionsKey("options");
static	CString	sIsVisibleKey("visible");

CString	CSceneItem::mItemInfoKey("itemInfo");

CString	CSceneItem::mPropertyNameKey("name");
CString	CSceneItem::mPropertyTitleKey("title");
CString	CSceneItem::mPropertyTypeKey("type");

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneItemInternals

class CSceneItemInternals {
	public:
		CSceneItemInternals() : mIsVisible(true), mOptions(kSceneItemOptionsNone) {}
		~CSceneItemInternals() {}

		bool				mIsVisible;
		CString				mName;
		ESceneItemOptions	mOptions;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneItem

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
CSceneItem::CSceneItem()
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new CSceneItemInternals();
}

//----------------------------------------------------------------------------------------------------------------------
CSceneItem::CSceneItem(const CDictionary& info)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new CSceneItemInternals();

	mInternals->mIsVisible = info.getBool(sIsVisibleKey, mInternals->mIsVisible);
	mInternals->mName = info.getString(sNameKey, mInternals->mName);
	mInternals->mOptions = (ESceneItemOptions) info.getUInt32(sOptionsKey, mInternals->mOptions);
}

//----------------------------------------------------------------------------------------------------------------------
CSceneItem::CSceneItem(const CSceneItem& other)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new CSceneItemInternals();

	mInternals->mIsVisible = other.mInternals->mIsVisible;
	mInternals->mName = other.mInternals->mName;
	mInternals->mOptions = other.mInternals->mOptions;
}

//----------------------------------------------------------------------------------------------------------------------
CSceneItem::~CSceneItem()
//----------------------------------------------------------------------------------------------------------------------
{
	DisposeOf(mInternals);
}

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
bool CSceneItem::getIsVisible() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mIsVisible;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItem::setIsVisible(bool isVisible)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mIsVisible = isVisible;
}

//----------------------------------------------------------------------------------------------------------------------
CString& CSceneItem::getName() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mName;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItem::setName(const CString& name)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mName = name;
}

//----------------------------------------------------------------------------------------------------------------------
ESceneItemOptions CSceneItem::getOptions() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mOptions;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItem::setOptions(ESceneItemOptions options)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mOptions = options;
}

//----------------------------------------------------------------------------------------------------------------------
CString CSceneItem::getDescription()
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mName + CString(" (") + getType() + CString(")");
}

//----------------------------------------------------------------------------------------------------------------------
TArray<CDictionary> CSceneItem::getProperties() const
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	TArray<CDictionary>	properties;

	// Add properties
	CDictionary	namePropertyInfo;
	namePropertyInfo.set(mPropertyTitleKey, CString("Name"));
	namePropertyInfo.set(mPropertyNameKey, sNameKey);
	namePropertyInfo.set(mPropertyTypeKey, kSceneItemPropertyTypeName);
	properties += namePropertyInfo;

	CDictionary	visiblePropertyInfo;
	visiblePropertyInfo.set(mPropertyTitleKey, CString("Visible"));
	visiblePropertyInfo.set(mPropertyNameKey, sIsVisibleKey);
	visiblePropertyInfo.set(mPropertyTypeKey, kSceneItemPropertyTypeBoolean);
	properties += visiblePropertyInfo;

	CDictionary	optionsPropertyInfo;
	optionsPropertyInfo.set(mPropertyTitleKey, CString("Options"));
	optionsPropertyInfo.set(mPropertyNameKey, sOptionsKey);
	optionsPropertyInfo.set(mPropertyTypeKey, kSceneItemPropertyTypeOptions);
	properties += optionsPropertyInfo;

	return properties;
}

//----------------------------------------------------------------------------------------------------------------------
CDictionary CSceneItem::getInfo() const
//----------------------------------------------------------------------------------------------------------------------
{
	CDictionary	info;

	info.set(sIsVisibleKey, (UInt32) (mInternals->mIsVisible ? 1 : 0));
	info.set(sNameKey, mInternals->mName);
	info.set(sOptionsKey, (UInt32) mInternals->mOptions);

	return info;
}
