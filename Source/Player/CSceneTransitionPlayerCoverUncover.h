//----------------------------------------------------------------------------------------------------------------------
//	CSceneTransitionPlayerCoverUncover.h			©2020 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CSceneTransitionPlayer.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CSceneTransitionPlayerCoverUncover

class CSceneTransitionPlayerCoverUncoverInternals;
class CSceneTransitionPlayerCoverUncover : public CSceneTransitionPlayer {
	// Methods
	public:
				// Lifecycle methods
				CSceneTransitionPlayerCoverUncover(CScenePlayer& currentScenePlayer, CScenePlayer& nextScenePlayer,
						const CDictionary& info, const S2DPointF32& initialTouchOrMousePoint, bool isCover,
						const Procs& procs);
				~CSceneTransitionPlayerCoverUncover();

				// CSceneTransitionPlayer methods
		void	update(UniversalTimeInterval deltaTimeInterval);
		void	render(CGPU& gpu) const;

	// Properties
	public:
		static	CString											mInfoIsAutoKey;		// Presence indicates
		static	CString											mInfoDirectionKey;	// CString
		static	CString											mInfoDurationKey;	// Float32

	private:
				CSceneTransitionPlayerCoverUncoverInternals*	mInternals;
};
