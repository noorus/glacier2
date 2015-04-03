#include "StdAfx.h"
#include "Engine.h"
#include "ServiceLocator.h"
#include "Graphics.h"
#include "FOVCone.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  FOVCone::FOVCone(): entity( nullptr ), node( nullptr )
  {
    auto scene = Locator::getGraphics().getScene();
    node = scene->createSceneNode();
  }

  FOVCone::~FOVCone()
  {
    auto scene = Locator::getGraphics().getScene();

    if ( entity )
      scene->destroyEntity( entity );
    if ( node )
      scene->destroySceneNode( node );
  }

  void FOVCone::set( Real viewDistance, const Radian& fieldOfView )
  {
    Real halfFov = Degree( fieldOfView.valueDegrees() / 2.0f ).valueRadians();
    Real viewRadius = ( viewDistance * tanf( halfFov ) );

    auto scene = Locator::getGraphics().getScene();
    node->detachAllObjects();

    if ( entity )
      scene->destroyEntity( entity );

    // The cone gets created with its base at the origin, pointing towards +Y.
    // Flip the cone via setPosition so it widens outwards towards -Y instead.
    mesh = Procedural::ConeGenerator( viewRadius, viewDistance )
      .setPosition( Vector3( 0.0f, -viewDistance, 0.0f ) )
      .realizeMesh();

    entity = scene->createEntity( mesh );
    entity->setMaterialName( "Debug/FOVVisualization" );

    // Fix our node's direction so it points towards the usual default -Z.
    node->attachObject( entity );
    node->setDirection( Vector3::NEGATIVE_UNIT_Z,
      Ogre::Node::TS_LOCAL, Vector3::NEGATIVE_UNIT_Y );
  }

}