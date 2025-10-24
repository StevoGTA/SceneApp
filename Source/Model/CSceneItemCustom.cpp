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
					mInfo(other.mInfo), mType(other.mType), mActionByKey(other.mActionByKey),
							mAudioInfoByKey(other.mAudioInfoByKey),
							mSceneItemAnimationByKey(other.mSceneItemAnimationByKey),
							mSceneItemButtonByKey(other.mSceneItemButtonByKey),
							mSceneItemTextByKey(other.mSceneItemTextByKey)
			{}

		CDictionary							mInfo;
		CString								mType;
		TNDictionary<CActions>				mActionByKey;
		TNDictionary<CAudioInfo>			mAudioInfoByKey;
		TNDictionary<CSceneItemAnimation>	mSceneItemAnimationByKey;
		TNDictionary<CSceneItemButton>		mSceneItemButtonByKey;
		TNDictionary<CSceneItemText>		mSceneItemTextByKey;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneItemCustom

// MARK: Lifecycle methods

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
	if (!mInternals->mActionByKey.contains(key))
		// Cache
		mInternals->mActionByKey.set(key, CActions(mInternals->mInfo.getDictionary(key)));

	return *mInternals->mActionByKey[key];
}

//----------------------------------------------------------------------------------------------------------------------
const CAudioInfo& CSceneItemCustom::getAudioInfo(const CString& key) const
//----------------------------------------------------------------------------------------------------------------------
{
	// Check if cached
	if (!mInternals->mAudioInfoByKey.contains(key))
		// Cache
		mInternals->mAudioInfoByKey.set(key, CAudioInfo(mInternals->mInfo.getDictionary(key)));

	return *mInternals->mAudioInfoByKey[key];
}

//----------------------------------------------------------------------------------------------------------------------
CSceneItemAnimation& CSceneItemCustom::getSceneItemAnimation(const CString& key) const
//----------------------------------------------------------------------------------------------------------------------
{
	// Check if cached
	if (!mInternals->mSceneItemAnimationByKey.contains(key))
		// Cache
		mInternals->mSceneItemAnimationByKey.set(key,
				CSceneItemAnimation(mInternals->mInfo.getDictionary(key).getDictionary(CSceneItem::mItemInfoKey)));

	return *mInternals->mSceneItemAnimationByKey[key];
}

//----------------------------------------------------------------------------------------------------------------------
CSceneItemButton& CSceneItemCustom::getSceneItemButton(const CString& key) const
//----------------------------------------------------------------------------------------------------------------------
{
	// Check if cached
	if (!mInternals->mSceneItemButtonByKey.contains(key))
		// Cache
		mInternals->mSceneItemButtonByKey.set(key,
				CSceneItemButton(mInternals->mInfo.getDictionary(key).getDictionary(CSceneItem::mItemInfoKey)));

	return *mInternals->mSceneItemButtonByKey[key];
}

//----------------------------------------------------------------------------------------------------------------------
CSceneItemText& CSceneItemCustom::getSceneItemText(const CString& key) const
//----------------------------------------------------------------------------------------------------------------------
{
	// Check if cached
	if (!mInternals->mSceneItemTextByKey.contains(key))
		// Cache
		mInternals->mSceneItemTextByKey.set(key,
				CSceneItemText(mInternals->mInfo.getDictionary(key).getDictionary(CSceneItem::mItemInfoKey)));

	return *mInternals->mSceneItemTextByKey[key];
}
