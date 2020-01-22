//----------------------------------------------------------------------------------------------------------------------
//	CSceneItemPlayerHotspot.cpp			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CSceneItemPlayerHotspot.h"

////----------------------------------------------------------------------------------------------------------------------
//// MARK: CSceneItemPlayerHotspotInternals
//
//class CSceneItemPlayerHotspotInternals {
//	public:
//		CSceneItemPlayerHotspotInternals() {}
//		~CSceneItemPlayerHotspotInternals() {}
//};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - Local proc declarations

//static	CSceneItemPlayer*	sCreateSceneItemPlayer(const CSceneItem& sceneItem,
//									const SSceneAppResourceManagementInfo& sceneAppResourceManagementInfo,
//									const SSceneItemPlayerProcsInfo& sceneItemPlayerProcsInfo, bool makeCopy);

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - Register Scene Item Player

//REGISTER_SCENE_ITEM_PLAYER(CSceneItemPlayerHotspot, sCreateSceneItemPlayer, CSceneItemHotspot::mType);

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneItemPlayerHotspot

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
CSceneItemPlayerHotspot::CSceneItemPlayerHotspot(const CSceneItemHotspot& sceneItemHotspot,
		const SSceneAppResourceManagementInfo& sceneAppResourceManagementInfo,
		const SSceneItemPlayerProcsInfo& sceneItemPlayerProcsInfo) :
		CSceneItemPlayer(sceneItemHotspot, sceneItemPlayerProcsInfo)
//----------------------------------------------------------------------------------------------------------------------
{
//	mInternals = new CSceneItemPlayerHotspotInternals();
}

//----------------------------------------------------------------------------------------------------------------------
CSceneItemPlayerHotspot::~CSceneItemPlayerHotspot()
//----------------------------------------------------------------------------------------------------------------------
{
//	DisposeOf(mInternals);
}

// MARK: CSceneItemPlayer methods

//----------------------------------------------------------------------------------------------------------------------
S2DRect32 CSceneItemPlayerHotspot::getCurrentScreenRect() const
//----------------------------------------------------------------------------------------------------------------------
{
	return getSceneItemHotspot().getScreenRect();
}

//----------------------------------------------------------------------------------------------------------------------
CActionArray CSceneItemPlayerHotspot::getAllActions() const
//----------------------------------------------------------------------------------------------------------------------
{
	CActionArray	actionsArray;

	OR<CActionArray>	actionArray = getSceneItemHotspot().getActionArray();
	if (actionArray.hasReference())
		actionsArray.addFrom(*actionArray);

	return actionsArray;
}

//----------------------------------------------------------------------------------------------------------------------
bool CSceneItemPlayerHotspot::handlesTouchOrMouseAtPoint(const S2DPoint32& point)
//----------------------------------------------------------------------------------------------------------------------
{
	return getSceneItemHotspot().getScreenRect().contains(point);
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerHotspot::touchBeganOrMouseDownAtPoint(const S2DPoint32& point, UInt32 tapOrClickCount,
		const void* reference)
//----------------------------------------------------------------------------------------------------------------------
{
	OR<CActionArray>	actionArray = getSceneItemHotspot().getActionArray();
	if (actionArray.hasReference())
		perform(*actionArray, point);
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - Local proc definitions

////----------------------------------------------------------------------------------------------------------------------
//CSceneItemPlayer* sCreateSceneItemPlayer(const CSceneItem& sceneItem,
//		const SSceneAppResourceManagementInfo& sceneAppResourceManagementInfo,
//		const SSceneItemPlayerProcsInfo& sceneItemPlayerProcsInfo, bool makeCopy)
////----------------------------------------------------------------------------------------------------------------------
//{
//	return new CSceneItemPlayerHotspot(*((CSceneItemHotspot*) &sceneItem), sceneAppResourceManagementInfo,
//			sceneItemPlayerProcsInfo, makeCopy);
//}
