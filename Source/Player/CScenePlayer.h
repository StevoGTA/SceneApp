//----------------------------------------------------------------------------------------------------------------------
//	CScenePlayer.h			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CSceneItemPlayer.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: SScenePlayerProcsInfo

class CScenePlayer;
typedef	S2DSize32				(*ScenePlayerGetViewportSizeProc)(void* userData);
typedef	OV<CSceneItemPlayer*>	(*ScenePlayerCreateSceneItemPlayerProc)(const CSceneItem& sceneItem,
										const SSceneAppResourceManagementInfo& sceneAppResourceManagementInfo,
										const SSceneItemPlayerProcsInfo& sceneItemPlayerProcsInfo, void* userData);
typedef	void					(*ScenePlayerActionArrayPerformProc)(const CActionArray& actionArray,
										const S2DPoint32& point, void* userData);
//typedef CImageX	(*ScenePlayerGetCurrentViewportImageProc)(CScenePlayer& scenePlayer, bool performRedraw,
//						void* userData);

struct SScenePlayerProcsInfo {
	// Lifecycle methods
	SScenePlayerProcsInfo(ScenePlayerGetViewportSizeProc getViewportSizeProc,
			ScenePlayerCreateSceneItemPlayerProc createSceneItemPlayerProc,
			ScenePlayerActionArrayPerformProc actionArrayPerformProc, void* userData) :
		mGetViewportSizeProc(getViewportSizeProc), mCreateSceneItemPlayerProc(createSceneItemPlayerProc),
				mActionArrayPerformProc(actionArrayPerformProc), mUserData(userData)
		{}

	// Properties
	ScenePlayerGetViewportSizeProc			mGetViewportSizeProc;
	ScenePlayerCreateSceneItemPlayerProc	mCreateSceneItemPlayerProc;
	ScenePlayerActionArrayPerformProc		mActionArrayPerformProc;
//	ScenePlayerGetCurrentViewportImageProc	mGetCurrentViewportImageProc;
	void*									mUserData;
};

//----------------------------------------------------------------------------------------------------------------------
// MARK: - CScenePlayer

class CScene;
class CScenePlayerInternals;
class CScenePlayer {
	// Methods
	public:
											// Lifecycle methods
											CScenePlayer(const CScene& scene,
													const SSceneAppResourceManagementInfo&
															sceneAppResourceManagementInfo,
													const SScenePlayerProcsInfo& scenePlayerProcsInfo);
		virtual								~CScenePlayer();

											// Instance methods
				const	CScene&				getScene() const;
						CActionArray		getAllActions() const;

						void				load();
						void				unload();

						void				start();
						void				reset();
						void				update(UniversalTimeInterval deltaTimeInterval);
						void				render(CGPU& gpu, const S2DPoint32& offset);

						void				setItemPlayerProperty(const CString& itemName, const CString& property,
													const SDictionaryValue& value) const;
						void				handleItemPlayerCommand(const CString& itemName, const CString& command,
													const CDictionary& commandInfo, const S2DPoint32& point) const;

						void				touchBeganOrMouseDownAtPoint(const S2DPoint32& point,
													UInt32 tapOrClickCount, const void* reference);
						void				touchOrMouseMovedFromPoint(const S2DPoint32& point1,
													const S2DPoint32& point2, const void* reference);
						void				touchEndedOrMouseUpAtPoint(const S2DPoint32& point, const void* reference);
						void				touchOrMouseCancelled(const void* reference);

						void				shakeBegan();
						void				shakeEnded();
						void				shakeCancelled();

											// Deprecated
//						CSceneItemPlayer*	getSceneItemPlayerOrNilForTouchOrMouseAtPoint(const S2DPoint32& point)
//													const;
//						CSceneItemPlayer*	getSceneItemPlayerOrNilForItemName(const CString& itemName) const;
//						CSceneItemPlayer*	createAndAddSceneItemPlayerForSceneItem(const CSceneItem& sceneItem,
//													bool makeCopy = false, bool autoLoad = true);
//						void				addSceneItemPlayer(CSceneItemPlayer& sceneItemPlayer, bool autoLoad = true);

	// Properties
	private:
		CScenePlayerInternals*	mInternals;
};

