#ifndef LC_GEOMETRYBUILDER_H
#define LC_GEOMETRYBUILDER_H

#include "lc_cachedentity.h"

class RS_Entity;

/**
 * @brief Converts RS_Entity into CachedEntity.
 * 
 * Extracts only the rendering metadata and simple geometry needed for visualization.
 * This builder is completely stateless, thread-safe, and independent of any GUI logic.
 */
class LC_GeometryBuilder {
public:
    static CachedEntity build(RS_Entity* entity);
};

#endif // LC_GEOMETRYBUILDER_H
