//----------------------------------------------------------------------------------------------------------------------
//	CSceneTransitionPlayer.h			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CScenePlayer.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: Transition States

enum ESceneTransitionState {
	// Transition is currently performing
	kSceneTransitionStateActive,

	// Transition completed with the new scene
	kSceneTransitionStateCompleted,

	// Transition completed with the existing scene
	kSceneTransitionStateReset,
};

//----------------------------------------------------------------------------------------------------------------------
// MARK: - Transition Directions

enum ESceneTransitionDirection {
	kSceneTransitionDirectionUp,
	kSceneTransitionDirectionDown,
	kSceneTransitionDirectionLeft,
	kSceneTransitionDirectionRight,
};

//----------------------------------------------------------------------------------------------------------------------
// MARK: - SSceneTransitionPlayerProcsInfo

typedef	S2DSizeF32	(*CSceneTransitionPlayerGetViewportSizeProc)(void* userData);

struct SSceneTransitionPlayerProcsInfo {
				// Lifecycle methods
				SSceneTransitionPlayerProcsInfo(CSceneTransitionPlayerGetViewportSizeProc getViewportSizeProc,
						void* userData) :
					mGetViewportSizeProc(getViewportSizeProc), mUserData(userData)
					{}

				// Instance methods
	S2DSizeF32	getViewportSize() const
					{ return mGetViewportSizeProc(mUserData); }

	// Properties
	private:
		CSceneTransitionPlayerGetViewportSizeProc	mGetViewportSizeProc;
		void*										mUserData;
};

//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneTransitionPlayer

class CSceneTransitionPlayerInternals;
class CSceneTransitionPlayer {
	// Methods
	public:
											// Lifecycle methods
											CSceneTransitionPlayer(CScenePlayer& fromScenePlayer,
													CScenePlayer& toScenePlayer);
		virtual								~CSceneTransitionPlayer();
						
											// Instance methods
				CScenePlayer&				getFromScenePlayer() const;
				CScenePlayer&				getToScenePlayer() const;

				ESceneTransitionState		getState() const;
				void						setState(ESceneTransitionState state);

		virtual	bool						supportsMultitouch() const { return false; }

		virtual	void						update(UniversalTimeInterval deltaTimeInterval);
		virtual	void						render(CGPU& gpu) const = 0;

		virtual	void						touchBeganOrMouseDownAtPoint(const S2DPointF32& point,
													UInt32 tapOrClickCount, const void* reference) {}
		virtual	void						touchOrMouseMovedFromPoint(const S2DPointF32& point1,
													const S2DPointF32& point2, const void* reference) {}
		virtual	void						touchEndedOrMouseUpAtPoint(const S2DPointF32& point, const void* reference)
												{}
		virtual	void						touchesOrMouseCancelled(const void* reference) {}

	protected:
											// Subclass methods
				ESceneTransitionDirection	getSceneTransitionDirection(const CString& string);

	// Properties
	public:
		static	CString								mInfoDirectionUpValue;
		static	CString								mInfoDirectionDownValue;
		static	CString								mInfoDirectionLeftValue;
		static	CString								mInfoDirectionRightValue;

	private:
				CSceneTransitionPlayerInternals*	mInternals;
};
