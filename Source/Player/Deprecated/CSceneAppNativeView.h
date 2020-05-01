//----------------------------------------------------------------------------------------------------------------------
//	CSceneAppNativeView.h			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

//----------------------------------------------------------------------------------------------------------------------
// MARK: CSceneAppNativeView

class CSceneAppNativeView {
	// Methods
	public:
						// Lifecycle methods
						CSceneAppNativeView();
		virtual			~CSceneAppNativeView();

						// Instance methods
				void	present() const;
				void	hide() const;

				void	setCenterPoint(const S2DPoint32& centerPoint);

		virtual	void*	getNativeViewPtr() const = 0;
};
