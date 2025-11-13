//----------------------------------------------------------------------------------------------------------------------
//	CSceneAppResourceLoading-Standard.h			©2021 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CSceneAppResourceLoading.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CSceneAppResourceLoadingStandard

class CSceneAppResourceLoadingStandard : public CSceneAppResourceLoading {
	// Classes
	class Internals;

	// Methods
	public:
									// Lifecycle methods
									CSceneAppResourceLoadingStandard(const CFolder contentFolder);
									~CSceneAppResourceLoadingStandard();

									// Instance methods
		I<CDataSource>				createDataSource(const CString& resourceFilename,
											const OV<CString>& logMessage = OV<CString>()) const;
		I<CRandomAccessDataSource>	createRandomAccessDataSource(const CString& resourceFilename,
											const OV<CString>& logMessage = OV<CString>()) const;

		TArray<CScenePackage::Info>	getScenePackageInfos() const;

	// Properties
	private:
		Internals*	mInternals;
};
