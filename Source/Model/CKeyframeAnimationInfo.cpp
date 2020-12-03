//----------------------------------------------------------------------------------------------------------------------
//	CKeyframeAnimationInfo.cpp			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CKeyframeAnimationInfo.h"

#include "CSceneItem.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: Local data

static	CString	sKeyframeArrayKey(OSSTR("keyframes"));

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CKeyframeAnimationInfoInternals

class CKeyframeAnimationInfoInternals : public TCopyOnWriteReferenceCountable<CKeyframeAnimationInfoInternals> {
	public:
		CKeyframeAnimationInfoInternals() : TCopyOnWriteReferenceCountable() {}
		CKeyframeAnimationInfoInternals(const CKeyframeAnimationInfoInternals& other) :
			TCopyOnWriteReferenceCountable(),
					mAnimationKeyframesArray(other.mAnimationKeyframesArray)
			{}

		TNArray<CAnimationKeyframe>	mAnimationKeyframesArray;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CKeyframeAnimationInfo

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
CKeyframeAnimationInfo::CKeyframeAnimationInfo()
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new CKeyframeAnimationInfoInternals();
}

//----------------------------------------------------------------------------------------------------------------------
CKeyframeAnimationInfo::CKeyframeAnimationInfo(const CDictionary& info)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new CKeyframeAnimationInfoInternals();

	TArray<CDictionary>	keyframeInfos = info.getArrayOfDictionaries(sKeyframeArrayKey);
	for (CArray::ItemIndex i = 0; i < keyframeInfos.getCount(); i++)
		mInternals->mAnimationKeyframesArray += CAnimationKeyframe(keyframeInfos[i]);
}

//----------------------------------------------------------------------------------------------------------------------
CKeyframeAnimationInfo::CKeyframeAnimationInfo(const CKeyframeAnimationInfo& other)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = other.mInternals->addReference();
}

//----------------------------------------------------------------------------------------------------------------------
CKeyframeAnimationInfo::~CKeyframeAnimationInfo()
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->removeReference();
}

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
TArray<CDictionary> CKeyframeAnimationInfo::getProperties() const
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	TNArray<CDictionary>	properties;

	// Add properties
	CDictionary	keyframesPropertyInfo;
	keyframesPropertyInfo.set(CSceneItem::mPropertyTitleKey, CString(OSSTR("Keyframes")));
	keyframesPropertyInfo.set(CSceneItem::mPropertyNameKey, sKeyframeArrayKey);
	keyframesPropertyInfo.set(CSceneItem::mPropertyTypeKey, kKeyframeAnimationPropertyTypeKeyframeArray);
	properties += keyframesPropertyInfo;

	return properties;
}

//----------------------------------------------------------------------------------------------------------------------
CDictionary CKeyframeAnimationInfo::getInfo() const
//----------------------------------------------------------------------------------------------------------------------
{
	CDictionary	info;

	TNArray<CDictionary>	keyframeInfos;
	for (CArray::ItemIndex i = 0; i < mInternals->mAnimationKeyframesArray.getCount(); i++)
		keyframeInfos += mInternals->mAnimationKeyframesArray[i].getInfo();
	info.set(sKeyframeArrayKey, keyframeInfos);

	return info;
}

//----------------------------------------------------------------------------------------------------------------------
const TArray<CAnimationKeyframe>& CKeyframeAnimationInfo::getAnimationKeyframesArray() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mAnimationKeyframesArray;
}

//----------------------------------------------------------------------------------------------------------------------
void CKeyframeAnimationInfo::addAnimationKeyframe(const CAnimationKeyframe& animationKeyframe)
//----------------------------------------------------------------------------------------------------------------------
{
	// Prepare to write
	mInternals = mInternals->prepareForWrite();

	// Update
	mInternals->mAnimationKeyframesArray += animationKeyframe;
}
