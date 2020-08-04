//----------------------------------------------------------------------------------------------------------------------
//	CScenePackage.h			©2019 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CFile.h"
#include "CScene.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: SScenePackageInfo

struct SScenePackageInfo {
	// Lifecycle methods
	SScenePackageInfo(const CString& filename, const S2DSizeF32& size, Float32 totalPixels, Float32 aspectRatio) :
		mFilename(filename), mSize(size), mTotalPixels(totalPixels), mAspectRatio(aspectRatio)
		{}

	// Properties
	CString		mFilename;
	S2DSizeF32	mSize;
	Float32		mTotalPixels;
	Float32		mAspectRatio;
};

//----------------------------------------------------------------------------------------------------------------------
// MARK: - CScenePackage

class CScenePackageInternals;
class CScenePackage {
	// Methods
	public:
											// Lifecycle methods
											CScenePackage();
											CScenePackage(const CDictionary& info);
											CScenePackage(const CScenePackage& other);
											~CScenePackage();

											// Instance methods
				CDictionary					getInfo() const;

		const	CScene&						getInitialScene() const;
				UInt32						getScenesCount() const;
		const	CScene&						getSceneAtIndex(CSceneIndex index) const;
				OV<CSceneIndex>				getIndexOfScene(const CScene& scene);

				CScenePackage&				operator=(const CScenePackage& other);

											// Class methods
		static	TArray<SScenePackageInfo>	getScenePackageInfos(const TArray<CFile>& files);

	// Properties
	private:
		CScenePackageInternals*	mInternals;
};
