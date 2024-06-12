//----------------------------------------------------------------------------------------------------------------------
//	CSceneTransitionPlayer.h			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CScenePlayer.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CSceneTransitionPlayer

class CSceneTransitionPlayer {
	// State
	public:
		enum State {
			// Transition is currently performing
			kStateActive,

			// Transition completed with the new scene
			kStateCompleted,

			// Transition completed with the existing scene
			kStateReset,
		};

	// Direction
	public:
		enum Direction {
			kDirectionUp,
			kDirectionDown,
			kDirectionLeft,
			kDirectionRight,
		};

	// Procs
	public:
		struct Procs {
			// Procs
			public:
				typedef	S2DSizeF32	(*GetViewportSizeProc)(void* userData);

			// Methods
			public:
							// Lifecycle methods
							Procs(GetViewportSizeProc getViewportSizeProc, void* userData) :
								mGetViewportSizeProc(getViewportSizeProc), mUserData(userData)
								{}

							// Instance methods
				S2DSizeF32	getViewportSize() const
								{ return mGetViewportSizeProc(mUserData); }

			// Properties
			private:
				GetViewportSizeProc	mGetViewportSizeProc;
				void*				mUserData;
		};

	// Classes
	private:
		class Internals;

	// Methods
	public:
								// Lifecycle methods
								CSceneTransitionPlayer(CScenePlayer& fromScenePlayer, CScenePlayer& toScenePlayer);
		virtual					~CSceneTransitionPlayer();
						
								// Instance methods
				CScenePlayer&	getFromScenePlayer() const;
				CScenePlayer&	getToScenePlayer() const;

				State			getState() const;
				void			setState(State state);

		virtual	bool			supportsMultitouch() const { return false; }

		virtual	void			update(UniversalTimeInterval deltaTimeInterval);
		virtual	void			render(CGPU& gpu) const = 0;

		virtual	void			touchBeganOrMouseDownAtPoint(const S2DPointF32& point, UInt32 tapOrClickCount,
										const void* reference) {}
		virtual	void			touchOrMouseMovedFromPoint(const S2DPointF32& point1, const S2DPointF32& point2,
										const void* reference) {}
		virtual	void			touchEndedOrMouseUpAtPoint(const S2DPointF32& point, const void* reference) {}
		virtual	void			touchesOrMouseCancelled(const void* reference) {}

	protected:
								// Subclass methods
				Direction		getDirection(const CString& string);

	// Properties
	public:
		static	CString		mInfoDirectionUpValue;
		static	CString		mInfoDirectionDownValue;
		static	CString		mInfoDirectionLeftValue;
		static	CString		mInfoDirectionRightValue;

	private:
				Internals*	mInternals;
};
