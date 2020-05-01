//----------------------------------------------------------------------------------------------------------------------
//	CKeyframeAnimationPlayer.h			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CGPURenderObject.h"
#include "CKeyframeAnimationInfo.h"
#include "CSceneApp.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: SKeyframeAnimationPlayerProcsInfo

class CKeyframeAnimationPlayer;
typedef	bool	(*CKeyframeAnimationPlayerShouldLoopProc)(CKeyframeAnimationPlayer& keyframeAnimationPlayer,
						UInt32 currentLoopCount, void* userData);
typedef	void	(*CKeyframeAnimationPlayerDidLoopProc)(CKeyframeAnimationPlayer& keyframeAnimationPlayer,
						void* userData);
typedef	void	(*CKeyframeAnimationPlayerDidFinishProc)(CKeyframeAnimationPlayer& keyframeAnimationPlayer,
						void* userData);
typedef	void	(*CKeyframeAnimationPlayerPerformActionsProc)(CKeyframeAnimationPlayer& keyframeAnimationPlayer,
						const CActions& actions, void* userData);

struct SKeyframeAnimationPlayerProcsInfo {
			// Lifecycle methods
			SKeyframeAnimationPlayerProcsInfo(CKeyframeAnimationPlayerShouldLoopProc shouldLoopProc,
					CKeyframeAnimationPlayerDidLoopProc didLoopProc,
					CKeyframeAnimationPlayerDidFinishProc didFinishProc,
					CKeyframeAnimationPlayerPerformActionsProc performActionsProc, void* userData) :
				mShouldLoopProc(shouldLoopProc), mDidLoopProc(didLoopProc), mDidFinishProc(didFinishProc),
						mPerformActionsProc(performActionsProc), mUserData(userData)
				{}

			// Instance methods
	bool	canPerformShouldLoop() const
				{ return mShouldLoopProc != nil; }
	bool	shouldLoop(CKeyframeAnimationPlayer& keyframeAnimationPlayer, UInt32 currentLoopCount) const
				{ return mShouldLoopProc(keyframeAnimationPlayer, currentLoopCount, mUserData); }
	void	didLoop(CKeyframeAnimationPlayer& keyframeAnimationPlayer) const
				{ if (mDidLoopProc != nil) mDidLoopProc(keyframeAnimationPlayer, mUserData); }
	void	didFinish(CKeyframeAnimationPlayer& keyframeAnimationPlayer) const
				{ if (mDidFinishProc != nil) mDidFinishProc(keyframeAnimationPlayer, mUserData); }
	void	performActions(CKeyframeAnimationPlayer& keyframeAnimationPlayer, const CActions& actions) const
				{ if (mPerformActionsProc != nil) mPerformActionsProc(keyframeAnimationPlayer, actions, mUserData); }

	// Properties
	private:
		CKeyframeAnimationPlayerShouldLoopProc		mShouldLoopProc;
		CKeyframeAnimationPlayerDidLoopProc			mDidLoopProc;
		CKeyframeAnimationPlayerDidFinishProc		mDidFinishProc;
		CKeyframeAnimationPlayerPerformActionsProc	mPerformActionsProc;
		void*										mUserData;
};

//----------------------------------------------------------------------------------------------------------------------
// MARK: - CKeyframeAnimationPlayer

class CKeyframeAnimationPlayerInternals;
class CKeyframeAnimationPlayer {
	// Methods
	public:
					// Lifecycle methods
					CKeyframeAnimationPlayer(const CKeyframeAnimationInfo& keyframeAnimationInfo,
							const SSceneAppResourceManagementInfo& sceneAppResourceManagementInfo,
							const SKeyframeAnimationPlayerProcsInfo& keyframeAnimationPlayerProcsInfo,
							const OV<UniversalTimeInterval>& startTimeInterval = OV<UniversalTimeInterval>());
					~CKeyframeAnimationPlayer();

					// Instance methods
		S2DRectF32	getScreenRect();

		CActions	getAllActions() const;

		void		load(CGPU& gpu, bool start);
		void		finishLoading();
		void		unload();

		void		reset(bool start);
		void		update(UniversalTimeInterval deltaTimeInterval, bool isRunning);
		void		render(CGPU& gpu, const SGPURenderObjectRenderInfo& renderInfo) const;
		bool		getIsFinished() const;

	// Properties
	private:
		CKeyframeAnimationPlayerInternals*	mInternals;
};
