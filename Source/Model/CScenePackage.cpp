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

// MARK: Class methods

//----------------------------------------------------------------------------------------------------------------------
TArray<SScenePackageInfo> CScenePackage::getScenePackageInfos(const TArray<CFile>& files)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	TNArray<SScenePackageInfo>	scenePackageInfos;

	// Iterate files
	for (TIteratorD<CFile> iterator = files.getIterator(); iterator.hasValue(); iterator.advance()) {
		// Get filename
		CString	filename = iterator.getValue().getName();

		// Check for Scenes package
		if (filename.hasPrefix(CString(OSSTR("Scenes_"))) && filename.hasSuffix(CString(OSSTR(".plist")))) {
			// Collect info
			TArray<CString>	dimensions =
									filename
											.breakUp(CString(OSSTR(".")))[0]
											.breakUp(CString(OSSTR("_")))[1]
											.breakUp(CString(OSSTR("x")));
			Float32			width = dimensions[0].getFloat32();
			Float32			height = dimensions[1].getFloat32();
			Float32			totalPixels = width * height;
			Float32			aspectRatio = width / height;

			// Add info
			scenePackageInfos.add(
					SScenePackageInfo(filename, S2DSizeF32(width, height), totalPixels, aspectRatio));
		}
	}

	return scenePackageInfos;
}
