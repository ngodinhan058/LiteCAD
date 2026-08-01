# LibreCAD GPU Modernization Project

You are working on a long-term architectural modernization of LibreCAD 2.

This project is NOT a prototype.

Every implementation must be production-quality and intended to remain in the final architecture.

============================================================
PROJECT GOAL
============================================================

Transform LibreCAD 2 from a CPU-bound QPainter renderer into a modern GPU-accelerated CAD application while preserving all existing CAD functionality.

The project must remain fully compatible with the existing LibreCAD document model.

The renderer is replaceable.

The CAD engine is not.

============================================================
CORE PRINCIPLES
============================================================

1.

Never modify CAD behavior unless explicitly requested.

2.

Rendering must be completely independent from CAD logic.

3.

Never allow rendering code to access RS_Entity directly.

Rendering consumes CachedEntity only.

4.

Do not introduce temporary architectural shortcuts.

If CachedEntity lacks required rendering data,

extend CachedEntity.

Do NOT bypass it.

5.

Do not sacrifice architecture for convenience.

6.

Prefer maintainability over implementation speed.

7.

Minimize coupling.

Maximize modularity.

8.

Graphics APIs must remain isolated.

Only renderer modules may know OpenGL.

9.

Future rendering backends (Vulkan, Metal, DirectX) should be possible without modifying the CAD engine.

============================================================
ARCHITECTURE
============================================================

RS_Entity

↓

LC_GeometryBuilder

↓

CachedEntity

↓

LC_GeometryCache

↓

LC_GPUBufferManager

↓

IRenderer

↓

LC_OpenGLRenderer

↓

OpenGL

This architecture must remain valid throughout the project.

============================================================
RESPONSIBILITIES
============================================================

RS_Entity

CAD only.

Never rendering.

----------------------------------------

GeometryBuilder

Convert CAD objects into CachedEntity.

----------------------------------------

CachedEntity

The only rendering input.

Renderer must never access RS_Entity.

----------------------------------------

GeometryCache

Responsible for:

- adaptive tessellation
- vertex generation
- dirty tracking

Outputs:

std::vector<Vertex>

Graphics API independent.

Must not include OpenGL headers.

----------------------------------------

GPUBufferManager

Responsible for:

- GPU memory
- VBO ownership
- VAO ownership
- incremental upload

No CAD logic.

----------------------------------------

IRenderer

Graphics API abstraction.

Future implementations:

OpenGL

Vulkan

Metal

DirectX

----------------------------------------

OpenGLRenderer

Responsible only for:

initialize()

resize()

render()

bind shaders

bind buffers

draw calls

No CAD logic.

No tessellation.

No geometry generation.

============================================================
RENDERING RULES
============================================================

Do not create one giant VBO.

Use multiple GPU buffers.

Preferred:

Line

Polyline

Circle

Arc

Ellipse

Spline

Point

Incremental upload only.

============================================================
TESSELLATION
============================================================

Adaptive.

Never fixed.

Depends on:

radius

zoom

screen-space error

Performed only inside GeometryCache.

============================================================
VERTEX FORMAT
============================================================

Use compact vertex layout.

Preferred:

struct Vertex
{
    float x;
    float y;
    uint32_t color;
};

============================================================
REVISION SYSTEM
============================================================

Scene owns revision.

GeometryCache detects changes.

GPU uploads only dirty geometry.

Pan and zoom must not trigger geometry uploads.

============================================================
OUT OF SCOPE
============================================================

Do not implement unless explicitly requested:

PAT parser

Hatch improvements

Text rendering

Printing

Selection

Snap

Dimension optimization

Image rendering

Gradient hatch

============================================================
WORKFLOW
============================================================

For every requested phase:

1.

Review existing architecture.

2.

Identify required changes.

3.

Explain trade-offs.

4.

Produce implementation plan.

5.

Wait for approval.

6.

Implement only the approved phase.

Never continue into future phases without approval.

============================================================
OUTPUT REQUIREMENTS
============================================================

For every implementation provide:

Architecture summary

Modified files

New files

Build status

Remaining work

Known limitations

Next recommended phase

============================================================
MOST IMPORTANT RULE
============================================================

This project is building a modern CAD engine.

Do not optimize for writing code quickly.

Optimize for clean architecture that will still be correct years from now.