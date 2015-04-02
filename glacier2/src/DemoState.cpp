#include "StdAfx.h"
#include "DemoState.h"
#include "Engine.h"
#include "Game.h"
#include "Director.h"
#include "Camera.h"
#include "Graphics.h"
#include "Engine.h"
#include "ModelViewer.h"
#include "ServiceLocator.h"
#include "Exception.h"
#include "Win32.h"
#include "PhysXPhysics.h"
#include "PhysicsScene.h"
#include "PhysicsDebugVisualizer.h"
#include "WorldPrimitives.h"
#include "EntityManager.h"
#include "World.h"
#include "FMODMusic.h"
#include "DeveloperEntities.h"
#include "Navigation.h"
#include "GlacierMath.h"
#include "InputManager.h"
#include "Character.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  const string cDemoStateTitle( "glacier² » demo" );

  DemoState::DemoState(): State( L"Demo" ) {}

  void DemoState::initialize( Game* game, GameTime time )
  {
    State::initialize( game, time );

    Locator::getGraphics().setRenderWindowTitle( cDemoStateTitle );

    Ogre::Plane plane( Vector3::UNIT_Y, 0.0f );
    Real width = 128.0f;
    Real height = 128.0f;
    mPrimitives.push_back( new Primitives::Plane( gEngine->getWorld()->getPhysics(), plane, width, height, Vector3::ZERO ) );
    mPrimitives.push_back( new Primitives::Box( gEngine->getWorld()->getPhysics(), Vector3( 1.0f, 10.0f, 1.0f ), Vector3( -5.5f, 5.0f, 5.5f ), Quaternion::IDENTITY ) );
    mPrimitives.push_back( new Primitives::Box( gEngine->getWorld()->getPhysics(), Vector3( 1.0f, 10.0f, 1.0f ), Vector3( 5.5f, 5.0f, -5.5f ), Quaternion::IDENTITY ) );
    mPrimitives.push_back( new Primitives::Box( gEngine->getWorld()->getPhysics(), Vector3( 1.0f, 10.0f, 1.0f ), Vector3( 5.5f, 5.0f, 5.5f ), Quaternion::IDENTITY ) );
    mPrimitives.push_back( new Primitives::Box( gEngine->getWorld()->getPhysics(), Vector3( 1.0f, 10.0f, 1.0f ), Vector3( -5.5f, 5.0f, -5.5f ), Quaternion::IDENTITY ) );

    // TODO this really needs to be in a background thread. or something.
    OgreEntityVector navSources;
    for ( auto primitive : mPrimitives )
    {
      navSources.push_back( primitive->getEntity() );
    }
    NavigationInputGeometry navGeometry( navSources );
    NavigationMeshParameters navParams;
    navParams.cellSize = 0.2f;
    navParams.cellHeight = 0.2f;
    navParams.agentMaxSlope = 20;
    navParams.agentHeight = 1.8f;
    navParams.agentMaxClimb = 1;
    navParams.agentRadius = 0.2f;
    navParams.edgeMaxLength = 12;
    navParams.edgeMaxError = 1.3f;
    navParams.regionMinSize = 50;
    navParams.regionMergeSize = 20;
    navParams.vertsPerPoly = DT_VERTS_PER_POLYGON;
    navParams.detailSampleDist = 6;
    navParams.detailSampleMaxError = 1;
    mNavigationMesh = new NavigationMesh( navParams );
    mNavigationMesh->buildFrom( &navGeometry );
    // mNavigationMesh->loadFrom( "poop.navmesh" );

#ifndef GLACIER_NO_NAVIGATION_DEBUG
    mNavVis = new NavigationDebugVisualizer( gEngine );
    duDebugDrawPolyMesh( mNavVis, *mNavigationMesh->getPolyMesh() );
#endif

    auto player = Locator::getEntities().create( "player" );
    gEngine->getInput()->getLocalController()->setCharacter( (Character*)player );
    player->spawn( Vector3( 0.0f, 1.0f, 0.0f ), Quaternion::IDENTITY );

    auto dummy = Locator::getEntities().create( "dev_dummy" );
    dummy->spawn( Vector3( 0.0f, 1.0f, 5.0f ), Quaternion::IDENTITY );

    for ( int i = 1; i < 11; i++ )
    {
      auto cube = (Entities::DevCube*)Locator::getEntities().create( "dev_cube" );
      cube->setType( Entities::DevCube::DevCube_025 );
      cube->spawn( Vector3( 5.0f, i * 15.0f, 0.0f ), Quaternion::IDENTITY );
    }
    for ( int i = 1; i < 11; i++ )
    {
      auto cube = ( Entities::DevCube* )Locator::getEntities().create( "dev_cube" );
      cube->setType( Entities::DevCube::DevCube_050 );
      cube->spawn( Vector3( -5.0f, i * 15.0f, 0.0f ), Quaternion::IDENTITY );
    }

    mDirector = new Director( &Locator::getGraphics(), player->getNode() );

    Locator::getMusic().beginScene();
  }

  void DemoState::pause( GameTime time )
  {
    State::pause( time );
  }

  void DemoState::resume( GameTime time )
  {
    State::resume( time );
  }

  void DemoState::update( GameTime tick, GameTime time )
  {
    mDirector->update( tick );
  }

  void DemoState::draw( GameTime delta, GameTime time )
  {
    //
  }

  void DemoState::shutdown( GameTime time )
  {
    gEngine->getInput()->getLocalController()->setCharacter( nullptr );
    Locator::getEntities().clear();
    Locator::getMusic().endScene();
#ifndef GLACIER_NO_NAVIGATION_DEBUG
    SAFE_DELETE( mNavVis );
#endif
    SAFE_DELETE( mNavigationMesh );
    for ( auto primitive : mPrimitives )
      delete primitive;
    SAFE_DELETE( mDirector );

    State::shutdown( time );
  }

}