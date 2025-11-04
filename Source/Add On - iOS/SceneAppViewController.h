//----------------------------------------------------------------------------------------------------------------------
//	SceneAppViewController.h			©2019 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#import "CSceneAppPlayer.h"
#import "UKTGPUView.h"

NS_ASSUME_NONNULL_BEGIN

//----------------------------------------------------------------------------------------------------------------------
// MARK: SceneAppViewController

@interface SceneAppViewController : UIViewController

// MARK: Types
typedef CSceneAppPlayer*	_Nonnull	(^CreateSceneAppPlayerProc)(CGPU& gpu, const CSceneAppPlayer::Procs& procs,
												const SSceneAppResourceLoading& sceneAppResourceLoading);

// MARK: Properties
@property (nonatomic, readonly)			CSceneAppPlayer&	sceneAppPlayer;

@property (nonatomic, strong, nullable)	void				(^openURLProc)(NSURL* url);
@property (nonatomic, strong, nullable)	void				(^handleCommandProc)(const CString& command,
																	const CDictionary& commandInfo);

// MARK: Instance methods
- (instancetype) initWithView:(UIView<UKTGPUView>*) view
		scenePackageInfo:(const CScenePackage::Info&) scenePackageInfo
		sceneAppContentFolder:(const CFolder&) sceneAppContentFolder;
- (instancetype) initWithView:(UIView<UKTGPUView>*) view
		scenePackageInfo:(const CScenePackage::Info&) scenePackageInfo
		sceneAppContentFolder:(const CFolder&) sceneAppContentFolder
		sceneAppPlayerCreationProc:(nullable CreateSceneAppPlayerProc) sceneAppPlayerCreationProc;

@end

NS_ASSUME_NONNULL_END
