//----------------------------------------------------------------------------------------------------------------------
//	CSceneItemPlayerHotspot.cpp			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CSceneItemPlayerHotspot.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CSceneItemPlayerHotspot

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
CSceneItemPlayerHotspot::CSceneItemPlayerHotspot(const CSceneItemHotspot& sceneItemHotspot, const Procs& procs) :
		CSceneItemPlayer(sceneItemHotspot, procs)
//----------------------------------------------------------------------------------------------------------------------
{
}

// MARK: CSceneItemPlayer methods

//----------------------------------------------------------------------------------------------------------------------
CActions CSceneItemPlayerHotspot::getAllActions() const
//----------------------------------------------------------------------------------------------------------------------
{
	CActions	actionsArray;

	const	OI<CActions>&	actions = getSceneItemHotspot().getActions();
	if (actions.hasInstance())
		actionsArray.addFrom(*actions);

	return actionsArray;
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
	const	OI<CActions>&	actions = getSceneItemHotspot().getActions();
	if (actions.hasInstance())
		perform(*actions, point);
}
