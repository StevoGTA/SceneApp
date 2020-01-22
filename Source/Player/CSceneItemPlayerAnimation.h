//----------------------------------------------------------------------------------------------------------------------
//	CSceneItemPlayerAnimation.h			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CSceneItemAnimation.h"
#include "CSceneItemPlayer.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: SSceneItemPlayerAnimationProcsInfo

class CSceneItemPlayerAnimation;
typedef	bool			(*SceneItemPlayerAnimationShouldLoopProc)(CSceneItemPlayerAnimation& sceneItemPlayerAnimation,
								UInt32 currentLoopCount, void* userData);

struct SSceneItemPlayerAnimationProcsInfo {
	// Lifecycle methods
	SSceneItemPlayerAnimationProcsInfo(SceneItemPlayerAnimationShouldLoopProc shouldLoopProc, void* userData) :
		mShouldLoopProc(shouldLoopProc), mUserData(userData)
		{}

	// Properties
	SceneItemPlayerAnimationShouldLoopProc	mShouldLoopProc;
	void*									mUserData;
};

//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneItemPlayerAnimation

class CSceneItemPlayerAnimationInternals;
class CSceneItemPlayerAnimation : public CSceneItemPlayer {
	// Methods
	public:
										// Lifecycle methods
										CSceneItemPlayerAnimation(const CSceneItemAnimation& sceneItemAnimation,
												const SSceneAppResourceManagementInfo& sceneAppResourceManagementInfo,
												const SSceneItemPlayerProcsInfo& sceneItemPlayerProcsInfo);
										~CSceneItemPlayerAnimation();

										// CSceneItemPlayer methods
				S2DRect32				getCurrentScreenRect() const;

				CActionArray			getAllActions() const;

				void					load();
				void					finishLoading();
				void					unload();
				bool					getIsFullyLoaded() const;

				UniversalTimeInterval	getStartTimeInterval() const;

				void					reset();
				void					update(UniversalTimeInterval deltaTimeInterval, bool isRunning);
				void					render(CGPU& gpu, const S2DPoint32& offset);

										// Instance methods
		const	CSceneItemAnimation&	getSceneItemAnimation() const
											{ return (const CSceneItemAnimation&) getSceneItem(); }

				bool					getIsFinished() const;

				void					setAudioGain(Float32 gain);
				void					resetAudioGain();

				void					setCommonTexturesSceneItemPlayerAnimation(
												const CSceneItemPlayerAnimation& commonTexturesSceneItemPlayerAnimation);

				void					setSceneItemPlayerAnimationProcsInfo(
												const SSceneItemPlayerAnimationProcsInfo& sceneItemPlayerAnimationProcsInfo);

	// Properties
	private:
		CSceneItemPlayerAnimationInternals*	mInternals;
};
