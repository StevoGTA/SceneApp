//----------------------------------------------------------------------------------------------------------------------
//	CSceneItemHotspot.cpp			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CSceneItemHotspot.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: Local data

static	CString	sActionsInfoKey(OSSTR("actionInfo"));
static	CString	sScreenRectKey(OSSTR("screenRect"));

CString	CSceneItemHotspot::mType(OSSTR("hotspot"));

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneItemHotspotInternals

class CSceneItemHotspotInternals : public TCopyOnWriteReferenceCountable<CSceneItemHotspotInternals> {
	public:
		CSceneItemHotspotInternals() : TCopyOnWriteReferenceCountable() {}
		CSceneItemHotspotInternals(const CSceneItemHotspotInternals& other) :
			TCopyOnWriteReferenceCountable(),
					mActions(other.mActions), mScreenRect(other.mScreenRect)
			{}

		OO<CActions>	mActions;
		S2DRectF32		mScreenRect;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneItemHotspot

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
CSceneItemHotspot::CSceneItemHotspot() : CSceneItem()
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new CSceneItemHotspotInternals();
}

//----------------------------------------------------------------------------------------------------------------------
CSceneItemHotspot::CSceneItemHotspot(const CDictionary& info) : CSceneItem(info)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new CSceneItemHotspotInternals();

	if (info.contains(sActionsInfoKey))
		mInternals->mActions = OO<CActions>(CActions(info.getDictionary(sActionsInfoKey)));
	mInternals->mScreenRect = S2DRectF32(info.getString(sScreenRectKey));
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
TArray<CDictionary> CSceneItemHotspot::getProperties() const
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	TArray<CDictionary>	properties = CSceneItem::getProperties();

	// Add properties
	CDictionary	actionPropertyInfo;
	actionPropertyInfo.set(mPropertyTitleKey, CString(OSSTR("Actions")));
	actionPropertyInfo.set(mPropertyNameKey, sActionsInfoKey);
	actionPropertyInfo.set(mPropertyTypeKey, kSceneItemPropertyTypeActions);
	properties += actionPropertyInfo;

	CDictionary	screenRectPropertyInfo;
	screenRectPropertyInfo.set(mPropertyTitleKey, CString(OSSTR("Screen Rect")));
	screenRectPropertyInfo.set(mPropertyNameKey, sScreenRectKey);
	screenRectPropertyInfo.set(mPropertyTypeKey, kSceneItemPropertyTypeScreenRect);
	properties += screenRectPropertyInfo;

	return properties;
}

//----------------------------------------------------------------------------------------------------------------------
CDictionary CSceneItemHotspot::getInfo() const
//----------------------------------------------------------------------------------------------------------------------
{
	CDictionary	info = CSceneItem::getInfo();

	if (mInternals->mActions.hasObject())
		info.set(sActionsInfoKey, mInternals->mActions->getInfo());
	info.set(sScreenRectKey, mInternals->mScreenRect.asString());

	return info;
}

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
const OO<CActions>& CSceneItemHotspot::getActions() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mActions;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemHotspot::setActions(const OO<CActions>& actions)
//----------------------------------------------------------------------------------------------------------------------
{
	// Prepare to write
	mInternals = mInternals->prepareForWrite();

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
	mInternals = mInternals->prepareForWrite();

	// Update
	mInternals->mScreenRect = rect;
}
