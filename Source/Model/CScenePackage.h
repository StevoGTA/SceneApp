//----------------------------------------------------------------------------------------------------------------------
//	CScenePackage.h			©2019 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CFolder.h"
#include "CScene.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CScenePackage

class CScenePackage {
	// Info
	public:
		struct Info {
			// Methods
			public:
									// Lifecycle methods
									Info(const CString& filename, const S2DSizeF32& pixelDimensions) :
										mFilename(filename), mPixelDimensions(pixelDimensions)
										{}

									// Instance methods
				const	CString&	getFilename() const
										{ return mFilename; }
				const	S2DSizeF32&	getPixelDimensions() const
										{ return mPixelDimensions; }
						Float32		getTotalPixels() const
										{ return mPixelDimensions.mWidth * mPixelDimensions.mHeight; }
						Float32		getAspectRatio() const
										{ return mPixelDimensions.mWidth / mPixelDimensions.mHeight; }

			// Properties
			private:
				CString		mFilename;
				S2DSizeF32	mPixelDimensions;
		};

	// Classes
	private:
		class Internals;

	// Methods
	public:
									// Lifecycle methods
									CScenePackage(const S2DSizeF32& pixelDimensions = S2DSizeF32());
									CScenePackage(const S2DSizeF32& pixelDimensions, const CDictionary& info);
									CScenePackage(const CScenePackage& other);
									~CScenePackage();

									// Instance methods
		const	S2DSizeF32&			getPixelDimensions() const;
				CDictionary			getInfo() const;

				UInt32				getScenesCount() const;
				UInt32				getInitialSceneIndex() const;
		const	CScene&				getSceneAtIndex(CScene::Index index) const;
				OV<CScene::Index>	getIndexOfScene(const CScene& scene);

				CScenePackage&		operator=(const CScenePackage& other);

	// Properties
	private:
		Internals*	mInternals;
};
