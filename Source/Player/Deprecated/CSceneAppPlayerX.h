//----------------------------------------------------------------------------------------------------------------------
//	CSceneAppPlayerX.h			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CAction.h"
#include "CSceneAppX.h"
#include "CSceneTransitionPlayer.h"
#include "CEvent.h"

#include "CScene.h"
#include "CSceneAppPlayer.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: - Touch handling

enum ESceneAppPlayerTouchHandlingOption {
	// Use Last Single Touch Only for cases where you...
	//	only want a single touch.  Set the view to only accept single touches
	//	want to track the last single touch.  Set the view to accept multiple touches
	kSceneAppPlayerTouchHandlingOptionLastSingleTouchOnly,
	
	// Use Multiple Individual Touches for cases where you want to be able to track touches
	// on multiple scene items simultaneously
	kSceneAppPlayerTouchHandlingOptionMultipleIndividualTouches,
};

//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneAppPlayerX

class CSceneAppPlayerInternalsX;
class CSceneAppPlayerX : public CEventHandler {
	// Methods
	public:
									// CEventHandler methods
		virtual	UError				handleEvent(CEvent& event);

									// Instance methods
				S2DPoint32			viewportPointFromScreenPoint(const S2DPoint32& screenPoint) const;
				S2DPoint32			screenPointFromViewportPoint(const S2DPoint32& viewportPoint) const;

				void				setFramesPerSecond(UInt32 framesPerSecond);
				UInt32				getFramesPerSecond() const;
				
				void				start();
				void				pause();
				void				resume();
				void				stop();
				void				updateAndDraw();

		virtual	bool				doesActionHaveALinkedScene(const CAction& action) const;
				CSceneIndex			getLinkedSceneIndexForAction(const CAction& action) const;
		virtual	void				performAction(const CAction& action, const S2DPoint32& point = S2DPoint32::mZero);
				
				CScenePlayer&		getCurrentScenePlayer() const;
				CScenePlayer*		getScenePlayerForSceneName(const CString& sceneName) const;
				CScenePlayer*		getScenePlayerForSceneIndex(CSceneIndex sceneIndex) const;

				void				setTouchHandlingOption(ESceneAppPlayerTouchHandlingOption touchHandlingOption);
				void				touchesBegan(const TPtrArray<SSceneAppTouchBeganInfo*>& touchBeganInfosArray);
				void				touchesMoved(const TPtrArray<SSceneAppTouchMovedInfo*>& touchMovedInfosArray);
				void				touchesEnded(const TPtrArray<SSceneAppTouchEndedInfo*>& touchEndedInfosArray);
				void				touchesCancelled(
											const TPtrArray<SSceneAppTouchCancelledInfo*>& touchCancelledInfosArray);

				void				motionBegan(ESceneAppMotionInputType motionInputType);
				void				motionEnded(ESceneAppMotionInputType motionInputType);
				void				motionCancelled(ESceneAppMotionInputType motionInputType);

//				CImageX				getCurrentViewportImage(bool performRedraw = false) const;

				void				shutdown();

									// Platform specific instance methods
				void				loadScenes(const S2DSize32& viewportPixelSize);

									// Class methods
		static	CSceneAppPlayerX&	shared();
		static	CString				resourceFilenameForViewport(const CString& resourceFilename);

	protected:
									// Lifecycle methods
									CSceneAppPlayerX();
		virtual						~CSceneAppPlayerX();

									// Instance methods
				void				setCurrentTransition(CSceneTransitionPlayer* sceneTransitionPlayer);

	private:
									// Internal methods
				void				loadScenes(const S2DSize32& viewportPixelSize,
											const TPtrArray<CString*>& scenesFilenamesArray);

									// Platform specific internal methods
				void				installPeriodic(void** periodicReference);
				void				removePeriodic(void** periodicReference);

	// Properties
	public:
											// Action name
		static	CString						mActionNameOpenURL;
		static	CString						mActionNamePlayAudio;
		static	CString						mActionNamePlayMovie;
		static	CString						mActionNameSceneCut;
		static	CString						mActionNameScenePush;
		static	CString						mActionNameSceneSlide;
		static	CString						mActionNameSetItemNameValue;
		static	CString						mActionNameSendItemCommand;
		static	CString						mActionNameSendAppCommand;

											// Action Info Dictionary Keys and value types
		static	CString						mActionInfoAudioFilenameKey;		// NSString
		static	CString						mActionInfoCommandKey;				// NSString
		static	CString						mActionInfoControlModeKey;			// NSNumber
		static	CString						mActionInfoItemNameKey;				// NSString
		static	CString						mActionInfoLoadSceneIndexFromKey;	// NSString
		static	CString						mActionInfoMovieFilenameKey;		// NSString
		static	CString						mActionInfoPropertyNameKey;			// NSString
		static	CString						mActionInfoPropertyValueKey;		// NSNumber/NSString
		static	CString						mActionInfoSceneIndexKey;			// NSNumber
		static	CString						mActionInfoSceneNameKey;			// NSString
		static	CString						mActionInfoURLKey;					// NSString
		static	CString						mActionInfoUseWebView;				// anything

	private:
				CSceneAppPlayerInternalsX*	mInternals;
};
