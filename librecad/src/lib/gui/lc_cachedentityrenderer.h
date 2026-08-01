#ifndef LC_CACHEDENTITYRENDERER_H
#define LC_CACHEDENTITYRENDERER_H

class RS_Painter;
struct CachedEntity;

/**
 * @brief Responsible for translating CachedEntity POD into RS_Painter drawing commands.
 * 
 * Preserves all anti-aliasing, line weight, and color configurations from RS_Painter
 * without requiring the original RS_Entity.
 */
class RS_GraphicView;

class LC_CachedEntityRenderer {
public:
    static void render(RS_Painter* painter, const CachedEntity* entity, RS_GraphicView* view);
};

#endif // LC_CACHEDENTITYRENDERER_H
