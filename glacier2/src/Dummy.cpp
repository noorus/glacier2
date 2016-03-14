#include "StdAfx.h"
#include "Engine.h"
#include "ServiceLocator.h"
#include "Graphics.h"
#include "World.h"
#include "Character.h"
#include "Dummy.h"
#include "AIState.h"
#include "AIFiniteStateMachine.h"
#include "EntityManager.h"

// Glacier� Game Engine � 2014 noorus
// All rights reserved.

namespace Glacier {

  ENGINE_DECLARE_ENTITY( dev_dummy, Dummy );

  class DummyAlertState: public AI::State {
  public:
    void enter( AI::FiniteStateMachine* machine, AI::Agent* agent )
    {
      AI::State::enter( machine, agent );
      auto dummy = (Dummy*)agent;
      dummy->getFOVCone().setAlert( true );
      dummy->getInput()->injectJump();
    }
    void execute( AI::FiniteStateMachine* machine, AI::Agent* agent, const GameTime delta )
    {
      AI::State::execute( machine, agent, delta );
      auto dummy = (Dummy*)agent;
      auto player = dummy->getWorld()->getEntities()->findByName( "player" );
      if ( !player || !dummy->canSee( player ) )
        machine->popState();
    }
  };

  static DummyAlertState dummyAlertState;

  class DummyIdleState: public AI::State {
  public:
    void enter( AI::FiniteStateMachine* machine, AI::Agent* agent )
    {
      AI::State::enter( machine, agent );
      auto dummy = (Dummy*)agent;
      dummy->getFOVCone().setAlert( false );
    }
    void execute( AI::FiniteStateMachine* machine, AI::Agent* agent, const GameTime delta )
    {
      AI::State::execute( machine, agent, delta );
      auto dummy = (Dummy*)agent;
      auto player = dummy->getWorld()->getEntities()->findByName( "player" );
      if ( player && dummy->canSee( player ) )
        machine->pushState( &dummyAlertState );
    }
  };

  static DummyIdleState dummyIdleState;

  Dummy::Dummy( World* world ):
  Character( world, &baseData, new AICharacterInputComponent( this ) ),
  AI::Agent(),
  mItem( nullptr ), mStates( this )
  {
    mEyePosition = Vector3( 0.0f, 0.5f, 0.0f );
    mFieldOfView = Radian( Ogre::Degree( 50.0f ) );
    mViewDistance = 8.0f;
    mStates.pushState( &dummyIdleState );
    mHeight = 0.8f;
    mRadius = 0.2f;
  }

  AICharacterInputComponent* Dummy::getInput()
  {
    return (AICharacterInputComponent*)mInput;
  }

  FOVCone& Dummy::getFOVCone()
  {
    return mFovCone;
  }

  void Dummy::spawn( const Vector3& position, const Quaternion& orientation )
  {
    Character::spawn( position, orientation );

    mMesh = Procedural::CapsuleGenerator( mRadius, mHeight, 8, 16, 1 ).realizeMesh();

    mItem = Locator::getGraphics().getScene()->createItem( mMesh );
    mItem->setQueryFlags( SceneQueryFlag_Character );
    mItem->setDatablock( "Developer/Placeholder/Dummy" );
    mItem->setCastShadows( true );
    mNode->attachObject( mItem );

    mFovCone.set( mViewDistance, mFieldOfView );

    mEyeNode = Locator::getGraphics().getScene()->createSceneNode();
    mEyeNode->addChild( mFovCone.node );
    mEyeNode->setPosition( mEyePosition );
    mEyeNode->setDirection( Vector3::NEGATIVE_UNIT_Z, Ogre::Node::TS_PARENT );

    mNode->addChild( mEyeNode );
  }

  void Dummy::think( const GameTime delta )
  {
    mStates.execute( delta );
    Character::think( delta );
    Quaternion qt;
    qt.FromAngleAxis( Degree( 0.5f ), Vector3::UNIT_Y );
    mFacing = qt * mFacing;
    mNode->setDirection( mFacing, Ogre::Node::TS_WORLD );
  }

  Ogre::MovableObject* Dummy::getMovable()
  {
    return mItem;
  }

  Dummy::~Dummy()
  {
    mEyeNode->removeAllChildren();
    if ( mItem )
      Locator::getGraphics().getScene()->destroyItem( mItem );
    if ( !mMesh.isNull() )
      Ogre::MeshManager::getSingleton().remove( mMesh->getHandle() );
  }

}