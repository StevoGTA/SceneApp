//
//  CScenePackage.cpp
//  StevoBrock-Core
//
//  Created by Stevo on 10/4/19.
//

#include "CScenePackage.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: Local data

static	CString	sInitialSceneIndexKey("initialSceneIndex");
static	CString	sViewportZoomKey("viewportZoom");
static	CString	sScenesKey("scenes");

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CScenePackageInternals

class CScenePackageInternals {
	public:
		CScenePackageInternals() : mScenes(true), mReferenceCount(1) {}

		CScenePackageInternals*	addReference()
									{ mReferenceCount++; return this; }
		void					removeReference()
									{
										// Remove reference and see if we are the last one
										if (--mReferenceCount == 0) {
											// Last one
											CScenePackageInternals*	THIS = this;
											DisposeOf(THIS);
										}
									}

		TPtrArray<CScene*>	mScenes;
		OV<SceneIndex>		mInitialSceneIndex;
		UInt32				mReferenceCount;
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

	mInternals->mInitialSceneIndex = info.getSInt32(sInitialSceneIndexKey, 0);

	TArray<CDictionary>	sceneInfos = info.getArrayOfDictionaries(sScenesKey);
	for (CArrayItemIndex i = 0; i < sceneInfos.getCount(); i++)
		mInternals->mScenes += new CScene(sceneInfos[i]);
}

//----------------------------------------------------------------------------------------------------------------------
CScenePackage::CScenePackage(const CScenePackage& other)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = mInternals->addReference();
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

	if (mInternals->mInitialSceneIndex.hasValue())
		info.set(sInitialSceneIndexKey, *mInternals->mInitialSceneIndex);

	TArray<CDictionary>	sceneInfos;
	for (CArrayItemIndex i = 0; i < mInternals->mScenes.getCount(); i++)
		sceneInfos += mInternals->mScenes[i]->getInfo();
	info.set(sScenesKey, sceneInfos);

	return info;
}

//----------------------------------------------------------------------------------------------------------------------
CScene& CScenePackage::getInitialScene() const
//----------------------------------------------------------------------------------------------------------------------
{
	// Validation check
	AssertFailIf(!mInternals->mInitialSceneIndex.hasValue());

	return *mInternals->mScenes[*mInternals->mInitialSceneIndex];
}

//----------------------------------------------------------------------------------------------------------------------
UInt32 CScenePackage::getScenesCount() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mScenes.getCount();
}

//----------------------------------------------------------------------------------------------------------------------
CScene& CScenePackage::getSceneAtIndex(SceneIndex index) const
//----------------------------------------------------------------------------------------------------------------------
{
	return *mInternals->mScenes[index];
}

//----------------------------------------------------------------------------------------------------------------------
OV<SceneIndex> CScenePackage::getIndexOfScene(const CScene& scene)
//----------------------------------------------------------------------------------------------------------------------
{
	// Get index
	OV<CArrayItemIndex>	index = mInternals->mScenes.getIndexOf((CScene*) &scene);

	return index.hasValue() ? OV<SceneIndex>(*index) : OV<SceneIndex>();
}

//----------------------------------------------------------------------------------------------------------------------
CScenePackage& CScenePackage::operator=(const CScenePackage& other)
//----------------------------------------------------------------------------------------------------------------------
{
	// Remove reference to ourselves
	mInternals->removeReference();

	// Add reference to other
	mInternals = other.mInternals->addReference();

	return *this;
}
