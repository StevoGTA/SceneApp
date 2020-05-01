//----------------------------------------------------------------------------------------------------------------------
//	CSceneAppNativeImageView.h			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CSceneAppNativeView.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CSceneAppNativeImageView

class CSceneAppNativeImageViewInternals;
class CSceneAppNativeImageView : public CSceneAppNativeView {
	// Methods
	public:
						// Lifecycle methods
						CSceneAppNativeImageView(const CImageX& initialImage);
		virtual			~CSceneAppNativeImageView();

						// Instance methods
		virtual	void*	getNativeViewPtr() const;

				void	setImage(const CImageX& image);

	// Properties
	private:
		CSceneAppNativeImageViewInternals*	mInternals;
};
