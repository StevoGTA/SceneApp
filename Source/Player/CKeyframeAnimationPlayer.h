//----------------------------------------------------------------------------------------------------------------------
//	CKeyframeAnimationPlayer.h			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CKeyframeAnimationInfo.h"
#include "CSceneApp.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: SKeyframeAnimationPlayerProcsInfo

class CKeyframeAnimationPlayer;
typedef	bool	(*KeyframeAnimationPlayerShouldLoopProc)(CKeyframeAnimationPlayer& keyframeAnimationPlayer,
						UInt32 currentLoopCount, void* userData);
typedef	void	(*KeyframeAnimationPlayerLoopingProc)(CKeyframeAnimationPlayer& keyframeAnimationPlayer,
						void* userData);
typedef	void	(*KeyframeAnimationPlayerFinishedProc)(CKeyframeAnimationPlayer& keyframeAnimationPlayer,
						void* userData);
typedef	void	(*KeyframeAnimationPlayerActionArrayHandlerProc)(CKeyframeAnimationPlayer& keyframeAnimationPlayer,
						const CActionArray& actionArray, void* userData);

struct SKeyframeAnimationPlayerProcsInfo {
	// Lifecycle methods
	SKeyframeAnimationPlayerProcsInfo(KeyframeAnimationPlayerShouldLoopProc shouldLoopProc,
			KeyframeAnimationPlayerLoopingProc loopingProc, KeyframeAnimationPlayerFinishedProc finishedProc,
			KeyframeAnimationPlayerActionArrayHandlerProc actionArrayHandlerProc, void* userData) :
		mShouldLoopProc(shouldLoopProc), mLoopingProc(loopingProc), mFinishedProc(finishedProc),
				mActionArrayHandlerProc(actionArrayHandlerProc), mUserData(userData)
		{}

	// Properties
	KeyframeAnimationPlayerShouldLoopProc			mShouldLoopProc;
	KeyframeAnimationPlayerLoopingProc				mLoopingProc;
	KeyframeAnimationPlayerFinishedProc				mFinishedProc;
	KeyframeAnimationPlayerActionArrayHandlerProc	mActionArrayHandlerProc;
	void*											mUserData;
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
										bool makeCopy = false, UniversalTimeInterval startTimeInterval = 0.0);
								~CKeyframeAnimationPlayer();
						
								// Instance methods
				S2DRect32		getScreenRect();

				CActionArray	getAllActions() const;

				void			load(bool start);
				void			finishLoading();
				void			unload();
				bool			getIsFullyLoaded() const;

				void			reset(bool start);
				void			update(UniversalTimeInterval deltaTimeInterval, bool isRunning);
				void			render(CGPU& gpu, const S2DPoint32& offset);
				bool			getIsFinished() const;

	// Properties
	private:
		CKeyframeAnimationPlayerInternals*	mInternals;
};
