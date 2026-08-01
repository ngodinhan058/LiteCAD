#ifndef I_RENDERER_H
#define I_RENDERER_H

class LC_CachedScene;
class QTransform;

/**
 * @brief Abstract interface for rendering backends.
 * 
 * Future implementations may include OpenGL, Vulkan, Metal, or DirectX.
 * The renderer must NOT contain CAD logic, tessellation logic, or geometry generation.
 * It strictly consumes the LC_CachedScene.
 */
class IRenderer {
public:
    virtual ~IRenderer() = default;

    /**
     * @brief Initializes the rendering backend (e.g., compile shaders, create buffers).
     */
    virtual void initialize() = 0;

    /**
     * @brief Resizes the rendering viewport.
     */
    virtual void resize(int w, int h) = 0;

    /**
     * @brief Issues draw calls to render the cached scene.
     * @param scene The cached scene containing rendering data.
     * @param offsetX The view X offset.
     * @param offsetY The view Y offset.
     * @param factorX The X zoom factor.
     * @param factorY The Y zoom factor.
     * @param viewWidth The viewport width.
     * @param viewHeight The viewport height.
     */
    virtual void render(const LC_CachedScene* scene, double offsetX, double offsetY, double factorX, double factorY, int viewWidth, int viewHeight) = 0;
};

#endif // I_RENDERER_H
