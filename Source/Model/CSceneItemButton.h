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

class CSceneItemButtonInternals;
class CSceneItemButton : public CSceneItem {
	// Methods
	public:
											// Lifecycle methods
											CSceneItemButton();
											CSceneItemButton(const CDictionary& info);
											CSceneItemButton(const CSceneItemButton& other);
											~CSceneItemButton();

											// CSceneItem methods
				CSceneItemButton*			copy() const
												{ return new CSceneItemButton(*this); }

		const	CString&					getType() const
												{ return mType; }
				TArray<CDictionary>			getProperties() const;
				CDictionary					getInfo() const;

											// Instance methods
		const	OI<CActions>&				getActions() const;
				void						setActions(const OI<CActions>& actions);

		const	OI<CAudioInfo>&				getAudioInfo() const;
				void						setAudioInfo(const OI<CAudioInfo>& audioInfo);

		const	OI<CKeyframeAnimationInfo>&	getUpKeyframeAnimationInfo() const;
				void						setUpKeyframeAnimationInfo(
													const OI<CKeyframeAnimationInfo>& keyframeAnimationInfo);

		const	OI<CKeyframeAnimationInfo>&	getDownKeyframeAnimationInfo() const;
				void						setDownKeyframeAnimationInfo(
													const OI<CKeyframeAnimationInfo>& keyframeAnimationInfo);

		const	OI<CKeyframeAnimationInfo>&	getDisabledKeyframeAnimationInfo() const;
				void						setDisabledKeyframeAnimationInfo(
													const OI<CKeyframeAnimationInfo>& keyframeAnimationInfo);

		const	OV<UniversalTimeInterval>&	getStartTimeInterval() const;
				void						setStartTimeInterval(const OV<UniversalTimeInterval>& startTimeInterval);

				Float32						getHitRadius() const;
				void						setHitRadius(Float32 hitRadius);

	// Properties
	public:
		static	CString						mType;

	private:
				CSceneItemButtonInternals*	mInternals;
};
