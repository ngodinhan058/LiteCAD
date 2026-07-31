# GPU Acceleration Architecture for LibreCAD

This document outlines the finalized architectural design and migration roadmap for introducing a robust, scalable GPU-accelerated rendering pipeline into LibreCAD. The goal is to support rendering millions of entities smoothly while establishing a software architecture capable of sustaining the next 10+ years of CAD development.

---

## 1. Rendering Architecture: Composition over Inheritance

We must ensure that the transition to a hardware-accelerated viewport poses minimal risk to the extensive existing CAD logic, event handling, and plugin ecosystem.

**Alternative Proposal: The Canvas Strategy (Recommended)**
```text
MainWindow
   └── RS_GraphicView (QWidget - Existing)
         ├── RenderCanvas (Interface / Abstract Surface)
         │     ├── QWidgetCanvas (Legacy QPainter support)
         │     └── OpenGLCanvas (QOpenGLWidget)
         └── CAD Logic, Event Handling, Overlays
```

**Comparison:**
- **Migration Risk & Plugin Compatibility**: Changing `RS_GraphicView` to inherit `QOpenGLWidget` (Option A) would subtly alter event routing, print/export pipelines, and potentially break plugins that expect standard QWidget behaviors. Option B eliminates this risk by treating the canvas purely as a child rendering surface.
- **Event Handling**: In Option B, `RS_GraphicView` intercepts all mouse/keyboard events. The `RenderCanvas` only receives resize and paint requests, keeping CAD logic entirely isolated from graphics API quirks.
- **Future Extensibility**: Option B makes it vastly easier to implement `VulkanCanvas` or `MetalCanvas` later, as the graphics surface is strictly an interchangeable component, rather than the backbone of the CAD view.

**Verdict**: **Option B** is vastly superior for LibreCAD.

---

## 2. Total Decoupling: The `CachedEntity` Pipeline

The GPU renderer must be completely isolated from the CAD object model. The renderer will never directly access or read an `RS_Entity`.

**The Pipeline:**
```text
DXF -> RS_Entity -> Geometry Builder -> CachedEntity -> GPU Renderer
```
The renderer will operate *exclusively* on the `CachedEntity` objects and the `ISpatialIndex`.

**Metadata Contained in CachedEntity:**
- Bounding Box
- Layer ID, Linetype ID, Color
- Vertex Offset, Vertex Count
- Visibility Flags, Dirty Flags

**Analysis:**
- **Advantages**: Perfect separation of concerns. The renderer becomes a high-performance black box that only consumes plain-old-data (POD). Thread safety is improved because background threads can extract geometry into the cache without locking the renderer.
- **Disadvantages & Memory Overhead**: Minor memory duplication (storing bounding boxes and state twice). However, `CachedEntity` is so lightweight (a few dozen bytes) that 1,000,000 entities consume less than 50MB of RAM—a negligible cost for the massive performance gains.
- **Synchronization & Editing Workflow**: When `RS_Entity` changes, CAD logic flags it dirty. A synchronization pass (pre-render) invokes the Geometry Builder to regenerate the `CachedEntity` and GPU vertices. The cache is immediately invalidated and re-synced before the renderer issues draw calls.

---

## 3. VBO Organization Strategy

Managing how geometry is stored in GPU memory is critical for scaling to massive drawings with thousands of layers.

**Option C: Hybrid Allocator / Shared Buffers (Recommended)**
- **How it works**: We use a few massive VBOs (e.g., 64MB chunks). Each layer is represented by *dynamic sub-ranges* (Offsets & Sizes) within these shared buffers.
- **Draw Calls & Layer Toggling**: When a layer is toggled off, the CPU simply omits its sub-range from the draw list. We achieve the benefit of skipping invisible geometry without the overhead of thousands of VBO bindings.
- **Memory Fragmentation**: If an entity changes size, the memory block is marked free, and new data is appended. A simple free-list manages fragmentation.
- **Scalability**: Unlike Option A (One VBO per layer), which creates severe OpenGL overhead for files with 10,000+ layers, Option C scales flawlessly. It minimizes state changes while maintaining precise visibility control.

---

## 4. Buffer Update Strategy

We will evolve the GPU memory management iteratively to minimize initial project risk.

**Phase 4 (Early Iteration): `glBufferSubData`**
- **Approach**: Synchronous, straightforward memory updates. When geometry changes, we upload the new bytes using `glBufferSubData`.
- **Why**: It is universally supported, easiest to debug, and fast enough for initial development. Introducing complex asynchronous mapping early on often leads to opaque driver-level sync bugs.

**Phase 7 (Advanced Optimization): Persistent Mapped Buffers & Orphaning**
- **Persistent Mapped Buffers (GL_ARB_buffer_storage)**: The holy grail of zero-overhead updates. We map the GPU memory permanently into CPU address space. The CPU writes directly to VRAM bypassing driver copies.
- **Buffer Orphaning (`glBufferData` with NULL)**: If the dynamic buffer for active drawing (e.g., crosshairs, active splines) fills up or stalls, we orphan the buffer and get a fresh memory block from the driver instantly, eliminating GPU pipeline stalls.
- **Ring Buffers**: For constantly streaming data (like selection boxes), a ring buffer ensures the CPU never waits for the GPU to finish reading a specific region.

---

## 5. Abstracting the Spatial Index (`ISpatialIndex`)

To efficiently cull entities before sending them to the renderer, we introduce an `ISpatialIndex` interface.
- **QuadTree**: Good generic structure, but suffers with heavily overlapping geometry.
- **Uniform Grid**: O(1) lookup but scales poorly for massive variance in scale (dense clustered drawings vs sparse elements).
- **R-Tree (Recommended)**: Perfectly encapsulates overlapping bounding boxes, which is the standard topology for 2D CAD files. The architecture will default to an R-Tree but remain swappable via the `ISpatialIndex` interface.

---

## 6. Migration Roadmap

### Phase 0: Extensive Benchmarking
Implement an automated benchmark suite measuring:
- FPS, Average Frame Time, 95th & 99th Percentile Frame Times.
- GPU Draw Time vs CPU Render Time.
- Number of Draw Calls, Visible Entities, Culled Entities.
- GPU & CPU Memory Usage.
- Latency for Snapping, Selection, and File Load.

### Phase 1: `RenderCanvas` & Architecture Setup
- Refactor `RS_GraphicView` to use the `RenderCanvas` composition pattern.
- Implement `QWidgetCanvas` as the legacy fallback.
- Implement `OpenGLCanvas` with a blank accelerated context.
- Verify zero behavioral changes and benchmark against Phase 0.

### Phase 2: Total Decoupling (`CachedEntity`)
- Implement the `CachedEntity` metadata layer and `Geometry Builder`.
- Implement `ISpatialIndex` (R-Tree) operating exclusively on `CachedEntity`.

### Phase 3: Geometry Extraction & Hybrid VBO Allocation
- Implement GPU extraction for Lines.
- Implement the VBO Manager using the **Hybrid Allocator** (large shared VBOs with layer-based offset tracking).
- Render static geometry on the GPU.

### Phase 4: Incremental Updates (`glBufferSubData`)
- Implement the Dirty Flag synchronization loop.
- Use `glBufferSubData` to update modified geometry.
- Manage free-lists to prevent fragmentation.

### Phase 5: Complex Entities & Text
- CPU-tessellate Arcs, Circles, Splines, and Stroke Fonts into the `Geometry Builder`.
- Output as line segments to the VBO to guarantee visual parity and plotting compatibility.

### Phase 6: Selection & Interaction Overlays
- Implement selection highlighting via uniform overrides or secondary passes.
- Maintain a dynamic, non-cached VBO for active drafting previews.

### Phase 7: Advanced Renderer Optimization
- Upgrade buffer management to **Persistent Mapped Buffers** and **Ring Buffers**.
- Implement **Buffer Orphaning** for dynamic geometry.
- Move from multiple `glDrawArrays` to **Multi Draw Indirect** (MDI) to batch layer ranges into a single draw call.

## User Review Required
> [!IMPORTANT]
> The architecture has been refined to include the `RenderCanvas` composition pattern, total `RS_Entity` decoupling, hybrid VBO allocation, progressive memory update strategies, and comprehensive benchmarking. Please review this finalized design. If approved, we are fully prepared to commence Phase 0.
