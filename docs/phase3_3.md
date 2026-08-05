# Phase 3.3: Curve Primitive Support

Implement Phase 3.3 according to the existing architecture.

Architecture must remain:

LC_CachedScene
    ↓
LC_GeometryCache
    ↓
LC_GPUBufferManager
    ↓
LC_OpenGLRenderer

============================================================
OBJECTIVE
============================================================

Extend the GeometryCache so it can generate CPU-side geometry for additional CAD primitives.

No architectural changes.

No rendering pipeline changes.

Only extend supported entity types.

============================================================
IMPLEMENT
============================================================

Support:

- RS_Polyline
- RS_Circle
- RS_Arc
- RS_Ellipse
- RS_Spline (if CachedEntity already provides sufficient data)

Generate line-segment geometry only.

All generated geometry remains CPU-side.

GeometryCache outputs only:

std::vector<Vertex>

============================================================
TESSELLATION
============================================================

Implement simple tessellation.

Do NOT implement zoom-dependent adaptive tessellation.

Choose reasonable fixed segment counts based on primitive geometry.

Favor correctness and visual quality over aggressive optimization.

============================================================
DO NOT MODIFY
============================================================

Do not modify:

- LC_OpenGLRenderer architecture
- LC_GPUBufferManager architecture
- CachedScene architecture
- synchronization model

============================================================
OUT OF SCOPE
============================================================

Do NOT implement:

- PAT parser
- Hatch improvements
- Text rendering
- Image rendering
- Dimension rendering
- GPU tessellation
- Instancing
- Compute shaders
- Multi-threading

============================================================
VERIFICATION
============================================================

Verify that all newly supported primitives render correctly and match the existing QPainter backend visually.

Do not begin implementation until the implementation plan has been reviewed and approved.