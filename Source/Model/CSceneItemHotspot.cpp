//----------------------------------------------------------------------------------------------------------------------
//	CSceneItemHotspot.cpp			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CSceneItemHotspot.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: Local data

static	CString	sActionArrayInfoKey("actionInfo");
static	CString	sScreenRectKey("screenRect");

CString	CSceneItemHotspot::mType("hotspot");

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneItemHotspotInternals

class CSceneItemHotspotInternals {
	public:
		CSceneItemHotspotInternals() : mActionArray(nil) {}
		~CSceneItemHotspotInternals()
			{
				DisposeOf(mActionArray);
			}

		CActionArray*	mActionArray;
		S2DRect32		mScreenRect;
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

	if (info.contains(sActionArrayInfoKey))
		mInternals->mActionArray = new CActionArray(info.getDictionary(sActionArrayInfoKey));
	mInternals->mScreenRect = S2DRect32(info.getString(sScreenRectKey));
}

//----------------------------------------------------------------------------------------------------------------------
CSceneItemHotspot::CSceneItemHotspot(const CSceneItemHotspot& other) : CSceneItem(other)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new CSceneItemHotspotInternals();

	if (other.mInternals->mActionArray != nil)
		mInternals->mActionArray = new CActionArray(*other.mInternals->mActionArray);
	mInternals->mScreenRect = other.mInternals->mScreenRect;
}

//----------------------------------------------------------------------------------------------------------------------
CSceneItemHotspot::~CSceneItemHotspot()
//----------------------------------------------------------------------------------------------------------------------
{
	DisposeOf(mInternals);
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
	actionPropertyInfo.set(mPropertyTitleKey, CString("Action Array"));
	actionPropertyInfo.set(mPropertyNameKey, sActionArrayInfoKey);
	actionPropertyInfo.set(mPropertyTypeKey, kSceneItemPropertyTypeActionArray);
	properties += actionPropertyInfo;

	CDictionary	screenRectPropertyInfo;
	screenRectPropertyInfo.set(mPropertyTitleKey, CString("Screen Rect"));
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

	if (mInternals->mActionArray != nil)
		info.set(sActionArrayInfoKey, mInternals->mActionArray->getInfo());
	info.set(sScreenRectKey, mInternals->mScreenRect.asString());

	return info;
}

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
OR<CActionArray> CSceneItemHotspot::getActionArray() const
//----------------------------------------------------------------------------------------------------------------------
{
	return (mInternals->mActionArray != nil) ? OR<CActionArray>(*mInternals->mActionArray) : OR<CActionArray>();
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemHotspot::setActionArray(const CActionArray& actionArray)
//----------------------------------------------------------------------------------------------------------------------
{
	DisposeOf(mInternals->mActionArray);
	mInternals->mActionArray = new CActionArray(actionArray);
}

//----------------------------------------------------------------------------------------------------------------------
const S2DRect32& CSceneItemHotspot::getScreenRect() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mScreenRect;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemHotspot::setScreenRect(const S2DRect32& rect)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mScreenRect = rect;
}
