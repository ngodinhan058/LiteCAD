#ifndef I_SPATIALINDEX_H
#define I_SPATIALINDEX_H

#include <vector>
#include "lc_rect.h"

struct CachedEntity;

/**
 * @brief Abstract interface for spatial indexing.
 * 
 * Provides a renderer-agnostic way to query visible entities.
 */
class ISpatialIndex {
public:
    virtual ~ISpatialIndex() = default;
    
    virtual void insert(const CachedEntity& entity) = 0;
    virtual void remove(unsigned int entityId) = 0;
    virtual void clear() = 0;
    
    // Populates outIds with the IDs of entities intersecting the viewport
    virtual void query(const LC_Rect& viewport, std::vector<unsigned int>& outIds) const = 0;
};

#endif // I_SPATIALINDEX_H
