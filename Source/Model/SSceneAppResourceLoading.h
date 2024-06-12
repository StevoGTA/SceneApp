//----------------------------------------------------------------------------------------------------------------------
//	SSceneAppResourceLoading.h			©2021 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CDataSource.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: SSceneAppResourceLoading

struct SSceneAppResourceLoading {
	// Procs
	public:
		typedef	I<CDataSource>				(*CreateDataSourceProc)(const CString& resourceFilename, void* userData);
		typedef	I<CRandomAccessDataSource>	(*CreateRandomAccessDataSourceProc)(const CString& resourceFilename,
													void* userData);

	// Methods
	public:
									// Lifecycle methods
									SSceneAppResourceLoading(CreateDataSourceProc createDataSourceProc,
											CreateRandomAccessDataSourceProc createRandomAccessDataSourceProc,
											void* userData) :
										mCreateDataSourceProc(createDataSourceProc),
												mCreateRandomAccessDataSourceProc(createRandomAccessDataSourceProc),
												mUserData(userData)
										{}

									// Instance methods
		I<CDataSource>				createDataSource(const CString& resourceFilename) const
										{ return mCreateDataSourceProc(resourceFilename, mUserData); }
		I<CRandomAccessDataSource>	createRandomAccessDataSource(const CString& resourceFilename) const
										{ return mCreateRandomAccessDataSourceProc(resourceFilename, mUserData); }

	// Properties
	private:
		CreateDataSourceProc				mCreateDataSourceProc;
		CreateRandomAccessDataSourceProc	mCreateRandomAccessDataSourceProc;
		void*								mUserData;
};
