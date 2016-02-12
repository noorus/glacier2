#include "StdAfx.h"
#include "Engine.h"
#include "ServiceLocator.h"
#include "Graphics.h"
#include "FOVCone.h"

// Glacier� Game Engine � 2014 noorus
// All rights reserved.

namespace Glacier {

  FOVCone::FOVCone(): entity( nullptr ), node( nullptr ), alert( false )
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

  void FOVCone::setAlert( const bool alert )
  {
    this->alert = alert;
    if ( !entity )
      return;

    ColourValue color = ( alert ? ColourValue::Red : ColourValue::Green );
    entity->getSubEntity( 0 )->getMaterial()->getTechnique( 0 )->getPass( 0 )->getTextureUnitState( 0 )->setColourOperationEx(
      Ogre::LayerBlendOperationEx::LBX_SOURCE1,
      Ogre::LayerBlendSource::LBS_MANUAL, Ogre::LayerBlendSource::LBS_CURRENT, color );
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
    setAlert( alert );

    // Fix our node's direction so it points towards the usual default -Z.
    node->attachObject( entity );
    node->setPosition( Vector3::ZERO );
    node->setDirection( Vector3::NEGATIVE_UNIT_Z,
      Ogre::Node::TS_LOCAL, Vector3::NEGATIVE_UNIT_Y );
  }

}