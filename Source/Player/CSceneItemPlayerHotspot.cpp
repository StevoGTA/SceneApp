//----------------------------------------------------------------------------------------------------------------------
//	CSceneItemPlayerHotspot.cpp			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CSceneItemPlayerHotspot.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CSceneItemPlayerHotspot

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
CSceneItemPlayerHotspot::CSceneItemPlayerHotspot(const CSceneItemHotspot& sceneItemHotspot,
		const SSceneAppResourceManagementInfo& sceneAppResourceManagementInfo,
		const SSceneItemPlayerProcsInfo& sceneItemPlayerProcsInfo) :
		CSceneItemPlayer(sceneItemHotspot, sceneItemPlayerProcsInfo)
//----------------------------------------------------------------------------------------------------------------------
{
}

// MARK: CSceneItemPlayer methods

//----------------------------------------------------------------------------------------------------------------------
CActions CSceneItemPlayerHotspot::getAllActions() const
//----------------------------------------------------------------------------------------------------------------------
{
	CActions	actionsArray;

	const	OO<CActions>&	actions = getSceneItemHotspot().getActions();
	if (actions.hasObject())
		actionsArray.addFrom(*actions);

	return actionsArray;
}

//----------------------------------------------------------------------------------------------------------------------
bool CSceneItemPlayerHotspot::handlesTouchOrMouseAtPoint(const S2DPoint32& point) const
//----------------------------------------------------------------------------------------------------------------------
{
	return getSceneItemHotspot().getScreenRect().contains(point);
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerHotspot::touchBeganOrMouseDownAtPoint(const S2DPoint32& point, UInt32 tapOrClickCount,
		const void* reference)
//----------------------------------------------------------------------------------------------------------------------
{
	const	OO<CActions>&	actions = getSceneItemHotspot().getActions();
	if (actions.hasObject())
		perform(*actions, point);
}
