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
		typedef	I<CDataSource>			(*CreateDataSourceProc)(const CString& resourceFilename, void* userData);
		typedef	I<CSeekableDataSource>	(*CreateSeekableDataSourceProc)(const CString& resourceFilename,
												void* userData);

	// Methods
	public:
							// Lifecycle methods
							SSceneAppResourceLoading(CreateDataSourceProc createDataSourceProc,
									CreateSeekableDataSourceProc createSeekableDataSourceProc, void* userData) :
								mCreateDataSourceProc(createDataSourceProc),
										mCreateSeekableDataSourceProc(createSeekableDataSourceProc), mUserData(userData)
								{}

							// Instance methods
	I<CDataSource>			createDataSource(const CString& resourceFilename) const
								{ return mCreateDataSourceProc(resourceFilename, mUserData); }
	I<CSeekableDataSource>	createSeekableDataSource(const CString& resourceFilename) const
								{ return mCreateSeekableDataSourceProc(resourceFilename, mUserData); }

	// Properties
	private:
		CreateDataSourceProc			mCreateDataSourceProc;
		CreateSeekableDataSourceProc	mCreateSeekableDataSourceProc;
		void*							mUserData;
};
