//----------------------------------------------------------------------------------------------------------------------
//	CSceneItemPlayerVideo.h			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CSceneItemPlayer.h"
#include "CSceneItemVideo.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CSceneItemPlayerVideo

class CSceneItemPlayerVideo : public CSceneItemPlayer {
	// Classes
	private:
		class Internals;

	// Methods
	public:
											// Lifecycle methods
											CSceneItemPlayerVideo(const CSceneItemVideo& sceneItemVideo,
													const SSceneAppResourceManagementInfo&
															sceneAppResourceManagementInfo,
													const Procs& procs);
											~CSceneItemPlayerVideo();

											// CSceneItemPlayer methods
				CActions					getAllActions() const;

				void						load(CGPU& gpu);
				void						unload();

		const	OV<UniversalTimeInterval>&	getStartTimeInterval() const;

				void						reset();
				void						update(UniversalTimeInterval deltaTimeInterval, bool isRunning);
				void						render(CGPU& gpu, const CGPURenderObject::RenderInfo& renderInfo) const;

				bool						handlesTouchOrMouseAtPoint(const S2DPointF32& point) const;

				void						setProperty(const CString& property, const SValue& value);
				bool						handleCommand(CGPU& gpu, const CString& command,
													const CDictionary& commandInfo, const S2DPointF32& point);

											// Instance methods
		const	CSceneItemVideo&			getSceneItemVideo() const
												{ return (CSceneItemVideo&) getSceneItem(); }

	// Properties
	public:
		static	CString		mFilenameProperty;		// String

		static	CString		mCommandNamePlay;
		static	CString		mCommandNameStop;

	private:
				Internals*	mInternals;
};
