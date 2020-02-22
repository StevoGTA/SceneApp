//----------------------------------------------------------------------------------------------------------------------
//	CSceneItemCustom.cpp			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CSceneItemCustom.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CSceneItemCustomInternals

class CSceneItemCustomInternals : public TCopyOnWriteReferenceCountable<CSceneItemCustomInternals> {
	public:
		CSceneItemCustomInternals(const CString& type, const CDictionary& info) :
			TCopyOnWriteReferenceCountable(),
					mInfo(info), mType(type)
			{}
		CSceneItemCustomInternals(const CSceneItemCustomInternals& other) :
			TCopyOnWriteReferenceCountable(),
					mInfo(other.mInfo), mType(other.mType), mActionsMap(other.mActionsMap),
							mAudioInfoMap(other.mAudioInfoMap), mSceneItemAnimationMap(other.mSceneItemAnimationMap),
							mSceneItemButtonMap(other.mSceneItemButtonMap)
//							, mSceneItemTextMap(other.mSceneItemTextMap)
			{}

		CDictionary							mInfo;
		CString								mType;
		TDictionary<CActions*>				mActionsMap;
		TDictionary<CAudioInfo*>			mAudioInfoMap;
		TDictionary<CSceneItemAnimation*>	mSceneItemAnimationMap;
		TDictionary<CSceneItemButton*>		mSceneItemButtonMap;
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
	mInternals = new CSceneItemCustomInternals(CString(OSSTR("<UNKNOWN>")), CDictionary());
}

//----------------------------------------------------------------------------------------------------------------------
CSceneItemCustom::CSceneItemCustom(const CString& type, const CDictionary& info) : CSceneItem(info)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new CSceneItemCustomInternals(type, info);
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
CDictionary CSceneItemCustom::getDictionary(const CString& key) const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mInfo.getDictionary(key);
}

//----------------------------------------------------------------------------------------------------------------------
CString CSceneItemCustom::getString(const CString& key) const
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
S2DPoint32 CSceneItemCustom::get2DPoint(const CString& key) const
//----------------------------------------------------------------------------------------------------------------------
{
	return S2DPoint32(mInternals->mInfo.getString(key));
}

//----------------------------------------------------------------------------------------------------------------------
S2DRect32 CSceneItemCustom::get2DRect(const CString& key) const
//----------------------------------------------------------------------------------------------------------------------
{
	return S2DRect32(mInternals->mInfo.getString(key));
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
		mInternals->mActionsMap.set(key, new CActions(mInternals->mInfo.getDictionary(key)));

	return **mInternals->mActionsMap[key];
}

//----------------------------------------------------------------------------------------------------------------------
const CAudioInfo& CSceneItemCustom::getAudioInfo(const CString& key) const
//----------------------------------------------------------------------------------------------------------------------
{
	// Check if cached
	if (!mInternals->mAudioInfoMap.contains(key))
		// Cache
		mInternals->mAudioInfoMap.set(key, new CAudioInfo(mInternals->mInfo.getDictionary(key)));

	return **mInternals->mAudioInfoMap[key];
}

//----------------------------------------------------------------------------------------------------------------------
const CSceneItemAnimation& CSceneItemCustom::getSceneItemAnimation(const CString& key) const
//----------------------------------------------------------------------------------------------------------------------
{
	// Check if cached
	if (!mInternals->mSceneItemAnimationMap.contains(key))
		// Cache
		mInternals->mSceneItemAnimationMap.set(key,
				new CSceneItemAnimation(mInternals->mInfo.getDictionary(key).getDictionary(CSceneItem::mItemInfoKey)));

	return **mInternals->mSceneItemAnimationMap[key];
}

//----------------------------------------------------------------------------------------------------------------------
const CSceneItemButton& CSceneItemCustom::getSceneItemButton(const CString& key) const
//----------------------------------------------------------------------------------------------------------------------
{
	// Check if cached
	if (!mInternals->mSceneItemButtonMap.contains(key))
		// Cache
		mInternals->mSceneItemButtonMap.set(key,
				new CSceneItemButton(mInternals->mInfo.getDictionary(key).getDictionary(CSceneItem::mItemInfoKey)));

	return **mInternals->mSceneItemButtonMap[key];
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
