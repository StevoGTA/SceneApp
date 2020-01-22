//----------------------------------------------------------------------------------------------------------------------
//	CAction.h			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CDictionary.h"

/*
	This class may seem a little weird at first.  Historically an action was only 1 thing.  But
	eventually the need arose to do multiple things from the same slot as a single action.  At
	that time, the "Action" class expanded to handle storage of a single action or multiple actions.
	The interface to query now forces to check for multiple even if the most often scenario is a
	single action.

	11/20/12 update: will now only support action array stored as a dictionary.  Please update
		all stored PLIST files.
*/

//----------------------------------------------------------------------------------------------------------------------
// MARK: Action Options

enum EActionOptions {
	kActionOptionsNone			= 0,
	kActionOptionsDontPreload	= 1 << 0,
	kActionOptionsUnloadCurrent	= 1 << 1,
};

//----------------------------------------------------------------------------------------------------------------------
// MARK: - CAction

class CActionInternals;
class CAction {
	// Methods
	public:
								// Lifecycle methods
								CAction();
								CAction(const CDictionary& info);
								CAction(const CString& name, const CDictionary& info,
										EActionOptions options = kActionOptionsNone);
								CAction(const CAction& other);
								~CAction();

								// Instance methods
				CDictionary		getInfo() const;

		const	CString&		getName() const;
				void			setName(const CString& name);

		const	CDictionary&	getActionInfo() const;
				void			setActionInfo(const CDictionary& info);

				EActionOptions	getOptions() const;
				void			setOptions(EActionOptions options);

	// Properties
	public:
									// Names
		static	CString				mNameOpenURL;
		static	CString				mNamePlayAudio;
		static	CString				mNamePlayMovie;
		static	CString				mNameSceneCover;
		static	CString				mNameSceneCut;
		static	CString				mNameScenePush;
		static	CString				mNameSceneUncover;
		static	CString				mNameSetItemNameValue;
		static	CString				mNameSendItemCommand;
		static	CString				mNameSendAppCommand;

									// Info keys and value types
		static	CString				mInfoAudioFilenameKey;		// CString
		static	CString				mInfoCommandKey;			// CString
		static	CString				mInfoControlModeKey;		// UInt32
		static	CString				mInfoItemNameKey;			// CString
		static	CString				mInfoLoadSceneIndexFromKey;	// CString
		static	CString				mInfoMovieFilenameKey;		// CString
		static	CString				mInfoPropertyNameKey;		// CString
		static	CString				mInfoPropertyValueKey;		// SDictionaryValue
		static	CString				mInfoSceneIndexKey;			// UInt32
		static	CString				mInfoSceneNameKey;			// CString
		static	CString				mInfoURLKey;				// CString
		static	CString				mInfoUseWebView;			// anything

	private:
				CActionInternals*	mInternals;
};

//----------------------------------------------------------------------------------------------------------------------
// MARK: - CActionArray

class CActionArray : public TPtrArray<CAction*> {
	// Methods
	public:
					// Lifecycle methods
					CActionArray();
					CActionArray(const CDictionary& info);
					CActionArray(const CActionArray& other);

					// Instance methods
		CDictionary	getInfo() const;
};
