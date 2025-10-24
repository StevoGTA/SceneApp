//----------------------------------------------------------------------------------------------------------------------
//	CSceneItemPlayerHotspot.cpp			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CSceneItemPlayerHotspot.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CSceneItemPlayerHotspot

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
CSceneItemPlayerHotspot::CSceneItemPlayerHotspot(CSceneItemHotspot& sceneItemHotspot, const Procs& procs) :
		CSceneItemPlayer(sceneItemHotspot, procs)
//----------------------------------------------------------------------------------------------------------------------
{
}

// MARK: CSceneItemPlayer methods

//----------------------------------------------------------------------------------------------------------------------
CActions CSceneItemPlayerHotspot::getAllActions() const
//----------------------------------------------------------------------------------------------------------------------
{
	return getSceneItemHotspot().getActions().getValue(CActions());
}

//----------------------------------------------------------------------------------------------------------------------
bool CSceneItemPlayerHotspot::handlesTouchOrMouseAtPoint(const S2DPointF32& point) const
//----------------------------------------------------------------------------------------------------------------------
{
	return getSceneItemHotspot().getScreenRect().contains(point);
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerHotspot::touchBeganOrMouseDownAtPoint(const S2DPointF32& point, UInt32 tapOrClickCount,
		const void* reference)
//----------------------------------------------------------------------------------------------------------------------
{
	const	OV<CActions>&	actions = getSceneItemHotspot().getActions();
	if (actions.hasValue())
		perform(*actions, point);
}
