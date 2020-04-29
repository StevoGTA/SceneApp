//----------------------------------------------------------------------------------------------------------------------
//	CSceneItemPlayerAnimation.h			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CSceneItemAnimation.h"
#include "CSceneItemPlayer.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: SSceneItemPlayerAnimationProcsInfo

class CSceneItemPlayerAnimation;
typedef	bool			(*CSceneItemPlayerAnimationShouldLoopProc)(CSceneItemPlayerAnimation& sceneItemPlayerAnimation,
								UInt32 currentLoopCount, void* userData);

struct SSceneItemPlayerAnimationProcsInfo {
			// Lifecycle methods
			SSceneItemPlayerAnimationProcsInfo(CSceneItemPlayerAnimationShouldLoopProc shouldLoopProc, void* userData) :
				mShouldLoopProc(shouldLoopProc), mUserData(userData)
				{}

			// Instance methods
	bool	canPerformShouldLoop() const
				{ return mShouldLoopProc != nil; }
	bool	shouldLoop(CSceneItemPlayerAnimation& sceneItemPlayerAnimation, UInt32 currentLoopCount) const
				{ return mShouldLoopProc(sceneItemPlayerAnimation, currentLoopCount, mUserData); }

	// Properties
	private:
		CSceneItemPlayerAnimationShouldLoopProc	mShouldLoopProc;
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
													const SSceneAppResourceManagementInfo&
															sceneAppResourceManagementInfo,
													const SSceneItemPlayerProcsInfo& sceneItemPlayerProcsInfo);
											~CSceneItemPlayerAnimation();

											// CSceneItemPlayer methods
				CActions					getAllActions() const;

				void						load(CGPU& gpu);
				void						unload();

		const	OV<UniversalTimeInterval>&	getStartTimeInterval() const;

				void						reset();
				void						update(UniversalTimeInterval deltaTimeInterval, bool isRunning);
				void						render(CGPU& gpu, const SGPURenderObjectRenderInfo& renderInfo) const;

											// Instance methods
		const	CSceneItemAnimation&		getSceneItemAnimation() const
												{ return (const CSceneItemAnimation&) getSceneItem(); }

				bool						getIsFinished() const;

				void						setAudioGain(Float32 gain);
				void						resetAudioGain();

				void						setCommonTexturesSceneItemPlayerAnimation(
													const CSceneItemPlayerAnimation&
															commonTexturesSceneItemPlayerAnimation);

				void						setSceneItemPlayerAnimationProcsInfo(
													const SSceneItemPlayerAnimationProcsInfo&
															sceneItemPlayerAnimationProcsInfo);

	// Properties
	private:
		CSceneItemPlayerAnimationInternals*	mInternals;
};
