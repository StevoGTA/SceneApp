//----------------------------------------------------------------------------------------------------------------------
//	CSceneAppPlayer.h			©2019 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CGPU.h"
#include "CScenePackage.h"
#include "CScenePlayer.h"
#include "CSceneItemPlayerCustom.h"
#include "CSceneItemPlayer.h"
#include "CSceneTransitionPlayer.h"
#include "CURL.h"

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
	SSceneAppPlayerMouseDownInfo(const S2DPointF32& viewportPoint, UInt32 clickCount) :
		mViewportPoint(viewportPoint), mClickCount(clickCount)
		{}
	SSceneAppPlayerMouseDownInfo(const SSceneAppPlayerMouseDownInfo& other) :
		mViewportPoint(other.mViewportPoint), mClickCount(other.mClickCount)
		{}

	// Properties
	S2DPointF32	mViewportPoint;
	UInt32		mClickCount;
};

struct SSceneAppPlayerMouseDraggedInfo {
	// Lifecycle methods
	SSceneAppPlayerMouseDraggedInfo(const S2DPointF32& viewportPreviousPoint, const S2DPointF32& viewportCurrentPoint) :
		mViewportPreviousPoint(viewportPreviousPoint), mViewportCurrentPoint(viewportCurrentPoint)
		{}
	SSceneAppPlayerMouseDraggedInfo(const SSceneAppPlayerMouseDraggedInfo& other) :
		mViewportPreviousPoint(other.mViewportPreviousPoint), mViewportCurrentPoint(other.mViewportCurrentPoint)
		{}

	// Properties
	S2DPointF32	mViewportPreviousPoint;
	S2DPointF32	mViewportCurrentPoint;
};

struct SSceneAppPlayerMouseUpInfo {
	// Lifecycle methods
	SSceneAppPlayerMouseUpInfo(const S2DPointF32& viewportPoint) : mViewportPoint(viewportPoint) {}
	SSceneAppPlayerMouseUpInfo(const SSceneAppPlayerMouseUpInfo& other) : mViewportPoint(other.mViewportPoint) {}

	// Properties
	S2DPointF32	mViewportPoint;
};

struct SSceneAppPlayerMouseCancelledInfo {};

struct SSceneAppPlayerTouchBeganInfo {
	// Lifecycle methods
	SSceneAppPlayerTouchBeganInfo(const S2DPointF32& viewportPoint, UInt32 tapCount, void* reference = nil) :
		mViewportPoint(viewportPoint), mTapCount(tapCount), mReference(reference)
		{}
	SSceneAppPlayerTouchBeganInfo(const SSceneAppPlayerTouchBeganInfo& other) :
		mViewportPoint(other.mViewportPoint), mTapCount(other.mTapCount), mReference(other.mReference)
		{}

	// Properties
	S2DPointF32	mViewportPoint;
	UInt32		mTapCount;
	void*		mReference;
};

struct SSceneAppPlayerTouchMovedInfo {
	// Lifecycle methods
	SSceneAppPlayerTouchMovedInfo(const S2DPointF32& viewportPreviousPoint, const S2DPointF32& viewportCurrentPoint,
			void* reference = nil) :
		mViewportPreviousPoint(viewportPreviousPoint), mViewportCurrentPoint(viewportCurrentPoint),
				mReference(reference)
		{}
	SSceneAppPlayerTouchMovedInfo(const SSceneAppPlayerTouchMovedInfo& other) :
		mViewportPreviousPoint(other.mViewportPreviousPoint), mViewportCurrentPoint(other.mViewportCurrentPoint),
				mReference(other.mReference)
		{}

	// Properties
	S2DPointF32	mViewportPreviousPoint;
	S2DPointF32	mViewportCurrentPoint;
	void*		mReference;
};

struct SSceneAppPlayerTouchEndedInfo {
	// Lifecycle methods
	SSceneAppPlayerTouchEndedInfo(const S2DPointF32& viewportPoint, void* reference = nil) :
		mViewportPoint(viewportPoint), mReference(reference)
		{}
	SSceneAppPlayerTouchEndedInfo(const SSceneAppPlayerTouchEndedInfo& other) :
		mViewportPoint(other.mViewportPoint), mReference(other.mReference)
		{}

	// Properties
	S2DPointF32	mViewportPoint;
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

typedef	CByteParceller	(*CSceneAppPlayerCreateByteParcellerProc)(const CString& resourceFilename, void* userData);
typedef	void			(*CSceneAppPlayerInstallPeriodicProc)(void* userData);
typedef	void			(*CSceneAppPlayerRemovePeriodicProc)(void* userData);
typedef	void			(*CSceneAppPlayerOpenURLProc)(const CURL& url, bool useWebView, void* userData);
typedef	void			(*CSceneAppPlayerHandleCommandProc)(const CString& command, const CDictionary& commandInfo,
								void* userData);

struct SSceneAppPlayerProcsInfo {
					// Lifecycle methods
					SSceneAppPlayerProcsInfo(CSceneAppPlayerCreateByteParcellerProc createByteParcellerProc,
							CSceneAppPlayerInstallPeriodicProc installPeriodicProc,
							CSceneAppPlayerRemovePeriodicProc removePeriodicProc,
							CSceneAppPlayerOpenURLProc openURLProc,
							CSceneAppPlayerHandleCommandProc handleCommandProc, void* userData) :
						mCreateByteParcellerProc(createByteParcellerProc), mInstallPeriodicProc(installPeriodicProc),
								mRemovePeriodicProc(removePeriodicProc), mOpenURLProc(openURLProc),
								mHandleCommandProc(handleCommandProc), mUserData(userData)
						{}

					// Instance methods
	CByteParceller	createByteParceller(const CString& resourceFilename) const
						{ return mCreateByteParcellerProc(resourceFilename, mUserData); }
	void			installPeriodic() const
						{ mInstallPeriodicProc(mUserData); }
	void			removePeriodic() const
						{ mRemovePeriodicProc(mUserData); }
	void			openURL(const CURL& url, bool useWebView) const
						{ mOpenURLProc(url, useWebView, mUserData); }
	void			handleCommand(const CString& command, const CDictionary& commandInfo) const
						{ if (mHandleCommandProc != nil) mHandleCommandProc(command, commandInfo, mUserData); }

	// Properties
	public:
		CSceneAppPlayerCreateByteParcellerProc	mCreateByteParcellerProc;

	private:
		CSceneAppPlayerInstallPeriodicProc		mInstallPeriodicProc;
		CSceneAppPlayerRemovePeriodicProc		mRemovePeriodicProc;
		CSceneAppPlayerOpenURLProc				mOpenURLProc;
		CSceneAppPlayerHandleCommandProc		mHandleCommandProc;
		void*									mUserData;
};

//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneAppPlayer

class CSceneAppPlayerInternals;
class CSceneAppPlayer {
	// Methods
	public:
															// Lifecycle methods
															CSceneAppPlayer(CGPU& gpu,
																	const SSceneAppPlayerProcsInfo&
																			sceneAppPlayerProcsInfo);
		virtual												~CSceneAppPlayer();

															// Instance methods
						ESceneAppPlayerOptions				getOptions() const;
						void								setOptions(ESceneAppPlayerOptions options);

						void								loadScenes(const SScenePackageInfo& scenePackageInfo);

						void								start(bool loadAllTextures = false);
						void								stop(bool unloadAllTextures = false);
						void								handlePeriodic(UniversalTime outputTime);

						void								mouseDown(
																	const SSceneAppPlayerMouseDownInfo& mouseDownInfo);
						void								mouseDragged(
																	const SSceneAppPlayerMouseDraggedInfo&
																			mouseDraggedInfo);
						void								mouseUp(const SSceneAppPlayerMouseUpInfo& mouseUpInfo);
						void								mouseCancelled(
																	const SSceneAppPlayerMouseCancelledInfo&
																			mouseCancelledInfo);

						void								touchesBegan(
																	const TArray<SSceneAppPlayerTouchBeganInfo>&
																			touchBeganInfosArray);
						void								touchesMoved(
																	const TArray<SSceneAppPlayerTouchMovedInfo>&
																			touchMovedInfosArray);
						void								touchesEnded(
																	const TArray<SSceneAppPlayerTouchEndedInfo>&
																			touchEndedInfosArray);
						void								touchesCancelled(
																	const TArray<SSceneAppPlayerTouchCancelledInfo>&
																			touchCancelledInfosArray);

						void								shakeBegan();
						void								shakeEnded();
						void								shakeCancelled();

															// Subclass methods
		virtual			CSceneItemPlayer*					createSceneItemPlayer(
																	const CSceneItemCustom& sceneItemCustom,
																	const SSceneAppResourceManagementInfo&
																			sceneAppResourceManagementInfo,
																	const SSceneItemPlayerProcsInfo&
																			sceneItemPlayerProcsInfo) const;
		virtual			void								performAction(const CAction& action,
																	const S2DPointF32& point = S2DPointF32());
		virtual			OV<CSceneIndex>						getSceneIndex(const CAction& action) const;

						CScenePlayer&						loadAndStartScenePlayer(CSceneIndex sceneIndex) const;
						void								setCurrent(CSceneTransitionPlayer* sceneTransitionPlayer,
																	CSceneIndex sceneIndex);

	protected:
				const	SSceneAppResourceManagementInfo&	getSceneAppResourceManagementInfo() const;
				const	SSceneTransitionPlayerProcsInfo&	getSceneTransitionPlayerProcsInfo() const;
						CScenePlayer&						getCurrentScenePlayer() const;

	// Properties
	private:
		CSceneAppPlayerInternals*	mInternals;
};
