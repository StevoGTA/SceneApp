//----------------------------------------------------------------------------------------------------------------------
//	CSceneItemAnimation.h			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CAction.h"
#include "CAudioInfo.h"
#include "CKeyframeAnimationInfo.h"
#include "CSceneItem.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: Trigger types

enum ESceneItemAnimationTriggerType {
	// Animation will start automatically at the time indicated by mStartTimeInterval.
	kSceneItemAnimationTriggerTypeTimed,
};

//----------------------------------------------------------------------------------------------------------------------
// MARK: - Loop Count values

// 0: Loop Forever
// 1: Play through once and stop
// 2 - 0xFFFFFFFE: Loop that many times and then stop
// 0xFFFFFFFF: Hold last frame
const	UInt32	kSceneItemAnimationLoopForever = 0;
const	UInt32	kSceneItemAnimationLoopHoldLastFrame = ~0;

//----------------------------------------------------------------------------------------------------------------------
// MARK: - Property info

enum {
	kSceneItemAnimationPropertyTypeAudioInfo	= MAKE_OSTYPE('A', 'u', 'I', 'n'),
	kSceneItemAnimationPropertyTypeLoopCount	= MAKE_OSTYPE('L', 'p', 'C', 't'),
};

//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneItemAnimation

class CCelAnimationInfo;
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
				OR<CActionArray>			getStartedActionArray() const;
				void						setStartedActionArray(const CActionArray& actionArray);

				OR<CActionArray>			getFinishedActionArray() const;
				void						setFinishedActionArray(const CActionArray& actionArray);

				OR<CAudioInfo>				getAudioInfo() const;
				void						setAudioInfo(const CAudioInfo& audioInfo);

				OR<CCelAnimationInfo>		getCelAnimationInfo() const;
				void						setCelAnimationInfo(const CCelAnimationInfo& celAnimationInfo);

				OR<CKeyframeAnimationInfo>	getKeyframeAnimationInfo() const;
				void						setKeyframeAnimationInfo(
													const CKeyframeAnimationInfo& keyframeAnimationInfo);

				UInt32						getLoopCount() const;
				void						setLoopCount(UInt32 loopCount);

				UniversalTimeInterval		getStartTimeInterval() const;
				void						setStartTimeInterval(UniversalTimeInterval startTimeInterval);

	// Properties
	public:
		static	CString							mType;

	private:
				CSceneItemAnimationInternals*	mInternals;
};
