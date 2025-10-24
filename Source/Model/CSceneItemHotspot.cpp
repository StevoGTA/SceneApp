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

		OV<CActions>	mActions;
		S2DRectF32		mScreenRect;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneItemHotspot

// MARK: Properties

const	CString	CSceneItemHotspot::mType(OSSTR("hotspot"));

const	CString	CSceneItemHotspot::mPropertyNameActions(OSSTR("actionInfo"));
const	CString	CSceneItemHotspot::mPropertyNameScreenRects(OSSTR("screenRect"));

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
CSceneItemHotspot::CSceneItemHotspot(const CDictionary& info) : CSceneItem(info)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new Internals();

	if (info.contains(mPropertyNameActions))
		mInternals->mActions.setValue(CActions(info.getDictionary(mPropertyNameActions)));
	mInternals->mScreenRect = S2DRectF32(info.getString(mPropertyNameScreenRects));
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
	actionPropertyInfo.set(mPropertyNameKey, mPropertyNameActions);
	actionPropertyInfo.set(mPropertyTypeKey, CSceneItem::kPropertyTypeActions);
	properties += actionPropertyInfo;

	CDictionary	screenRectPropertyInfo;
	screenRectPropertyInfo.set(mPropertyTitleKey, CString(OSSTR("Screen Rect")));
	screenRectPropertyInfo.set(mPropertyNameKey, mPropertyNameScreenRects);
	screenRectPropertyInfo.set(mPropertyTypeKey, CSceneItem::kPropertyTypeScreenRect);
	properties += screenRectPropertyInfo;

	return properties;
}

//----------------------------------------------------------------------------------------------------------------------
CDictionary CSceneItemHotspot::getInfo() const
//----------------------------------------------------------------------------------------------------------------------
{
	CDictionary	info = CSceneItem::getInfo();

	if (mInternals->mActions.hasValue())
		info.set(mPropertyNameActions, mInternals->mActions->getInfo());
	info.set(mPropertyNameScreenRects, mInternals->mScreenRect.asString());

	return info;
}

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
const OV<CActions>& CSceneItemHotspot::getActions() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mActions;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemHotspot::setActions(const OV<CActions>& actions)
//----------------------------------------------------------------------------------------------------------------------
{
	// Check if changed
	if (actions != mInternals->mActions) {
		// Prepare to write
		Internals::prepareForWrite(&mInternals);

		// Update
		mInternals->mActions = actions;

		// Note updated
		noteUpdated(mPropertyNameActions);
	}
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
	// Check if changed
	if (rect != mInternals->mScreenRect) {
		// Prepare to write
		Internals::prepareForWrite(&mInternals);

		// Update
		mInternals->mScreenRect = rect;

		// Note updated
		noteUpdated(mPropertyNameScreenRects);
	}
}
