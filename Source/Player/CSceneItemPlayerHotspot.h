//----------------------------------------------------------------------------------------------------------------------
//	CSceneItemPlayerHotspot.h			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CSceneItemHotspot.h"
#include "CSceneItemPlayer.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CSceneItemPlayerHotspot

class CSceneItemPlayerHotspotInternals;
class CSceneItemPlayerHotspot : public CSceneItemPlayer {
	// Methods
	public:
									// Lifecycle methods
									CSceneItemPlayerHotspot(const CSceneItemHotspot& sceneItemHotspot,
											const SSceneAppResourceManagementInfo& sceneAppResourceManagementInfo,
											const SSceneItemPlayerProcsInfo& sceneItemPlayerProcsInfo);

									// CSceneItemPlayer methods
				CActions			getAllActions() const;

				void				render(CGPU& gpu, const SGPURenderObjectRenderInfo& renderInfo) const {}

				bool				handlesTouchOrMouseAtPoint(const S2DPointF32& point) const;
				void				touchBeganOrMouseDownAtPoint(const S2DPointF32& point, UInt32 tapOrClickCount,
											const void* reference);

									// Instance methods
		const	CSceneItemHotspot&	getSceneItemHotspot() const
										{ return (CSceneItemHotspot&) getSceneItem(); }
};
