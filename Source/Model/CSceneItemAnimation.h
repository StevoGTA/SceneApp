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
		const	OI<CActions>&				getStartedActions() const;
				void						setStartedActions(const OI<CActions>& actions);

		const	OI<CActions>&				getFinishedActions() const;
				void						setFinishedActions(const OI<CActions>& actions);

		const	OI<CAudioInfo>&				getAudioInfo() const;
				void						setAudioInfo(const OI<CAudioInfo>& audioInfo);

//		const	OI<CCelAnimationInfo>&		getCelAnimationInfo() const;
//				void						setCelAnimationInfo(const OI<CCelAnimationInfo>& celAnimationInfo);

		const	OI<CKeyframeAnimationInfo>&	getKeyframeAnimationInfo() const;
				void						setKeyframeAnimationInfo(
													const OI<CKeyframeAnimationInfo>& keyframeAnimationInfo);

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
