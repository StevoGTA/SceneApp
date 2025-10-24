//----------------------------------------------------------------------------------------------------------------------
//	CSceneItemPlayerText.cpp			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CSceneItemPlayerText.h"

#include "CGPURenderObject2D.h"

#if defined(TARGET_OS_IOS) || defined(TARGET_OS_MACOS)
	#include "CCoreGraphicsRenderer.h"
#endif

//----------------------------------------------------------------------------------------------------------------------
// MARK: CSceneItemPlayerText::Internals

class CSceneItemPlayerText::Internals {
	public:
						Internals(const SSceneAppResourceManagementInfo& sceneAppResourceManagementInfo) :
							mGPUTextureManager(sceneAppResourceManagementInfo.mGPUTextureManager),
									mRenderObject2DNeedsUpdate(true)
							{}

		CGPUTextureManager&		mGPUTextureManager;

		bool					mRenderObject2DNeedsUpdate;
		OI<CGPURenderObject2D>	mRenderObject2D;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneItemPlayerText

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
CSceneItemPlayerText::CSceneItemPlayerText(CSceneItemText& sceneItemText,
		const SSceneAppResourceManagementInfo& sceneAppResourceManagementInfo,
		const CSceneItemPlayer::Procs& sceneItemPlayerProcs) :
		CSceneItemPlayer(sceneItemText, sceneItemPlayerProcs)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	mInternals = new Internals(sceneAppResourceManagementInfo);
}

//----------------------------------------------------------------------------------------------------------------------
CSceneItemPlayerText::~CSceneItemPlayerText()
//----------------------------------------------------------------------------------------------------------------------
{
	// Cleanup
	Delete(mInternals);
}

// MARK: CSceneItemPlayer methods

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerText::update(CGPU& gpu, UniversalTimeInterval deltaTimeInterval, bool isRunning)
//----------------------------------------------------------------------------------------------------------------------
{
	// Check if we have Render Object 2D
	if (mInternals->mRenderObject2DNeedsUpdate) {
		// Setup
		const	S2DRectF32&						screenRect = getSceneItemText().getScreenRect();
				S2DRectF32						textureRect(S2DPointF32(), screenRect.mSize);

				C2DRenderer::TextPositioning	rendererTextPositioning;
				S2DPointF32						point;
		switch (getSceneItemText().getHorizontalPosition()) {
			case CSceneItemText::kHorizontalPositionLeftJustified:
				// Left-justified
				rendererTextPositioning = C2DRenderer::kTextPositioningTowardTrailingCenter;
				point = textureRect.getMidLeft();
				break;

			case CSceneItemText::kHorizontalPositionCentered:
				// Centered
				rendererTextPositioning = C2DRenderer::kTextPositioningCenter;
				point = textureRect.getCenter();
				break;

			case CSceneItemText::kHorizontalPositionRightJustified:
				// Right-justified
				rendererTextPositioning = C2DRenderer::kTextPositioningTowardLeadingCenter;
				point = textureRect.getMidRight();
				break;
		}

		// Render text to bitmap
		CBitmap	bitmap(S2DSizeS32(textureRect.mSize.mWidth, textureRect.mSize.mHeight));

#if defined(TARGET_OS_IOS) || defined(TARGET_OS_MACOS)
		CCoreGraphicsRenderer	renderer(bitmap);
#endif

		renderer.setFillColor(getSceneItemText().getTextColor());
		renderer.setStrokeColor(getSceneItemText().getOutlineColor());
		renderer.strokeText(getSceneItemText().getText(), C2DRenderer::Font(getSceneItemText().getFontName(),
				getSceneItemText().getFontSize()), point, rendererTextPositioning,
				OV<Float32>(getSceneItemText().getOutlineWidth()));

		// Create Render Object 2D
		mInternals->mRenderObject2D.setInstance(
				new CGPURenderObject2D(gpu, CGPURenderObject2D::Item(screenRect, textureRect),
						mInternals->mGPUTextureManager.gpuTextureReference(bitmap)));
	}
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerText::render(CGPU& gpu, const CGPURenderObject::RenderInfo& renderInfo) const
//----------------------------------------------------------------------------------------------------------------------
{
	// Check if can render
	if (mInternals->mRenderObject2D.hasInstance()) {
		// Render
		mInternals->mRenderObject2D->finishLoading();
		mInternals->mRenderObject2D->render(CGPURenderObject2D::Indexes::forIndex(0), renderInfo);
	}
}

// MARK: CSCeneItemPlayer methods

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerText::noteSceneItemUpdated(const CString& propertyName)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mRenderObject2DNeedsUpdate = true;
}
