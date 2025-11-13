//----------------------------------------------------------------------------------------------------------------------
//	CScenePackage.cpp			©2019 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CScenePackage.h"

#include "CReferenceCountable.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CScenePackage::Internals

class CScenePackage::Internals : public TReferenceCountable<Internals> {
	public:
		Internals(const S2DSizeF32& pixelDimensions) : TReferenceCountable(), mPixelDimensions(pixelDimensions) {}

		S2DSizeF32			mPixelDimensions;
		TNArray<CScene>		mScenes;
		OV<CScene::Index>	mInitialSceneIndex;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CScenePackage

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
CScenePackage::CScenePackage(const S2DSizeF32& pixelDimensions)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new Internals(pixelDimensions);
}

//----------------------------------------------------------------------------------------------------------------------
CScenePackage::CScenePackage(const S2DSizeF32& pixelDimensions, const CDictionary& info)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new Internals(pixelDimensions);

	mInternals->mScenes =
			TNArray<CScene>(info.getArrayOfDictionaries(CString(OSSTR("scenes"))),
					(TNArray<CScene>::MapProc) CScene::makeFrom);
	mInternals->mInitialSceneIndex = info.getSInt32(CString(OSSTR("initialSceneIndex")), 0);
}

//----------------------------------------------------------------------------------------------------------------------
CScenePackage::CScenePackage(const CScenePackage& other)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = other.mInternals->addReference();
}

//----------------------------------------------------------------------------------------------------------------------
CScenePackage::~CScenePackage()
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->removeReference();
}

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
const S2DSizeF32& CScenePackage::getPixelDimensions() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mPixelDimensions;
}

//----------------------------------------------------------------------------------------------------------------------
CDictionary CScenePackage::getInfo() const
//----------------------------------------------------------------------------------------------------------------------
{
	CDictionary	info;

	info.set(CString(OSSTR("scenes")),
			TNArray<CDictionary>(mInternals->mScenes, (TNArray<CDictionary>::MapProc) CScene::getInfoFrom));
	if (mInternals->mInitialSceneIndex.hasValue())
		info.set(CString(OSSTR("initialSceneIndex")), *mInternals->mInitialSceneIndex);

	return info;
}

//----------------------------------------------------------------------------------------------------------------------
UInt32 CScenePackage::getScenesCount() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mScenes.getCount();
}

//----------------------------------------------------------------------------------------------------------------------
UInt32 CScenePackage::getInitialSceneIndex() const
//----------------------------------------------------------------------------------------------------------------------
{
	// Validation check
	AssertFailIf(!mInternals->mInitialSceneIndex.hasValue());

	return *mInternals->mInitialSceneIndex;
}

//----------------------------------------------------------------------------------------------------------------------
const CScene& CScenePackage::getSceneAtIndex(CScene::Index index) const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mScenes[index];
}

//----------------------------------------------------------------------------------------------------------------------
OV<CScene::Index> CScenePackage::getIndexOfScene(const CScene& scene)
//----------------------------------------------------------------------------------------------------------------------
{
	// Get index
	OV<CArray::ItemIndex>	index = mInternals->mScenes.getIndexOf(scene);

	return index.hasValue() ? OV<CScene::Index>(*index) : OV<CScene::Index>();
}

//----------------------------------------------------------------------------------------------------------------------
CScenePackage& CScenePackage::operator=(const CScenePackage& other)
//----------------------------------------------------------------------------------------------------------------------
{
	// Check if assignment to self
	if (this == &other)
		return *this;

	// Remove reference to ourselves
	mInternals->removeReference();

	// Add reference to other
	mInternals = other.mInternals->addReference();

	return *this;
}
