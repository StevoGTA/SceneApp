//----------------------------------------------------------------------------------------------------------------------
//	CSceneItemPlayer.h			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CAction.h"
#include "CGPURenderObject.h"
#include "CSceneItem.h"
#include "SSceneAppResourceManagementInfo.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CSceneItemPlayer

class CSceneItemPlayerInternals;
class CSceneItemPlayer {
	// Structs
	public:
		struct Procs {
			// Procs
			typedef	void	(*PerformActionsProc)(const CActions& actions, const S2DPointF32& point, void* userData);

					// Lifecycle methods
					Procs(PerformActionsProc performActionsProc, void* userData) :
						mPerformActionsProc(performActionsProc), mUserData(userData)
						{}

					// Instance methods
			void	performActions(const CActions& actions, const S2DPointF32& point = S2DPointF32()) const
						{ mPerformActionsProc(actions, point, mUserData); }

			// Properties
			private:
				PerformActionsProc	mPerformActionsProc;
				void*				mUserData;
		};

	// Procs
	public:
		typedef	CSceneItemPlayer*	(*CreateProc)(const CSceneItem& sceneItem,
											const SSceneAppResourceManagementInfo& sceneAppResourceManagementInfo,
											const Procs& procs);

	// Methods
	public:
													// Lifecycle methods
													CSceneItemPlayer(const CSceneItem& sceneItem, const Procs& procs);
		virtual										~CSceneItemPlayer();

													// Instance methods
				const	CSceneItem&					getSceneItem() const;
				
		virtual			bool						getIsVisible() const;
		virtual			void						setIsVisible(bool isVisible);

		virtual			CActions					getAllActions() const;
						void						perform(const CActions& actions,
															const S2DPointF32& point = S2DPointF32());

		virtual			void						load(CGPU& gpu);
		virtual			void						allPeersHaveLoaded();
		virtual			void						unload();

		virtual	const	OV<UniversalTimeInterval>&	getStartTimeInterval() const;

		virtual			void						reset();
		virtual			void						update(UniversalTimeInterval deltaTimeInterval, bool isRunning);
		virtual			void						render(CGPU& gpu, const CGPURenderObject::RenderInfo& renderInfo)
															const = 0;

		virtual			bool						handlesTouchOrMouseAtPoint(const S2DPointF32& point) const;
		virtual			void						touchBeganOrMouseDownAtPoint(const S2DPointF32& point,
															UInt32 tapOrClickCount, const void* reference);
		virtual			void						touchOrMouseMovedFromPoint(const S2DPointF32& point1,
															const S2DPointF32& point2, const void* reference);
		virtual			void						touchEndedOrMouseUpAtPoint(const S2DPointF32& point,
															const void* reference);
		virtual			void						touchOrMouseCancelled(const void* reference);

		virtual			void						shakeBegan();
		virtual			void						shakeEnded();
		virtual			void						shakeCancelled();

		virtual			void						setProperty(const CString& property, const SValue& value);
		virtual			bool						handleCommand(CGPU& gpu, const CString& command,
															const CDictionary& commandInfo, const S2DPointF32& point);

													// Subclass methods
						void						setPeerProperty(const CString& sceneName, const CString& name,
															const CString& property, const SValue& value) const;
						void						setPeerProperty(const CString& name, const CString& property,
															const SValue& value) const;
						void						sendPeerCommand(const CString& name, const CString& command) const;

	// Properties
	public:
		static	CString						mIsVisiblePropertyName;		// bool or 1

		static	CString						mCommandNameLoad;
		static	CString						mCommandNameUnload;
		static	CString						mCommandNameReset;

	private:
				CSceneItemPlayerInternals*	mInternals;
};
