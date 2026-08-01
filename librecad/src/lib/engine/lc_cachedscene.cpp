#include "lc_cachedscene.h"
#include "lc_rtreespatialindex.h"
#include "lc_geometrybuilder.h"
#include "rs_entity.h"
#include "rs_entitycontainer.h"

LC_CachedScene::LC_CachedScene() {
    m_spatialIndex = std::make_unique<LC_RTreeSpatialIndex>();
}

LC_CachedScene::~LC_CachedScene() = default;

void LC_CachedScene::clear() {
    m_entities.clear();
    m_spatialIndex->clear();
    m_revision++;
}

void LC_CachedScene::populate(RS_EntityContainer* container) {
    clear();
    if (!container) return;

    // Traverse all entities at the top level of this container
    for (RS_Entity* entity = container->firstEntity(RS2::ResolveNone);
         entity != nullptr;
         entity = container->nextEntity(RS2::ResolveNone)) {
         
         notifyEntityChanged(entity);
    }
    m_revision++;
}

void LC_CachedScene::notifyEntityChanged(RS_Entity* entity) {
    if (!entity) return;
    
    unsigned int id = entity->getId();
    
    // First remove from index if it exists
    m_spatialIndex->remove(id);
    
    // Build new cached entity
    CachedEntity ce = LC_GeometryBuilder::build(entity);
    
    if (ce.visible) {
        m_entities[id] = ce;
        m_spatialIndex->insert(ce);
    } else {
        // If it's not visible (or deleted, or not implemented), erase it
        m_entities.erase(id);
    }
    m_revision++;
}

void LC_CachedScene::notifyEntityRemoved(unsigned int entityId) {
    m_spatialIndex->remove(entityId);
    m_entities.erase(entityId);
    m_revision++;
}

void LC_CachedScene::getVisibleEntities(const LC_Rect& viewport, std::vector<CachedEntity>& outEntities) const {
    std::vector<unsigned int> visibleIds;
    // Reserve an arbitrary capacity to minimize reallocation during querying
    visibleIds.reserve(2000); 
    
    m_spatialIndex->query(viewport, visibleIds);
    
    for (unsigned int id : visibleIds) {
        auto it = m_entities.find(id);
        if (it != m_entities.end()) {
            outEntities.push_back(it->second);
        }
    }
}
