//----------------------------------------------------------------------------------------------------------------------
//	CSceneItemCustom.cpp			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CSceneItemCustom.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CSceneItemCustomActionArray
class CSceneItemCustomActionArray : public CActionArray {
	public:
		CSceneItemCustomActionArray(const CDictionary& info, const CString& key) : CActionArray(info), mKey(key) {}
		~CSceneItemCustomActionArray() {}

		CString	mKey;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneItemCustomAudioInfo

class CSceneItemCustomAudioInfo : public CAudioInfo {
	public:
		CSceneItemCustomAudioInfo(const CDictionary& info, const CString& key) : CAudioInfo(info), mKey(key) {}
		~CSceneItemCustomAudioInfo() {}

		CString	mKey;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneItemCustomAnimation

class CSceneItemCustomAnimation : public CSceneItemAnimation {
	public:
		CSceneItemCustomAnimation(const CDictionary& info, const CString& key) : CSceneItemAnimation(info), mKey(key) {}
		~CSceneItemCustomAnimation() {}

		CString	mKey;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneItemCustomButton

class CSceneItemCustomButton : public CSceneItemButton {
	public:
		CSceneItemCustomButton(const CDictionary& info, const CString& key) : CSceneItemButton(info), mKey(key) {}
		~CSceneItemCustomButton() {}

		CString	mKey;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneItemCustomText

class CSceneItemCustomText : public CSceneItemText {
	public:
		CSceneItemCustomText(const CDictionary& info, const CString& key) : CSceneItemText(info), mKey(key) {}
		~CSceneItemCustomText() {}

		CString	mKey;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneItemCustomInternals

class CSceneItemCustomInternals {
	public:
		CSceneItemCustomInternals() :
			mType("<UNKNOWN>"), mActionArraysArray(true), mAnimationsArray(true), mButtonsArray(true), mTextsArray(true)
			{}
		~CSceneItemCustomInternals() {}

		CDictionary								mInfo;
		CString									mType;
		TPtrArray<CSceneItemCustomActionArray*>	mActionArraysArray;
		TPtrArray<CSceneItemCustomAudioInfo*>	mAudioInfosArray;
		TPtrArray<CSceneItemCustomAnimation*>	mAnimationsArray;
		TPtrArray<CSceneItemCustomButton*>		mButtonsArray;
		TPtrArray<CSceneItemCustomText*>		mTextsArray;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneItemCustom

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
CSceneItemCustom::CSceneItemCustom() : CSceneItem()
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new CSceneItemCustomInternals();
}

//----------------------------------------------------------------------------------------------------------------------
CSceneItemCustom::CSceneItemCustom(const CString& type, const CDictionary& info) : CSceneItem(info)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new CSceneItemCustomInternals();

	mInternals->mInfo = info;
	mInternals->mType = type;
}

//----------------------------------------------------------------------------------------------------------------------
CSceneItemCustom::CSceneItemCustom(const CSceneItemCustom& other) : CSceneItem(other)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new CSceneItemCustomInternals();

	mInternals->mInfo = other.mInternals->mInfo;
	mInternals->mType = other.mInternals->mType;
}

//----------------------------------------------------------------------------------------------------------------------
CSceneItemCustom::~CSceneItemCustom()
//----------------------------------------------------------------------------------------------------------------------
{
	DisposeOf(mInternals);
}

// MARK: CSceneItem methods

//----------------------------------------------------------------------------------------------------------------------
CString& CSceneItemCustom::getType() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mType;
}

//----------------------------------------------------------------------------------------------------------------------
TArray<CDictionary> CSceneItemCustom::getProperties() const
//----------------------------------------------------------------------------------------------------------------------
{
	return CSceneItem::getProperties();
}

//----------------------------------------------------------------------------------------------------------------------
CDictionary CSceneItemCustom::getInfo() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mInfo;
}

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
bool CSceneItemCustom::hasValueForKey(const CString& key) const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mInfo.contains(key);
}

//----------------------------------------------------------------------------------------------------------------------
bool CSceneItemCustom::getBoolValueForKey(const CString& key) const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mInfo.getBool(key);
}

//----------------------------------------------------------------------------------------------------------------------
Float32 CSceneItemCustom::getFloat32(const CString& key) const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mInfo.getFloat32(key);
}

//----------------------------------------------------------------------------------------------------------------------
S2DPoint32 CSceneItemCustom::get2DPointValueForKey(const CString& key) const
//----------------------------------------------------------------------------------------------------------------------
{
	return S2DPoint32(mInternals->mInfo.getString(key));
}

//----------------------------------------------------------------------------------------------------------------------
S2DRect32 CSceneItemCustom::get2DRectValueForKey(const CString& key) const
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
UniversalTimeInterval CSceneItemCustom::getUniversalTimeIntervalValueForKey(const CString& key) const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mInfo.getFloat32(key);
}

//----------------------------------------------------------------------------------------------------------------------
CDictionary CSceneItemCustom::getDictionaryForKey(const CString& key) const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mInfo.getDictionary(key);
}

//----------------------------------------------------------------------------------------------------------------------
CString CSceneItemCustom::getStringForKey(const CString& key) const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mInfo.getString(key);
}

//----------------------------------------------------------------------------------------------------------------------
CActionArray& CSceneItemCustom::getActionArrayForKey(const CString& key)
//----------------------------------------------------------------------------------------------------------------------
{
	// Search through already loaded animations
	CSceneItemCustomActionArray*	actionArray;
	for (CArrayItemIndex i = 0; i < mInternals->mActionArraysArray.getCount(); i++) {
		actionArray = mInternals->mActionArraysArray[i];
		if (actionArray->mKey == key)
			// Found it
			return *actionArray;
	}

	// Not loaded yet.  Load now.
	CDictionary	info = mInternals->mInfo.getDictionary(key);
	actionArray = new CSceneItemCustomActionArray(info, key);
	mInternals->mActionArraysArray += actionArray;

	return *actionArray;
}

//----------------------------------------------------------------------------------------------------------------------
CAudioInfo& CSceneItemCustom::getAudioInfoForKey(const CString& key)
//----------------------------------------------------------------------------------------------------------------------
{
	// Search through already loaded animations
	CSceneItemCustomAudioInfo*	audioInfo;
	for (CArrayItemIndex i = 0; i < mInternals->mAudioInfosArray.getCount(); i++) {
		audioInfo = mInternals->mAudioInfosArray[i];
		if (audioInfo->mKey == key)
			// Found it
			return *audioInfo;
	}

	// Not loaded yet.  Load now.
	CDictionary	info = mInternals->mInfo.getDictionary(key);
	audioInfo = new CSceneItemCustomAudioInfo(info, key);
	mInternals->mAudioInfosArray += audioInfo;

	return *audioInfo;
}

//----------------------------------------------------------------------------------------------------------------------
CSceneItemAnimation& CSceneItemCustom::getSceneItemAnimationForKey(const CString& key)
//----------------------------------------------------------------------------------------------------------------------
{
	// Search through already loaded animations
	CSceneItemCustomAnimation*	animation;
	for (CArrayItemIndex i = 0; i < mInternals->mAnimationsArray.getCount(); i++) {
		animation = mInternals->mAnimationsArray[i];
		if (animation->mKey == key)
			// Found it
			return *animation;
	}

	// Not loaded yet.  Load now.
	CDictionary	info = mInternals->mInfo.getDictionary(key);
	animation = new CSceneItemCustomAnimation(info.getDictionary(CSceneItem::mItemInfoKey), key);
	mInternals->mAnimationsArray += animation;

	return *animation;
}

//----------------------------------------------------------------------------------------------------------------------
CSceneItemButton& CSceneItemCustom::getSceneItemButtonForKey(const CString& key)
//----------------------------------------------------------------------------------------------------------------------
{
	// Search through already loaded buttons
	CSceneItemCustomButton*	button;
	for (CArrayItemIndex i = 0; i < mInternals->mButtonsArray.getCount(); i++) {
		button = mInternals->mButtonsArray[i];
		if (button->mKey == key)
			// Found it
			return *button;
	}

	// Not loaded yet.  Load now.
	CDictionary	info = mInternals->mInfo.getDictionary(key);
	button = new CSceneItemCustomButton(info.getDictionary(CSceneItem::mItemInfoKey), key);
	mInternals->mButtonsArray += button;

	return *button;
}

//----------------------------------------------------------------------------------------------------------------------
CSceneItemText& CSceneItemCustom::getSceneItemTextForKey(const CString& key)
//----------------------------------------------------------------------------------------------------------------------
{
	// Search through already loaded texts
	CSceneItemCustomText*	text;
	for (CArrayItemIndex i = 0; i < mInternals->mTextsArray.getCount(); i++) {
		text = mInternals->mTextsArray[i];
		if (text->mKey == key)
			// Found it
			return *text;
	}

	// Not loaded yet.  Load now.
	CDictionary	info = mInternals->mInfo.getDictionary(key);
	text = new CSceneItemCustomText(info.getDictionary(CSceneItem::mItemInfoKey), key);
	mInternals->mTextsArray += text;

	return *text;
}
