//----------------------------------------------------------------------------------------------------------------------
//	CKeyframeAnimationPlayer.h			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CGPURenderObject.h"
#include "CKeyframeAnimationInfo.h"
#include "SSceneAppResourceManagementInfo.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CKeyframeAnimationPlayer

class CKeyframeAnimationPlayerInternals;
class CKeyframeAnimationPlayer {
	// Structs
	public:
		struct Procs {
			// Procs
			typedef	bool	(*ShouldLoopProc)(CKeyframeAnimationPlayer& keyframeAnimationPlayer,
									UInt32 currentLoopCount, void* userData);
			typedef	void	(*DidLoopProc)(CKeyframeAnimationPlayer& keyframeAnimationPlayer, void* userData);
			typedef	void	(*DidFinishProc)(CKeyframeAnimationPlayer& keyframeAnimationPlayer, void* userData);
			typedef	void	(*PerformActionsProc)(CKeyframeAnimationPlayer& keyframeAnimationPlayer,
									const CActions& actions, void* userData);

					// Lifecycle methods
					Procs(ShouldLoopProc shouldLoopProc, DidLoopProc didLoopProc, DidFinishProc didFinishProc,
							PerformActionsProc performActionsProc, void* userData) :
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
						{ if (mPerformActionsProc != nil)
								mPerformActionsProc(keyframeAnimationPlayer, actions, mUserData); }

			// Properties
			private:
				ShouldLoopProc		mShouldLoopProc;
				DidLoopProc			mDidLoopProc;
				DidFinishProc		mDidFinishProc;
				PerformActionsProc	mPerformActionsProc;
				void*				mUserData;
		};

	// Methods
	public:
					// Lifecycle methods
					CKeyframeAnimationPlayer(const CKeyframeAnimationInfo& keyframeAnimationInfo,
							const SSceneAppResourceManagementInfo& sceneAppResourceManagementInfo, const Procs& procs,
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
		void		render(CGPU& gpu, const CGPURenderObject::RenderInfo& renderInfo) const;
		bool		getIsFinished() const;

	// Properties
	private:
		CKeyframeAnimationPlayerInternals*	mInternals;
};
