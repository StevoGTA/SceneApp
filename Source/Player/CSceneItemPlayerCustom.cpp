//----------------------------------------------------------------------------------------------------------------------
//	CSceneItemPlayerCustom.cpp			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CSceneItemPlayerCustom.h"

////----------------------------------------------------------------------------------------------------------------------
//// MARK: CSceneItemPlayerCustomInternals
//
//class CSceneItemPlayerCustomInternals {
//	public:
//		CSceneItemPlayerCustomInternals() {}
//		~CSceneItemPlayerCustomInternals() {}
//};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneItemPlayerCustom

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
CSceneItemPlayerCustom::CSceneItemPlayerCustom(const CSceneItemCustom& sceneItemCustom,
		const SSceneAppResourceManagementInfo& sceneAppResourceManagementInfo,
		const SSceneItemPlayerProcsInfo& sceneItemPlayerProcsInfo) :
		CSceneItemPlayer(sceneItemCustom, sceneItemPlayerProcsInfo)
//----------------------------------------------------------------------------------------------------------------------
{
//	mInternals = new CSceneItemPlayerCustomInternals();
}

//----------------------------------------------------------------------------------------------------------------------
CSceneItemPlayerCustom::~CSceneItemPlayerCustom()
//----------------------------------------------------------------------------------------------------------------------
{
//	DisposeOf(mInternals);
}

// MARK: CSceneItemPlayer methods

//----------------------------------------------------------------------------------------------------------------------
S2DRect32 CSceneItemPlayerCustom::getCurrentScreenRect() const
//----------------------------------------------------------------------------------------------------------------------
{
	return getCurrentScreenRect(getSceneItemCustom());
}

//----------------------------------------------------------------------------------------------------------------------
CActionArray CSceneItemPlayerCustom::getAllActions() const
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
void CSceneItemPlayerCustom::finishLoading()
//----------------------------------------------------------------------------------------------------------------------
{
	finishLoading(getSceneItemCustom());

	CSceneItemPlayer::finishLoading();
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
bool CSceneItemPlayerCustom::getIsFullyLoaded() const
//----------------------------------------------------------------------------------------------------------------------
{
	return getIsFullyLoaded(getSceneItemCustom());
}

//----------------------------------------------------------------------------------------------------------------------
UniversalTimeInterval CSceneItemPlayerCustom::getStartTimeInterval() const
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
void CSceneItemPlayerCustom::render(CGPU& gpu, const S2DPoint32& offset)
//----------------------------------------------------------------------------------------------------------------------
{
	render(getSceneItemCustom(), gpu, offset);

	CSceneItemPlayer::render(gpu, offset);
}

//----------------------------------------------------------------------------------------------------------------------
bool CSceneItemPlayerCustom::handlesTouchOrMouseAtPoint(const S2DPoint32& point)
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
S2DRect32 CSceneItemPlayerCustom::getCurrentScreenRect(const CSceneItemCustom& sceneItemCustom) const
//----------------------------------------------------------------------------------------------------------------------
{
	return S2DRect32();
}

//----------------------------------------------------------------------------------------------------------------------
CActionArray CSceneItemPlayerCustom::getAllActions(const CSceneItemCustom& sceneItemCustom) const
//----------------------------------------------------------------------------------------------------------------------
{
	return CActionArray();
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerCustom::load(const CSceneItemCustom& sceneItemCustom)
//----------------------------------------------------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerCustom::finishLoading(const CSceneItemCustom& sceneItemCustom)
//----------------------------------------------------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerCustom::allPeersHaveLoaded(const CSceneItemCustom& sceneItemCustom)
//----------------------------------------------------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerCustom::unload(const CSceneItemCustom& sceneItemCustom)
//----------------------------------------------------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------------------------------------------------
bool CSceneItemPlayerCustom::getIsFullyLoaded(const CSceneItemCustom& sceneItemCustom) const
//----------------------------------------------------------------------------------------------------------------------
{
	return true;
}

//----------------------------------------------------------------------------------------------------------------------
UniversalTimeInterval CSceneItemPlayerCustom::getStartTimeInterval(const CSceneItemCustom& sceneItemCustom) const
//----------------------------------------------------------------------------------------------------------------------
{
	return 0.0;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerCustom::reset(const CSceneItemCustom& sceneItemCustom)
//----------------------------------------------------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerCustom::update(const CSceneItemCustom& sceneItemCustom, UniversalTimeInterval deltaTimeInterval,
		bool isRunning)
//----------------------------------------------------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerCustom::render(const CSceneItemCustom& sceneItemCustom, CGPU& gpu, const S2DPoint32& offset)
//----------------------------------------------------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------------------------------------------------
bool CSceneItemPlayerCustom::handlesTouchOrMouseAtPoint(const CSceneItemCustom& sceneItemCustom,
		const S2DPoint32& point)
//----------------------------------------------------------------------------------------------------------------------
{
	return false;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerCustom::touchBeganOrMouseDownAtPoint(const CSceneItemCustom& sceneItemCustom,
		const S2DPoint32& point, UInt32 tapOrClickCount, const void* reference)
//----------------------------------------------------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerCustom::touchOrMouseMovedFromPoint(const CSceneItemCustom& sceneItemCustom,
		const S2DPoint32& point1, const S2DPoint32& point2, const void* reference)
//----------------------------------------------------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerCustom::touchEndedOrMouseUpAtPoint(const CSceneItemCustom& sceneItemCustom,
		const S2DPoint32& point, const void* reference)
//----------------------------------------------------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerCustom::touchOrMouseCancelled(const CSceneItemCustom& sceneItemCustom, const void* reference)
//----------------------------------------------------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerCustom::shakeBegan(const CSceneItemCustom& sceneItemCustom)
//----------------------------------------------------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerCustom::shakeEnded(const CSceneItemCustom& sceneItemCustom)
//----------------------------------------------------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerCustom::shakeCancelled(const CSceneItemCustom& sceneItemCustom)
//----------------------------------------------------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerCustom::setProperty(const CSceneItemCustom& sceneItemCustom, const CString& propertyName,
		const SDictionaryValue& value)
//----------------------------------------------------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------------------------------------------------
bool CSceneItemPlayerCustom::handleCommand(const CSceneItemCustom& sceneItemCustom, const CString& command,
		const CDictionary& commandInfo, const S2DPoint32& point)
//----------------------------------------------------------------------------------------------------------------------
{
	return false;
}
