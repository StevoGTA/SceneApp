//----------------------------------------------------------------------------------------------------------------------
//	CSceneItem.cpp			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CSceneItem.h"

#include "CReferenceCountable.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CSceneItem::Internals

class CSceneItem::Internals : public TCopyOnWriteReferenceCountable<Internals> {
	public:
		Internals(const CString& name, bool isInitiallyVisible, Options options) :
			TCopyOnWriteReferenceCountable(),
					mName(name), mIsInitiallyVisible(isInitiallyVisible), mOptions(options),
					mNoteUpdatedProc(nil), mNoteUpdatedProcUserData(nil)
			{}
		Internals(const Internals& other) :
			TCopyOnWriteReferenceCountable(),
					mIsInitiallyVisible(other.mIsInitiallyVisible), mName(other.mName), mOptions(other.mOptions),
					mNoteUpdatedProc(other.mNoteUpdatedProc), mNoteUpdatedProcUserData(other.mNoteUpdatedProcUserData)
			{}

		CString				mName;
		bool				mIsInitiallyVisible;
		CSceneItem::Options	mOptions;

		NoteUpdatedProc		mNoteUpdatedProc;
		void*				mNoteUpdatedProcUserData;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneItem

// MARK: Properties

const	CString	CSceneItem::mItemInfoKey(OSSTR("itemInfo"));

const	CString	CSceneItem::mPropertyNameKey(OSSTR("name"));
const	CString	CSceneItem::mPropertyTitleKey(OSSTR("title"));
const	CString	CSceneItem::mPropertyTypeKey(OSSTR("type"));

const	CString	CSceneItem::mPropertyNameIsInitiallyVisible(OSSTR("isInitiallyVisible"));
const	CString	CSceneItem::mPropertyNameName(OSSTR("name"));
const	CString	CSceneItem::mPropertyNameOptions(OSSTR("options"));

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
CSceneItem::CSceneItem(const CString& name, bool isInitiallyVisible, Options options)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new Internals(name, isInitiallyVisible, options);
}

//----------------------------------------------------------------------------------------------------------------------
CSceneItem::CSceneItem(const CDictionary& info)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals =
			new Internals(info.getString(mPropertyNameName, CString::mEmpty),
					info.getBool(mPropertyNameIsInitiallyVisible, true),
					(Options) info.getUInt32(mPropertyNameOptions, kOptionsNone));
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
bool CSceneItem::getIsInitiallyVisible() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mIsInitiallyVisible;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItem::setIsInitiallyVisible(bool isInitiallyVisible)
//----------------------------------------------------------------------------------------------------------------------
{
	// Check if changed
	if (isInitiallyVisible != mInternals->mIsInitiallyVisible) {
		// Prepare to write
		Internals::prepareForWrite(&mInternals);

		// Update
		mInternals->mIsInitiallyVisible = isInitiallyVisible;

		// Note updated
		noteUpdated(mPropertyNameIsInitiallyVisible);
	}
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
	// Check if changed
	if (name != mInternals->mName) {
		// Prepare to write
		Internals::prepareForWrite(&mInternals);

		// Update
		mInternals->mName = name;

		// Note updated
		noteUpdated(mPropertyNameName);
	}
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
	// Check if changed
	if (options != mInternals->mOptions) {
		// Prepare to write
		Internals::prepareForWrite(&mInternals);

		// Update
		mInternals->mOptions = options;

		// Note updated
		noteUpdated(mPropertyNameOptions);
	}
}

//----------------------------------------------------------------------------------------------------------------------
CString CSceneItem::getDescription()
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mName + CString(OSSTR(" (")) + getType() + CString(OSSTR(")"));
}

//----------------------------------------------------------------------------------------------------------------------
TMArray<CDictionary> CSceneItem::getProperties() const
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	TNArray<CDictionary>	properties;

	// Add properties
	CDictionary	namePropertyInfo;
	namePropertyInfo.set(mPropertyTitleKey, CString(OSSTR("Name")));
	namePropertyInfo.set(mPropertyNameKey, mPropertyNameName);
	namePropertyInfo.set(mPropertyTypeKey, kPropertyTypeName);
	properties += namePropertyInfo;

	CDictionary	visiblePropertyInfo;
	visiblePropertyInfo.set(mPropertyTitleKey, CString(OSSTR("Initially Visible")));
	visiblePropertyInfo.set(mPropertyNameKey, mPropertyNameIsInitiallyVisible);
	visiblePropertyInfo.set(mPropertyTypeKey, kPropertyTypeBoolean);
	properties += visiblePropertyInfo;

	CDictionary	optionsPropertyInfo;
	optionsPropertyInfo.set(mPropertyTitleKey, CString(OSSTR("Options")));
	optionsPropertyInfo.set(mPropertyNameKey, mPropertyNameOptions);
	optionsPropertyInfo.set(mPropertyTypeKey, kPropertyTypeOptions);
	properties += optionsPropertyInfo;

	return properties;
}

//----------------------------------------------------------------------------------------------------------------------
CDictionary CSceneItem::getInfo() const
//----------------------------------------------------------------------------------------------------------------------
{
	CDictionary	info;

	info.set(mPropertyNameIsInitiallyVisible, (UInt32) (mInternals->mIsInitiallyVisible ? 1 : 0));
	info.set(mPropertyNameName, mInternals->mName);
	info.set(mPropertyNameOptions, (UInt32) mInternals->mOptions);

	return info;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItem::setNoteUpdatedProc(NoteUpdatedProc noteUpdatedProc, void* userData)
//----------------------------------------------------------------------------------------------------------------------
{
	// Store
	mInternals->mNoteUpdatedProc = noteUpdatedProc;
	mInternals->mNoteUpdatedProcUserData = userData;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItem::noteUpdated(const CString& propertyName)
//----------------------------------------------------------------------------------------------------------------------
{
	// Check if have proc
	if (mInternals->mNoteUpdatedProc != nil)
		// Call proc
		mInternals->mNoteUpdatedProc(propertyName, mInternals->mNoteUpdatedProcUserData);
}
