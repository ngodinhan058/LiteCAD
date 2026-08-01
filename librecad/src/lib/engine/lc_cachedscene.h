#ifndef LC_CACHEDSCENE_H
#define LC_CACHEDSCENE_H

#include <unordered_map>
#include <vector>
#include <memory>
#include "lc_cachedentity.h"

class ISpatialIndex;
class RS_Entity;

/**
 * @brief Manages a collection of CachedEntity objects and their spatial index.
 * 
 * Thread-safe foundation (assuming proper locks are added later).
 * The renderer accesses this scene instead of iterating RS_Entity directly.
 */
class LC_CachedScene {
public:
    LC_CachedScene();
    ~LC_CachedScene();

    /**
     * @brief Populate cache from an existing container.
     */
    void populate(class RS_EntityContainer* container);

    /**
     * @brief Clear the entire cache.
     */
    void clear();

    /**
     * @brief Synchronize a single entity from the CAD model.
     * 
     * If the entity is marked deleted/invisible, it's removed.
     * Otherwise it's built via GeometryBuilder and inserted.
     */
    void notifyEntityChanged(RS_Entity* entity);
    
    /**
     * @brief Synchronize removal of an entity by ID.
     */
    void notifyEntityRemoved(unsigned int entityId);

    /**
     * @brief Query visible entities intersecting the viewport.
     * @param viewport The area to query.
     * @param outEntities Populated with pointers to the cached entities.
     */
    void getVisibleEntities(const LC_Rect& viewport, std::vector<CachedEntity>& outEntities) const;

private:
    std::unordered_map<unsigned int, CachedEntity> m_entities;
    std::unique_ptr<ISpatialIndex> m_spatialIndex;
};

#endif // LC_CACHEDSCENE_H
