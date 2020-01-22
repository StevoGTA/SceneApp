//----------------------------------------------------------------------------------------------------------------------
//	CSceneItemPlayerCustom.h			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CSceneItemCustom.h"
#include "CSceneItemPlayer.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CSceneItemPlayerCustom

//class CSceneItemPlayerCustomInternals;
class CSceneItemPlayerCustom : public CSceneItemPlayer {
	// Methods
	public:
												// Lifecycle methods
												CSceneItemPlayerCustom(const CSceneItemCustom& sceneItemCustom,
														const SSceneAppResourceManagementInfo&
																sceneAppResourceManagementInfo,
														const SSceneItemPlayerProcsInfo& sceneItemPlayerProcsInfo);
												~CSceneItemPlayerCustom();

												// CSceneItemPlayer methods
						S2DRect32				getCurrentScreenRect() const;

						CActionArray			getAllActions() const;

						void					load();
						void					finishLoading();
						void					allPeersHaveLoaded();
						void					unload();
						bool					getIsFullyLoaded() const;

						UniversalTimeInterval	getStartTimeInterval() const;

						void					reset();
						void					update(UniversalTimeInterval deltaTimeInterval, bool isRunning);
						void					render(CGPU& gpu, const S2DPoint32& offset);

						bool					handlesTouchOrMouseAtPoint(const S2DPoint32& point);
						void					touchBeganOrMouseDownAtPoint(const S2DPoint32& point,
														UInt32 tapOrClickCount, const void* reference);
						void					touchOrMouseMovedFromPoint(const S2DPoint32& point1,
														const S2DPoint32& point2, const void* reference);
						void					touchEndedOrMouseUpAtPoint(const S2DPoint32& point,
														const void* reference);
						void					touchOrMouseCancelled(const void* reference);

						void					shakeBegan();
						void					shakeEnded();
						void					shakeCancelled();

						void					setProperty(const CString& propertyName, const SDictionaryValue& value);
						bool					handleCommand(const CString& command, const CDictionary& commandInfo,
														const S2DPoint32& point);

												// Instance methods
				const	CSceneItemCustom&		getSceneItemCustom() const
													{ return (CSceneItemCustom&) getSceneItem(); }

	protected:
												// Subclass methods
		virtual			S2DRect32				getCurrentScreenRect(const CSceneItemCustom& sceneItemCustom) const;
		
		virtual			CActionArray			getAllActions(const CSceneItemCustom& sceneItemCustom) const;

		virtual			void					load(const CSceneItemCustom& sceneItemCustom);
		virtual			void					finishLoading(const CSceneItemCustom& sceneItemCustom);
		virtual			void					allPeersHaveLoaded(const CSceneItemCustom& sceneItemCustom);
		virtual			void					unload(const CSceneItemCustom& sceneItemCustom);
		virtual			bool					getIsFullyLoaded(const CSceneItemCustom& sceneItemCustom) const;

		virtual			UniversalTimeInterval	getStartTimeInterval(const CSceneItemCustom& sceneItemCustom) const;

		virtual			void					reset(const CSceneItemCustom& sceneItemCustom);
		virtual			void					update(const CSceneItemCustom& sceneItemCustom,
														UniversalTimeInterval deltaTimeInterval, bool isRunning);
		virtual			void					render(const CSceneItemCustom& sceneItemCustom, CGPU& gpu,
														const S2DPoint32& offset);

		virtual			bool					handlesTouchOrMouseAtPoint(const CSceneItemCustom& sceneItemCustom,
														const S2DPoint32& point);
		virtual			void					touchBeganOrMouseDownAtPoint(const CSceneItemCustom& sceneItemCustom,
														const S2DPoint32& point, UInt32 tapOrClickCount,
														const void* reference);
		virtual			void					touchOrMouseMovedFromPoint(const CSceneItemCustom& sceneItemCustom,
														const S2DPoint32& point1, const S2DPoint32& point2,
														const void* reference);
		virtual			void					touchEndedOrMouseUpAtPoint(const CSceneItemCustom& sceneItemCustom,
														const S2DPoint32& point, const void* reference);
		virtual			void					touchOrMouseCancelled(const CSceneItemCustom& sceneItemCustom,
		const void* reference);

		virtual			void					shakeBegan(const CSceneItemCustom& sceneItemCustom);
		virtual			void					shakeEnded(const CSceneItemCustom& sceneItemCustom);
		virtual			void					shakeCancelled(const CSceneItemCustom& sceneItemCustom);

		virtual			void					setProperty(const CSceneItemCustom& sceneItemCustom,
														const CString& propertyName, const SDictionaryValue& value);
		virtual			bool					handleCommand(const CSceneItemCustom& sceneItemCustom,
														const CString& command, const CDictionary& commandInfo,
														const S2DPoint32& point);

	// Properties
	private:
//		CSceneItemPlayerCustomInternals*	mInternals;
};
