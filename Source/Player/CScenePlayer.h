//----------------------------------------------------------------------------------------------------------------------
//	CScenePlayer.h			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CScene.h"
#include "CSceneItemPlayer.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: SScenePlayerProcsInfo

class CScenePlayer;
typedef	S2DSize32			(*CScenePlayerGetViewportSizeProc)(void* userData);
typedef	CSceneItemPlayer*	(*CScenePlayerCreateSceneItemPlayerProc)(const CSceneItem& sceneItem,
									const SSceneAppResourceManagementInfo& sceneAppResourceManagementInfo,
									const SSceneItemPlayerProcsInfo& sceneItemPlayerProcsInfo, void* userData);
typedef	void				(*CScenePlayerPerformActionsProc)(const CActions& actions, const S2DPoint32& point,
									void* userData);

struct SScenePlayerProcsInfo {
						// Lifecycle methods
						SScenePlayerProcsInfo(CScenePlayerGetViewportSizeProc getViewportSizeProc,
								CScenePlayerCreateSceneItemPlayerProc createSceneItemPlayerProc,
								CScenePlayerPerformActionsProc performActionsProc, void* userData) :
							mGetViewportSizeProc(getViewportSizeProc),
									mCreateSceneItemPlayerProc(createSceneItemPlayerProc),
									mPerformActionsProc(performActionsProc), mUserData(userData)
							{}

						// Instance methods
	S2DSize32			getViewportSize() const
							{ return mGetViewportSizeProc(mUserData); }
	CSceneItemPlayer*	createSceneItemPlayer(const CSceneItem& sceneItem,
								const SSceneAppResourceManagementInfo& sceneAppResourceManagementInfo,
								const SSceneItemPlayerProcsInfo& sceneItemPlayerProcsInfo) const
							{ return mCreateSceneItemPlayerProc(sceneItem, sceneAppResourceManagementInfo,
									sceneItemPlayerProcsInfo, mUserData); }
	void				performActions(const CActions& actions, const S2DPoint32& point = S2DPoint32()) const
							{ mPerformActionsProc(actions, point, mUserData); }

	// Properties
	private:
		CScenePlayerGetViewportSizeProc			mGetViewportSizeProc;
		CScenePlayerCreateSceneItemPlayerProc	mCreateSceneItemPlayerProc;
		CScenePlayerPerformActionsProc			mPerformActionsProc;
		void*									mUserData;
};

//----------------------------------------------------------------------------------------------------------------------
// MARK: - CScenePlayer

class CScenePlayerInternals;
class CScenePlayer {
	// Methods
	public:
							// Lifecycle methods
							CScenePlayer(const CScene& scene,
									const SSceneAppResourceManagementInfo& sceneAppResourceManagementInfo,
									const SScenePlayerProcsInfo& scenePlayerProcsInfo);
							~CScenePlayer();

							// Instance methods
		const	CScene&		getScene() const;
				CActions	getAllActions() const;

				void		load(CGPU& gpu);
				void		unload();

				void		start();
				void		reset();
				void		update(UniversalTimeInterval deltaTimeInterval);
				void		render(CGPU& gpu,
									const SGPURenderObjectRenderInfo& renderInfo = SGPURenderObjectRenderInfo())
									const;

				void		setItemPlayerProperty(const CString& itemName, const CString& property,
									const SDictionaryValue& value) const;
				void		handleItemPlayerCommand(CGPU& gpu, const CString& itemName, const CString& command,
									const CDictionary& commandInfo, const S2DPoint32& point) const;

				void		touchBeganOrMouseDownAtPoint(const S2DPoint32& point, UInt32 tapOrClickCount,
									const void* reference);
				void		touchOrMouseMovedFromPoint(const S2DPoint32& point1, const S2DPoint32& point2,
									const void* reference);
				void		touchEndedOrMouseUpAtPoint(const S2DPoint32& point, const void* reference);
				void		touchOrMouseCancelled(const void* reference);

				void		shakeBegan();
				void		shakeEnded();
				void		shakeCancelled();

	// Properties
	private:
		CScenePlayerInternals*	mInternals;
};

