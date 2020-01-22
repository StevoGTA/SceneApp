//
//  CSceneAppPlayer.h
//  StevoBrock-Core
//
//  Created by Stevo on 10/4/19.
//

#pragma once

#include "CDictionary.h"
#include "CGPU.h"
#include "CSceneApp.h"
#include "CTimeInfo.h"
#include "CURL.h"
#include "TLockingArray.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: Options

enum ESceneAppPlayerOptions {

	//------------------------------------------------------------------------------------------------------------------
	// Options are loaded from the scenes file, but can be set interactively as well

	//------------------------------------------------------------------------------------------------------------------
	// Default
	kSceneAppPlayerOptionsDefault					= 0,

	//------------------------------------------------------------------------------------------------------------------
	// Touch handling
	// Use Multiple Individual Touches for cases where you want to be able to track touches on multiple scene items
	//	simultaneously
	// Use Last Single Touch Only for cases where you...
	//	a) only want a single touch => set the view to only accept single touches
	//	b) want to track the last single touch => set the view to accept multiple touches
	kSceneAppPlayerOptionsMultipleIndividualTouches	= 0,
	kSceneAppPlayerOptionsLastSingleTouchOnly		= 1 << 0,
	kSceneAppPlayerOptionsTouchHandlingMask			= 1 << 0,

//	//------------------------------------------------------------------------------------------------------------------
//	// MARK: Graphics
//	kSceneAppPlayerOptionsMapRGB888ToRGBA8888		= 0,
//	kSceneAppPlayerOptionsMapRGB888ToRGB565			= 1 << 8,
};

//----------------------------------------------------------------------------------------------------------------------
// MARK: - Input

struct SSceneAppPlayerMouseDownInfo {
	// Lifecycle methods
	SSceneAppPlayerMouseDownInfo(const S2DPoint32& viewportPoint, UInt32 clickCount) :
		mViewportPoint(viewportPoint), mClickCount(clickCount)
		{}
	SSceneAppPlayerMouseDownInfo(const SSceneAppPlayerMouseDownInfo& other) :
		mViewportPoint(other.mViewportPoint), mClickCount(other.mClickCount)
		{}

	// Properties
	S2DPoint32	mViewportPoint;
	UInt32		mClickCount;
};

struct SSceneAppPlayerMouseDraggedInfo {
	// Lifecycle methods
	SSceneAppPlayerMouseDraggedInfo(const S2DPoint32& viewportPreviousPoint, const S2DPoint32& viewportCurrentPoint) :
		mViewportPreviousPoint(viewportPreviousPoint), mViewportCurrentPoint(viewportCurrentPoint)
		{}
	SSceneAppPlayerMouseDraggedInfo(const SSceneAppPlayerMouseDraggedInfo& other) :
		mViewportPreviousPoint(other.mViewportPreviousPoint), mViewportCurrentPoint(other.mViewportCurrentPoint)
		{}

	// Properties
	S2DPoint32	mViewportPreviousPoint;
	S2DPoint32	mViewportCurrentPoint;
};

struct SSceneAppPlayerMouseUpInfo {
	// Lifecycle methods
	SSceneAppPlayerMouseUpInfo(const S2DPoint32& viewportPoint) : mViewportPoint(viewportPoint) {}
	SSceneAppPlayerMouseUpInfo(const SSceneAppPlayerMouseUpInfo& other) : mViewportPoint(other.mViewportPoint) {}

	// Properties
	S2DPoint32	mViewportPoint;
};

struct SSceneAppPlayerMouseCancelledInfo {};

struct SSceneAppPlayerTouchBeganInfo {
	// Lifecycle methods
	SSceneAppPlayerTouchBeganInfo(const S2DPoint32& viewportPoint, UInt32 tapCount, void* reference = nil) :
		mViewportPoint(viewportPoint), mTapCount(tapCount), mReference(reference)
		{}
	SSceneAppPlayerTouchBeganInfo(const SSceneAppPlayerTouchBeganInfo& other) :
		mViewportPoint(other.mViewportPoint), mTapCount(other.mTapCount), mReference(other.mReference)
		{}

	// Properties
	S2DPoint32	mViewportPoint;
	UInt32		mTapCount;
	void*		mReference;
};

struct SSceneAppPlayerTouchMovedInfo {
	// Lifecycle methods
	SSceneAppPlayerTouchMovedInfo(const S2DPoint32& viewportPreviousPoint, const S2DPoint32& viewportCurrentPoint,
			void* reference = nil) :
		mViewportPreviousPoint(viewportPreviousPoint), mViewportCurrentPoint(viewportCurrentPoint),
				mReference(reference)
		{}
	SSceneAppPlayerTouchMovedInfo(const SSceneAppPlayerTouchMovedInfo& other) :
		mViewportPreviousPoint(other.mViewportPreviousPoint), mViewportCurrentPoint(other.mViewportCurrentPoint),
				mReference(other.mReference)
		{}

	// Properties
	S2DPoint32	mViewportPreviousPoint;
	S2DPoint32	mViewportCurrentPoint;
	void*		mReference;
};

struct SSceneAppPlayerTouchEndedInfo {
	// Lifecycle methods
	SSceneAppPlayerTouchEndedInfo(const S2DPoint32& viewportPoint, void* reference = nil) :
		mViewportPoint(viewportPoint), mReference(reference)
		{}
	SSceneAppPlayerTouchEndedInfo(const SSceneAppPlayerTouchEndedInfo& other) :
		mViewportPoint(other.mViewportPoint), mReference(other.mReference)
		{}

	// Properties
	S2DPoint32	mViewportPoint;
	void*		mReference;
};

struct SSceneAppPlayerTouchCancelledInfo {
	// Lifecycle methods
	SSceneAppPlayerTouchCancelledInfo(void* reference = nil) : mReference(reference) {}
	SSceneAppPlayerTouchCancelledInfo(const SSceneAppPlayerTouchCancelledInfo& other) : mReference(other.mReference) {}

	// Properties
	void*	mReference;
};

//----------------------------------------------------------------------------------------------------------------------
// MARK: - SSceneAppPlayerProcsInfo

typedef	CByteParceller	(*SceneAppPlayerCreateByteParcellerProc)(const CString& resourceFilename);
typedef	void			(*SceneAppPlayerInstallPeriodicProc)(void* userData);
typedef	void			(*SceneAppPlayerRemovePeriodicProc)(void* userData);
typedef	void			(*SceneAppPlayerOpenURLProc)(const CURL& URL, bool useWebView, void* userData);
typedef	void			(*SceneAppPlayerHandleCommandProc)(const CString& command, const CDictionary& commandInfo,
								void* userData);
typedef	S2DSize32		(*SceneAppPlayerGetViewportSizeProc)(void* userData);

struct SSceneAppPlayerProcsInfo {
	// Lifecycle methods
	SSceneAppPlayerProcsInfo(SceneAppPlayerCreateByteParcellerProc createByteParcellerProc,
			SceneAppPlayerInstallPeriodicProc installPeriodicProc, SceneAppPlayerRemovePeriodicProc removePeriodicProc,
			SceneAppPlayerOpenURLProc openURLProc, SceneAppPlayerHandleCommandProc handleCommandProc,
			SceneAppPlayerGetViewportSizeProc getViewportSizeProc, void* userData) :
		mCreateByteParcellerProc(createByteParcellerProc), mInstallPeriodicProc(installPeriodicProc),
				mRemovePeriodicProc(removePeriodicProc), mOpenURLProc(openURLProc),
				mHandleCommandProc(handleCommandProc), mGetViewportSizeProc(getViewportSizeProc), mUserData(userData)
		{}

	// Properties
	SceneAppPlayerCreateByteParcellerProc	mCreateByteParcellerProc;
	SceneAppPlayerInstallPeriodicProc		mInstallPeriodicProc;
	SceneAppPlayerRemovePeriodicProc		mRemovePeriodicProc;
	SceneAppPlayerOpenURLProc				mOpenURLProc;
	SceneAppPlayerHandleCommandProc			mHandleCommandProc;
	SceneAppPlayerGetViewportSizeProc		mGetViewportSizeProc;
	void*									mUserData;
};

//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneAppPlayer

class CSceneAppPlayerInternals;
class CSceneAppPlayer {
	// Methods
	public:
								// Lifecycle methods
								CSceneAppPlayer(CGPU& gpu, const SSceneAppPlayerProcsInfo& sceneAppPlayerProcsInfo);
								~CSceneAppPlayer();

								// Instance methods
		ESceneAppPlayerOptions	getOptions() const;
		void					setOptions(ESceneAppPlayerOptions options);

		void					loadScenes(const SScenePackageInfo& scenePackageInfo);

		void					start();
		void					stop();
		void					handlePeriodic(UniversalTime outputTime);

		void					mouseDown(const SSceneAppPlayerMouseDownInfo& mouseDownInfo);
		void					mouseDragged(const SSceneAppPlayerMouseDraggedInfo& mouseDraggedInfo);
		void					mouseUp(const SSceneAppPlayerMouseUpInfo& mouseUpInfo);
		void					mouseCancelled(const SSceneAppPlayerMouseCancelledInfo& mouseCancelledInfo);

		void					touchesBegan(const TArray<SSceneAppPlayerTouchBeganInfo>& touchBeganInfosArray);
		void					touchesMoved(const TArray<SSceneAppPlayerTouchMovedInfo>& touchMovedInfosArray);
		void					touchesEnded(const TArray<SSceneAppPlayerTouchEndedInfo>& touchEndedInfosArray);
		void					touchesCancelled(
										const TArray<SSceneAppPlayerTouchCancelledInfo>& touchCancelledInfosArray);

		void					shakeBegan();
		void					shakeEnded();
		void					shakeCancelled();

	// Properties
	private:
		CSceneAppPlayerInternals*	mInternals;
};
