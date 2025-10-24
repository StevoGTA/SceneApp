//----------------------------------------------------------------------------------------------------------------------
//	CScenePlayer.h			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CScene.h"
#include "CSceneItemPlayer.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CScenePlayer

class CScenePlayer {
	// Procs
	public:
		struct Procs {
			// Procs
			public:
				typedef	S2DSizeF32			(*GetViewportSizeProc)(void* userData);
				typedef	I<CSceneItemPlayer>	(*CreateSceneItemPlayerProc)(const CSceneItem& sceneItem,
													const SSceneAppResourceManagementInfo&
															sceneAppResourceManagementInfo,
													const CSceneItemPlayer::Procs& procs, void* userData);
				typedef	void				(*PerformActionsProc)(const CActions& actions, const S2DPointF32& point,
													void* userData);

			// Methods
			public:
									// Lifecycle methods
									Procs(GetViewportSizeProc getViewportSizeProc,
											CreateSceneItemPlayerProc createSceneItemPlayerProc,
											PerformActionsProc performActionsProc, void* userData) :
										mGetViewportSizeProc(getViewportSizeProc),
												mCreateSceneItemPlayerProc(createSceneItemPlayerProc),
												mPerformActionsProc(performActionsProc), mUserData(userData)
										{}

									// Instance methods
				S2DSizeF32			getViewportSize() const
										{ return mGetViewportSizeProc(mUserData); }
				I<CSceneItemPlayer>	createSceneItemPlayer(const CSceneItem& sceneItem,
											const SSceneAppResourceManagementInfo& sceneAppResourceManagementInfo,
											const CSceneItemPlayer::Procs& procs) const
										{ return mCreateSceneItemPlayerProc(sceneItem, sceneAppResourceManagementInfo,
												procs, mUserData); }
				void				performActions(const CActions& actions, const S2DPointF32& point = S2DPointF32())
											const
										{ mPerformActionsProc(actions, point, mUserData); }

			// Properties
			private:
				GetViewportSizeProc			mGetViewportSizeProc;
				CreateSceneItemPlayerProc	mCreateSceneItemPlayerProc;
				PerformActionsProc			mPerformActionsProc;
				void*						mUserData;
		};

	// Classes
	private:
		class Internals;

	// Methods
	public:
							// Lifecycle methods
							CScenePlayer(const CScene& scene,
									const SSceneAppResourceManagementInfo& sceneAppResourceManagementInfo,
									const Procs& procs);
							CScenePlayer(const CScenePlayer& other);
							~CScenePlayer();

							// Instance methods
		const	CScene&		getScene() const;
				CActions	getAllActions() const;

				void		load(CGPU& gpu);
				void		unload();

				void		start(CGPU& gpu);
				void		reset();
				void		update(CGPU& gpu, UniversalTimeInterval deltaTimeInterval);
				void		render(CGPU& gpu,
									const CGPURenderObject::RenderInfo& renderInfo = CGPURenderObject::RenderInfo())
									const;

				void		setItemPlayerProperty(const CString& itemName, const CString& property, const SValue& value)
									const;
				void		handleItemPlayerCommand(CGPU& gpu, const CString& itemName, const CString& command,
									const CDictionary& commandInfo, const S2DPointF32& point) const;

				void		touchBeganOrMouseDownAtPoint(const S2DPointF32& point, UInt32 tapOrClickCount,
									const void* reference);
				void		touchOrMouseMovedFromPoint(const S2DPointF32& point1, const S2DPointF32& point2,
									const void* reference);
				void		touchEndedOrMouseUpAtPoint(const S2DPointF32& point, const void* reference);
				void		touchOrMouseCancelled(const void* reference);

				void		shakeBegan();
				void		shakeEnded();
				void		shakeCancelled();

	// Properties
	private:
		Internals*	mInternals;
};
