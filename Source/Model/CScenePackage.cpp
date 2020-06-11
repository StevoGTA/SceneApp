//----------------------------------------------------------------------------------------------------------------------
//	CScenePackage.cpp			©2019 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CScenePackage.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: Local data

static	CString	sInitialSceneIndexKey(OSSTR("initialSceneIndex"));
static	CString	sViewportZoomKey(OSSTR("viewportZoom"));
static	CString	sScenesKey(OSSTR("scenes"));

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CScenePackageInternals

class CScenePackageInternals : public TReferenceCountable<CScenePackageInternals> {
	public:
		CScenePackageInternals() : TReferenceCountable() {}

		TNArray<CScene>	mScenes;
		OV<CSceneIndex>	mInitialSceneIndex;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CScenePackage

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
CScenePackage::CScenePackage()
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new CScenePackageInternals();
}

//----------------------------------------------------------------------------------------------------------------------
CScenePackage::CScenePackage(const CDictionary& info)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new CScenePackageInternals();

	mInternals->mScenes =
			TNArray<CScene>(info.getArrayOfDictionaries(sScenesKey), (CScene (*)(CArrayItemRef item)) CScene::makeFrom);
	mInternals->mInitialSceneIndex = info.getSInt32(sInitialSceneIndexKey, 0);
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
CDictionary CScenePackage::getInfo() const
//----------------------------------------------------------------------------------------------------------------------
{
	CDictionary	info;

	info.set(sScenesKey,
			TNArray<CDictionary>(mInternals->mScenes, (CDictionary (*)(CArrayItemRef item)) CScene::getInfoFrom));
	if (mInternals->mInitialSceneIndex.hasValue())
		info.set(sInitialSceneIndexKey, *mInternals->mInitialSceneIndex);

	return info;
}

//----------------------------------------------------------------------------------------------------------------------
const CScene& CScenePackage::getInitialScene() const
//----------------------------------------------------------------------------------------------------------------------
{
	// Validation check
	AssertFailIf(!mInternals->mInitialSceneIndex.hasValue());

	return mInternals->mScenes[*mInternals->mInitialSceneIndex];
}

//----------------------------------------------------------------------------------------------------------------------
UInt32 CScenePackage::getScenesCount() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mScenes.getCount();
}

//----------------------------------------------------------------------------------------------------------------------
const CScene& CScenePackage::getSceneAtIndex(CSceneIndex index) const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mScenes[index];
}

//----------------------------------------------------------------------------------------------------------------------
OV<CSceneIndex> CScenePackage::getIndexOfScene(const CScene& scene)
//----------------------------------------------------------------------------------------------------------------------
{
	// Get index
	OV<CArrayItemIndex>	index = mInternals->mScenes.getIndexOf(scene);

	return index.hasValue() ? OV<CSceneIndex>(*index) : OV<CSceneIndex>();
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
