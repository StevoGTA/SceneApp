//----------------------------------------------------------------------------------------------------------------------
//	CScenePackage.h			©2019 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CFile.h"
#include "CScene.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CScenePackage

class CScenePackageInternals;
class CScenePackage {
	// Structs
	public:
		struct Info {
			// Lifecycle methods
			Info(const CString& filename, const S2DSizeF32& size, Float32 totalPixels, Float32 aspectRatio) :
				mFilename(filename), mSize(size), mTotalPixels(totalPixels), mAspectRatio(aspectRatio)
				{}

			// Properties
			CString		mFilename;
			S2DSizeF32	mSize;
			Float32		mTotalPixels;
			Float32		mAspectRatio;
		};

	// Methods
	public:
									// Lifecycle methods
									CScenePackage(const S2DSizeF32& size = S2DSizeF32());
									CScenePackage(const S2DSizeF32& size, const CDictionary& info);
									CScenePackage(const CScenePackage& other);
									~CScenePackage();

									// Instance methods
		const	S2DSizeF32&			getSize() const;
				CDictionary			getInfo() const;

				UInt32				getScenesCount() const;
				UInt32				getInitialSceneIndex() const;
		const	CScene&				getSceneAtIndex(CScene::Index index) const;
				OV<CScene::Index>	getIndexOfScene(const CScene& scene);

				CScenePackage&		operator=(const CScenePackage& other);

									// Class methods
		static	TArray<Info>		getScenePackageInfos(const TArray<CFile>& files);

	// Properties
	private:
		CScenePackageInternals*	mInternals;
};
