//
//  CScenePackage.h
//  StevoBrock-Core
//
//  Created by Stevo on 10/4/19.
//

#pragma once

#include "CScene.h"

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
		CDictionary		getInfo() const;

		CScene&			getInitialScene() const;
		UInt32			getScenesCount() const;
		CScene&			getSceneAtIndex(SceneIndex index) const;
		OV<SceneIndex>	getIndexOfScene(const CScene& scene);


		CScenePackage&	operator=(const CScenePackage& other);

	// Properties
	private:
		CScenePackageInternals*	mInternals;
};
