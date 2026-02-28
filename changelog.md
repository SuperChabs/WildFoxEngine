# Changelog

## Module Update
### v0.1.1
##### 08.02.2026
-- Add Rendering and Resource modules

### v0.1.2
##### 08.02.2026
-- Add UIModule add integrate into engine class

### v0.1.3
##### 09.02.2026
-- Add ECS module
-- Delete all legacy camera mantions

### v0.1.4
##### 12.02.2026
-- fixed errors related to UI module

### v0.1.5
##### 14.02.2026
-- Implemented input registration in Lua with functions for key and mouse events.
-- Created a cube controller script for movement and rotation using keyboard inputs.
-- Added a simple rotation script for continuous rotation of objects.
-- Established a SceneModule for scene serialization and management.

### v0.1.6
##### 15.02.2026
-- Introduced an EditorCommandHandler for managing editor commands.
-- Add EventBus class
-- Update SceneModule with adding in it SceneManager
-- Fix Camera movement
-- Will show only those components an entity has

### v0.1.7
##### 18.02.2026
-- Enchance mesh rendering
-- Fix gizmo rotation feedback loop

### v0.1.8
##### 20.02.2026
-- Bug fix

### v0.1.9
##### 25.02.2026
-- Updated ScriptPanel to change default script file extension from .lua to .as.
-- Enhanced UpdateScript method to properly release AngelScript context and module.
-- Modified TransformPanel to handle quaternion rotations using degrees.
-- Improved InspectorWindow to allow adding components via a popup menu.
-- Fixed MenuBarWindow to ensure scene loading only occurs if a valid path is provided.
-- Refactored Engine to initialize AngelScript instead of Lua, removing Lua-related code.
-- Updated EngineCommandHandler to accommodate changes in model loading and component addition.
-- Adjusted ModelLoader to return both model and root entity upon loading.
-- Changed ModelManager to return root entity when loading models with ECS.
-- Enhanced SceneManager to properly reset script components when entering play mode.
-- Updated SceneSerializer to handle model components and hierarchy during scene loading.
-- Removed Lua bindings and related files, transitioning to AngelScript bindings.
-- Added new AngelScript bindings and registration API for ECS and input handling.
-- Introduced a sample controller script for player movement using AngelScript.