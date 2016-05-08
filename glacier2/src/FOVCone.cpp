#include "StdAfx.h"
#include "Engine.h"
#include "ServiceLocator.h"
#include "Graphics.h"
#include "FOVCone.h"
#include <OgreHlmsUnlitDatablock.h>

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  FOVCone::FOVCone(): item( nullptr ), node( nullptr ), alert( false )
  {
    auto scene = Locator::getGraphics().getScene();
    node = scene->createSceneNode();
  }

  FOVCone::~FOVCone()
  {
    auto scene = Locator::getGraphics().getScene();

    if ( item )
      scene->destroyItem( item );
    if ( node )
      scene->destroySceneNode( node );
  }

  void FOVCone::setAlert( const bool alert )
  {
    this->alert = alert;
    if ( !item )
      return;

    ColourValue color = ( alert ? ColourValue::Red : ColourValue::Green );
    color.a = 0.2f;
    
    auto db = static_cast<Ogre::HlmsUnlitDatablock*>( item->getSubItem( 0 )->getDatablock() );
    db->setColour( color );
  }

  void FOVCone::set( Real viewDistance, const Radian& fieldOfView )
  {
    Real halfFov = Degree( fieldOfView.valueDegrees() / 2.0f ).valueRadians();
    Real viewRadius = ( viewDistance * tanf( halfFov ) );

    auto scene = Locator::getGraphics().getScene();
    node->detachAllObjects();

    if ( item )
      scene->destroyItem( item );

    // The cone gets created with its base at the origin, pointing towards +Y.
    // Flip the cone via setPosition so it widens outwards towards -Y instead.
    mesh = Procedural::ConeGenerator( viewRadius, viewDistance )
      .setPosition( Vector3( 0.0f, -viewDistance, 0.0f ) )
      .realizeMesh();

    item = scene->createItem( mesh );
    item->setDatablockOrMaterialName( "Debug/FOVVisualization" );
    setAlert( alert );

    // Fix our node's direction so it points towards the usual default -Z.
    node->attachObject( item );
    node->setPosition( Vector3::ZERO );
    node->setDirection( Vector3::NEGATIVE_UNIT_Z,
      Ogre::Node::TS_LOCAL, Vector3::NEGATIVE_UNIT_Y );
  }

}