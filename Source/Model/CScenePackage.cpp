//----------------------------------------------------------------------------------------------------------------------
//	CScenePackage.cpp			©2019 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CScenePackage.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CScenePackageInternals

class CScenePackageInternals : public TReferenceCountable<CScenePackageInternals> {
	public:
		CScenePackageInternals(const S2DSizeF32& size) : TReferenceCountable(), mSize(size) {}

		S2DSizeF32			mSize;
		TNArray<CScene>		mScenes;
		OV<CScene::Index>	mInitialSceneIndex;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CScenePackage

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
CScenePackage::CScenePackage(const S2DSizeF32& size)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new CScenePackageInternals(size);
}

//----------------------------------------------------------------------------------------------------------------------
CScenePackage::CScenePackage(const S2DSizeF32& size, const CDictionary& info)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new CScenePackageInternals(size);

	mInternals->mScenes =
			TNArray<CScene>(info.getArrayOfDictionaries(CString(OSSTR("scenes"))),
					(CScene (*)(CArray::ItemRef item)) CScene::makeFrom);
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
const S2DSizeF32& CScenePackage::getSize() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mSize;
}

//----------------------------------------------------------------------------------------------------------------------
CDictionary CScenePackage::getInfo() const
//----------------------------------------------------------------------------------------------------------------------
{
	CDictionary	info;

	info.set(CString(OSSTR("scenes")),
			TNArray<CDictionary>(mInternals->mScenes, (CDictionary (*)(CArray::ItemRef item)) CScene::getInfoFrom));
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

// MARK: Class methods

//----------------------------------------------------------------------------------------------------------------------
TArray<CScenePackage::Info> CScenePackage::getScenePackageInfos(const TArray<CFile>& files)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	TNArray<Info>	scenePackageInfos;

	// Iterate files
	for (TIteratorD<CFile> iterator = files.getIterator(); iterator.hasValue(); iterator.advance()) {
		// Get filename
		CString	filename = iterator.getValue().getName();

		// Check for Scenes package
		if (filename.hasPrefix(CString(OSSTR("Scenes_"))) && filename.hasSuffix(CString(OSSTR(".plist")))) {
			// Collect info
			TArray<CString>	dimensions =
									filename
											.components(CString(OSSTR(".")))[0]
											.components(CString(OSSTR("_")))[1]
											.components(CString(OSSTR("x")));
			Float32			width = dimensions[0].getFloat32();
			Float32			height = dimensions[1].getFloat32();
			Float32			totalPixels = width * height;
			Float32			aspectRatio = width / height;

			// Add info
			scenePackageInfos.add(Info(filename, S2DSizeF32(width, height), totalPixels, aspectRatio));
		}
	}

	return scenePackageInfos;
}
