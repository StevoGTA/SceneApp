//----------------------------------------------------------------------------------------------------------------------
//	CSceneAppView.cpp			©2020 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CSceneAppView.h"

#include "C2DGeometry.h"
#include "CDirectXGPU.h"
#include "CFilesystem.h"
#include "CFileDataSource.h"

#undef Delete

#include <ppltasks.h>
#include <wrl.h>

#define Delete(x)	{ delete x; x = nil; }

using namespace concurrency;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::UI::Core;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;

//----------------------------------------------------------------------------------------------------------------------
// MARK: StepTimer

class StepTimer {
	public:
		StepTimer(double targetElapsedSeconds) : 
			mQPCSecondCounter(0),
			mTargetElapsedTicks(SecondsToTicks(targetElapsedSeconds)),
			mElapsedTicks(0),
			mTotalTicks(0),
			mLeftOverTicks(0),
			mFrameCount(0),
			mFPS(0),
			mFramesThisSecond(0)
			{
				if (!QueryPerformanceFrequency(&mQPCFrequency))
					throw ref new Platform::FailureException();

				if (!QueryPerformanceCounter(&mQPCLastTime))
					throw ref new Platform::FailureException();

				// Initialize max delta to 1/10 of a second.
				mQPCMaxDelta = mQPCFrequency.QuadPart / 10;
			}

		// Get total time since the start of the program.
		double GetTotalSeconds() const						{ return TicksToSeconds(mTotalTicks); }

		// Get the current framerate.
		uint32 GetFramesPerSecond() const					{ return mFPS; }

		// Integer format represents time using 10,000,000 ticks per second.
		static const uint64 TicksPerSecond = 10000000;

		static double TicksToSeconds(uint64 ticks)			{ return static_cast<double>(ticks) / TicksPerSecond; }
		static uint64 SecondsToTicks(double seconds)		{ return static_cast<uint64>(seconds * TicksPerSecond); }

		// Update timer state, calling the specified Update function the appropriate number of times.
		void Update() {
			// Query the current time.
			LARGE_INTEGER currentTime;

			if (!QueryPerformanceCounter(&currentTime))
				throw ref new Platform::FailureException();

			uint64 timeDelta = currentTime.QuadPart - mQPCLastTime.QuadPart;

			mQPCLastTime = currentTime;
			mQPCSecondCounter += timeDelta;

			// Clamp excessively large time deltas (e.g. after paused in the debugger).
			if (timeDelta > mQPCMaxDelta)
				timeDelta = mQPCMaxDelta;

			// Convert QPC units into a canonical tick format. This cannot overflow due to the previous clamp.
			timeDelta *= TicksPerSecond;
			timeDelta /= mQPCFrequency.QuadPart;

			uint32 lastFrameCount = mFrameCount;

			// If the app is running very close to the target elapsed time (within 1/4 of a millisecond) just clamp
			// the clock to exactly match the target value. This prevents tiny and irrelevant errors
			// from accumulating over time. Without this clamping, a game that requested a 60 fps
			// fixed update, running with vsync enabled on a 59.94 NTSC display, would eventually
			// accumulate enough tiny errors that it would drop a frame. It is better to just round 
			// small deviations down to zero to leave things running smoothly.

			if (abs(static_cast<int64>(timeDelta - mTargetElapsedTicks)) < TicksPerSecond / 4000)
				timeDelta = mTargetElapsedTicks;

			mLeftOverTicks += timeDelta;

			while (mLeftOverTicks >= mTargetElapsedTicks) {
				mElapsedTicks = mTargetElapsedTicks;
				mTotalTicks += mTargetElapsedTicks;
				mLeftOverTicks -= mTargetElapsedTicks;
				mFrameCount++;
			}

			// Track the current framerate.
			if (mFrameCount != lastFrameCount)
				mFramesThisSecond++;

			if (mQPCSecondCounter >= static_cast<uint64>(mQPCFrequency.QuadPart)) {
				mFPS = mFramesThisSecond;
				mFramesThisSecond = 0;
				mQPCSecondCounter %= mQPCFrequency.QuadPart;
			}
		}

	private:
		// Source timing data uses QPC units.
		LARGE_INTEGER	mQPCFrequency;
		LARGE_INTEGER	mQPCLastTime;
		UInt64			mQPCMaxDelta;
		UInt64			mQPCSecondCounter;

		// Derived timing data uses a canonical tick format.
		UInt64			mTargetElapsedTicks;
		UInt64			mElapsedTicks;
		UInt64			mTotalTicks;
		UInt64			mLeftOverTicks;

		// Members for tracking the framerate.
		UInt32			mFrameCount;
		UInt32			mFPS;
		UInt32			mFramesThisSecond;
	};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - Local proc declarations

static	CoreWindow^					sDirectXGPUGetCoreWindowProc(CSceneAppViewInternals^ internals);
static	SDirectXDisplaySupportInfo	sDirectXGPUGetDisplaySupportInfo(CSceneAppViewInternals^ internals);
static	UInt32						sDirectXGPUGetFPS(CSceneAppViewInternals^ internals);
static	Float32						sDirectXGPUGetDPI(CSceneAppViewInternals^ internals);
static	S2DSizeF32					sDirectXGPUGetSize(CSceneAppViewInternals^ internals);
static	DisplayOrientations			sDirectXGPUGetOrientation(CSceneAppViewInternals^ internals);
static	bool						sDirectXGPURequiresDeviceValidation(CSceneAppViewInternals^ internals);
static	void						sDirectXGPUHandledDeviceValidation(CSceneAppViewInternals^ internals);

static	CByteParceller				sSceneAppPlayerCreateByteParceller(const CString& resourceFilename,
											CSceneAppViewInternals^ internals);
static	void						sSceneAppPlayerInstallPeriodic(CSceneAppViewInternals^ internals);
static	void						sSceneAppPlayerRemovePeriodic(CSceneAppViewInternals^ internals);
static	void						sSceneAppPlayerOpenURL(const CURL& url, bool useWebView,
											CSceneAppViewInternals^ internals);
static	void						sSceneAppPlayerHandleCommand(const CString& command,
											const CDictionary& commandInfo, CSceneAppViewInternals^ internals);

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneAppViewInternals

ref class CSceneAppViewInternals sealed : Platform::Object {
	internal:
				CSceneAppViewInternals(const SDirectXDisplaySupportInfo& displaySupportInfo) :
					mWindowIsClosed(false), mWindowIsVisible(true), mDisplaySupportInfo(displaySupportInfo),
							mIsRunning(true),
							mGPU(
									SGPUProcsInfo(
											(CDirectXGPUGetCoreWindowProc) sDirectXGPUGetCoreWindowProc,
											(CDirectXGPUGetDisplaySupportInfoProc)
													sDirectXGPUGetDisplaySupportInfo,
											(CDirectXGPUGetFPSProc) sDirectXGPUGetFPS,
											(CDirectXGPUGetDPIProc) sDirectXGPUGetDPI,
											(CDirectXGPUGetSizeProc) sDirectXGPUGetSize,
											(CDirectXGPUGetOrientationProc) sDirectXGPUGetOrientation,
											(CDirectXGPURequiresDeviceValidationProc) sDirectXGPURequiresDeviceValidation,
											(CDirectXGPUHandledDeviceValidationProc) sDirectXGPUHandledDeviceValidation,
											(void*) this)),
							mSceneAppPlayer(NULL),
							mLastPointerPressedTimestamp(0.0), mPointerClickCount(0),
							mStepTimer(1.0 / 60.0)
					{}

		void	initialize(CoreApplicationView^ applicationView)
					{
						// Register event handlers for app lifecycle. This example includes Activated, so that we
						// can make the CoreWindow active and start rendering on the window.
						applicationView->Activated +=
								ref new TypedEventHandler<CoreApplicationView^, IActivatedEventArgs^>(this,
										&CSceneAppViewInternals::onApplicationActivated);
						CoreApplication::Suspending +=
								ref new EventHandler<SuspendingEventArgs^>(this,
										&CSceneAppViewInternals::onApplicationSuspending);
						CoreApplication::Resuming +=
								ref new EventHandler<Platform::Object^>(this,
										&CSceneAppViewInternals::onApplicationResuming);
					}
		void	setWindow(CoreWindow^ window)
					{
						// Store
						mCoreWindow = window;
						mSize = S2DSizeF32(mCoreWindow->Bounds.Width, mCoreWindow->Bounds.Height);

						// Setup Window event handlers
						window->SizeChanged += 
								ref new TypedEventHandler<CoreWindow^, WindowSizeChangedEventArgs^>(this,
										&CSceneAppViewInternals::onWindowSizeChanged);
						window->VisibilityChanged +=
								ref new TypedEventHandler<CoreWindow^, VisibilityChangedEventArgs^>(this,
										&CSceneAppViewInternals::onWindowVisibilityChanged);
						window->Closed += 
								ref new TypedEventHandler<CoreWindow^, CoreWindowEventArgs^>(this,
										&CSceneAppViewInternals::onWindowClosed);

						window->PointerPressed +=
								ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this,
										&CSceneAppViewInternals::onPointerPressed);
						window->PointerMoved +=
								ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this,
										&CSceneAppViewInternals::onPointerMoved);
						window->PointerReleased +=
								ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this,
										&CSceneAppViewInternals::onPointerReleased);
						window->PointerCaptureLost +=
								ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this,
										&CSceneAppViewInternals::onPointerCaptureLost);

						// Setup Display Information event handlers
						DisplayInformation^	displayInformation = DisplayInformation::GetForCurrentView();

						displayInformation->DpiChanged +=
								ref new TypedEventHandler<DisplayInformation^, Object^>(this,
										&CSceneAppViewInternals::onDisplayDPIChanged);

						displayInformation->OrientationChanged +=
								ref new TypedEventHandler<DisplayInformation^, Object^>(this,
										&CSceneAppViewInternals::onDisplayOrientationChanged);

						DisplayInformation::DisplayContentsInvalidated +=
								ref new TypedEventHandler<DisplayInformation^, Object^>(this,
										&CSceneAppViewInternals::onDisplayContentsInvalidated);
					}

		void	load(Platform::String^ entryPoint)
					{}
		void	run()
					{
						// Set window
						while (!mWindowIsClosed) {
							// Check if window is visible
							if (mWindowIsVisible && mIsRunning) {
								// Process events
								CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(
										CoreProcessEventsOption::ProcessAllIfPresent);

								// Update
								mStepTimer.Update();

								// Handle periodic
								mSceneAppPlayer->handlePeriodic(mStepTimer.GetTotalSeconds());
							} else
								// Process events
								CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessOneAndAllPending);
						}
					}

	protected:
		void	onApplicationActivated(CoreApplicationView^ applicationView, IActivatedEventArgs^ args)
					{
						// Run() won't start until the CoreWindow is activated.
						CoreWindow::GetForCurrentThread()->Activate();

						// Start
						mSceneAppPlayer->start(true);
					}
		void	onApplicationSuspending(Platform::Object^ sender, SuspendingEventArgs^ args)
					{
						// Stop
						mSceneAppPlayer->stop(true);
					}
		void	onApplicationResuming(Platform::Object^ sender, Platform::Object^ args)
					{
						// Start
						mSceneAppPlayer->start(true);
					}

		void	onWindowSizeChanged(CoreWindow^ sender, WindowSizeChangedEventArgs^ args)
					{
						// Store
						mSize = S2DSizeF32(sender->Bounds.Width, sender->Bounds.Height);
					}
		void	onWindowVisibilityChanged(CoreWindow^ sender, VisibilityChangedEventArgs^ args)
					{
						// Update
						mWindowIsVisible = args->Visible;
					}
		void	onWindowClosed(CoreWindow^ sender, CoreWindowEventArgs^ args)
					{
						// Update
						mWindowIsClosed = true;
					}

		void	onPointerPressed(CoreWindow^ sender, PointerEventArgs^ args)
					{
						// Setup
						Windows::UI::Input::PointerPoint^	pointerPoint = args->CurrentPoint;

						S2DPointF32	point(pointerPoint->Position.X / mSize.mWidth * mScenePackageSize.mWidth,
											pointerPoint->Position.Y / mSize.mHeight * mScenePackageSize.mHeight);

						Float32		timestamp = Float32(pointerPoint->Timestamp) / 1000000.0F;
						if ((timestamp - mLastPointerPressedTimestamp) <= 0.250)
							// Click in samem place
							mPointerClickCount++;
						else
							// New click
							mPointerClickCount = 1;

						// Inform SceneAppPlayer
						mSceneAppPlayer->mouseDown(SSceneAppPlayerMouseDownInfo(point, mPointerClickCount));

						// Store
						mPreviousPoint = point;
						mLastPointerPressedTimestamp = timestamp;

						// Handled
						args->Handled = true;
					}
		void	onPointerMoved(CoreWindow^ sender, PointerEventArgs^ args)
					{
						// Setup
						Windows::UI::Input::PointerPoint^	pointerPoint = args->CurrentPoint;

						S2DPointF32	point(pointerPoint->Position.X / mSize.mWidth * mScenePackageSize.mWidth,
											pointerPoint->Position.Y / mSize.mHeight * mScenePackageSize.mHeight);

						// Inform SceneAppPlayer
						mSceneAppPlayer->mouseDragged(SSceneAppPlayerMouseDraggedInfo(mPreviousPoint, point));

						// Store
						mPreviousPoint = point;

						// Handled
						args->Handled = true;
					}
		void	onPointerReleased(CoreWindow^ sender, PointerEventArgs^ args)
					{
						// Setup
						Windows::UI::Input::PointerPoint^	pointerPoint = args->CurrentPoint;

						S2DPointF32	point(pointerPoint->Position.X / mSize.mWidth * mScenePackageSize.mWidth,
											pointerPoint->Position.Y / mSize.mHeight * mScenePackageSize.mHeight);

						// Inform SceneAppPlayer
						mSceneAppPlayer->mouseUp(SSceneAppPlayerMouseUpInfo(point));

						// Handled
						args->Handled = true;
					}
		void	onPointerCaptureLost(CoreWindow^ sender, PointerEventArgs^ args)
					{
						// Inform SceneAppPlayer
						mSceneAppPlayer->mouseCancelled(SSceneAppPlayerMouseCancelledInfo());
					}

		void	onDisplayDPIChanged(DisplayInformation^ sender, Platform::Object^ args)
					{
						// Store
						mDPI = sender->LogicalDpi;
					}
		void	onDisplayOrientationChanged(DisplayInformation^ sender, Platform::Object^ args)
					{
						// Store
						mDisplayOrientation = sender->CurrentOrientation;
					}
		void	onDisplayContentsInvalidated(DisplayInformation^ sender, Platform::Object^ args)
					{
						// Update
						mRequiresDeviceValidation = true;
					}

	private:
				~CSceneAppViewInternals()
					{
						// Cleanup
						Delete(mSceneAppContentRootFilesystemPath);
						Delete(mSceneAppPlayer);
					}

	internal:
		Agile<CoreWindow>			mCoreWindow;
		bool						mWindowIsClosed;
		bool						mWindowIsVisible;
		SDirectXDisplaySupportInfo	mDisplaySupportInfo;
		S2DSizeF32					mSize;
		DisplayOrientations			mDisplayOrientation;
		Float32						mDPI;
		bool						mRequiresDeviceValidation;

		bool						mIsRunning;
		CGPU						mGPU;

		CFilesystemPath*			mSceneAppContentRootFilesystemPath;
		CSceneAppPlayer*			mSceneAppPlayer;
		S2DSizeF32					mScenePackageSize;
		S2DPointF32					mPreviousPoint;
		Float32						mLastPointerPressedTimestamp;
		UInt32						mPointerClickCount;

		// Rendering loop timer.
		StepTimer					mStepTimer;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneAppView

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
CSceneAppView::CSceneAppView(const SDirectXDisplaySupportInfo& displaySupportInfo)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	mInternals = ref new CSceneAppViewInternals(displaySupportInfo);
}

// MARK: IFrameworkView Methods

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppView::Initialize(CoreApplicationView^ applicationView)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->initialize(applicationView);
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppView::SetWindow(CoreWindow^ window)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->setWindow(window);
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppView::Load(Platform::String^ entryPoint)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->load(entryPoint);
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppView::Run()
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->run();
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppView::Uninitialize()
//----------------------------------------------------------------------------------------------------------------------
{
}

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
void CSceneAppView::loadScenes(const SScenePackageInfo& scenePackageInfo, const CFolder& sceneAppContentFolder,
		CSceneAppViewSceneAppPlayerCreationProc sceneAppPlayerCreationProc)
//----------------------------------------------------------------------------------------------------------------------
{
	// Cleanup
	Delete(mInternals->mSceneAppContentRootFilesystemPath);
	Delete(mInternals->mSceneAppPlayer);

	// Store
	mInternals->mSceneAppContentRootFilesystemPath = new CFilesystemPath(sceneAppContentFolder.getFilesystemPath());
	mInternals->mScenePackageSize = scenePackageInfo.mSize;

	// Setup Scene App Player
	SSceneAppPlayerProcsInfo	sceneAppPlayerProcsInfo(
										(CSceneAppPlayerCreateByteParcellerProc)
												sSceneAppPlayerCreateByteParceller,
										(CSceneAppPlayerInstallPeriodicProc)
												sSceneAppPlayerInstallPeriodic,
										(CSceneAppPlayerRemovePeriodicProc) sSceneAppPlayerRemovePeriodic,
										(CSceneAppPlayerOpenURLProc) sSceneAppPlayerOpenURL,
										(CSceneAppPlayerHandleCommandProc) sSceneAppPlayerHandleCommand,
										(void*) mInternals);
	mInternals->mSceneAppPlayer =
			(sceneAppPlayerCreationProc != nil) ?
					sceneAppPlayerCreationProc(mInternals->mGPU, sceneAppPlayerProcsInfo) :
					new CSceneAppPlayer(mInternals->mGPU, sceneAppPlayerProcsInfo);

	// Load scenes
	mInternals->mSceneAppPlayer->loadScenes(scenePackageInfo);
}

// MARK: Class methods

//----------------------------------------------------------------------------------------------------------------------
TArray<SScenePackageInfo> CSceneAppView::getScenePackageInfos(const CFolder& folder)
//----------------------------------------------------------------------------------------------------------------------
{
	// Get files in folder
	TNArray<CFile>	files;
	CFilesystem::getFiles(folder, files);

	return CScenePackage::getScenePackageInfos(files);
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - Local proc definitions

//----------------------------------------------------------------------------------------------------------------------
static CoreWindow^ sDirectXGPUGetCoreWindowProc(CSceneAppViewInternals^ internals)
//----------------------------------------------------------------------------------------------------------------------
{
	return internals->mCoreWindow.Get();
}

//----------------------------------------------------------------------------------------------------------------------
SDirectXDisplaySupportInfo sDirectXGPUGetDisplaySupportInfo(CSceneAppViewInternals^ internals)
//----------------------------------------------------------------------------------------------------------------------
{
	return internals->mDisplaySupportInfo;
}

//----------------------------------------------------------------------------------------------------------------------
UInt32 sDirectXGPUGetFPS(CSceneAppViewInternals^ internals)
//----------------------------------------------------------------------------------------------------------------------
{
	return internals->mStepTimer.GetFramesPerSecond();
}

//----------------------------------------------------------------------------------------------------------------------
Float32 sDirectXGPUGetDPI(CSceneAppViewInternals^ internals)
//----------------------------------------------------------------------------------------------------------------------
{
	return internals->mDPI;
}

//----------------------------------------------------------------------------------------------------------------------
S2DSizeF32 sDirectXGPUGetSize(CSceneAppViewInternals^ internals)
//----------------------------------------------------------------------------------------------------------------------
{
	return internals->mSize;
}

//----------------------------------------------------------------------------------------------------------------------
DisplayOrientations sDirectXGPUGetOrientation(CSceneAppViewInternals^ internals)
//----------------------------------------------------------------------------------------------------------------------
{
	return internals->mDisplayOrientation;
}

//----------------------------------------------------------------------------------------------------------------------
bool sDirectXGPURequiresDeviceValidation(CSceneAppViewInternals^ internals)
//----------------------------------------------------------------------------------------------------------------------
{
	return internals->mRequiresDeviceValidation;
}

//----------------------------------------------------------------------------------------------------------------------
void sDirectXGPUHandledDeviceValidation(CSceneAppViewInternals^ internals)
//----------------------------------------------------------------------------------------------------------------------
{
	internals->mRequiresDeviceValidation = false;
}

//----------------------------------------------------------------------------------------------------------------------
CByteParceller sSceneAppPlayerCreateByteParceller(const CString& resourceFilename,
		CSceneAppViewInternals^ internals)
//----------------------------------------------------------------------------------------------------------------------
{
	return CByteParceller(
			new CMappedFileDataSource(
					CFile(
							internals->mSceneAppContentRootFilesystemPath->appendingComponent(
									resourceFilename.replacingSubStrings(CString(OSSTR("/")), CString(OSSTR("\\")))))));
}

//----------------------------------------------------------------------------------------------------------------------
void sSceneAppPlayerInstallPeriodic(CSceneAppViewInternals^ internals)
//----------------------------------------------------------------------------------------------------------------------
{
	// Run
	internals->mIsRunning = true;
}

//----------------------------------------------------------------------------------------------------------------------
void sSceneAppPlayerRemovePeriodic(CSceneAppViewInternals^ internals)
//----------------------------------------------------------------------------------------------------------------------
{
	// Stop
	internals->mIsRunning = false;
}

//----------------------------------------------------------------------------------------------------------------------
void sSceneAppPlayerOpenURL(const CURL& url, bool useWebView, CSceneAppViewInternals^ internals)
//----------------------------------------------------------------------------------------------------------------------
{
	AssertFailUnimplemented();
}

//----------------------------------------------------------------------------------------------------------------------
void sSceneAppPlayerHandleCommand(const CString& command, const CDictionary& commandInfo,
		CSceneAppViewInternals^ internals)
//----------------------------------------------------------------------------------------------------------------------
{
	AssertFailUnimplemented();
}
