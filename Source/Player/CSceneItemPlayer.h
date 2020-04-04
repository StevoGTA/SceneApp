//----------------------------------------------------------------------------------------------------------------------
//	CSceneItemPlayer.h			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CAction.h"
#include "CGPURenderObject.h"
#include "CSceneApp.h"
#include "CSceneItem.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: SSceneItemPlayerProcsInfo

typedef	void	(*CSceneItemPlayerPerformActionsProc)(const CActions& actions, const S2DPoint32& point, void* userData);

struct SSceneItemPlayerProcsInfo {
			// Lifecycle methods
			SSceneItemPlayerProcsInfo(CSceneItemPlayerPerformActionsProc performActionsProc, void* userData) :
				mPerformActionsProc(performActionsProc), mUserData(userData)
				{}

			// Instance methods
	void	performActions(const CActions& actions, const S2DPoint32& point = S2DPoint32()) const
				{ mPerformActionsProc(actions, point, mUserData); }

	// Properties
	private:
		CSceneItemPlayerPerformActionsProc	mPerformActionsProc;
		void*								mUserData;
};

//----------------------------------------------------------------------------------------------------------------------
// MARK: - Procs

class CSceneItemPlayer;
typedef	CSceneItemPlayer*	(*CSceneItemPlayerCreateProc)(const CSceneItem& sceneItem,
									const SSceneAppResourceManagementInfo& sceneAppResourceManagementInfo,
									const SSceneItemPlayerProcsInfo& sceneItemPlayerProcsInfo);

//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneItemPlayer

class CSceneItemPlayerInternals;
class CSceneItemPlayer {
	// Methods
	public:
													// Lifecycle methods
													CSceneItemPlayer(const CSceneItem& sceneItem,
															const SSceneItemPlayerProcsInfo& sceneItemPlayerProcsInfo);
		virtual										~CSceneItemPlayer();

													// Instance methods
				const	CSceneItem&					getSceneItem() const;
				
		virtual			bool						getIsVisible() const;
		virtual			void						setIsVisible(bool isVisible);

		virtual			CActions					getAllActions() const;
						void						perform(const CActions& actions,
															const S2DPoint32& point = S2DPoint32());

		virtual			void						load();
		virtual			void						allPeersHaveLoaded();
		virtual			void						unload();

		virtual	const	OV<UniversalTimeInterval>&	getStartTimeInterval() const;

		virtual			void						reset();
		virtual			void						update(UniversalTimeInterval deltaTimeInterval, bool isRunning);
		virtual			void						render(CGPU& gpu, const SGPURenderObjectRenderInfo& renderInfo)
															const = 0;

		virtual			bool						handlesTouchOrMouseAtPoint(const S2DPoint32& point) const;
		virtual			void						touchBeganOrMouseDownAtPoint(const S2DPoint32& point,
															UInt32 tapOrClickCount, const void* reference);
		virtual			void						touchOrMouseMovedFromPoint(const S2DPoint32& point1,
															const S2DPoint32& point2, const void* reference);
		virtual			void						touchEndedOrMouseUpAtPoint(const S2DPoint32& point,
															const void* reference);
		virtual			void						touchOrMouseCancelled(const void* reference);

		virtual			void						shakeBegan();
		virtual			void						shakeEnded();
		virtual			void						shakeCancelled();

		virtual			void						setProperty(const CString& property, const SDictionaryValue& value);
		virtual			bool						handleCommand(const CString& command,
															const CDictionary& commandInfo, const S2DPoint32& point);

													// Subclass methods
						void						setPeerProperty(const CString& sceneName, const CString& name,
															const CString& property, const SDictionaryValue& value)
															const;
						void						setPeerProperty(const CString& name, const CString& property,
															const SDictionaryValue& value) const;

	// Properties
	public:
		static	CString						mIsVisiblePropertyName;		// CNumber

		static	CString						mCommandNameLoad;
		static	CString						mCommandNameUnload;
		static	CString						mCommandNameReset;

	private:
				CSceneItemPlayerInternals*	mInternals;
};
