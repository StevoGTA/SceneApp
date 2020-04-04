//----------------------------------------------------------------------------------------------------------------------
//	CSceneItemPlayerCustom.cpp			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CSceneItemPlayerCustom.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CSceneItemPlayerCustom

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
CSceneItemPlayerCustom::CSceneItemPlayerCustom(const CSceneItemCustom& sceneItemCustom,
		const SSceneItemPlayerProcsInfo& sceneItemPlayerProcsInfo) :
		CSceneItemPlayer(sceneItemCustom, sceneItemPlayerProcsInfo)
//----------------------------------------------------------------------------------------------------------------------
{
}

// MARK: CSceneItemPlayer methods

//----------------------------------------------------------------------------------------------------------------------
CActions CSceneItemPlayerCustom::getAllActions() const
//----------------------------------------------------------------------------------------------------------------------
{
	return getAllActions(getSceneItemCustom());
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerCustom::load()
//----------------------------------------------------------------------------------------------------------------------
{
	CSceneItemPlayer::load();

	load(getSceneItemCustom());
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerCustom::allPeersHaveLoaded()
//----------------------------------------------------------------------------------------------------------------------
{
	allPeersHaveLoaded(getSceneItemCustom());

	CSceneItemPlayer::allPeersHaveLoaded();
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerCustom::unload()
//----------------------------------------------------------------------------------------------------------------------
{
	unload(getSceneItemCustom());

	CSceneItemPlayer::unload();
}

//----------------------------------------------------------------------------------------------------------------------
const OV<UniversalTimeInterval>& CSceneItemPlayerCustom::getStartTimeInterval() const
//----------------------------------------------------------------------------------------------------------------------
{
	return getStartTimeInterval(getSceneItemCustom());
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerCustom::reset()
//----------------------------------------------------------------------------------------------------------------------
{
	reset(getSceneItemCustom());

	CSceneItemPlayer::reset();
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerCustom::update(UniversalTimeInterval deltaTimeInterval, bool isRunning)
//----------------------------------------------------------------------------------------------------------------------
{
	update(getSceneItemCustom(), deltaTimeInterval, isRunning);

	CSceneItemPlayer::update(deltaTimeInterval, isRunning);
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerCustom::render(CGPU& gpu, const SGPURenderObjectRenderInfo& renderInfo) const
//----------------------------------------------------------------------------------------------------------------------
{
	render(getSceneItemCustom(), gpu, renderInfo);
}

//----------------------------------------------------------------------------------------------------------------------
bool CSceneItemPlayerCustom::handlesTouchOrMouseAtPoint(const S2DPoint32& point) const
//----------------------------------------------------------------------------------------------------------------------
{
	return handlesTouchOrMouseAtPoint(getSceneItemCustom(), point);
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerCustom::touchBeganOrMouseDownAtPoint(const S2DPoint32& point, UInt32 tapOrClickCount,
		const void* reference)
//----------------------------------------------------------------------------------------------------------------------
{
	touchBeganOrMouseDownAtPoint(getSceneItemCustom(), point, tapOrClickCount, reference);

	CSceneItemPlayer::touchBeganOrMouseDownAtPoint(point, tapOrClickCount, reference);
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerCustom::touchOrMouseMovedFromPoint(const S2DPoint32& point1, const S2DPoint32& point2,
		const void* reference)
//----------------------------------------------------------------------------------------------------------------------
{
	touchOrMouseMovedFromPoint(getSceneItemCustom(), point1, point2, reference);

	CSceneItemPlayer::touchOrMouseMovedFromPoint(point1, point2, reference);
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerCustom::touchEndedOrMouseUpAtPoint(const S2DPoint32& point, const void* reference)
//----------------------------------------------------------------------------------------------------------------------
{
	touchEndedOrMouseUpAtPoint(getSceneItemCustom(), point, reference);

	CSceneItemPlayer::touchEndedOrMouseUpAtPoint(point, reference);
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerCustom::touchOrMouseCancelled(const void* reference)
//----------------------------------------------------------------------------------------------------------------------
{
	touchOrMouseCancelled(getSceneItemCustom(), reference);

	CSceneItemPlayer::touchOrMouseCancelled(reference);
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerCustom::shakeBegan()
//----------------------------------------------------------------------------------------------------------------------
{
	shakeBegan(getSceneItemCustom());

	CSceneItemPlayer::shakeBegan();
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerCustom::shakeEnded()
//----------------------------------------------------------------------------------------------------------------------
{
	shakeEnded(getSceneItemCustom());

	CSceneItemPlayer::shakeEnded();
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerCustom::shakeCancelled()
//----------------------------------------------------------------------------------------------------------------------
{
	shakeCancelled(getSceneItemCustom());

	CSceneItemPlayer::shakeCancelled();
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerCustom::setProperty(const CString& propertyName, const SDictionaryValue& value)
//----------------------------------------------------------------------------------------------------------------------
{
	setProperty(getSceneItemCustom(), propertyName, value);

	CSceneItemPlayer::setProperty(propertyName, value);
}

//----------------------------------------------------------------------------------------------------------------------
bool CSceneItemPlayerCustom::handleCommand(const CString& command, const CDictionary& commandInfo,
		const S2DPoint32& point)
//----------------------------------------------------------------------------------------------------------------------
{
	// Try subclass
	if (handleCommand(getSceneItemCustom(), command, commandInfo, point))
		// Handled
		return true;

	return CSceneItemPlayer::handleCommand(command, commandInfo, point);
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
