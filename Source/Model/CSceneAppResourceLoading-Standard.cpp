//----------------------------------------------------------------------------------------------------------------------
//	CSceneAppResourceLoading-Standard.cpp			©2025 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CSceneAppResourceLoading-Standard.h"

#include "CFolder.h"
#include "CLogServices.h"
#include "CFileDataSource.h"
#include "CFilesystem.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CSceneAppResourceLoadingStandard::Internals

class CSceneAppResourceLoadingStandard::Internals {
	public:
		Internals(const CFolder contentFolder) : mSceneAppContentFolder(contentFolder) {}

		CFolder	mSceneAppContentFolder;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - SSceneAppResourceLoading

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
CSceneAppResourceLoadingStandard::CSceneAppResourceLoadingStandard(const CFolder contentFolder)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new Internals(contentFolder);
}

//----------------------------------------------------------------------------------------------------------------------
CSceneAppResourceLoadingStandard::~CSceneAppResourceLoadingStandard()
//----------------------------------------------------------------------------------------------------------------------
{
	Delete(mInternals);
}

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
I<CDataSource> CSceneAppResourceLoadingStandard::createDataSource(const CString& resourceFilename,
		const OV<CString>& logMessage) const
//----------------------------------------------------------------------------------------------------------------------
{
	// Check if have log message
	if (logMessage.hasValue())
		// Log
		CLogServices::logMessage(*logMessage);

	//return I<CDataSource>(new CMappedFileDataSource(mInternals->mSceneAppContentFolder.getFile(resourceFilename)));
	return I<CDataSource>(new CFileDataSource(mInternals->mSceneAppContentFolder.getFile(resourceFilename)));
}

//----------------------------------------------------------------------------------------------------------------------
I<CRandomAccessDataSource> CSceneAppResourceLoadingStandard::createRandomAccessDataSource(const CString& resourceFilename,
		const OV<CString>& logMessage) const
//----------------------------------------------------------------------------------------------------------------------
{
	// Check if have log message
	if (logMessage.hasValue())
		// Log
		CLogServices::logMessage(*logMessage);

	return I<CRandomAccessDataSource>(new CMappedFileDataSource(mInternals->mSceneAppContentFolder.getFile(resourceFilename)));
}

//----------------------------------------------------------------------------------------------------------------------
TArray<CScenePackage::Info> CSceneAppResourceLoadingStandard::getScenePackageInfos() const
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	TNArray<CScenePackage::Info>	scenePackageInfos;

	// Get files
	TArray<CFile>	files = *CFilesystem::getFiles(mInternals->mSceneAppContentFolder);

	// Iterate files
	for (TIteratorD<CFile> iterator = files.getIterator(); iterator.hasValue(); iterator.advance()) {
		// Get filename
		CString	filename = iterator->getName();

		// Check for Scenes package
		if (filename.hasPrefix(CString(OSSTR("Scenes_"))) && filename.hasSuffix(CString(OSSTR(".plist")))) {
			// Collect info
			TArray<CString>	dimensions =
									filename
											.components(CString(OSSTR(".")))[0]
											.components(CString(OSSTR("_")))[1]
											.components(CString(OSSTR("x")));

			// Add info
			scenePackageInfos.add(
					CScenePackage::Info(iterator->getName(),
							S2DSizeF32(dimensions[0].getFloat32(), dimensions[1].getFloat32())));
		}
	}

	return scenePackageInfos;
}
