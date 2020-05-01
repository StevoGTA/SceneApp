//----------------------------------------------------------------------------------------------------------------------
//	CSceneAppNativeButton.h			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CSceneAppNativeView.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: SSceneAppNativeButtonProcsInfo

class CSceneAppNativeButton;
typedef void	(*CSceneAppNativeButtonActionProc)(CSceneAppNativeButton& button, void* userData);

struct SSceneAppNativeButtonProcsInfo {
	CSceneAppNativeButtonActionProc	mActionProc;
	void*							mUserData;
};

//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneAppNativeButton

class CSceneAppNativeButtonInternals;
class CSceneAppNativeButton : public CSceneAppNativeView {
	// Methods
	public:
						// Lifecycle methods
						CSceneAppNativeButton(const S2DRect32& frame);
		virtual			~CSceneAppNativeButton();

						// Instance methods
		virtual	void*	getNativeViewPtr() const;

				void	setNormalImage(const CImageX& image);
				void	setHighlightedImage(const CImageX& image);

				void	setSceneAppNativeButtonProcsInfo(
								const SSceneAppNativeButtonProcsInfo& sceneAppNativeButtonProcsInfo);

	// Properties
	private:
		CSceneAppNativeButtonInternals*	mInternals;
};
