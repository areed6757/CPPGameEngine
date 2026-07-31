#pragma once

// Core
#include <Core/Application.h>
#include <Core/Layer.h>
#include <Core/LayerStack.h>
#include <Core/Base.h>
#include <Core/Common.h>
#include <Core/Logger.h>
#include <Core/Core.h>

//GUI
#include <GUI/ImGUILayer.h>

// Events
#include <Events/Event.h>
#include <Events/KeyEvent.h>
#include <Events/MouseEvent.h>

// ECS
#include <ECS/ECSWrapper.h>
#include <ECS/Component.h>
#include <ECS/EntityRegister.h>
#include <ECS/TickedSystem.h>

// Utilities
#include <Utilities/Scheduler.h>
#include <Utilities/ThreadPool.h>
#include <Utilities/JobController.h>
#include <Utilities/GameClock.h>
#include <Utilities/CommandBuffer.h>
#include <Utilities/Job.h>

// Graphics
#include <Graphics/Renderer.h>
#include <Graphics/Camera.h>
#include <Graphics/Mesh.h>
#include <Graphics/Shader.h>
#include <Graphics/ComputeShader.h>
#include <Graphics/Window.h>
#include <Graphics/MeshID.h>
#include <Graphics/Texture.h>
#include <Graphics/TextureID.h>

// Input
#include <Input/ActionMap.h>
#include <Input/InputHandler.h>

// Physics
#include <Physics/Vector2double.h>
#include <Physics/Vector2float.h>
#include <Physics/Vector3double.h>
#include <Physics/Vector3float.h>
#include <Physics/AABBTree.h>

