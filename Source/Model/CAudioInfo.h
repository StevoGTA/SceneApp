//----------------------------------------------------------------------------------------------------------------------
//	CAudioInfo.h			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CDictionary.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CAudioInfo

class CAudioInfoInternals;
class CAudioInfo {
	// Enums
	public:
		enum Options {
			kOptionsNone						= 0,
			kOptionsStopBeforePlay				= 1 << 0,
			kOptionsUseDefinedLoopInResource	= 1 << 1,
			kOptionsLoadFileIntoMemory			= 1 << 2,
		};

		enum PropertyType {
			kPropertyTypeAudioInfo	= MAKE_OSTYPE('A', 'u', 'I', 'n'),

			kPropertyTypeGain		= MAKE_OSTYPE('A', 'u', 'I', 'G'),
			kPropertyTypeLoopCount	= MAKE_OSTYPE('A', 'u', 'I', 'L'),
			kPropertyTypeOptions	= MAKE_OSTYPE('A', 'u', 'I', 'O'),
		};

	// Methods
	public:
									// Lifecycle methods
									CAudioInfo(const CString& resourceFilename);
									CAudioInfo(const CDictionary& info);
									CAudioInfo(const CAudioInfo& other);
									~CAudioInfo();

									// Instance methods
				TArray<CDictionary>	getProperties() const;
				CDictionary			getInfo() const;
				
				Float32				getGain() const;
				void				setGain(Float32 gain);

		const	CString&			getResourceFilename() const;
				void				setResourceFilename(const CString& resourceFilename);

				OV<UInt32>			getLoopCount() const;
				void				setLoopCount(OV<UInt32> loopCount);

				Options				getOptions() const;
				void				setOptions(Options options);

	// Properties
	private:
		CAudioInfoInternals*	mInternals;
};
