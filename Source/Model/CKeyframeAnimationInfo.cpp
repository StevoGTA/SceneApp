//----------------------------------------------------------------------------------------------------------------------
//	CKeyframeAnimationInfo.cpp			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CKeyframeAnimationInfo.h"

#include "CReferenceCountable.h"
#include "CSceneItem.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CKeyframeAnimationInfo::Internals

class CKeyframeAnimationInfo::Internals : public TCopyOnWriteReferenceCountable<Internals> {
	public:
		Internals() : TCopyOnWriteReferenceCountable() {}
		Internals(const Internals& other) :
			TCopyOnWriteReferenceCountable(),
					mAnimationKeyframes(other.mAnimationKeyframes)
			{}

		TNArray<CAnimationKeyframe>	mAnimationKeyframes;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CKeyframeAnimationInfo

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
CKeyframeAnimationInfo::CKeyframeAnimationInfo(const CAnimationKeyframe& animationKeyframe)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	mInternals = new Internals();
	mInternals->mAnimationKeyframes += animationKeyframe;
}

//----------------------------------------------------------------------------------------------------------------------
CKeyframeAnimationInfo::CKeyframeAnimationInfo(const CDictionary& info)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new Internals();

	TArray<CDictionary>	keyframeInfos = info.getArrayOfDictionaries(CString(OSSTR("keyframes")));
	for (CArray::ItemIndex i = 0; i < keyframeInfos.getCount(); i++)
		mInternals->mAnimationKeyframes += CAnimationKeyframe(keyframeInfos[i]);
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
	keyframesPropertyInfo.set(CSceneItem::mPropertyNameKey, CString(OSSTR("keyframes")));
	keyframesPropertyInfo.set(CSceneItem::mPropertyTypeKey, kPropertyTypeKeyframeArray);
	properties += keyframesPropertyInfo;

	return properties;
}

//----------------------------------------------------------------------------------------------------------------------
CDictionary CKeyframeAnimationInfo::getInfo() const
//----------------------------------------------------------------------------------------------------------------------
{
	CDictionary	info;

	TNArray<CDictionary>	keyframeInfos;
	for (CArray::ItemIndex i = 0; i < mInternals->mAnimationKeyframes.getCount(); i++)
		keyframeInfos += mInternals->mAnimationKeyframes[i].getInfo();
	info.set(CString(OSSTR("keyframes")), keyframeInfos);

	return info;
}

//----------------------------------------------------------------------------------------------------------------------
const TArray<CAnimationKeyframe>& CKeyframeAnimationInfo::getAnimationKeyframes() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mAnimationKeyframes;
}

//----------------------------------------------------------------------------------------------------------------------
void CKeyframeAnimationInfo::addAnimationKeyframe(const CAnimationKeyframe& animationKeyframe)
//----------------------------------------------------------------------------------------------------------------------
{
	// Prepare to write
	Internals::prepareForWrite(&mInternals);

	// Update
	mInternals->mAnimationKeyframes += animationKeyframe;
}
