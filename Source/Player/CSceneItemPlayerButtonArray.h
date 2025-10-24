//----------------------------------------------------------------------------------------------------------------------
//	CSceneItemPlayerButtonArray.h			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CSceneItemButtonArray.h"
#include "CSceneItemPlayer.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CSceneItemPlayerButtonArray

class CSceneItemPlayerButtonArray : public CSceneItemPlayer {
	// Classes
	private:
		class Internals;

	// Methods
	public:
										// Lifecycle methods
										CSceneItemPlayerButtonArray(CSceneItemButtonArray& sceneItemButtonArray,
												const SSceneAppResourceManagementInfo& sceneAppResourceManagementInfo,
												const Procs& procs);
										~CSceneItemPlayerButtonArray();

										// CSceneItemPlayer methods
				CActions				getAllActions() const;

				void					load(CGPU& gpu);
				void					unload();

				void					reset();
				void					update(CGPU& gpu, UniversalTimeInterval deltaTimeInterval, bool isRunning);
				void					render(CGPU& gpu, const CGPURenderObject::RenderInfo& renderInfo) const;

				bool					handlesTouchOrMouseAtPoint(const S2DPointF32& point) const;
				void					touchBeganOrMouseDownAtPoint(const S2DPointF32& point, UInt32 tapOrClickCount,
												const void* reference);
				void					touchOrMouseMovedFromPoint(const S2DPointF32& point1, const S2DPointF32& point2,
												const void* reference);
				void					touchEndedOrMouseUpAtPoint(const S2DPointF32& point, const void* reference);
				void					touchOrMouseCancelled(const void* reference);

										// Instance methods
		const	CSceneItemButtonArray&	getSceneItemButtonArray() const
											{ return (CSceneItemButtonArray&) *getSceneItem(); }

	// Properties
	private:
		Internals*	mInternals;
};
