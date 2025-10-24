//----------------------------------------------------------------------------------------------------------------------
//	CSceneItem.h			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CDictionary.h"
#include "CUUID.h"
#include "TimeAndDate.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CSceneItem

class CSceneItem {
	// Options
	public:
		enum Options {
			kOptionsNone				= 0,

			// Anchor to Screen (instead of default anchor to background)
			kOptionsAnchorToScreen		= 1 << 0,

			// Don't load with scene (instead of default to load when scene loads)
			kOptionsDontLoadWithScene	= 1 << 1,

			// Hide if audio input is unavailabale
			kOptionsHideIfNoAudioInput	= 1 << 2,
		};

	// PropertyType
	public:
		// Query the properties method which will return a TArray<CDictionary>.  In each CDictionary are the following:
		//		Key: mPropertyNameKey		Value: CString of object property name
		//		Key: mPropertyTitleKey		Value: CString of display title
		//		Key: mPropertyTypeKey		Value: OSType of property type (see below)
		enum PropertyType {
			kPropertyTypeName				= MAKE_OSTYPE('N', 'a', 'm', 'e'),
			kPropertyTypeFilename			= MAKE_OSTYPE('F', 'N', 'a', 'm'),
			kPropertyTypeText				= MAKE_OSTYPE('T', 'e', 'x', 't'),
			kPropertyTypeBoolean			= MAKE_OSTYPE('B', 'o', 'o', 'l'),
			kPropertyTypeOptions			= MAKE_OSTYPE('O', 'p', 't', 's'),
			kPropertyTypeScreenRect			= MAKE_OSTYPE('R', 'e', 'c', 't'),
			kPropertyTypeScreenRectArray	= MAKE_OSTYPE('A', 'R', 'c', 't'),
			kPropertyTypeScreenPoint		= MAKE_OSTYPE('S', 'c', 'P', 't'),
			kPropertyTypeActions			= MAKE_OSTYPE('A', 'c', 't', 'n'),
			kPropertyTypeStartTimeInterval	= MAKE_OSTYPE('S', 't', 'T', 'm'),
			kPropertyTypeColor				= MAKE_OSTYPE('C', 'o', 'l', 'r'),
			kPropertyTypePixelWidth			= MAKE_OSTYPE('P', 'x', 'l', 'W'),
			kPropertyTypeFontName			= MAKE_OSTYPE('F', 'n', 't', 'N'),
			kPropertyTypeFontSize			= MAKE_OSTYPE('F', 'n', 't', 'S'),
		};

	// Updated Proc
	public:
		typedef	void	(*NoteUpdatedProc)(const CString& propertyName, void* userData);

	// Classes
	private:
		class Internals;

	// Methods
	public:
												// Lifecycle methods
												CSceneItem(const CString& name = CUUID().getBase64String(),
														bool isInitiallyVisible = true, Options options = kOptionsNone);
												CSceneItem(const CDictionary& info);
												CSceneItem(const CSceneItem& other);
		virtual									~CSceneItem();
						
												// Instance methods
						bool					getIsInitiallyVisible() const;
						void					setIsInitiallyVisible(bool isInitiallyVisible);

				const	CString&				getName() const;
						void					setName(const CString& name);

						Options					getOptions() const;
						void					setOptions(Options options);

		virtual	const	CString&				getType() const = 0;
		virtual			CString					getDescription();
		virtual			TMArray<CDictionary>	getProperties() const;
		virtual			CDictionary				getInfo() const;

						void					setNoteUpdatedProc(NoteUpdatedProc noteUpdatedProc, void* userData);

	protected:
												// Subclass methods
						void					noteUpdated(const CString& propertyName);

	// Properties
	public:
		static	const	CString		mItemInfoKey;

		static	const	CString		mPropertyNameKey;
		static	const	CString		mPropertyTitleKey;
		static	const	CString		mPropertyTypeKey;

		static	const	CString		mPropertyNameIsInitiallyVisible;
		static	const	CString		mPropertyNameName;
		static	const	CString		mPropertyNameOptions;

	private:
						Internals*	mInternals;
};
