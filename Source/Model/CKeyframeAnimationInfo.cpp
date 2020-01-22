//----------------------------------------------------------------------------------------------------------------------
//	CKeyframeAnimationInfo.cpp			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CKeyframeAnimationInfo.h"

#include "CSceneItem.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: Local data

static	CString	sKeyframeArrayKey("keyframes");

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CKeyframeAnimationInfoInternals

class CKeyframeAnimationInfoInternals {
	public:
		CKeyframeAnimationInfoInternals() : mAnimationKeyframesArray(true) {}
		~CKeyframeAnimationInfoInternals() {}

		TPtrArray<CAnimationKeyframe*>	mAnimationKeyframesArray;
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
	for (CArrayItemIndex i = 0; i < keyframeInfos.getCount(); i++)
		mInternals->mAnimationKeyframesArray += new CAnimationKeyframe(keyframeInfos[i]);
}

//----------------------------------------------------------------------------------------------------------------------
CKeyframeAnimationInfo::CKeyframeAnimationInfo(const CKeyframeAnimationInfo& other)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new CKeyframeAnimationInfoInternals();

	for (CArrayItemIndex i = 0; i < other.mInternals->mAnimationKeyframesArray.getCount(); i++)
		mInternals->mAnimationKeyframesArray += new CAnimationKeyframe(*other.mInternals->mAnimationKeyframesArray[i]);
}

//----------------------------------------------------------------------------------------------------------------------
CKeyframeAnimationInfo::~CKeyframeAnimationInfo()
//----------------------------------------------------------------------------------------------------------------------
{
	DisposeOf(mInternals);
}

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
TArray<CDictionary> CKeyframeAnimationInfo::getProperties() const
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	TArray<CDictionary>	properties;

	// Add properties
	CDictionary	keyframesPropertyInfo;
	keyframesPropertyInfo.set(CSceneItem::mPropertyTitleKey, CString("Keyframes"));
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

	TArray<CDictionary>	keyframeInfos;
	for (CArrayItemIndex i = 0; i < mInternals->mAnimationKeyframesArray.getCount(); i++)
		keyframeInfos += mInternals->mAnimationKeyframesArray[i]->getInfo();
	info.set(sKeyframeArrayKey, keyframeInfos);

	return info;
}

//----------------------------------------------------------------------------------------------------------------------
const TPtrArray<CAnimationKeyframe*>& CKeyframeAnimationInfo::getAnimationKeyframesArray() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mAnimationKeyframesArray;
}

//----------------------------------------------------------------------------------------------------------------------
void CKeyframeAnimationInfo::addAnimationKeyframe(const CAnimationKeyframe& animationKeyframe)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mAnimationKeyframesArray += new CAnimationKeyframe(animationKeyframe);
}
