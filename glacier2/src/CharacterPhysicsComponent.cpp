#include "StdAfx.h"
#include "PhysXPhysics.h"
#include "Engine.h"
#include "Exception.h"
#include "ServiceLocator.h"
#include "PhysicsScene.h"
#include "Graphics.h"
#include "GlacierMath.h"
#include "Character.h"
#include "World.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  using namespace physx;

  const Real cMaxSlopeDelta = 10.0f;

  CharacterPhysicsComponent::CharacterPhysicsComponent(
  World* world, const Vector3& position, const Real height, const Real radius ):
  mPosition( position ), mController( nullptr ), mMaterial( nullptr ),
  mWorld( world ), mHeight( height ), mRadius( radius )
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

  const Real CharacterPhysicsComponent::getOffsetFromGround()
  {
    return ( mHeight * 0.5f );
  }

  CharacterPhysicsComponent::GroundQuery CharacterPhysicsComponent::groundQuery( const Vector3& position )
  {
    GroundQuery query;
    PxRaycastHit hit;

    PxVec3 origin( position.x, position.y + cMaxSlopeDelta, position.z );
    PxVec3 direction( 0.0f, -1.0f, 0.0f );

    const PxHitFlags flags = PxHitFlag::eDEFAULT;
    PxQueryFilterData filter( PxQueryFlag::eSTATIC | PxQueryFlag::eDYNAMIC );
    if ( mScene->raycastSingle( origin, direction, cMaxSlopeDelta * 2.0f, flags, hit, filter ) )
    {
      query.hit = true;
      query.position = Math::pxVec3ToOgre( hit.position );
    }

    return query;
  }

  const PxControllerCollisionFlags& CharacterPhysicsComponent::move(
  const Vector3& displacement, const GameTime delta )
  {
    const PxF32 minimumDistance = 0.001f;

    PxControllerFilters filters;
    mCollisionFlags = mController->move(
      Math::ogreVec3ToPx( displacement ),
      minimumDistance, (PxF32)delta, filters, nullptr );

    return mCollisionFlags;
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