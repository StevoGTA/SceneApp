//----------------------------------------------------------------------------------------------------------------------
//	CSceneItemPlayerText.h			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CSceneItemPlayer.h"
#include "CSceneItemText.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CSceneItemPlayerText

class CSceneItemPlayerText : public CSceneItemPlayer {
	// Classes
	private:
		class Internals;

	// Methods
	public:
								// Lifecycle methods
								CSceneItemPlayerText(CSceneItemText& sceneItemText,
										const SSceneAppResourceManagementInfo& sceneAppResourceManagementInfo,
										const Procs& procs);
								~CSceneItemPlayerText();

								// CSceneItemPlayer methods
				void			update(CGPU& gpu, UniversalTimeInterval deltaTimeInterval, bool isRunning);
				void			render(CGPU& gpu, const CGPURenderObject::RenderInfo& renderInfo) const;

								// Instance methods
		const	CSceneItemText&	getSceneItemText() const
									{ return (const CSceneItemText&) *getSceneItem(); }

	protected:
								// CSCeneItemPlayer methods
				void			noteSceneItemUpdated(const CString& propertyName);

	// Properties
	private:
				Internals*	mInternals;
};
