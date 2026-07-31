#pragma once
#include <Engine.h>
#include <GameECS.h>

// Ship Stuff
#include <Ships/PartTypes.h>
#include <Ships/PartVariant.h>
#include <Ships/PartRegistry.h>
#include <Ships/ShipGrid.h>
#include <Ships/ShipCollisionGeometry.h>
#include <Ships/ShipFactory.h>

// Components
#include <Components/Position.h>
#include <Components/Movement.h>
#include <Components/Physics.h>
#include <Components/Renderable.h>
#include <Components/Thruster.h>
#include <Components/Health.h>
#include <Components/DamagePayload.h>
#include <Components/Lifetime.h>
#include <Components/Weapon.h>
#include <Components/Mount.h>
#include <Components/MovementDamper.h>
#include <Components/ShipVisual.h>
#include <Components/Stability.h>

// Graphics
#include <Graphics/TextureRegistry.h>
#include <Graphics/MeshRegistry.h>

// Systems
#include <Systems/CollisionSystem.h>
#include <Systems/LifetimeSystem.h>
#include <Systems/CameraController.h>
#include <Systems/MovementTicks.h>
#include <Systems/ImpulseSystem.h>
#include <Systems/ThrusterSystem.h>
#include <Systems/DamageSystem.h>
#include <Systems/RenderSystem.h>
#include <Systems/ParticleSystem.h>
#include <Systems/WeaponSystem.h>
#include <Systems/DamperSystem.h>
#include <Systems/MountFollowSystem.h>
#include <Systems/MountLifecycleSystem.h>
#include <Systems/PartRenderSystem.h>

// Tests
#include <Test/CoreSystemsTest.h>
#include <Test/EntityStressTest.h>
#include <Test/ThreadingStressTest.h>
#include <Test/WeaponTest.h>
#include <Test/DamperTest.h>
#include <Test/ShipCollisionTest.h>