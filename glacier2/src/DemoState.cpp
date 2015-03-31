#include "StdAfx.h"
#include "DemoState.h"
#include "Engine.h"
#include "Game.h"
#include "Director.h"
#include "Camera.h"
#include "Graphics.h"
#include "Engine.h"
#include "ActionManager.h"
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
#include "MovableTextOverlay.h"
#include "DeveloperEntities.h"
#include "Navigation.h"
#include "GlacierMath.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  const string cDemoStateTitle( "glacier² » demo" );
  MovableTextOverlay* mOverlay = nullptr;

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
    navParams.cellSize = 0.25f;
    navParams.cellHeight = 0.2f;
    navParams.agentMaxSlope = 20;
    navParams.agentHeight = 1.8f;
    navParams.agentMaxClimb = 1;
    navParams.agentRadius = 0.4f;
    navParams.edgeMaxLength = 12;
    navParams.edgeMaxError = 1.3f;
    navParams.regionMinSize = 50;
    navParams.regionMergeSize = 20;
    navParams.vertsPerPoly = DT_VERTS_PER_POLYGON;
    navParams.detailSampleDist = 6;
    navParams.detailSampleMaxError = 1;
    mNavigationMesh = new NavigationMesh( navParams );
    mNavigationMesh->build( &navGeometry );

    mNavVis = new NavigationDebugVisualizer( gEngine );
    duDebugDrawPolyMesh( mNavVis, *mNavigationMesh->getPolyMesh() );

    gEngine->getConsole()->printf( Console::srcGame, L"Yay built navmesh ok!" );
    gEngine->getConsole()->printf( Console::srcGame, L"Navmesh has %d polygons, %d vertices",
      mNavigationMesh->getPolyMesh()->npolys,
      mNavigationMesh->getPolyMesh()->nverts );
    Vector3 minbound = Math::floatArrayToOgreVec3( mNavigationMesh->getPolyMesh()->bmin );
    gEngine->getConsole()->printf( Console::srcGame, L"Min bound: %f %f %f",
      minbound.x, minbound.y, minbound.z );
    Vector3 maxbound = Math::floatArrayToOgreVec3( mNavigationMesh->getPolyMesh()->bmax );
    gEngine->getConsole()->printf( Console::srcGame, L"Max bound: %f %f %f",
      maxbound.x, maxbound.y, maxbound.z );

    auto player = Locator::getEntities().create( "player" );
    player->spawn( Vector3( 0.0f, 1.0f, 0.0f ), Quaternion::IDENTITY );

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

    auto attrs = new MovableTextOverlayAttributes( gEngine->getGraphics()->getScene()->getCamera( "defaultcamera" ),
      "Banksia", 17, Locator::getColors().base( Colors::Color_Text_Warning ), "Debug/EntityNameRect" );

    mOverlay = new MovableTextOverlay( "charname", "player",
      player->getMovable(), attrs );
    mOverlay->enable( true );

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
    mDirector->getCamera()->applyMovement(
      gEngine->getActionManager()->getCameraController()->getMovement() );
  }

  void DemoState::draw( GameTime delta, GameTime time )
  {
    mDirector->getCamera()->update( (float)delta );
    mOverlay->update();
  }

  void DemoState::shutdown( GameTime time )
  {
    SAFE_DELETE( mOverlay );
    Locator::getEntities().clear();
    Locator::getMusic().endScene();
    SAFE_DELETE( mNavVis );
    SAFE_DELETE( mNavigationMesh );
    for ( auto primitive : mPrimitives )
      delete primitive;
    SAFE_DELETE( mDirector );

    State::shutdown( time );
  }

}