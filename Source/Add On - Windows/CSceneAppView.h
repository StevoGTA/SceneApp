//----------------------------------------------------------------------------------------------------------------------
//	CSceneAppView.h			©2020 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CSceneAppPlayer.h"
#include "SDirectXDisplaySupportInfo.h"

using namespace Windows::ApplicationModel::Core;
using namespace Windows::UI::Core;
using namespace Platform;

//----------------------------------------------------------------------------------------------------------------------
// MARK: CSceneAppView

typedef	CSceneAppPlayer*	(*CSceneAppViewSceneAppPlayerCreationProc)(CGPU& gpu,
									const CSceneAppPlayer::Procs& sceneAppPlayerProcsInfo);

ref class CSceneAppViewInternals;
ref class CSceneAppView sealed : public IFrameworkView {
	// Methods
	public:
											// IFrameworkView Methods
		virtual	void						Initialize(CoreApplicationView^ applicationView);
		virtual	void						SetWindow(CoreWindow^ window);
		virtual	void						Load(String^ entryPoint);
		virtual	void						Run();
		virtual	void						Uninitialize();

	internal:
											// Lifecycle methods
											CSceneAppView(const SDirectXDisplaySupportInfo& displaySupportInfo);

											// Instance methods
				void						loadScenes(const CScenePackage::Info& scenePackageInfo,
													const CFolder& sceneAppContentFolder,
													CSceneAppViewSceneAppPlayerCreationProc
															sceneAppPlayerCreationProc = NULL);

											// Class methods
		static	TArray<CScenePackage::Info>	getScenePackageInfos(const CFolder& folder);

	// Properties
	private:
		CSceneAppViewInternals^	mInternals;
};
