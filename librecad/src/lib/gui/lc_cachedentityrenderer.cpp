#include "lc_cachedentityrenderer.h"
#include "../engine/lc_cachedentity.h"
#include "rs_painter.h"
#include "rs_graphicview.h"
#include "rs_entity.h"

void LC_CachedEntityRenderer::render(RS_Painter* painter, const CachedEntity* entity, RS_GraphicView* view) {
    if (!entity || !entity->visible) return;

    // Set the pen properties from the cached entity
    RS_Pen pen(entity->color, entity->penWidth, entity->linetype);
    painter->setPen(pen);

    // Depending on if the entity is selected, we might want to highlight it.
    // In LibreCAD, RS_Painter handles highlight styling if we ask it to,
    // but for now we'll just draw the geometry with the pen we created.

    switch (entity->type) {
        case RS2::EntityLine:
            painter->drawLine(RS_Vector(entity->p1_x, entity->p1_y),
                              RS_Vector(entity->p2_x, entity->p2_y));
            break;
            
        case RS2::EntityCircle:
            painter->drawCircle(RS_Vector(entity->p1_x, entity->p1_y), entity->radius);
            break;
            
        case RS2::EntityArc:
            // Assuming false for reversed by default for arcs
            painter->drawArc(RS_Vector(entity->p1_x, entity->p1_y), entity->radius,
                             entity->angle1, entity->angle2, false);
            break;
            
        default:
            // Temporary compatibility path for unsupported entities (Hatch, Text, etc.)
            if (entity->originalEntity && view) {
                double dummyOffset = 0.0;
                entity->originalEntity->draw(painter, view, dummyOffset);
            }
            break;
    }
}
