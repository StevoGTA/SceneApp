//----------------------------------------------------------------------------------------------------------------------
//	CSceneAppResourceLoading.h			©2021 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CDataSource.h"
#include "CScenePackage.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CSceneAppResourceLoading

class CSceneAppResourceLoading {
	// Methods
	public:
											// Lifecycle methods
											CSceneAppResourceLoading() {}
		virtual								~CSceneAppResourceLoading() {}

											// Instance methods
		virtual	I<CDataSource>				createDataSource(const CString& resourceFilename,
													const OV<CString>& logMessage = OV<CString>()) const = 0;
				I<CDataSource>				createDataSource(const CString& resourceFilename,
													const CString& logMessage) const
												{ return createDataSource(resourceFilename, OV<CString>(logMessage)); }
		virtual	I<CRandomAccessDataSource>	createRandomAccessDataSource(const CString& resourceFilename,
													const OV<CString>& logMessage = OV<CString>()) const = 0;
				I<CRandomAccessDataSource>	createRandomAccessDataSource(const CString& resourceFilename,
													const CString& logMessage)
												{ return createRandomAccessDataSource(resourceFilename,
														OV<CString>(logMessage)); }

		virtual	TArray<CScenePackage::Info>	getScenePackageInfos() const = 0;
};
