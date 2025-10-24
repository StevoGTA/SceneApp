//----------------------------------------------------------------------------------------------------------------------
//	CSceneTransitionPlayerPush.h			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CSceneTransitionPlayer.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CSceneTransitionPlayerPush

class CSceneTransitionPlayerPush : public CSceneTransitionPlayer {
	// Classes
	private:
		class Internals;

	// Methods
	public:
				// Lifecycle methods
				CSceneTransitionPlayerPush(CScenePlayer& currentScenePlayer, CScenePlayer& nextScenePlayer,
						const CDictionary& info, const S2DPointF32& initialTouchOrMousePoint, const Procs& procs);
				~CSceneTransitionPlayerPush();

				// CSceneTransitionPlayer methods
		void	update(CGPU& gput, UniversalTimeInterval deltaTimeInterval);
		void	render(CGPU& gpu) const;

	// Properties
	public:
		static	CString		mInfoIsAutoKey;		// Presence indicates
		static	CString		mInfoDirectionKey;	// CString
		static	CString		mInfoDurationKey;	// Float32
		
	private:
				Internals*	mInternals;
};
