//----------------------------------------------------------------------------------------------------------------------
//	CSceneItemPlayerText.h			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CSceneItemPlayer.h"
#include "CSceneItemText.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CSceneItemPlayerText

class CSceneItemPlayerTextInternals;
class CSceneItemPlayerText : public CSceneItemPlayer {
	// Methods
	public:
								// Lifecycle methods
								CSceneItemPlayerText(const CSceneItemText& sceneItemText,
										const SSceneAppResourceManagementInfo& sceneAppResourceManagementInfo,
										const SSceneItemPlayerProcsInfo& sceneItemPlayerProcsInfo);
								~CSceneItemPlayerText();

								// CSceneItemPlayer methods
//				S2DRect32		getCurrentScreenRect() const;

				void			update(UniversalTimeInterval deltaTimeInterval, bool isRunning);
				void			render(CGPURenderEngine& gpuRenderEngine, const S2DPoint32& offset);

								// Instance methods
		const	CSceneItemText&	getSceneItemText() const
									{ return (CSceneItemText&) getSceneItem(); }

				void			setScreenRect(const S2DRect32& screenRect);
				void			setText(const CString& string);

	// Properties
	private:
		CSceneItemPlayerTextInternals*	mInternals;
};
