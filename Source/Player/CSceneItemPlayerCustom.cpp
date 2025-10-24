//----------------------------------------------------------------------------------------------------------------------
//	CSceneItemPlayerCustom.cpp			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CSceneItemPlayerCustom.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CSceneItemPlayerCustom

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
CSceneItemPlayerCustom::CSceneItemPlayerCustom(CSceneItemCustom& sceneItemCustom, const Procs& procs) :
		CSceneItemPlayer(sceneItemCustom, procs)
//----------------------------------------------------------------------------------------------------------------------
{
}

// MARK: CSceneItemPlayer methods

//----------------------------------------------------------------------------------------------------------------------
CActions CSceneItemPlayerCustom::getAllActions() const
//----------------------------------------------------------------------------------------------------------------------
{
	return getSceneItemCustom().hasReference() ? getAllActions(*getSceneItemCustom()) : CActions();
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerCustom::load(CGPU& gpu)
//----------------------------------------------------------------------------------------------------------------------
{
	// Check for Scene Item Custom
	if (getSceneItemCustom().hasReference())
		// Load
		load(gpu, *getSceneItemCustom());

	// Do super
	CSceneItemPlayer::load(gpu);
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerCustom::allPeersHaveLoaded()
//----------------------------------------------------------------------------------------------------------------------
{
	// Check for Scene Item Custom
	if (getSceneItemCustom().hasReference())
		// All ppers have loaded
		allPeersHaveLoaded(*getSceneItemCustom());

	// Do super
	CSceneItemPlayer::allPeersHaveLoaded();
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerCustom::unload()
//----------------------------------------------------------------------------------------------------------------------
{
	// Check for Scene Item Custom
	if (getSceneItemCustom().hasReference())
		// Unload
		unload(*getSceneItemCustom());

	// Do super
	CSceneItemPlayer::unload();
}

//----------------------------------------------------------------------------------------------------------------------
const OV<UniversalTimeInterval>& CSceneItemPlayerCustom::getStartTimeInterval() const
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	static	OV<UniversalTimeInterval>	sNone;
	return getSceneItemCustom().hasReference() ? getStartTimeInterval(*getSceneItemCustom()) : sNone;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerCustom::reset()
//----------------------------------------------------------------------------------------------------------------------
{
	// Check for Scene Item Custom
	if (getSceneItemCustom().hasReference())
		// Reset
		reset(*getSceneItemCustom());

	// Do super
	CSceneItemPlayer::reset();
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerCustom::update(CGPU& gpu, UniversalTimeInterval deltaTimeInterval, bool isRunning)
//----------------------------------------------------------------------------------------------------------------------
{
	// Check for Scene Item Custom
	if (getSceneItemCustom().hasReference())
		// Update
		update(*getSceneItemCustom(), gpu, deltaTimeInterval, isRunning);

	// Do super
	CSceneItemPlayer::update(gpu, deltaTimeInterval, isRunning);
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerCustom::render(CGPU& gpu, const CGPURenderObject::RenderInfo& renderInfo) const
//----------------------------------------------------------------------------------------------------------------------
{
	// Check for Scene Item Custom
	if (getSceneItemCustom().hasReference())
		// Render
		render(*getSceneItemCustom(), gpu, renderInfo);
}

//----------------------------------------------------------------------------------------------------------------------
bool CSceneItemPlayerCustom::handlesTouchOrMouseAtPoint(const S2DPointF32& point) const
//----------------------------------------------------------------------------------------------------------------------
{
	return getSceneItemCustom().hasReference() ? handlesTouchOrMouseAtPoint(*getSceneItemCustom(), point) : false;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerCustom::touchBeganOrMouseDownAtPoint(const S2DPointF32& point, UInt32 tapOrClickCount,
		const void* reference)
//----------------------------------------------------------------------------------------------------------------------
{
	// Check for Scene Item Custom
	if (getSceneItemCustom().hasReference())
		// Touch began or mouse down at point
		touchBeganOrMouseDownAtPoint(*getSceneItemCustom(), point, tapOrClickCount, reference);

	// Do super
	CSceneItemPlayer::touchBeganOrMouseDownAtPoint(point, tapOrClickCount, reference);
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerCustom::touchOrMouseMovedFromPoint(const S2DPointF32& point1, const S2DPointF32& point2,
		const void* reference)
//----------------------------------------------------------------------------------------------------------------------
{
	// Check for Scene Item Custom
	if (getSceneItemCustom().hasReference())
		// Touch or mouse moved from point
		touchOrMouseMovedFromPoint(*getSceneItemCustom(), point1, point2, reference);

	// Do super
	CSceneItemPlayer::touchOrMouseMovedFromPoint(point1, point2, reference);
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerCustom::touchEndedOrMouseUpAtPoint(const S2DPointF32& point, const void* reference)
//----------------------------------------------------------------------------------------------------------------------
{
	// Check for Scene Item Custom
	if (getSceneItemCustom().hasReference())
		// Touch ended or mouse up at point
		touchEndedOrMouseUpAtPoint(*getSceneItemCustom(), point, reference);

	// Do super
	CSceneItemPlayer::touchEndedOrMouseUpAtPoint(point, reference);
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerCustom::touchOrMouseCancelled(const void* reference)
//----------------------------------------------------------------------------------------------------------------------
{
	// Check for Scene Item Custom
	if (getSceneItemCustom().hasReference())
		// Touch or mouse cancelled
		touchOrMouseCancelled(*getSceneItemCustom(), reference);

	// Do super
	CSceneItemPlayer::touchOrMouseCancelled(reference);
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerCustom::shakeBegan()
//----------------------------------------------------------------------------------------------------------------------
{
	// Check for Scene Item Custom
	if (getSceneItemCustom().hasReference())
		// Shake began
		shakeBegan(*getSceneItemCustom());

	// Do super
	CSceneItemPlayer::shakeBegan();
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerCustom::shakeEnded()
//----------------------------------------------------------------------------------------------------------------------
{
	// Check for Scene Item Custom
	if (getSceneItemCustom().hasReference())
		// Shake ended
		shakeEnded(*getSceneItemCustom());

	// Do super
	CSceneItemPlayer::shakeEnded();
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerCustom::shakeCancelled()
//----------------------------------------------------------------------------------------------------------------------
{
	// Check for Scene Item Custom
	if (getSceneItemCustom().hasReference())
		// Shake cancelled
		shakeCancelled(*getSceneItemCustom());

	// Do super
	CSceneItemPlayer::shakeCancelled();
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerCustom::setProperty(const CString& propertyName, const SValue& value)
//----------------------------------------------------------------------------------------------------------------------
{
	// Check for Scene Item Custom
	if (getSceneItemCustom().hasReference())
		// Set property
		setProperty(*getSceneItemCustom(), propertyName, value);

	CSceneItemPlayer::setProperty(propertyName, value);
}

//----------------------------------------------------------------------------------------------------------------------
bool CSceneItemPlayerCustom::handleCommand(CGPU& gpu, const CString& command, const CDictionary& commandInfo,
		const S2DPointF32& point)
//----------------------------------------------------------------------------------------------------------------------
{
	// Try subclass
	if (getSceneItemCustom().hasReference() && handleCommand(gpu, *getSceneItemCustom(), command, commandInfo, point))
		// Handled
		return true;

	return CSceneItemPlayer::handleCommand(gpu, command, commandInfo, point);
}

// MARK: Subclass methods

//----------------------------------------------------------------------------------------------------------------------
const OV<UniversalTimeInterval>& CSceneItemPlayerCustom::getStartTimeInterval(const CSceneItemCustom& sceneItemCustom)
		const
//----------------------------------------------------------------------------------------------------------------------
{
	static	OV<UniversalTimeInterval>	sStartTimeIntervalUndefined;

	return sStartTimeIntervalUndefined;
}
