//----------------------------------------------------------------------------------------------------------------------
//	CAction.cpp			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CAction.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: Local data

static	CString	sNameKey("actionName");
static	CString	sInfoKey("actionInfo");
static	CString	sDontPreloadKey("dontPreload");		// Can be anything
static	CString	sUnloadCurrentKey("unloadCurrent");	// Can be anything

static	CString	sActionsKey("actions");

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CActionInternals

class CActionInternals {
	public:
		CActionInternals() : mOptions(kActionOptionsNone) {}
		~CActionInternals() {}

		CString			mName;
		CDictionary		mInfo;
		EActionOptions	mOptions;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CAction

// MARK: Properties

// Names
CString	CAction::mNameOpenURL("openURL");
CString	CAction::mNamePlayAudio("playAudio");
CString	CAction::mNamePlayMovie("playMovie");
CString	CAction::mNameSceneCover("sceneCover");
CString	CAction::mNameSceneCut("sceneCut");
CString	CAction::mNameScenePush("scenePush");
CString	CAction::mNameSceneUncover("sceneUncover");
CString	CAction::mNameSetItemNameValue("setItemNameValue");
CString	CAction::mNameSendItemCommand("sendItemCommand");
CString	CAction::mNameSendAppCommand("sendAppCommand");

// Info keys and value types
CString	CAction::mInfoAudioFilenameKey("audioFilename");
CString	CAction::mInfoCommandKey("command");
CString	CAction::mInfoControlModeKey("controlMode");
CString	CAction::mInfoItemNameKey("itemName");
CString	CAction::mInfoLoadSceneIndexFromKey("loadSceneIndexFrom");
CString	CAction::mInfoMovieFilenameKey("movieFilename");
CString	CAction::mInfoPropertyNameKey("propertyName");
CString	CAction::mInfoPropertyValueKey("propertyValue");
CString	CAction::mInfoSceneIndexKey("sceneIndex");
CString	CAction::mInfoSceneNameKey("sceneName");
CString	CAction::mInfoURLKey("URL");
CString	CAction::mInfoUseWebView("useWebView");

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
CAction::CAction()
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new CActionInternals();
}

//----------------------------------------------------------------------------------------------------------------------
CAction::CAction(const CDictionary& info)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new CActionInternals();

	mInternals->mName = info.getString(sNameKey);
	mInternals->mInfo = info.getDictionary(sInfoKey);

	if (mInternals->mInfo.contains(sDontPreloadKey))
		mInternals->mOptions = (EActionOptions) (mInternals->mOptions | kActionOptionsDontPreload);
	if (mInternals->mInfo.contains(sUnloadCurrentKey))
		mInternals->mOptions = (EActionOptions) (mInternals->mOptions | kActionOptionsUnloadCurrent);
}

//----------------------------------------------------------------------------------------------------------------------
CAction::CAction(const CString& name, const CDictionary& info, EActionOptions options)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new CActionInternals();
	mInternals->mName = name;
	mInternals->mInfo = info;
	mInternals->mOptions = options;
}

//----------------------------------------------------------------------------------------------------------------------
CAction::CAction(const CAction& other)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new CActionInternals();
	mInternals->mName = other.mInternals->mName;
	mInternals->mInfo = other.mInternals->mInfo;
	mInternals->mOptions = other.mInternals->mOptions;
}

//----------------------------------------------------------------------------------------------------------------------
CAction::~CAction()
//----------------------------------------------------------------------------------------------------------------------
{
	DisposeOf(mInternals);
}

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
CDictionary CAction::getInfo() const
//----------------------------------------------------------------------------------------------------------------------
{
	CDictionary	info;

	info.set(sNameKey, mInternals->mName);
	info.set(sInfoKey, mInternals->mInfo);
	if (mInternals->mOptions & kActionOptionsDontPreload)
		info.set(sDontPreloadKey, (UInt32) 1);
	if (mInternals->mOptions & kActionOptionsUnloadCurrent)
		info.set(sUnloadCurrentKey, (UInt32) 1);

	return info;
}

//----------------------------------------------------------------------------------------------------------------------
const CString& CAction::getName() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mName;
}

//----------------------------------------------------------------------------------------------------------------------
void CAction::setName(const CString& actionName)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mName = actionName;
}

//----------------------------------------------------------------------------------------------------------------------
const CDictionary& CAction::getActionInfo() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mInfo;
}

//----------------------------------------------------------------------------------------------------------------------
void CAction::setActionInfo(const CDictionary& actionInfo)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mInfo = actionInfo;
}

//----------------------------------------------------------------------------------------------------------------------
EActionOptions CAction::getOptions() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mOptions;
}

//----------------------------------------------------------------------------------------------------------------------
void CAction::setOptions(EActionOptions options)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mOptions = options;
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CActionArray

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
CActionArray::CActionArray() : TPtrArray<CAction*>()
//----------------------------------------------------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------------------------------------------------
CActionArray::CActionArray(const CDictionary& info) : TPtrArray<CAction*>(true)
//----------------------------------------------------------------------------------------------------------------------
{
	TArray<CDictionary>	actionInfos = info.getArrayOfDictionaries(sActionsKey);
	for (CArrayItemIndex i = 0; i < actionInfos.getCount(); i++)
		*this += new CAction(actionInfos[i]);
}

//----------------------------------------------------------------------------------------------------------------------
CActionArray::CActionArray(const CActionArray& other)
//----------------------------------------------------------------------------------------------------------------------
{
	for (TIteratorS<CAction*> iterator = other.getIterator(); iterator.hasValue(); iterator.advance())
		(*this) += new CAction(*iterator.getValue());
}

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
CDictionary CActionArray::getInfo() const
//----------------------------------------------------------------------------------------------------------------------
{
	CDictionary	info;

	TArray<CDictionary>	actionInfos;
	for (TIteratorS<CAction*> iterator = getIterator(); iterator.hasValue(); iterator.advance())
		actionInfos += iterator.getValue()->getInfo();
	info.set(sActionsKey, actionInfos);

	return info;
}
