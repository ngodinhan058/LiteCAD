#include "lc_rtreespatialindex.h"
#include "lc_cachedentity.h"

LC_RTreeSpatialIndex::LC_RTreeSpatialIndex() = default;

LC_RTreeSpatialIndex::~LC_RTreeSpatialIndex() = default;

void LC_RTreeSpatialIndex::insert(const CachedEntity& entity) {
    if (!entity.visible) return;

    RTreeBox box(RTreePoint(entity.boundingBox.minP().x, entity.boundingBox.minP().y),
                 RTreePoint(entity.boundingBox.maxP().x, entity.boundingBox.maxP().y));
    
    m_rtree.insert(std::make_pair(box, entity.id));
    m_entityBounds[entity.id] = box;
}

void LC_RTreeSpatialIndex::remove(unsigned int entityId) {
    auto it = m_entityBounds.find(entityId);
    if (it != m_entityBounds.end()) {
        m_rtree.remove(std::make_pair(it->second, entityId));
        m_entityBounds.erase(it);
    }
}

void LC_RTreeSpatialIndex::clear() {
    m_rtree.clear();
    m_entityBounds.clear();
}

void LC_RTreeSpatialIndex::query(const LC_Rect& viewport, std::vector<unsigned int>& outIds) const {
    RTreeBox queryBox(RTreePoint(viewport.minP().x, viewport.minP().y),
                      RTreePoint(viewport.maxP().x, viewport.maxP().y));
    
    // Perform a spatial query finding all entities intersecting the viewport
    for (auto it = m_rtree.qbegin(bgi::intersects(queryBox)); it != m_rtree.qend(); ++it) {
        outIds.push_back(it->second);
    }
}
