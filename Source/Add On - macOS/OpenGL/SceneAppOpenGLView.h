//----------------------------------------------------------------------------------------------------------------------
//	SceneAppOpenGLView.h			©2019 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#import "SceneAppGPUView.h"

#import <OpenGL/OpenGL.h>

NS_ASSUME_NONNULL_BEGIN

//----------------------------------------------------------------------------------------------------------------------
// MARK: SceneAppOpenGLView

@interface SceneAppOpenGLView : NSOpenGLView <SceneAppGPUView>

// MARK: Instance methods
- (instancetype) initWithFrame:(NSRect) frame;

@end

NS_ASSUME_NONNULL_END
