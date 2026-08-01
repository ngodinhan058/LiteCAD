#include "lc_geometrybuilder.h"
#include "rs_entity.h"
#include "rs_line.h"
#include "rs_circle.h"
#include "rs_arc.h"
#include "rs_insert.h"
#include <cmath>
#include "rs_layer.h"
#include "rs_pen.h"

CachedEntity LC_GeometryBuilder::build(RS_Entity* entity) {
    CachedEntity ce;
    
    // Safety check
    if (!entity) {
        ce.visible = false;
        return ce;
    }
    
    RS_Vector minP = entity->getMin();
    RS_Vector maxP = entity->getMax();
    
    if (!minP.valid || !maxP.valid || 
        std::isnan(minP.x) || std::isnan(minP.y) || 
        std::isnan(maxP.x) || std::isnan(maxP.y)) {
        ce.visible = false;
        return ce;
    }
    
    ce.id = entity->getId();
    ce.type = entity->rtti();
    ce.boundingBox = LC_Rect(minP, maxP);
    
    RS_Pen pen = entity->getPen(true);
    ce.color = pen.getColor();
    ce.linetype = pen.getLineType();
    ce.penWidth = pen.getWidth();
    
    ce.visible = entity->isVisible();
    ce.dirty = false;
    ce.selected = entity->isSelected();
    
    ce.geometryOffset = 0;
    ce.geometrySize = 0;
    ce.originalEntity = entity;
    
    // Extract geometry
    switch (ce.type) {
        case RS2::EntityLine: {
            auto* line = static_cast<RS_Line*>(entity);
            ce.p1_x = line->getStartpoint().x;
            ce.p1_y = line->getStartpoint().y;
            ce.p2_x = line->getEndpoint().x;
            ce.p2_y = line->getEndpoint().y;
            break;
        }
        case RS2::EntityCircle: {
            auto* circle = static_cast<RS_Circle*>(entity);
            ce.p1_x = circle->getCenter().x;
            ce.p1_y = circle->getCenter().y;
            ce.radius = circle->getRadius();
            break;
        }
        case RS2::EntityArc: {
            auto* arc = static_cast<RS_Arc*>(entity);
            ce.p1_x = arc->getCenter().x;
            ce.p1_y = arc->getCenter().y;
            ce.radius = arc->getRadius();
            ce.angle1 = arc->getAngle1();
            ce.angle2 = arc->getAngle2();
            break;
        }
        default:
            // For unsupported entities (Hatch, Text, etc.), we don't extract payload.
            // They will be drawn using the originalEntity pointer fallback in Phase 2.
            break;
    }
    
    return ce;
}
