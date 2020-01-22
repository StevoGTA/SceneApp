//----------------------------------------------------------------------------------------------------------------------
//	CScene.cpp			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CScene.h"

#include "CSceneItemAnimation.h"
#include "CSceneItemButton.h"
#include "CSceneItemButtonArray.h"
#include "CSceneItemCustom.h"
#include "CSceneItemHotspot.h"
#include "CSceneItemMovie.h"
#include "CSceneItemText.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: Local data

static	CString	sBoundsRectKey("bounds");
static	CString	sDontUnloadKey("dontUnload");
static	CString	sDoubleTapActionArrayInfoKey("doubleTapAction");
static	CString	sItemsKey("items");
static	CString	sItemTypeKey("itemType");
static	CString	sNameKey("name");
static	CString	sStoreSceneIndexAsKey("storeSceneIndexAs");

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneInternals

class CSceneInternals {
	public:
		CSceneInternals() : mItemsArray(true), mDoubleTapActionArray(nil), mOptions(kSceneOptionsNone) {}
		~CSceneInternals()
			{
				DisposeOf(mDoubleTapActionArray);
			}

		CActionArray*			mDoubleTapActionArray;
		ESceneOptions			mOptions;
		CString					mName;
		CString					mStoreSceneIndexAsString;
		S2DRect32				mBoundsRect;
		TPtrArray<CSceneItem*>	mItemsArray;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneApp methods

//----------------------------------------------------------------------------------------------------------------------
CSceneItem*	eCreateSceneItemFromInfo(const CDictionary& sceneItemInfo)
//----------------------------------------------------------------------------------------------------------------------
{
	CString		itemType = sceneItemInfo.getString(sItemTypeKey);
	CDictionary	itemInfo = sceneItemInfo.getDictionary(CSceneItem::mItemInfoKey);

	// What we get?
	if (itemType == CSceneItemAnimation::mType)
		// Animation
		return new CSceneItemAnimation(itemInfo);
	else if (itemType == CSceneItemButton::mType)
		// Button
		return new CSceneItemButton(itemInfo);
	else if (itemType == CSceneItemButtonArray::mType)
		// Button Array
		return new CSceneItemButtonArray(itemInfo);
	else if (itemType == CSceneItemHotspot::mType)
		// Hotspot
		return new CSceneItemHotspot(itemInfo);
	else if (itemType == CSceneItemMovie::mType)
		// Movie
		return new CSceneItemMovie(itemInfo);
	else if (itemType == CSceneItemText::mType)
		// Text
		return new CSceneItemText(itemInfo);
	else
		// Custom
		return new CSceneItemCustom(itemType, itemInfo);
}

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
	mInternals->mBoundsRect = S2DRect32(info.getString(sBoundsRectKey));
	if (info.contains(sDoubleTapActionArrayInfoKey))
		mInternals->mDoubleTapActionArray = new CActionArray(info.getDictionary(sDoubleTapActionArrayInfoKey));
	mInternals->mStoreSceneIndexAsString = info.getString(sStoreSceneIndexAsKey);

	if (info.contains(sDontUnloadKey))
		mInternals->mOptions = (ESceneOptions) (mInternals->mOptions | kSceneOptionsDontUnload);

	TArray<CDictionary>	itemInfos = info.getArrayOfDictionaries(sItemsKey);
	for (CArrayItemIndex i = 0; i < itemInfos.getCount(); i++)
		// Create and add item
		mInternals->mItemsArray += eCreateSceneItemFromInfo(itemInfos[i]);
}

//----------------------------------------------------------------------------------------------------------------------
CScene::~CScene()
//----------------------------------------------------------------------------------------------------------------------
{
	DisposeOf(mInternals);
}

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
TArray<CDictionary> CScene::getProperties() const
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	TArray<CDictionary>	properties;

	// Add properties

	return properties;
}

//----------------------------------------------------------------------------------------------------------------------
CDictionary CScene::getInfo() const
//----------------------------------------------------------------------------------------------------------------------
{
	CDictionary	info;

	info.set(sNameKey, mInternals->mName);
	info.set(sBoundsRectKey, mInternals->mBoundsRect.asString());
	if (mInternals->mDoubleTapActionArray != nil)
		info.set(sDoubleTapActionArrayInfoKey, mInternals->mDoubleTapActionArray->getInfo());
	if (mInternals->mOptions & kSceneOptionsDontUnload)
		info.set(sDontUnloadKey, (UInt32) 1);
	info.set(sStoreSceneIndexAsKey, mInternals->mStoreSceneIndexAsString);

	TArray<CDictionary>	itemInfos;
	for (CArrayItemIndex i = 0; i < mInternals->mItemsArray.getCount(); i++) {
		CDictionary	sceneItemInfo;
		sceneItemInfo.set(sItemTypeKey, mInternals->mItemsArray[i]->getType());
		sceneItemInfo.set(CSceneItem::mItemInfoKey, mInternals->mItemsArray[i]->getInfo());
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
	mInternals->mName = name;
}

//----------------------------------------------------------------------------------------------------------------------
const S2DRect32& CScene::getBoundsRect() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mBoundsRect;
}

//----------------------------------------------------------------------------------------------------------------------
void CScene::setBoundsRect(const S2DRect32& rect)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mBoundsRect = rect;
}

//----------------------------------------------------------------------------------------------------------------------
OR<CActionArray> CScene::getDoubleTapActionArray() const
//----------------------------------------------------------------------------------------------------------------------
{
	return (mInternals->mDoubleTapActionArray != nil) ?
			OR<CActionArray>(*mInternals->mDoubleTapActionArray) : OR<CActionArray>();
}

//----------------------------------------------------------------------------------------------------------------------
void CScene::setDoubleTapActionArray(OR<CActionArray> doubleTapActionArray)
//----------------------------------------------------------------------------------------------------------------------
{
	DisposeOf(mInternals->mDoubleTapActionArray);
	if (doubleTapActionArray.hasReference())
		mInternals->mDoubleTapActionArray = new CActionArray(*doubleTapActionArray);
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
	mInternals->mStoreSceneIndexAsString = string;
}

//----------------------------------------------------------------------------------------------------------------------
UInt32 CScene::getSceneItemsCount() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mItemsArray.getCount();
}

//----------------------------------------------------------------------------------------------------------------------
CSceneItem& CScene::getSceneItemAtIndex(UInt32 index) const
//----------------------------------------------------------------------------------------------------------------------
{
	return *mInternals->mItemsArray[index];
}
