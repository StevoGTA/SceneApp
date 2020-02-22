//----------------------------------------------------------------------------------------------------------------------
//	CSceneItemAnimation.h			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CAction.h"
#include "CAudioInfo.h"
#include "CKeyframeAnimationInfo.h"
#include "CSceneItem.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: Loop Count values

// 0: Loop Forever
// 1 - 0xFFFFFFFE: Loop that many times and then hold last frame
const	UInt32	kSceneItemAnimationLoopForever = 0;

//----------------------------------------------------------------------------------------------------------------------
// MARK: - Property info

enum {
	kSceneItemAnimationPropertyTypeAudioInfo	= MAKE_OSTYPE('A', 'u', 'I', 'n'),
	kSceneItemAnimationPropertyTypeLoopCount	= MAKE_OSTYPE('L', 'p', 'C', 't'),
};

//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneItemAnimation

class CSceneItemAnimationInternals;
class CSceneItemAnimation : public CSceneItem {
	// Methods
	public:
											// Lifecycle methods
											CSceneItemAnimation();
											CSceneItemAnimation(const CDictionary& info);
											CSceneItemAnimation(const CSceneItemAnimation& other);
											~CSceneItemAnimation();

											// CSceneItem methods
				CSceneItem*					copy() const
												{ return new CSceneItemAnimation(*this); }

		const	CString&					getType() const
												{ return mType; }
				TArray<CDictionary>			getProperties() const;
				CDictionary					getInfo() const;

											// Instance methods
		const	OO<CActions>&				getStartedActions() const;
				void						setStartedActions(const OO<CActions>& actions);

		const	OO<CActions>&				getFinishedActions() const;
				void						setFinishedActions(const OO<CActions>& actions);

		const	OO<CAudioInfo>&				getAudioInfo() const;
				void						setAudioInfo(const OO<CAudioInfo>& audioInfo);

//		const	OO<CCelAnimationInfo>&		getCelAnimationInfo() const;
//				void						setCelAnimationInfo(const OO<CCelAnimationInfo>& celAnimationInfo);

		const	OO<CKeyframeAnimationInfo>&	getKeyframeAnimationInfo() const;
				void						setKeyframeAnimationInfo(
													const OO<CKeyframeAnimationInfo>& keyframeAnimationInfo);

				UInt32						getLoopCount() const;
				void						setLoopCount(UInt32 loopCount);

		const	OV<UniversalTimeInterval>&	getStartTimeInterval() const;
				void						setStartTimeInterval(const OV<UniversalTimeInterval>& startTimeInterval);

	// Properties
	public:
		static	CString							mType;

	private:
				CSceneItemAnimationInternals*	mInternals;
};
