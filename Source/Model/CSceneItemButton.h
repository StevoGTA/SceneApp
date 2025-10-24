//----------------------------------------------------------------------------------------------------------------------
//	CSceneItemButton.h			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CAction.h"
#include "CAudioInfo.h"
#include "CKeyframeAnimationInfo.h"
#include "CSceneItem.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CSceneItemButton

class CSceneItemButton : public CSceneItem {
	// Classes
	private:
		class Internals;

	// Methods
	public:
											// Lifecycle methods
											CSceneItemButton(const CDictionary& info);
											CSceneItemButton(const CSceneItemButton& other);
											~CSceneItemButton();

											// CSceneItem methods
		const	CString&					getType() const
												{ return mType; }
				TMArray<CDictionary>		getProperties() const;
				CDictionary					getInfo() const;

											// Instance methods
		const	OV<CActions>&				getActions() const;
				void						setActions(const OV<CActions>& actions);

		const	OV<CAudioInfo>&				getAudioInfo() const;
				void						setAudioInfo(const OV<CAudioInfo>& audioInfo);

		const	OV<CKeyframeAnimationInfo>&	getUpKeyframeAnimationInfo() const;
				void						setUpKeyframeAnimationInfo(
													const OV<CKeyframeAnimationInfo>& keyframeAnimationInfo);

		const	OV<CKeyframeAnimationInfo>&	getDownKeyframeAnimationInfo() const;
				void						setDownKeyframeAnimationInfo(
													const OV<CKeyframeAnimationInfo>& keyframeAnimationInfo);

		const	OV<CKeyframeAnimationInfo>&	getDisabledKeyframeAnimationInfo() const;
				void						setDisabledKeyframeAnimationInfo(
													const OV<CKeyframeAnimationInfo>& keyframeAnimationInfo);

		const	OV<UniversalTimeInterval>&	getStartTimeInterval() const;
				void						setStartTimeInterval(
													const OV<UniversalTimeInterval>& startTimeInterval);

				Float32						getHitRadius() const;
				void						setHitRadius(Float32 hitRadius);

	// Properties
	public:
		static	const	CString		mType;

		static	const	CString		mPropertyNameActions;
		static	const	CString		mPropertyNameAudio;
		static	const	CString		mPropertyNameUpAnimation;
		static	const	CString		mPropertyNameDownAnimation;
		static	const	CString		mPropertyNameDisabledAnimation;
		static	const	CString		mPropertyNameStartTime;
		static	const	CString		mPropertyNameHitRadius;

	private:
						Internals*	mInternals;
};
