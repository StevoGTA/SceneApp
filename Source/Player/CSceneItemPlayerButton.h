//----------------------------------------------------------------------------------------------------------------------
//	CSceneItemPlayerButton.h			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CSceneItemButton.h"
#include "CSceneItemPlayer.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CSceneItemPlayerButton

class CSceneItemPlayerButton : public CSceneItemPlayer {
	// Classes
	private:
		class Internals;

	// Methods
	public:
									// Lifecycle methods
									CSceneItemPlayerButton(const CSceneItemButton& sceneItemButton,
											const SSceneAppResourceManagementInfo& sceneAppResourceManagementInfo,
											const Procs& procs);
									~CSceneItemPlayerButton();

									// CSceneItemPlayer methods
				CActions			getAllActions() const;

				void				load(CGPU& gpu);
				void				unload();

				void				reset();
				void				update(UniversalTimeInterval deltaTimeInterval, bool isRunning);
				void				render(CGPU& gpu, const CGPURenderObject::RenderInfo& renderInfo) const;

				bool				handlesTouchOrMouseAtPoint(const S2DPointF32& point) const;
				void				touchBeganOrMouseDownAtPoint(const S2DPointF32& point, UInt32 tapOrClickCount,
											const void* reference);
				void				touchOrMouseMovedFromPoint(const S2DPointF32& point1, const S2DPointF32& point2,
											const void* reference);
				void				touchEndedOrMouseUpAtPoint(const S2DPointF32& point, const void* reference);
				void				touchOrMouseCancelled(const void* reference);

				void				setProperty(const CString& propertyName, const SValue& value);

									// Instance methods
		const	CSceneItemButton&	getSceneItemButton() const
										{ return (CSceneItemButton&) getSceneItem(); }

				void				setIsEnabled(bool isEnabled);

	// Properties
	public:
		static	CString		mPropertyNameEnabled;

	private:
				Internals*	mInternals;
};
