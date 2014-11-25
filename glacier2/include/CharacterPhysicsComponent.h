#pragma once
#include "Types.h"
#include "Entity.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  class World;

  class Character: public Entity {
  protected:
    enum Flags {
      Flag_On_Ground = 0
    };
    std::bitset<8> mFlags;
  public:
    virtual void onHitGround();
    virtual void onLeaveGround();
    virtual const bool isOnGround();
  };

  class CharacterJump {
  protected:
    bool mJumping;
    bool mInAir;
    Vector3 mBaseVelocity;
    Vector3 mVelocity;
  public:
    CharacterJump(): mJumping( false ), mInAir( false ) {}
    virtual void begin( Vector3& velocity, Vector3& directional, const GameTime time );
    virtual void generate( Vector3& velocity, const GameTime time );
    virtual void landed( Vector3& velocity );
    virtual void end();
    inline const bool jumping() const throw() { return mJumping; }
    inline const bool inAir() const throw() { return mInAir; }
  };

  struct CharacterMoveData {
  public:
    enum ControlMode {
      ControlMode_Impulse,
      ControlMode_Directional
    } mode;
    enum Affector {
      Affector_Forward = 0,
      Affector_Backward,
      Affector_Left,
      Affector_Right,
      Affector_Run,
      Affector_Crouch
    };
    std::bitset<8> affectors;
    Real forward;
    Real backward;
    Real left;
    Real right;
    Real run;
    Real crouch;
    Real speed;
    Vector3 direction;
    Vector2 directional;
    Vector3 facing;
    Vector3 velocity;
    enum MoveStatus {
      Move_Idle,
      Move_Walking,
      Move_Running,
      Move_In_Air
    } moveStatus;
    enum CrouchStatus {
      Crouch_None,
      Crouch_Crouching,
      Crouch_Crouched
    } crouchStatus;
    bool jumpImpulse;
    CharacterJump jump;
  };

  class CharacterPhysicsComponent {
  protected:
    World* mWorld;
    Real mHeight;
    Real mRadius;
    physx::PxScene* mScene;
    physx::PxCapsuleController* mController;
    physx::PxMaterial* mMaterial;
    Vector3 mPosition;
    physx::PxControllerCollisionFlags mCollisionFlags;
    struct GroundQuery {
      bool hit;
      Vector3 position;
      GroundQuery(): hit( false ), position( Vector3::ZERO ) {}
    };
  public:
    CharacterPhysicsComponent( World* world, const Vector3& position, const Real height, const Real radius );
    virtual const Real getHeight() const throw() { return mHeight; }
    virtual const Real getRadius() const throw() { return mRadius; }
    virtual physx::PxCapsuleController* const getController() const throw() { return mController; }
    virtual physx::PxMaterial* const getMaterial() const throw( ) { return mMaterial; }
    virtual const Vector3& getPosition() const throw() { return mPosition; }
    virtual void setPosition( const Vector3& position );
    virtual const physx::PxControllerCollisionFlags& move( const Vector3& displacement, const GameTime delta );
    virtual const physx::PxControllerCollisionFlags& getLastCollisionFlags() const throw( ) { return mCollisionFlags; }
    virtual GroundQuery groundQuery( const Vector3& position );
    virtual const Real getOffsetFromGround();
    virtual void update();
    virtual ~CharacterPhysicsComponent();
  };

  class CharacterMovementComponent {
  protected:
    Character* mCharacter;
    Vector3 mDisplacement;
    Vector3 mPosition;
    Vector3 mVelocity;
  public:
    CharacterMovementComponent( Character* character );
    void generate( CharacterMoveData& move, const GameTime time, CharacterPhysicsComponent* physics );
  };

}