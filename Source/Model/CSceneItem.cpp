//----------------------------------------------------------------------------------------------------------------------
//	CSceneItem.cpp			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CSceneItem.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: Local data

static	CString	sNameKey(OSSTR("name"));
static	CString	sOptionsKey(OSSTR("options"));
static	CString	sIsVisibleKey(OSSTR("visible"));

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneItemInternals

class CSceneItemInternals : public TCopyOnWriteReferenceCountable<CSceneItemInternals> {
	public:
		CSceneItemInternals() : TCopyOnWriteReferenceCountable(), mIsVisible(true), mOptions(kSceneItemOptionsNone) {}
		CSceneItemInternals(const CSceneItemInternals& other) :
			TCopyOnWriteReferenceCountable(),
					mIsVisible(other.mIsVisible), mName(other.mName), mOptions(other.mOptions)
			{}

		bool				mIsVisible;
		CString				mName;
		ESceneItemOptions	mOptions;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneItem

// MARK: Properties

CString	CSceneItem::mItemInfoKey(OSSTR("itemInfo"));

CString	CSceneItem::mPropertyNameKey(OSSTR("name"));
CString	CSceneItem::mPropertyTitleKey(OSSTR("title"));
CString	CSceneItem::mPropertyTypeKey(OSSTR("type"));

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
	mInternals = other.mInternals->addReference();
}

//----------------------------------------------------------------------------------------------------------------------
CSceneItem::~CSceneItem()
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->removeReference();
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
	// Prepare to write
	mInternals->prepareForWrite();

	// Update
	mInternals->mIsVisible = isVisible;
}

//----------------------------------------------------------------------------------------------------------------------
const CString& CSceneItem::getName() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mName;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItem::setName(const CString& name)
//----------------------------------------------------------------------------------------------------------------------
{
	// Prepare to write
	mInternals->prepareForWrite();

	// Update
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
	// Prepare to write
	mInternals->prepareForWrite();

	// Update
	mInternals->mOptions = options;
}

//----------------------------------------------------------------------------------------------------------------------
CString CSceneItem::getDescription()
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mName + CString(OSSTR(" (")) + getType() + CString(OSSTR(")"));
}

//----------------------------------------------------------------------------------------------------------------------
TArray<CDictionary> CSceneItem::getProperties() const
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	TNArray<CDictionary>	properties;

	// Add properties
	CDictionary	namePropertyInfo;
	namePropertyInfo.set(mPropertyTitleKey, CString(OSSTR("Name")));
	namePropertyInfo.set(mPropertyNameKey, sNameKey);
	namePropertyInfo.set(mPropertyTypeKey, kSceneItemPropertyTypeName);
	properties += namePropertyInfo;

	CDictionary	visiblePropertyInfo;
	visiblePropertyInfo.set(mPropertyTitleKey, CString(OSSTR("Visible")));
	visiblePropertyInfo.set(mPropertyNameKey, sIsVisibleKey);
	visiblePropertyInfo.set(mPropertyTypeKey, kSceneItemPropertyTypeBoolean);
	properties += visiblePropertyInfo;

	CDictionary	optionsPropertyInfo;
	optionsPropertyInfo.set(mPropertyTitleKey, CString(OSSTR("Options")));
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
