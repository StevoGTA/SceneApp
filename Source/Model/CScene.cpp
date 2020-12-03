//----------------------------------------------------------------------------------------------------------------------
//	CScene.cpp			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CScene.h"

#include "CSceneItemAnimation.h"
#include "CSceneItemButton.h"
#include "CSceneItemButtonArray.h"
#include "CSceneItemCustom.h"
#include "CSceneItemHotspot.h"
//#include "CSceneItemMovie.h"
//#include "CSceneItemText.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: Local data

static	CString	sBackground1AudioInfoKey(OSSTR("background1AudioInfo"));
static	CString	sBackground2AudioInfoKey(OSSTR("background2AudioInfo"));
static	CString	sBoundsRectKey(OSSTR("bounds"));
static	CString	sDoubleTapActionsInfoKey(OSSTR("doubleTapAction"));
static	CString	sItemsKey(OSSTR("items"));
static	CString	sItemTypeKey(OSSTR("itemType"));
static	CString	sNameKey(OSSTR("name"));
static	CString	sOptionsKey(OSSTR("options"));
static	CString	sStoreSceneIndexAsKey(OSSTR("storeSceneIndexAs"));

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneInternals

class CSceneInternals : public TCopyOnWriteReferenceCountable<CSceneInternals> {
	public:
		CSceneInternals() : TCopyOnWriteReferenceCountable(), mOptions(kSceneOptionsNone) {}
		CSceneInternals(const CSceneInternals& other) :
			TCopyOnWriteReferenceCountable(),
					mDoubleTapActions(other.mDoubleTapActions), mBackground1AudioInfo(other.mBackground1AudioInfo),
							mBackground2AudioInfo(other.mBackground2AudioInfo), mOptions(other.mOptions),
							mName(other.mName), mStoreSceneIndexAsString(other.mStoreSceneIndexAsString),
							mBoundsRect(other.mBoundsRect), mSceneItems(other.mSceneItems)
					{}

		OI<CActions>		mDoubleTapActions;
		OI<CAudioInfo>		mBackground1AudioInfo;
		OI<CAudioInfo>		mBackground2AudioInfo;
		ESceneOptions		mOptions;
		CString				mName;
		CString				mStoreSceneIndexAsString;
		S2DRectF32			mBoundsRect;
		TCArray<CSceneItem>	mSceneItems;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CScene

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
CScene::CScene()
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new CSceneInternals();
}

//----------------------------------------------------------------------------------------------------------------------
CScene::CScene(const CDictionary& info)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new CSceneInternals();

	mInternals->mName = info.getString(sNameKey);
	mInternals->mOptions = (ESceneOptions) info.getUInt8(sOptionsKey);
	mInternals->mStoreSceneIndexAsString = info.getString(sStoreSceneIndexAsKey);
	mInternals->mBoundsRect = S2DRectF32(info.getString(sBoundsRectKey));
	if (info.contains(sBackground1AudioInfoKey))
		mInternals->mBackground1AudioInfo = OI<CAudioInfo>(CAudioInfo(info.getDictionary(sBackground1AudioInfoKey)));
	if (info.contains(sBackground2AudioInfoKey))
		mInternals->mBackground2AudioInfo = OI<CAudioInfo>(CAudioInfo(info.getDictionary(sBackground2AudioInfoKey)));
	if (info.contains(sDoubleTapActionsInfoKey))
		mInternals->mDoubleTapActions = new CActions(info.getDictionary(sDoubleTapActionsInfoKey));

	TArray<CDictionary>	itemInfos = info.getArrayOfDictionaries(sItemsKey);
	for (CArray::ItemIndex i = 0; i < itemInfos.getCount(); i++) {
		// Create and add item
		const	CDictionary&	sceneItemInfo = itemInfos[i];
		const	CString			itemType = sceneItemInfo.getString(sItemTypeKey);
		const	CDictionary		itemInfo = sceneItemInfo.getDictionary(CSceneItem::mItemInfoKey);

		// What we get?
		if (itemType == CSceneItemAnimation::mType)
			// Animation
			mInternals->mSceneItems += CSceneItemAnimation(itemInfo);
		else if (itemType == CSceneItemButton::mType)
			// Button
			mInternals->mSceneItems += CSceneItemButton(itemInfo);
		else if (itemType == CSceneItemButtonArray::mType)
			// Button Array
			mInternals->mSceneItems += CSceneItemButtonArray(itemInfo);
		else if (itemType == CSceneItemHotspot::mType)
			// Hotspot
			mInternals->mSceneItems += CSceneItemHotspot(itemInfo);
//		else if (itemType == CSceneItemMovie::mType)
//			// Movie
//			mInternals->mSceneItems += CSceneItemMovie(itemInfo);
//		else if (itemType == CSceneItemText::mType)
//			// Text
//			mInternals->mSceneItems += CSceneItemText(itemInfo);
		else
			// Custom
			mInternals->mSceneItems += CSceneItemCustom(itemType, itemInfo);
	}
}

//----------------------------------------------------------------------------------------------------------------------
CScene::CScene(const CScene& other)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = other.mInternals->addReference();
}

//----------------------------------------------------------------------------------------------------------------------
CScene::~CScene()
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->removeReference();
}

// MARK: CEquatable methods

//----------------------------------------------------------------------------------------------------------------------
bool CScene::operator==(const CEquatable& other) const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals == ((const CScene&) other).mInternals;
}

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
TArray<CDictionary> CScene::getProperties() const
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	TNArray<CDictionary>	properties;

	// Add properties

	return properties;
}

//----------------------------------------------------------------------------------------------------------------------
CDictionary CScene::getInfo() const
//----------------------------------------------------------------------------------------------------------------------
{
	CDictionary	info;

	info.set(sNameKey, mInternals->mName);
	info.set(sOptionsKey, mInternals->mOptions);
	info.set(sStoreSceneIndexAsKey, mInternals->mStoreSceneIndexAsString);
	info.set(sBoundsRectKey, mInternals->mBoundsRect.asString());
	if (mInternals->mBackground1AudioInfo.hasInstance())
		info.set(sBackground1AudioInfoKey, mInternals->mBackground1AudioInfo->getInfo());
	if (mInternals->mBackground2AudioInfo.hasInstance())
		info.set(sBackground2AudioInfoKey, mInternals->mBackground2AudioInfo->getInfo());
	if (mInternals->mDoubleTapActions.hasInstance())
		info.set(sDoubleTapActionsInfoKey, mInternals->mDoubleTapActions->getInfo());

	TNArray<CDictionary>	itemInfos;
	for (CArray::ItemIndex i = 0; i < mInternals->mSceneItems.getCount(); i++) {
		CDictionary	sceneItemInfo;
		sceneItemInfo.set(sItemTypeKey, mInternals->mSceneItems[i].getType());
		sceneItemInfo.set(CSceneItem::mItemInfoKey, mInternals->mSceneItems[i].getInfo());
		itemInfos += sceneItemInfo;
	}
	info.set(sItemsKey, itemInfos);

	return info;
}

//----------------------------------------------------------------------------------------------------------------------
const CString& CScene::getName() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mName;
}

//----------------------------------------------------------------------------------------------------------------------
void CScene::setName(const CString& name)
//----------------------------------------------------------------------------------------------------------------------
{
	// Prepare to write
	mInternals = mInternals->prepareForWrite();

	// Update
	mInternals->mName = name;
}

//----------------------------------------------------------------------------------------------------------------------
ESceneOptions CScene::getOptions() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mOptions;
}

//----------------------------------------------------------------------------------------------------------------------
void CScene::setOptions(ESceneOptions options)
//----------------------------------------------------------------------------------------------------------------------
{
	// Prepare to write
	mInternals = mInternals->prepareForWrite();

	// Update
	mInternals->mOptions = options;
}

//----------------------------------------------------------------------------------------------------------------------
const CString& CScene::getStoreSceneIndexAsString() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mStoreSceneIndexAsString;
}

//----------------------------------------------------------------------------------------------------------------------
void CScene::setStoreSceneIndexAsString(const CString& string)
//----------------------------------------------------------------------------------------------------------------------
{
	// Prepare to write
	mInternals = mInternals->prepareForWrite();

	// Update
	mInternals->mStoreSceneIndexAsString = string;
}

//----------------------------------------------------------------------------------------------------------------------
const S2DRectF32& CScene::getBoundsRect() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mBoundsRect;
}

//----------------------------------------------------------------------------------------------------------------------
void CScene::setBoundsRect(const S2DRectF32& rect)
//----------------------------------------------------------------------------------------------------------------------
{
	// Prepare to write
	mInternals = mInternals->prepareForWrite();

	// Update
	mInternals->mBoundsRect = rect;
}

//----------------------------------------------------------------------------------------------------------------------
const OI<CAudioInfo>& CScene::getBackground1AudioInfo() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mBackground1AudioInfo;
}

//----------------------------------------------------------------------------------------------------------------------
void CScene::setBackground1AudioInfo(const OI<CAudioInfo>& audioInfo)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mBackground1AudioInfo = audioInfo;
}

//----------------------------------------------------------------------------------------------------------------------
const OI<CAudioInfo>& CScene::getBackground2AudioInfo() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mBackground2AudioInfo;
}

//----------------------------------------------------------------------------------------------------------------------
void CScene::setBackground2AudioInfo(const OI<CAudioInfo>& audioInfo)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mBackground2AudioInfo = audioInfo;
}

//----------------------------------------------------------------------------------------------------------------------
const OI<CActions>& CScene::getDoubleTapActions() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mDoubleTapActions;
}

//----------------------------------------------------------------------------------------------------------------------
void CScene::setDoubleTapActions(const OI<CActions>& doubleTapActions)
//----------------------------------------------------------------------------------------------------------------------
{
	// Prepare to write
	mInternals = mInternals->prepareForWrite();

	// Update
	mInternals->mDoubleTapActions = doubleTapActions;
}

//----------------------------------------------------------------------------------------------------------------------
const TArray<CSceneItem>& CScene::getSceneItems() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mSceneItems;
}
