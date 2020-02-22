//
//  CSceneApp.cpp
//  StevoBrock-Core
//
//  Created by Stevo on 9/27/19.
//

#include "CSceneApp.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CSceneApp

// MARK: Class methods

//----------------------------------------------------------------------------------------------------------------------
TArray<SScenePackageInfo> CSceneApp::scenePackageInfosForScenePackageFilenames(
		const TArray<CString>& scenePackageFilenames)
{
	// Iterate all scene package filenames
	TNArray<SScenePackageInfo>	scenePackageInfos;
	for (TIteratorD<CString> iterator = scenePackageFilenames.getIterator(); iterator.hasValue(); iterator.advance()) {
		// Collect info
		TArray<CString>	dimensions =
								iterator.getValue().breakUp(CString(OSSTR(".")))[0].breakUp(CString(OSSTR("_")))[2]
										.breakUp(CString(OSSTR("x")));
		Float32			width = dimensions[0].getFloat32();
		Float32			height = dimensions[1].getFloat32();
		Float32			totalPixels = width * height;
		Float32			aspectRatio = width / height;

		// Add info
		scenePackageInfos.add(
				SScenePackageInfo(iterator.getValue(), S2DSize32(width, height), totalPixels, aspectRatio));
	}

	return scenePackageInfos;
}
