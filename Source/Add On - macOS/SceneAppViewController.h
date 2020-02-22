//----------------------------------------------------------------------------------------------------------------------
//	SceneAppViewController.h			©2019 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#import <AppKit/AppKit.h>

#import "CSceneAppPlayer.h"
#import "SceneAppGPUView.h"

NS_ASSUME_NONNULL_BEGIN

//----------------------------------------------------------------------------------------------------------------------
// MARK: SceneAppViewController
@interface SceneAppViewController : NSViewController

// MARK: Properties
@property (class, nonatomic, readonly)	TArray<CString>	scenePackageFilenames;

// MARK: Instance methods
- (instancetype) initWithView:(NSView<SceneAppGPUView>*) view;
- (instancetype) initWithView:(NSView<SceneAppGPUView>*) view
		sceneAppPlayerCreationProc:
				(nullable CSceneAppPlayer* (^)(CGPU& gpu, const SSceneAppPlayerProcsInfo& sceneAppPlayerProcsInfo))
						sceneAppPlayerCreationProc;

- (void) loadScenesFrom:(const SScenePackageInfo&) scenePackageInfo;

@end

NS_ASSUME_NONNULL_END
