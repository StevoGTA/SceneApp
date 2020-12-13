//----------------------------------------------------------------------------------------------------------------------
//	CSceneItemPlayerAnimation.h			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CSceneItemAnimation.h"
#include "CSceneItemPlayer.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CSceneItemPlayerAnimation

class CSceneItemPlayerAnimationInternals;
class CSceneItemPlayerAnimation : public CSceneItemPlayer {
	// Struct
	public:
		struct Procs {
			// Procs
			typedef	bool	(*AnimationShouldLoopProc)(CSceneItemPlayerAnimation& sceneItemPlayerAnimation,
									UInt32 currentLoopCount, void* userData);

					// Lifecycle methods
					Procs(AnimationShouldLoopProc shouldLoopProc, void* userData) :
						mShouldLoopProc(shouldLoopProc), mUserData(userData)
						{}

					// Instance methods
			bool	canPerformShouldLoop() const
						{ return mShouldLoopProc != nil; }
			bool	shouldLoop(CSceneItemPlayerAnimation& sceneItemPlayerAnimation, UInt32 currentLoopCount) const
						{ return mShouldLoopProc(sceneItemPlayerAnimation, currentLoopCount, mUserData); }

			// Properties
			private:
				AnimationShouldLoopProc	mShouldLoopProc;
				void*					mUserData;
		};

	// Methods
	public:
											// Lifecycle methods
											CSceneItemPlayerAnimation(const CSceneItemAnimation& sceneItemAnimation,
													const SSceneAppResourceManagementInfo&
															sceneAppResourceManagementInfo,
													const CSceneItemPlayer::Procs& sceneItemPlayerProcs);
											~CSceneItemPlayerAnimation();

											// CSceneItemPlayer methods
				CActions					getAllActions() const;

				void						load(CGPU& gpu);
				void						unload();

		const	OV<UniversalTimeInterval>&	getStartTimeInterval() const;

				void						reset();
				void						update(UniversalTimeInterval deltaTimeInterval, bool isRunning);
				void						render(CGPU& gpu, const CGPURenderObject::RenderInfo& renderInfo) const;

											// Instance methods
		const	CSceneItemAnimation&		getSceneItemAnimation() const
												{ return (const CSceneItemAnimation&) getSceneItem(); }

				bool						getIsFinished() const;

				void						setAudioGain(Float32 gain);
				void						resetAudioGain();

				void						setCommonTexturesSceneItemPlayerAnimation(
													const CSceneItemPlayerAnimation&
															commonTexturesSceneItemPlayerAnimation);

				void						setSceneItemPlayerAnimationProcs(const Procs& procs);

	// Properties
	private:
		CSceneItemPlayerAnimationInternals*	mInternals;
};
