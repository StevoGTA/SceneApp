//----------------------------------------------------------------------------------------------------------------------
//	CScene.cpp			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CScene.h"

#include "CReferenceCountable.h"
#include "CSceneItemAnimation.h"
#include "CSceneItemButton.h"
#include "CSceneItemButtonArray.h"
#include "CSceneItemCustom.h"
#include "CSceneItemHotspot.h"
#include "CSceneItemVideo.h"
//#include "CSceneItemText.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CScene::Internals

class CScene::Internals : public TCopyOnWriteReferenceCountable<Internals> {
	public:
		Internals() : TCopyOnWriteReferenceCountable(), mOptions(CScene::kOptionsNone) {}
		Internals(const Internals& other) :
			TCopyOnWriteReferenceCountable(),
					mDoubleTapActions(other.mDoubleTapActions), mBackground1AudioInfo(other.mBackground1AudioInfo),
							mBackground2AudioInfo(other.mBackground2AudioInfo), mOptions(other.mOptions),
							mName(other.mName), mStoreSceneIndexAsString(other.mStoreSceneIndexAsString),
							mBoundsRect(other.mBoundsRect), mSceneItems(other.mSceneItems)
					{}

		OI<CActions>			mDoubleTapActions;
		OI<CAudioInfo>			mBackground1AudioInfo;
		OI<CAudioInfo>			mBackground2AudioInfo;
		CScene::Options			mOptions;
		CString					mName;
		CString					mStoreSceneIndexAsString;
		S2DRectF32				mBoundsRect;
		TNArray<I<CSceneItem> >	mSceneItems;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CScene

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
CScene::CScene()
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new Internals();
}

//----------------------------------------------------------------------------------------------------------------------
CScene::CScene(const CDictionary& info)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new Internals();

	mInternals->mName = info.getString(CString(OSSTR("name")));
	mInternals->mOptions = (Options) info.getUInt8(CString(OSSTR("options")));
	mInternals->mStoreSceneIndexAsString = info.getString(CString(OSSTR("storeSceneIndexAs")));
	mInternals->mBoundsRect = S2DRectF32(info.getString(CString(OSSTR("bounds"))));
	if (info.contains(CString(OSSTR("background1AudioInfo"))))
		mInternals->mBackground1AudioInfo =
				OI<CAudioInfo>(CAudioInfo(info.getDictionary(CString(OSSTR("background1AudioInfo")))));
	if (info.contains(CString(OSSTR("background2AudioInfo"))))
		mInternals->mBackground2AudioInfo =
				OI<CAudioInfo>(CAudioInfo(info.getDictionary(CString(OSSTR("background2AudioInfo")))));
	if (info.contains(CString(OSSTR("doubleTapAction"))))
		mInternals->mDoubleTapActions = new CActions(info.getDictionary(CString(OSSTR("doubleTapAction"))));

	TArray<CDictionary>	itemInfos = info.getArrayOfDictionaries(CString(OSSTR("items")));
	for (CArray::ItemIndex i = 0; i < itemInfos.getCount(); i++) {
		// Create and add item
		const	CDictionary&	sceneItemInfo = itemInfos[i];
		const	CString			itemType = sceneItemInfo.getString(CString(OSSTR("itemType")));
		const	CDictionary		itemInfo = sceneItemInfo.getDictionary(CSceneItem::mItemInfoKey);

		// What we get?
		if (itemType == CSceneItemAnimation::mType)
			// Animation
			mInternals->mSceneItems += I<CSceneItem>(new CSceneItemAnimation(itemInfo));
		else if (itemType == CSceneItemButton::mType)
			// Button
			mInternals->mSceneItems += I<CSceneItem>(new CSceneItemButton(itemInfo));
		else if (itemType == CSceneItemButtonArray::mType)
			// Button Array
			mInternals->mSceneItems += I<CSceneItem>(new CSceneItemButtonArray(itemInfo));
		else if (itemType == CSceneItemHotspot::mType)
			// Hotspot
			mInternals->mSceneItems += I<CSceneItem>(new CSceneItemHotspot(itemInfo));
		else if (itemType == CSceneItemVideo::mType)
			// Video
			mInternals->mSceneItems += I<CSceneItem>(new CSceneItemVideo(itemInfo));
//		else if (itemType == CSceneItemText::mType)
//			// Text
//			mInternals->mSceneItems += I<CSceneItem>(new CSceneItemText(itemInfo));
		else
			// Custom
			mInternals->mSceneItems += I<CSceneItem>(new CSceneItemCustom(itemType, itemInfo));
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

	info.set(CString(OSSTR("name")), mInternals->mName);
	info.set(CString(OSSTR("options")), mInternals->mOptions);
	info.set(CString(OSSTR("storeSceneIndexAs")), mInternals->mStoreSceneIndexAsString);
	info.set(CString(OSSTR("bounds")), mInternals->mBoundsRect.asString());
	if (mInternals->mBackground1AudioInfo.hasInstance())
		info.set(CString(OSSTR("background1AudioInfo")), mInternals->mBackground1AudioInfo->getInfo());
	if (mInternals->mBackground2AudioInfo.hasInstance())
		info.set(CString(OSSTR("background2AudioInfo")), mInternals->mBackground2AudioInfo->getInfo());
	if (mInternals->mDoubleTapActions.hasInstance())
		info.set(CString(OSSTR("doubleTapAction")), mInternals->mDoubleTapActions->getInfo());

	TNArray<CDictionary>	itemInfos;
	for (CArray::ItemIndex i = 0; i < mInternals->mSceneItems.getCount(); i++) {
		// Collect info
		CDictionary	sceneItemInfo;
		sceneItemInfo.set(CString(OSSTR("itemType")), mInternals->mSceneItems[i]->getType());
		sceneItemInfo.set(CSceneItem::mItemInfoKey, mInternals->mSceneItems[i]->getInfo());

		// Add
		itemInfos += sceneItemInfo;
	}
	info.set(CString(OSSTR("items")), itemInfos);

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
	Internals::prepareForWrite(&mInternals);

	// Update
	mInternals->mName = name;
}

//----------------------------------------------------------------------------------------------------------------------
CScene::Options CScene::getOptions() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mOptions;
}

//----------------------------------------------------------------------------------------------------------------------
void CScene::setOptions(Options options)
//----------------------------------------------------------------------------------------------------------------------
{
	// Prepare to write
	Internals::prepareForWrite(&mInternals);

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
	Internals::prepareForWrite(&mInternals);

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
	Internals::prepareForWrite(&mInternals);

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
	Internals::prepareForWrite(&mInternals);

	// Update
	mInternals->mDoubleTapActions = doubleTapActions;
}

//----------------------------------------------------------------------------------------------------------------------
const TArray<I<CSceneItem> >& CScene::getSceneItems() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mSceneItems;
}
