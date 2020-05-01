//----------------------------------------------------------------------------------------------------------------------
//	CCelAnimationPlayer.h			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CAction.h"
#include "CCelAnimationInfo.h"
#include "CSceneApp.h"

#include "TimeAndDate.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: Did finish action

enum ECelAnimationPlayerDidFinishAction {
	kCelAnimationPlayerDidFinishActionFinish,
	kCelAnimationPlayerDidFinishActionLoop,
	kCelAnimationPlayerDidFinishActionHoldLastFrame,
};

//----------------------------------------------------------------------------------------------------------------------
// MARK: - SCelAnimationPlayerProcsInfo

class CCelAnimationPlayer;
typedef	ECelAnimationPlayerDidFinishAction	(*CCelAnimationPlayerGetDidFinishActionProc)(
													CCelAnimationPlayer& celAnimationPlayer, UInt32 currentLoopCount,
													void* userData);
typedef	void								(*CCelAnimationPlayerDidLoopProc)(CCelAnimationPlayer& celAnimationPlayer,
													void* userData);
typedef	void								(*CCelAnimationPlayerDidFinishProc)(CCelAnimationPlayer& celAnimationPlayer,
													void* userData);
typedef	void								(*CCelAnimationPlayerCurrentFrameIndexChangedProc)(
													CCelAnimationPlayer& celAnimationPlayer,
													UInt32 newCurrentFrameIndex, void* userData);

struct SCelAnimationPlayerProcsInfo {
										// Lifecycle methods
										SCelAnimationPlayerProcsInfo(
												CCelAnimationPlayerGetDidFinishActionProc getDidFinishActionProc,
												CCelAnimationPlayerDidLoopProc didLoopProc,
												CCelAnimationPlayerDidFinishProc didFinishProc,
												CCelAnimationPlayerCurrentFrameIndexChangedProc
														currentFrameIndexChangedProc, void* userData) :
											mGetDidFinishActionProc(getDidFinishActionProc), mDidLoopProc(didLoopProc),
													mDidFinishProc(didFinishProc),
													mCurrentFrameIndexChangedProc(currentFrameIndexChangedProc),
													mUserData(userData)
											{}

										// Instance methods
	ECelAnimationPlayerDidFinishAction	getDidFinishAction(CCelAnimationPlayer& celAnimationPlayer,
												UInt32 currentLoopCount) const
											{ return mGetDidFinishActionProc(celAnimationPlayer, currentLoopCount,
													mUserData); }
	void								didLoop(CCelAnimationPlayer& celAnimationPlayer) const
											{ return mDidLoopProc(celAnimationPlayer, mUserData); }
	void								didFinish(CCelAnimationPlayer& celAnimationPlayer) const
											{ return mDidFinishProc(celAnimationPlayer, mUserData); }
	void								currentFrameIndexChanged(CCelAnimationPlayer& celAnimationPlayer,
												UInt32 newCurrentFrameIndex) const
											{ return mCurrentFrameIndexChangedProc(celAnimationPlayer,
													newCurrentFrameIndex, mUserData); }

	// Properties
	CCelAnimationPlayerGetDidFinishActionProc		mGetDidFinishActionProc;
	CCelAnimationPlayerDidLoopProc					mDidLoopProc;
	CCelAnimationPlayerDidFinishProc				mDidFinishProc;
	CCelAnimationPlayerCurrentFrameIndexChangedProc	mCurrentFrameIndexChangedProc;
	void*											mUserData;
};

//----------------------------------------------------------------------------------------------------------------------
// MARK: - CCelAnimationPlayer

class CCelAnimationPlayerInternals;
class CCelAnimationPlayer {
	// Methods
	public:
									// Lifecycle methods
									CCelAnimationPlayer(CCelAnimationInfo& celAnimationInfo,
											const SSceneAppResourceManagementInfo& sceneAppResourceManagementInfo,
											const SCelAnimationPlayerProcsInfo& celAnimationPlayerProcsInfo,
											bool makeCopy = false, UniversalTimeInterval startTimeInterval = 0.0);
		virtual						~CCelAnimationPlayer();
				
									// Instance methods
				CCelAnimationInfo&	getCelAnimationInfo() const;

				void				load(bool start);
				void				finishLoading();
				void				unload();
//				bool				getIsFullyLoaded() const;

				void				reset(bool start);
				void				update(UniversalTimeInterval deltaTimeInterval, bool isRunning);
				void				render(CGPURenderEngine& gpuRenderEngine, const S2DPoint32& offset);
				void				setCurrentFrameIndex(UInt32 currentFrameIndex);
				void				setCommonTexturesCelAnimationPlayer(
											CCelAnimationPlayer& commonTexturesCelAnimationPlayer);
				bool				getIsFinished() const;

									// Class methods
		static	void				setFramesPerSecond(UInt32 framesPerSecond);

		static	void				startTiming();
		static	void				stopTimingAndLogResults();

	// Properties
	private:
		CCelAnimationPlayerInternals*	mInternals;
};
