#include "StdAfx.h"
#include "PhysXPhysics.h"
#include "Engine.h"
#include "Exception.h"
#include "ServiceLocator.h"
#include "PhysicsScene.h"
#include "PhysicsDebugVisualizer.h"
#include "Graphics.h"
#include "GlacierMath.h"
#include "CharacterPhysicsComponent.h"
#include "World.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  using namespace physx;

  CharacterPhysicsComponent::CharacterPhysicsComponent( World* world, const Vector3& position ):
  mPosition( position ), mController( nullptr ), mMaterial( nullptr ), mWorld( world )
  {
    mScene = mWorld->getPhysics()->getScene();

    mMaterial = mScene->getPhysics().createMaterial( 0.5f, 0.5f, 0.1f );

    PxCapsuleControllerDesc descriptor;
    descriptor.setToDefault();
    descriptor.position = Math::ogreVec3ToPxExt( mPosition );
    descriptor.height = mHeight;
    descriptor.radius = mRadius;
    descriptor.contactOffset = 0.0125f;
    descriptor.stepOffset = 0.125f;
    descriptor.slopeLimit = 0.0f;
    descriptor.climbingMode = PxCapsuleClimbingMode::eEASY;
    descriptor.density = 10.0f;
    descriptor.behaviorCallback = nullptr;
    descriptor.reportCallback = nullptr;
    descriptor.material = mMaterial;
    descriptor.userData = nullptr;

    if ( !descriptor.isValid() )
      ENGINE_EXCEPT( "Invalid character controller descriptor" );

    mController = static_cast<PxCapsuleController*>(
      mWorld->getPhysics()->getControllerManager()->createController( descriptor )
    );

    if ( !mController )
      ENGINE_EXCEPT( "Could not create character controller" );
  }

  void CharacterPhysicsComponent::setPosition( const Vector3& position )
  {
    mPosition = position;
    mController->setPosition( Math::ogreVec3ToPxExt( mPosition ) );
  }

  PxControllerCollisionFlags CharacterPhysicsComponent::move(
  const Vector3& displacement, const GameTime delta )
  {
    const PxF32 minimumDistance = 0.001f;

    PxControllerFilters filters;
    auto flags = mController->move(
      Math::ogreVec3ToPx( displacement ),
      minimumDistance, (PxF32)delta, filters, nullptr );

    return flags;
  }

  void CharacterPhysicsComponent::update()
  {
    mPosition = Math::pxExtVec3ToOgre( mController->getPosition() );
  }

  CharacterPhysicsComponent::~CharacterPhysicsComponent()
  {
    SAFE_RELEASE_PHYSX( mController );
    SAFE_RELEASE_PHYSX( mMaterial );
  }

}