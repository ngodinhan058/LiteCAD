# Phase 3.2 – Geometry Cache & GPU Buffer Management

## Objective

Implement the CPU-side geometry cache and GPU buffer management layer according to the project architecture.

This phase establishes the data pipeline between `LC_CachedScene` and the OpenGL renderer.

The objective is to eliminate full-scene GPU uploads and prepare the renderer for incremental updates.

---

## Prerequisites

The following components must already be completed and working:

* Phase 3.1
* `LC_OpenGLCanvas`
* `IRenderer`
* `LC_OpenGLRenderer`
* Runtime renderer switching
* Basic line rendering from `CachedEntity`

---

## Scope

Implement only:

* `LC_GeometryCache`
* `LC_GPUBufferManager`
* Scene revision tracking
* Dirty geometry detection
* Incremental GPU upload
* Line primitive support

Do **not** implement any future features.

---

## Components

### LC_GeometryCache

Responsibilities:

* Consume `CachedEntity`
* Generate CPU-side vertex arrays
* Track dirty entities
* Track scene revisions
* Produce graphics API independent geometry

Output:

```cpp
std::vector<Vertex>
```

The GeometryCache must not include:

* OpenGL
* QOpenGLBuffer
* QOpenGLFunctions
* Shader code

GeometryCache must remain graphics API independent.

---

### LC_GPUBufferManager

Responsibilities:

* Own GPU buffers
* Create and destroy VBOs
* Upload dirty geometry only
* Manage GPU memory
* Prepare buffers for rendering

The GPUBufferManager must not:

* Generate geometry
* Tessellate curves
* Access CAD entities

---

## GPU Buffer Organization

Do not create one giant VBO.

Use separate buffers for each primitive type.

At this phase only:

* LineVBO

Future phases will introduce:

* PolylineVBO
* CircleVBO
* ArcVBO
* EllipseVBO
* SplineVBO
* PointVBO

---

## Revision System

Use revision-based synchronization.

Requirements:

* `LC_CachedScene` owns a revision counter.
* GeometryCache detects revision changes.
* Only dirty geometry is regenerated.
* Only dirty GPU buffers are uploaded.

Pan and zoom must never trigger geometry uploads.

---

## Vertex Format

Use the project standard vertex format.

```cpp
struct Vertex
{
    float x;
    float y;
    uint32_t color;
};
```

Avoid floating-point RGBA values.

---

## Out of Scope

Do not implement:

* Adaptive tessellation
* Circle rendering
* Arc rendering
* Ellipse rendering
* Polyline rendering
* Spline rendering
* Point rendering
* PAT parser
* Hatch improvements
* Text rendering
* Printing
* Selection optimization
* Snap optimization

These belong to later phases.

---

## Deliverables

Provide:

1. Architecture summary
2. New classes
3. Modified files
4. Build status
5. Performance impact
6. Remaining work for Phase 3.3

---

## Success Criteria

The implementation is complete when:

* GeometryCache is operational.
* GPUBufferManager is operational.
* Only line primitives are supported.
* Dirty geometry uploads work correctly.
* Full-scene uploads are avoided whenever possible.
* The project builds successfully.
* Rendering output remains identical to Phase 3.1.

---

## Stop Condition

After all objectives above are completed:

Stop.

Do not continue to Phase 3.3.

Wait for review and approval before implementing the next phase.
