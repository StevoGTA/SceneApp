//----------------------------------------------------------------------------------------------------------------------
//	CSceneItemPlayerPDF.h			©2014 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CSceneItemPDF.h"
#include "CSceneItemPlayer.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CSceneItemPlayerPDF

class CSceneItemPlayerPDFInternals;
class CSceneItemPlayerPDF : public CSceneItemPlayer {
	// Methods
	public:
								// Lifecycle methods
								CSceneItemPlayerPDF(const CSceneItemPDF& sceneItemPDF,
										const SSceneAppResourceManagementInfo& sceneAppResourceManagementInfo,
										const SSceneItemPlayerProcsInfo& sceneItemPlayerProcsInfo);
								~CSceneItemPlayerPDF();

								// CSceneItemPlayer methods
//				S2DRect32		getCurrentScreenRect() const;

				CActions		getAllActions() const;

				void			load();
				void			unload();
//				bool			getIsFullyLoaded() const;

				void			render(CGPURenderEngine& gpuRenderEngine, const S2DPoint32& offset);

				bool			handlesTouchOrMouseAtPoint(const S2DPoint32& point) const;
				void			touchBeganOrMouseDownAtPoint(const S2DPoint32& point, UInt32 tapOrClickCount,
										const void* reference);
				void			touchOrMouseMovedFromPoint(const S2DPoint32& point1, const S2DPoint32& point2,
										const void* reference);
				void			touchEndedOrMouseUpAtPoint(const S2DPoint32& point, const void* reference);
				void			touchOrMouseCancelled(const void* reference);

								// Instance methods
		const	CSceneItemPDF&	getSceneItemPDF() const
									{ return (CSceneItemPDF&) getSceneItem(); }

	// Properties
	private:
		CSceneItemPlayerPDFInternals*	mInternals;
};
