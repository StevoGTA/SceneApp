//----------------------------------------------------------------------------------------------------------------------
//	CSceneItemPlayerButtonArray.h			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CSceneItemButtonArray.h"
#include "CSceneItemPlayer.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CSceneItemPlayerButtonArray

class CSceneItemPlayerButtonArrayInternals;
class CSceneItemPlayerButtonArray : public CSceneItemPlayer {
	// Methods
	public:
										// Lifecycle methods
										CSceneItemPlayerButtonArray(const CSceneItemButtonArray& sceneItemButtonArray,
												const SSceneAppResourceManagementInfo& sceneAppResourceManagementInfo,
												const SSceneItemPlayerProcsInfo& sceneItemPlayerProcsInfo);
										~CSceneItemPlayerButtonArray();

										// CSceneItemPlayer methods
				CActions				getAllActions() const;

				void					load(CGPU& gpu);
				void					unload();

				void					reset();
				void					update(UniversalTimeInterval deltaTimeInterval, bool isRunning);
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
											{ return (CSceneItemButtonArray&) getSceneItem(); }

	// Properties
	private:
		CSceneItemPlayerButtonArrayInternals*	mInternals;
};
