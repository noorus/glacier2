#pragma once
#include "Types.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  class World;

  class CharacterPhysicsComponent {
  protected:
    World* mWorld;
    Real mHeight;
    Real mRadius;
    physx::PxScene* mScene;
    physx::PxCapsuleController* mController;
    physx::PxMaterial* mMaterial;
    Vector3 mPosition;
  public:
    CharacterPhysicsComponent( World* world, const Vector3& position );
    virtual const Real getHeight() const throw() { return mHeight; }
    virtual const Real getRadius() const throw() { return mRadius; }
    virtual PxCapsuleController* const getController() const throw() { return mController; }
    virtual PxMaterial* const getMaterial() const throw() { return mMaterial; }
    virtual const Vector3& getPosition() const throw() { return mPosition; }
    virtual void setPosition( const Vector3& position );
    PxControllerCollisionFlags move( const Vector3& displacement, const GameTime delta );
    virtual void update();
    virtual ~CharacterPhysicsComponent();
  };

}