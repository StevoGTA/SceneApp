//----------------------------------------------------------------------------------------------------------------------
//	CSceneItemPlayerCustom.h			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CSceneItemCustom.h"
#include "CSceneItemPlayer.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CSceneItemPlayerCustom

class CSceneItemPlayerCustom : public CSceneItemPlayer {
	// Methods
	public:
													// Lifecycle methods
													CSceneItemPlayerCustom(const CSceneItemCustom& sceneItemCustom,
															const SSceneItemPlayerProcsInfo& sceneItemPlayerProcsInfo);

													// CSceneItemPlayer methods
						CActions					getAllActions() const;

						void						load();
						void						allPeersHaveLoaded();
						void						unload();

				const	OV<UniversalTimeInterval>&	getStartTimeInterval() const;

						void						reset();
						void						update(UniversalTimeInterval deltaTimeInterval, bool isRunning);
						void						render(CGPU& gpu, const S2DPoint32& offset = S2DPoint32()) const;

						bool						handlesTouchOrMouseAtPoint(const S2DPoint32& point) const;
						void						touchBeganOrMouseDownAtPoint(const S2DPoint32& point,
															UInt32 tapOrClickCount, const void* reference);
						void						touchOrMouseMovedFromPoint(const S2DPoint32& point1,
															const S2DPoint32& point2, const void* reference);
						void						touchEndedOrMouseUpAtPoint(const S2DPoint32& point,
															const void* reference);
						void						touchOrMouseCancelled(const void* reference);

						void						shakeBegan();
						void						shakeEnded();
						void						shakeCancelled();

						void						setProperty(const CString& propertyName,
															const SDictionaryValue& value);
						bool						handleCommand(const CString& command,
															const CDictionary& commandInfo, const S2DPoint32& point);

													// Instance methods
				const	CSceneItemCustom&			getSceneItemCustom() const
														{ return (CSceneItemCustom&) getSceneItem(); }

	protected:
													// Subclass methods
		virtual			CActions					getAllActions(const CSceneItemCustom& sceneItemCustom) const
														{ return CActions(); }

		virtual			void						load(const CSceneItemCustom& sceneItemCustom) {}
		virtual			void						allPeersHaveLoaded(const CSceneItemCustom& sceneItemCustom) {}
		virtual			void						unload(const CSceneItemCustom& sceneItemCustom) {}

		virtual	const	OV<UniversalTimeInterval>&	getStartTimeInterval(const CSceneItemCustom& sceneItemCustom) const;

		virtual			void						reset(const CSceneItemCustom& sceneItemCustom) {}
		virtual			void						update(const CSceneItemCustom& sceneItemCustom,
															UniversalTimeInterval deltaTimeInterval, bool isRunning) {}
		virtual			void						render(const CSceneItemCustom& sceneItemCustom, CGPU& gpu,
															const S2DPoint32& offset) const = 0;

		virtual			bool						handlesTouchOrMouseAtPoint(const CSceneItemCustom& sceneItemCustom,
															const S2DPoint32& point) const
														{ return false; }
		virtual			void						touchBeganOrMouseDownAtPoint(
															const CSceneItemCustom& sceneItemCustom,
															const S2DPoint32& point, UInt32 tapOrClickCount,
															const void* reference) {}
		virtual			void						touchOrMouseMovedFromPoint(const CSceneItemCustom& sceneItemCustom,
															const S2DPoint32& point1, const S2DPoint32& point2,
															const void* reference) {}
		virtual			void						touchEndedOrMouseUpAtPoint(const CSceneItemCustom& sceneItemCustom,
															const S2DPoint32& point, const void* reference) {}
		virtual			void						touchOrMouseCancelled(const CSceneItemCustom& sceneItemCustom,
															const void* reference) {}

		virtual			void						shakeBegan(const CSceneItemCustom& sceneItemCustom) {}
		virtual			void						shakeEnded(const CSceneItemCustom& sceneItemCustom) {}
		virtual			void						shakeCancelled(const CSceneItemCustom& sceneItemCustom) {}

		virtual			void						setProperty(const CSceneItemCustom& sceneItemCustom,
															const CString& propertyName, const SDictionaryValue& value)
														{}
		virtual			bool						handleCommand(const CSceneItemCustom& sceneItemCustom,
															const CString& command, const CDictionary& commandInfo,
															const S2DPoint32& point)
														{ return false; }
};