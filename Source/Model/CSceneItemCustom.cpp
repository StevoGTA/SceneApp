//----------------------------------------------------------------------------------------------------------------------
//	CSceneItemCustom.cpp			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CSceneItemCustom.h"

#include "CReferenceCountable.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CSceneItemCustom::Internals

class CSceneItemCustom::Internals : public TCopyOnWriteReferenceCountable<Internals> {
	public:
		Internals(const CString& type, const CDictionary& info) :
			TCopyOnWriteReferenceCountable(),
					mInfo(info), mType(type)
			{}
		Internals(const Internals& other) :
			TCopyOnWriteReferenceCountable(),
					mInfo(other.mInfo), mType(other.mType), mActionsMap(other.mActionsMap),
							mAudioInfoMap(other.mAudioInfoMap), mSceneItemAnimationMap(other.mSceneItemAnimationMap),
							mSceneItemButtonMap(other.mSceneItemButtonMap)
//							, mSceneItemTextMap(other.mSceneItemTextMap)
			{}

		CDictionary							mInfo;
		CString								mType;
		TNDictionary<CActions>				mActionsMap;
		TNDictionary<CAudioInfo>			mAudioInfoMap;
		TNDictionary<CSceneItemAnimation>	mSceneItemAnimationMap;
		TNDictionary<CSceneItemButton>		mSceneItemButtonMap;
//		TDictionary<CSceneItemText*>		mSceneItemTextMap;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneItemCustom

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
CSceneItemCustom::CSceneItemCustom() : CSceneItem()
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new Internals(CString(OSSTR("<UNKNOWN>")), CDictionary());
}

//----------------------------------------------------------------------------------------------------------------------
CSceneItemCustom::CSceneItemCustom(const CString& type, const CDictionary& info) : CSceneItem(info)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new Internals(type, info);
}

//----------------------------------------------------------------------------------------------------------------------
CSceneItemCustom::CSceneItemCustom(const CSceneItemCustom& other) : CSceneItem(other)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = other.mInternals->addReference();
}

//----------------------------------------------------------------------------------------------------------------------
CSceneItemCustom::~CSceneItemCustom()
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->removeReference();
}

// MARK: CSceneItem methods

//----------------------------------------------------------------------------------------------------------------------
CString& CSceneItemCustom::getType() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mType;
}

//----------------------------------------------------------------------------------------------------------------------
CDictionary CSceneItemCustom::getInfo() const
//----------------------------------------------------------------------------------------------------------------------
{
	CDictionary	info = CSceneItem::getInfo();

	info += mInternals->mInfo;

	return info;
}

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
bool CSceneItemCustom::contains(const CString& key) const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mInfo.contains(key);
}

//----------------------------------------------------------------------------------------------------------------------
bool CSceneItemCustom::getBool(const CString& key) const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mInfo.getBool(key);
}

//----------------------------------------------------------------------------------------------------------------------
const TArray<CString>& CSceneItemCustom::getArrayOfStrings(const CString& key) const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mInfo.getArrayOfStrings(key);
}

//----------------------------------------------------------------------------------------------------------------------
const CDictionary& CSceneItemCustom::getDictionary(const CString& key) const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mInfo.getDictionary(key);
}

//----------------------------------------------------------------------------------------------------------------------
const CString& CSceneItemCustom::getString(const CString& key) const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mInfo.getString(key);
}

//----------------------------------------------------------------------------------------------------------------------
Float32 CSceneItemCustom::getFloat32(const CString& key) const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mInfo.getFloat32(key);
}

//----------------------------------------------------------------------------------------------------------------------
S2DPointF32 CSceneItemCustom::get2DPoint(const CString& key) const
//----------------------------------------------------------------------------------------------------------------------
{
	return S2DPointF32(mInternals->mInfo.getString(key));
}

//----------------------------------------------------------------------------------------------------------------------
S2DRectF32 CSceneItemCustom::get2DRect(const CString& key) const
//----------------------------------------------------------------------------------------------------------------------
{
	return S2DRectF32(mInternals->mInfo.getString(key));
}

//----------------------------------------------------------------------------------------------------------------------
UInt32 CSceneItemCustom::getUInt32(const CString& key) const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mInfo.getUInt32(key);
}

//----------------------------------------------------------------------------------------------------------------------
UniversalTimeInterval CSceneItemCustom::getUniversalTimeInterval(const CString& key) const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mInfo.getFloat32(key);
}

//----------------------------------------------------------------------------------------------------------------------
const CActions& CSceneItemCustom::getActions(const CString& key) const
//----------------------------------------------------------------------------------------------------------------------
{
	// Check if cached
	if (!mInternals->mActionsMap.contains(key))
		// Cache
		mInternals->mActionsMap.set(key, CActions(mInternals->mInfo.getDictionary(key)));

	return *mInternals->mActionsMap[key];
}

//----------------------------------------------------------------------------------------------------------------------
const CAudioInfo& CSceneItemCustom::getAudioInfo(const CString& key) const
//----------------------------------------------------------------------------------------------------------------------
{
	// Check if cached
	if (!mInternals->mAudioInfoMap.contains(key))
		// Cache
		mInternals->mAudioInfoMap.set(key, CAudioInfo(mInternals->mInfo.getDictionary(key)));

	return *mInternals->mAudioInfoMap[key];
}

//----------------------------------------------------------------------------------------------------------------------
const CSceneItemAnimation& CSceneItemCustom::getSceneItemAnimation(const CString& key) const
//----------------------------------------------------------------------------------------------------------------------
{
	// Check if cached
	if (!mInternals->mSceneItemAnimationMap.contains(key))
		// Cache
		mInternals->mSceneItemAnimationMap.set(key,
				CSceneItemAnimation(mInternals->mInfo.getDictionary(key).getDictionary(CSceneItem::mItemInfoKey)));

	return *mInternals->mSceneItemAnimationMap[key];
}

//----------------------------------------------------------------------------------------------------------------------
const CSceneItemButton& CSceneItemCustom::getSceneItemButton(const CString& key) const
//----------------------------------------------------------------------------------------------------------------------
{
	// Check if cached
	if (!mInternals->mSceneItemButtonMap.contains(key))
		// Cache
		mInternals->mSceneItemButtonMap.set(key,
				CSceneItemButton(mInternals->mInfo.getDictionary(key).getDictionary(CSceneItem::mItemInfoKey)));

	return *mInternals->mSceneItemButtonMap[key];
}

////----------------------------------------------------------------------------------------------------------------------
//const CSceneItemText& CSceneItemCustom::getSceneItemText(const CString& key) const
////----------------------------------------------------------------------------------------------------------------------
//{
//	// Check if cached
//	if (!mInternals->mSceneItemTextMap.contains(key))
//		// Cache
//		mInternals->mSceneItemTextMap.set(key,
//				new CSceneItemText(mInternals->mInfo.getDictionary(key).getDictionary(CSceneItem::mItemInfoKey)));
//
//	return **mInternals->mSceneItemTextMap[key];
//}
