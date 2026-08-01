# Phase 3.1 – OpenGL Rendering Foundation

## Objective

Establish the OpenGL rendering foundation for the new GPU pipeline.

This phase must only prove that a CachedEntity can be rendered through the new renderer architecture.

## Scope

Implement only:

- LC_OpenGLCanvas
- IRenderer interface
- LC_OpenGLRenderer
- Runtime renderer switching
- Basic line rendering using CachedEntity
- OpenGL context initialization
- Shader compilation
- VAO/VBO setup for line rendering

## Constraints

- Do NOT implement GeometryCache.
- Do NOT implement GPUBufferManager.
- Do NOT implement adaptive tessellation.
- Do NOT implement circles, arcs, ellipses, splines, or points.
- Do NOT implement any optimization.
- Do NOT modify CAD behavior.
- Do NOT access RS_Entity directly from the renderer.

## Success Criteria

- Project builds successfully.
- Application launches.
- Renderer can switch between QPainter and OpenGL.
- Line entities render correctly using CachedEntity.
- Existing LibreCAD functionality remains unchanged.

## Deliverables

- Architecture summary
- New files created
- Existing files modified
- Build result
- Remaining work for Phase 3.2

Stop immediately after Phase 3.1 is complete. Do not continue into future phases.