//----------------------------------------------------------------------------------------------------------------------
//	CSceneTransitionPlayerPush.h			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CSceneTransitionPlayer.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CSceneTransitionPlayerPush

class CSceneTransitionPlayerPushInternals;
class CSceneTransitionPlayerPush : public CSceneTransitionPlayer {
	// Methods
	public:
				// Lifecycle methods
				CSceneTransitionPlayerPush(CScenePlayer& currentScenePlayer, CScenePlayer& nextScenePlayer,
						const CDictionary& info, const S2DPoint32& initialTouchOrMousePoint,
						const SSceneTransitionPlayerProcsInfo& sceneTransitionPlayerProcsInfo);
				~CSceneTransitionPlayerPush();

				// CSceneTransitionPlayer methods
		void	update(UniversalTimeInterval deltaTimeInterval);
		void	render(CGPU& gpu);

	// Properties
	public:
		static	CString									mInfoIsAutoKey;		// Presence indicates
		static	CString									mInfoDirectionKey;	// CString
		static	CString									mInfoDurationKey;	// Float32
		
	private:
				CSceneTransitionPlayerPushInternals*	mInternals;
};
