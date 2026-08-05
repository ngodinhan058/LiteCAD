#ifndef LC_CACHEDENTITY_H
#define LC_CACHEDENTITY_H

#include "rs.h"
#include "lc_rect.h"
#include "rs_color.h"
#include "rs_pen.h"

/**
 * @brief Plain Old Data (POD) structure for an entity's rendering metadata.
 * 
 * This completely decouples the rendering pipeline from RS_Entity.
 * It contains no CAD logic and is designed to be efficiently stored,
 * queried via spatial index, and consumed by the renderer (or OpenGL in the future).
 */
struct CachedEntity {
    unsigned int id;
    RS2::EntityType type;
    LC_Rect boundingBox;
    
    // Core properties
    bool visible;
    bool dirty;
    bool selected; // Needed for rendering selection highlight
    
    // Pen properties
    RS2::LineWidth penWidth;
    RS_Color color;
    RS2::LineType linetype;
    
    // Basic geometry payload (to avoid pointer chasing or heap allocation for simple entities)
    double p1_x, p1_y;
    double p2_x, p2_y;
    double radius;
    double angle1, angle2;
    
    // Reserved for Phase 3 (VBO)
    unsigned int geometryOffset;
    unsigned int geometrySize;
    bool reversed;
    
    // Fallback for Phase 2 unsupported entities
    class RS_Entity* originalEntity;
};

#endif // LC_CACHEDENTITY_H
