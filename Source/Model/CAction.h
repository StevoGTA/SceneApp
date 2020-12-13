//----------------------------------------------------------------------------------------------------------------------
//	CAction.h			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CDictionary.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CAction

class CActionInternals;
class CAction {
	// Enums
	public:
		enum Options {
			kOptionsNone			= 0,
			kOptionsDontPreload		= 1 << 0,
		};

	// Methods
	public:
								// Lifecycle methods
								CAction(const CString& name, const CDictionary& info, Options options = kOptionsNone);
								CAction(const CDictionary& info);
								CAction(const CAction& other);
								~CAction();

								// Instance methods
				CDictionary		getInfo() const;

		const	CString&		getName() const;
//				void			setName(const CString& name);

		const	CDictionary&	getActionInfo() const;
//				void			setActionInfo(const CDictionary& info);

				Options			getOptions() const;
//				void			setOptions(Options options);

	// Properties
	public:
									// Names
		static	CString				mNameOpenURL;
		static	CString				mNamePlayMovie;
		static	CString				mNameSceneCover;
		static	CString				mNameSceneCut;
		static	CString				mNameScenePush;
		static	CString				mNameSceneUncover;
		static	CString				mNameSetItemNameValue;
		static	CString				mNameSendItemCommand;
		static	CString				mNameSendAppCommand;

									// Info keys and value types
		static	CString				mInfoCommandKey;			// CString
		static	CString				mInfoControlModeKey;		// UInt32
		static	CString				mInfoFilenameKey;			// CString
		static	CString				mInfoItemNameKey;			// CString
		static	CString				mInfoLoadSceneIndexFromKey;	// CString
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
// MARK: - CActions

class CActions : public TNArray<CAction> {
	// Methods
	public:
					// Lifecycle methods
					CActions();
					CActions(const CAction& action);
					CActions(const CDictionary& info);
					CActions(const CActions& other);

					// Instance methods
		CDictionary	getInfo() const;
};
