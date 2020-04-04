//----------------------------------------------------------------------------------------------------------------------
//	CSceneItemPlayerButton.h			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CSceneItemButton.h"
#include "CSceneItemPlayer.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CSceneItemPlayerButton

class CSceneItemPlayerButtonInternals;
class CSceneItemPlayerButton : public CSceneItemPlayer {
	// Methods
	public:
									// Lifecycle methods
									CSceneItemPlayerButton(const CSceneItemButton& sceneItemButton,
											const SSceneAppResourceManagementInfo& sceneAppResourceManagementInfo,
											const SSceneItemPlayerProcsInfo& sceneItemPlayerProcsInfo);
									~CSceneItemPlayerButton();

									// CSceneItemPlayer methods
				CActions			getAllActions() const;

				void				load();
				void				unload();

				void				reset();
				void				update(UniversalTimeInterval deltaTimeInterval, bool isRunning);
				void				render(CGPU& gpu, const SGPURenderObjectRenderInfo& renderInfo) const;

				bool				handlesTouchOrMouseAtPoint(const S2DPoint32& point) const;
				void				touchBeganOrMouseDownAtPoint(const S2DPoint32& point, UInt32 tapOrClickCount,
											const void* reference);
				void				touchOrMouseMovedFromPoint(const S2DPoint32& point1, const S2DPoint32& point2,
											const void* reference);
				void				touchEndedOrMouseUpAtPoint(const S2DPoint32& point, const void* reference);
				void				touchOrMouseCancelled(const void* reference);

				void				setProperty(const CString& propertyName, const SDictionaryValue& value);

									// Instance methods
		const	CSceneItemButton&	getSceneItemButton() const
										{ return (CSceneItemButton&) getSceneItem(); }

				void				setIsEnabled(bool isEnabled);

	// Properties
	public:
		static	CString								mPropertyNameEnabled;

	private:
				CSceneItemPlayerButtonInternals*	mInternals;
};
