//----------------------------------------------------------------------------------------------------------------------
//	CSceneItem.cpp			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CSceneItem.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CSceneItemInternals

class CSceneItemInternals : public TCopyOnWriteReferenceCountable<CSceneItemInternals> {
	public:
		CSceneItemInternals() :
			TCopyOnWriteReferenceCountable(), mIsVisible(true), mOptions(CSceneItem::kOptionsNone)
			{}
		CSceneItemInternals(const CSceneItemInternals& other) :
			TCopyOnWriteReferenceCountable(),
					mIsVisible(other.mIsVisible), mName(other.mName), mOptions(other.mOptions)
			{}

		bool				mIsVisible;
		CString				mName;
		CSceneItem::Options	mOptions;
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

	mInternals->mIsVisible = info.getBool(CString(OSSTR("visible")), mInternals->mIsVisible);
	mInternals->mName = info.getString(CString(OSSTR("name")), mInternals->mName);
	mInternals->mOptions = (Options) info.getUInt32(CString(OSSTR("options")), mInternals->mOptions);
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
	mInternals = mInternals->prepareForWrite();

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
	mInternals = mInternals->prepareForWrite();

	// Update
	mInternals->mName = name;
}

//----------------------------------------------------------------------------------------------------------------------
CSceneItem::Options CSceneItem::getOptions() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mOptions;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItem::setOptions(Options options)
//----------------------------------------------------------------------------------------------------------------------
{
	// Prepare to write
	mInternals = mInternals->prepareForWrite();

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
	namePropertyInfo.set(mPropertyNameKey, CString(OSSTR("name")));
	namePropertyInfo.set(mPropertyTypeKey, kPropertyTypeName);
	properties += namePropertyInfo;

	CDictionary	visiblePropertyInfo;
	visiblePropertyInfo.set(mPropertyTitleKey, CString(OSSTR("Visible")));
	visiblePropertyInfo.set(mPropertyNameKey, CString(OSSTR("visible")));
	visiblePropertyInfo.set(mPropertyTypeKey, kPropertyTypeBoolean);
	properties += visiblePropertyInfo;

	CDictionary	optionsPropertyInfo;
	optionsPropertyInfo.set(mPropertyTitleKey, CString(OSSTR("Options")));
	optionsPropertyInfo.set(mPropertyNameKey, CString(OSSTR("options")));
	optionsPropertyInfo.set(mPropertyTypeKey, kPropertyTypeOptions);
	properties += optionsPropertyInfo;

	return properties;
}

//----------------------------------------------------------------------------------------------------------------------
CDictionary CSceneItem::getInfo() const
//----------------------------------------------------------------------------------------------------------------------
{
	CDictionary	info;

	info.set(CString(OSSTR("visible")), (UInt32) (mInternals->mIsVisible ? 1 : 0));
	info.set(CString(OSSTR("name")), mInternals->mName);
	info.set(CString(OSSTR("options")), (UInt32) mInternals->mOptions);

	return info;
}
