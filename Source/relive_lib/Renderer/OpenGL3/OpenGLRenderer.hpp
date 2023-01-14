#pragma once

#include "../IRenderer.hpp"
#include "../TextureCache.hpp"
#include "../PaletteCache.hpp"

#include <GL/glew.h>
#include <memory>
#include <SDL_opengl.h>

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#include "../../../relive_lib/data_conversion/rgb_conversion.hpp"
#include "../../../relive_lib/Animation.hpp"
#include "../../../relive_lib/ResourceManagerWrapper.hpp"
#include "GLContext.hpp"
#include "GLFramebuffer.hpp"
#include "GLShader.hpp"
#include "GLShaderProgram.hpp"
#include "GLTexture2D.hpp"

#include "../Vulkan/Batcher.hpp"

enum class AnimId;

class OpenGLRenderer final : public IRenderer
{
public:
    explicit OpenGLRenderer(TWindowHandleType window);
    ~OpenGLRenderer() override;

    void Clear(u8 r, u8 g, u8 b) override;
    void Draw(const Prim_GasEffect& gasEffect) override;
    void Draw(const Line_G2& line) override;
    void Draw(const Line_G4& line) override;
    void Draw(const Poly_G3& poly) override;
    void Draw(const Poly_FT4& poly) override;
    void Draw(const Poly_G4& poly) override;
    void EndFrame() override;
    void SetClip(const Prim_PrimClipper& clipper) override;
    void SetTPage(u16 tPage) override;
    void StartFrame() override;
    void ToggleFilterScreen() override;

private:
    static constexpr u32 kAvailablePalettes = 256;
    static constexpr u32 kPaletteDepth = 256;

    static constexpr u32 kCamTextureLifetime = 300;
    static constexpr u32 kSpriteTextureLifetime = 300;

    static constexpr u32 kSpriteTextureUnitCount = 12;

private:
    struct PassthruVertexData final
    {
        f32 x, y;
        f32 u, v;
    };

    struct Stats final
    {
        u32 mCamUploadCount = 0;
        u32 mFg1UploadCount = 0;
        u32 mAnimUploadCount = 0;
        u32 mPalUploadCount = 0;
        u32 mFontUploadCount = 0;
        u32 mInvalidationsCount = 0;

        void Reset()
        {
            mCamUploadCount = 0;
            mFg1UploadCount = 0;
            mAnimUploadCount = 0;
            mPalUploadCount = 0;
            mFontUploadCount = 0;
            mInvalidationsCount = 0;
        }
    };

private:
    u16 GetTPageBlendMode(u16 tPage);
    u32 PreparePalette(AnimationPal& pCache);
    std::shared_ptr<GLTexture2D> PrepareTextureFromAnim(Animation& anim);
    std::shared_ptr<GLTexture2D> PrepareTextureFromPoly(const Poly_FT4& poly);

    void PushFramebufferVertexData(const PassthruVertexData* vertices, int count);

    void DrawFramebufferToScreen(s32 x, s32 y, s32 width, s32 height);
    bool HasFramebufferPolysToDraw();
    void RenderFramebufferPolys();
    void SetupBlendMode(u16 blendMode);
    void UpdateFilterFramebuffer();

    void DecreaseResourceLifetimes();

    void DebugWindow();

    void DrawBatches();

private:
    GLContext mContext;
    GLuint mVAO = 0;

    GLShaderProgram mPassthruShader;
    GLShaderProgram mPassthruFilterShader;
    GLShaderProgram mPsxShader;

    GLFramebuffer mPsxFramebuffer;
    GLFramebuffer mPsxFbFramebuffer;
    GLFramebuffer mFilterFramebuffer;

    Stats mStats;

    bool mFrameStarted = false;

    bool mFramebufferFilter = true;
    u16 mGlobalTPage = 0;

    struct BatchData
    {

    };
    Batcher<GLTexture2D, BatchData, kSpriteTextureUnitCount> mBatcher;

    PaletteCache mPaletteCache;
    TextureCache<std::shared_ptr<GLTexture2D>> mTextureCache;

    std::vector<PassthruVertexData> mFbData;
    std::vector<u32> mFbIndicies;

    std::shared_ptr<GLTexture2D> mPaletteTexture;
    std::shared_ptr<GLTexture2D> mCurGasTexture;

    GLint mTextureUnits[kSpriteTextureUnitCount];
};
