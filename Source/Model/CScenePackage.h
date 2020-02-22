//----------------------------------------------------------------------------------------------------------------------
//	CScenePackage.h			©2019 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CScene.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CScenePackage

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
				CDictionary		getInfo() const;

		const	CScene&			getInitialScene() const;
				UInt32			getScenesCount() const;
		const	CScene&			getSceneAtIndex(CSceneIndex index) const;
				OV<CSceneIndex>	getIndexOfScene(const CScene& scene);


				CScenePackage&	operator=(const CScenePackage& other);

	// Properties
	private:
		CScenePackageInternals*	mInternals;
};
