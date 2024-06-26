//----------------------------------------------------------------------------------------------------------------------
//	CSceneItemHotspot.cpp			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CSceneItemHotspot.h"

#include "CReferenceCountable.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CSceneItemHotspot::Internals

class CSceneItemHotspot::Internals : public TCopyOnWriteReferenceCountable<Internals> {
	public:
		Internals() : TCopyOnWriteReferenceCountable() {}
		Internals(const Internals& other) :
			TCopyOnWriteReferenceCountable(),
					mActions(other.mActions), mScreenRect(other.mScreenRect)
			{}

		OI<CActions>	mActions;
		S2DRectF32		mScreenRect;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneItemHotspot

// MARK: Properties

CString	CSceneItemHotspot::mType(OSSTR("hotspot"));

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
CSceneItemHotspot::CSceneItemHotspot() : CSceneItem()
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new Internals();
}

//----------------------------------------------------------------------------------------------------------------------
CSceneItemHotspot::CSceneItemHotspot(const CDictionary& info) : CSceneItem(info)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new Internals();

	if (info.contains(CString(OSSTR("actionInfo"))))
		mInternals->mActions = OI<CActions>(CActions(info.getDictionary(CString(OSSTR("actionInfo")))));
	mInternals->mScreenRect = S2DRectF32(info.getString(CString(OSSTR("screenRect"))));
}

//----------------------------------------------------------------------------------------------------------------------
CSceneItemHotspot::CSceneItemHotspot(const CSceneItemHotspot& other) : CSceneItem(other)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = other.mInternals->addReference();
}

//----------------------------------------------------------------------------------------------------------------------
CSceneItemHotspot::~CSceneItemHotspot()
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->removeReference();
}

// MARK: CSceneItem methods

//----------------------------------------------------------------------------------------------------------------------
TMArray<CDictionary> CSceneItemHotspot::getProperties() const
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	TMArray<CDictionary>	properties = CSceneItem::getProperties();

	// Add properties
	CDictionary	actionPropertyInfo;
	actionPropertyInfo.set(mPropertyTitleKey, CString(OSSTR("Actions")));
	actionPropertyInfo.set(mPropertyNameKey, CString(OSSTR("actionInfo")));
	actionPropertyInfo.set(mPropertyTypeKey, CSceneItem::kPropertyTypeActions);
	properties += actionPropertyInfo;

	CDictionary	screenRectPropertyInfo;
	screenRectPropertyInfo.set(mPropertyTitleKey, CString(OSSTR("Screen Rect")));
	screenRectPropertyInfo.set(mPropertyNameKey, CString(OSSTR("screenRect")));
	screenRectPropertyInfo.set(mPropertyTypeKey, CSceneItem::kPropertyTypeScreenRect);
	properties += screenRectPropertyInfo;

	return properties;
}

//----------------------------------------------------------------------------------------------------------------------
CDictionary CSceneItemHotspot::getInfo() const
//----------------------------------------------------------------------------------------------------------------------
{
	CDictionary	info = CSceneItem::getInfo();

	if (mInternals->mActions.hasInstance())
		info.set(CString(OSSTR("actionInfo")), mInternals->mActions->getInfo());
	info.set(CString(OSSTR("screenRect")), mInternals->mScreenRect.asString());

	return info;
}

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
const OI<CActions>& CSceneItemHotspot::getActions() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mActions;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemHotspot::setActions(const OI<CActions>& actions)
//----------------------------------------------------------------------------------------------------------------------
{
	// Prepare to write
	Internals::prepareForWrite(&mInternals);

	// Update
	mInternals->mActions = actions;
}

//----------------------------------------------------------------------------------------------------------------------
const S2DRectF32& CSceneItemHotspot::getScreenRect() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mScreenRect;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemHotspot::setScreenRect(const S2DRectF32& rect)
//----------------------------------------------------------------------------------------------------------------------
{
	// Prepare to write
	Internals::prepareForWrite(&mInternals);

	// Update
	mInternals->mScreenRect = rect;
}
