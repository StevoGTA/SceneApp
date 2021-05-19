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
// MARK: CSceneAppPlayer

class CSceneAppPlayerInternals;
class CSceneAppPlayer {
	// Enums
	public:
		enum Options {

			//------------------------------------------------------------------------------------------------------------------
			// Options are loaded from the scenes file, but can be set interactively as well

			//------------------------------------------------------------------------------------------------------------------
			// Default
			kOptionsDefault					= 0,

			//------------------------------------------------------------------------------------------------------------------
			// Touch handling
			// Use Multiple Individual Touches for cases where you want to be able to track touches on multiple scene items
			//	simultaneously
			// Use Last Single Touch Only for cases where you...
			//	a) only want a single touch => set the view to only accept single touches
			//	b) want to track the last single touch => set the view to accept multiple touches
			kOptionsMultipleIndividualTouches	= 0,
			kOptionsLastSingleTouchOnly			= 1 << 0,
			kOptionsTouchHandlingMask			= 1 << 0,

		//	//------------------------------------------------------------------------------------------------------------------
		//	// MARK: Graphics
		//	kOptionsMapRGB888ToRGBA8888			= 0,
		//	kOptionsMapRGB888ToRGB565			= 1 << 8,
		};

	// Structs
	public:
		struct MouseDownInfo {
			// Lifecycle methods
			MouseDownInfo(const S2DPointF32& viewportPoint, UInt32 clickCount) :
				mViewportPoint(viewportPoint), mClickCount(clickCount)
				{}
			MouseDownInfo(const MouseDownInfo& other) :
				mViewportPoint(other.mViewportPoint), mClickCount(other.mClickCount)
				{}

			// Properties
			S2DPointF32	mViewportPoint;
			UInt32		mClickCount;
		};

		struct MouseDraggedInfo {
			// Lifecycle methods
			MouseDraggedInfo(const S2DPointF32& viewportPreviousPoint, const S2DPointF32& viewportCurrentPoint) :
				mViewportPreviousPoint(viewportPreviousPoint), mViewportCurrentPoint(viewportCurrentPoint)
				{}
			MouseDraggedInfo(const MouseDraggedInfo& other) :
				mViewportPreviousPoint(other.mViewportPreviousPoint), mViewportCurrentPoint(other.mViewportCurrentPoint)
				{}

			// Properties
			S2DPointF32	mViewportPreviousPoint;
			S2DPointF32	mViewportCurrentPoint;
		};

		struct MouseUpInfo {
			// Lifecycle methods
			MouseUpInfo(const S2DPointF32& viewportPoint) : mViewportPoint(viewportPoint) {}
			MouseUpInfo(const MouseUpInfo& other) : mViewportPoint(other.mViewportPoint) {}

			// Properties
			S2DPointF32	mViewportPoint;
		};

		struct MouseCancelledInfo {};

		struct TouchBeganInfo {
			// Lifecycle methods
			TouchBeganInfo(const S2DPointF32& viewportPoint, UInt32 tapCount, void* reference = nil) :
				mViewportPoint(viewportPoint), mTapCount(tapCount), mReference(reference)
				{}
			TouchBeganInfo(const TouchBeganInfo& other) :
				mViewportPoint(other.mViewportPoint), mTapCount(other.mTapCount), mReference(other.mReference)
				{}

			// Properties
			S2DPointF32	mViewportPoint;
			UInt32		mTapCount;
			void*		mReference;
		};

		struct TouchMovedInfo {
			// Lifecycle methods
			TouchMovedInfo(const S2DPointF32& viewportPreviousPoint, const S2DPointF32& viewportCurrentPoint,
					void* reference = nil) :
				mViewportPreviousPoint(viewportPreviousPoint), mViewportCurrentPoint(viewportCurrentPoint),
						mReference(reference)
				{}
			TouchMovedInfo(const TouchMovedInfo& other) :
				mViewportPreviousPoint(other.mViewportPreviousPoint), mViewportCurrentPoint(other.mViewportCurrentPoint),
						mReference(other.mReference)
				{}

			// Properties
			S2DPointF32	mViewportPreviousPoint;
			S2DPointF32	mViewportCurrentPoint;
			void*		mReference;
		};

		struct TouchEndedInfo {
			// Lifecycle methods
			TouchEndedInfo(const S2DPointF32& viewportPoint, void* reference = nil) :
				mViewportPoint(viewportPoint), mReference(reference)
				{}
			TouchEndedInfo(const TouchEndedInfo& other) :
				mViewportPoint(other.mViewportPoint), mReference(other.mReference)
				{}

			// Properties
			S2DPointF32	mViewportPoint;
			void*		mReference;
		};

		struct TouchCancelledInfo {
			// Lifecycle methods
			TouchCancelledInfo(void* reference = nil) : mReference(reference) {}
			TouchCancelledInfo(const TouchCancelledInfo& other) : mReference(other.mReference) {}

			// Properties
			void*	mReference;
		};

		struct Procs {
			// Procs
			typedef	I<CDataSource>	(*CreateDataSourceProc)(const CString& resourceFilename, void* userData);
			typedef	void			(*InstallPeriodicProc)(void* userData);
			typedef	void			(*RemovePeriodicProc)(void* userData);
			typedef	void			(*OpenURLProc)(const CURL& url, bool useWebView, void* userData);
			typedef	void			(*HandleCommandProc)(const CString& command, const CDictionary& commandInfo,
											void* userData);

							// Lifecycle methods
							Procs(CreateDataSourceProc createDataSourceProc,
									InstallPeriodicProc installPeriodicProc, RemovePeriodicProc removePeriodicProc,
									OpenURLProc openURLProc, HandleCommandProc handleCommandProc, void* userData) :
								mCreateDataSourceProc(createDataSourceProc),
										mInstallPeriodicProc(installPeriodicProc),
										mRemovePeriodicProc(removePeriodicProc), mOpenURLProc(openURLProc),
										mHandleCommandProc(handleCommandProc), mUserData(userData)
								{}

							// Instance methods
			I<CDataSource>	createDataSource(const CString& resourceFilename) const
								{ return mCreateDataSourceProc(resourceFilename, mUserData); }
			void			installPeriodic() const
								{ mInstallPeriodicProc(mUserData); }
			void			removePeriodic() const
								{ mRemovePeriodicProc(mUserData); }
			void			openURL(const CURL& url, bool useWebView) const
								{ mOpenURLProc(url, useWebView, mUserData); }
			void			handleCommand(const CString& command, const CDictionary& commandInfo) const
								{ if (mHandleCommandProc != nil) mHandleCommandProc(command, commandInfo, mUserData); }

			// Properties
			private:
				CreateDataSourceProc	mCreateDataSourceProc;
				InstallPeriodicProc		mInstallPeriodicProc;
				RemovePeriodicProc		mRemovePeriodicProc;
				OpenURLProc				mOpenURLProc;
				HandleCommandProc		mHandleCommandProc;
				void*					mUserData;
		};

	// Methods
	public:
															// Lifecycle methods
															CSceneAppPlayer(CGPU& gpu, const Procs& procs);
		virtual												~CSceneAppPlayer();

															// Instance methods
						Options								getOptions() const;
						void								setOptions(Options options);

						void								loadScenes(const CScenePackage::Info& scenePackageInfo);

						void								start(bool loadAllTextures = false);
						void								stop(bool unloadAllTextures = false);
						void								handlePeriodic(UniversalTimeInterval outputTime);

						void								mouseDown(const MouseDownInfo& mouseDownInfo);
						void								mouseDragged(const MouseDraggedInfo& mouseDraggedInfo);
						void								mouseUp(const MouseUpInfo& mouseUpInfo);
						void								mouseCancelled(
																	const MouseCancelledInfo& mouseCancelledInfo);

						void								touchesBegan(
																	const TArray<TouchBeganInfo>& touchBeganInfosArray);
						void								touchesMoved(
																	const TArray<TouchMovedInfo>& touchMovedInfosArray);
						void								touchesEnded(
																	const TArray<TouchEndedInfo>& touchEndedInfosArray);
						void								touchesCancelled(
																	const TArray<TouchCancelledInfo>&
																			touchCancelledInfosArray);

						void								shakeBegan();
						void								shakeEnded();
						void								shakeCancelled();

															// Subclass methods
		virtual			CSceneItemPlayer*					createSceneItemPlayer(
																	const CSceneItemCustom& sceneItemCustom,
																	const SSceneAppResourceManagementInfo&
																			sceneAppResourceManagementInfo,
																	const CSceneItemPlayer::Procs& procs) const;
		virtual			void								performAction(const CAction& action,
																	const S2DPointF32& point = S2DPointF32());
		virtual			OV<CScene::Index>					getSceneIndex(const CAction& action) const;

						CScenePlayer&						loadAndStartScenePlayer(CScene::Index sceneIndex) const;
						void								setCurrent(CSceneTransitionPlayer* sceneTransitionPlayer,
																	CScene::Index sceneIndex);

	protected:
				const	SSceneAppResourceManagementInfo&	getSceneAppResourceManagementInfo() const;
				const	CSceneTransitionPlayer::Procs&		getSceneTransitionPlayerProcs() const;
						CScenePlayer&						getCurrentScenePlayer() const;

	// Properties
	private:
		CSceneAppPlayerInternals*	mInternals;
};
