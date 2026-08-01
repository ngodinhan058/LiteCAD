#ifndef LC_RTREESPATIALINDEX_H
#define LC_RTREESPATIALINDEX_H

#include "i_spatialindex.h"
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/index/rtree.hpp>

namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;

// Define point and box types for R-tree
typedef bg::model::point<double, 2, bg::cs::cartesian> RTreePoint;
typedef bg::model::box<RTreePoint> RTreeBox;
// Store bounding box and Entity ID
typedef std::pair<RTreeBox, unsigned int> RTreeValue;

class LC_RTreeSpatialIndex : public ISpatialIndex {
public:
    LC_RTreeSpatialIndex();
    ~LC_RTreeSpatialIndex() override;

    void insert(const CachedEntity& entity) override;
    void remove(unsigned int entityId) override;
    void clear() override;
    
    void query(const LC_Rect& viewport, std::vector<unsigned int>& outIds) const override;

private:
    bgi::rtree<RTreeValue, bgi::quadratic<16>> m_rtree;
    
    // Auxiliary map to quickly find an entity's old bounding box for removal
    // (Boost R-Tree requires the exact value that was inserted to remove it)
    std::unordered_map<unsigned int, RTreeBox> m_entityBounds;
};

#endif // LC_RTREESPATIALINDEX_H
