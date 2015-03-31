#include "StdAfx.h"
#include "Navigation.h"
#include "Engine.h"
#include "Exception.h"
#include "ServiceLocator.h"
#include "Graphics.h"
#include "GlacierMath.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  inline void colorToComponents( unsigned int clr, float& r, float& g, float& b )
  {
    r = float( (clr)& 0xff ) / 255.0f;
    g = float( ( clr >> 8 ) & 0xff ) / 255.0f;
    b = float( ( clr >> 16 ) & 0xff ) / 255.0f;
  }

  NavigationDebugVisualizer::NavigationDebugVisualizer( Engine* engine ):
  mEngine( engine ), mDrawing( false )
  {
    assert( mEngine && mEngine->getGraphics() );
    mScene = mEngine->getGraphics()->getScene();

    mNode = (PCZSceneNode*)mScene->getRootSceneNode()->createChildSceneNode();
    mManualObject = mScene->createManualObject();

    mMaterial = Ogre::MaterialManager::getSingleton().getByName(
      "Debug/NavigationVisualization", "Bootload" );

    mNode->attachObject( mManualObject );
    mManualObject->setRenderQueueGroup( Ogre::RENDER_QUEUE_SKIES_LATE );
    mManualObject->setCastShadows( false );

    mAlpha = 0.8f;
  }

  void NavigationDebugVisualizer::depthMask( bool state )
  {
    //
  }

  void NavigationDebugVisualizer::texture( bool state )
  {
    //
  }

  void NavigationDebugVisualizer::begin( duDebugDrawPrimitives prim, float size )
  {
    if ( prim == DU_DRAW_LINES )
    {
      mManualObject->begin( mMaterial->getName(),
        Ogre::RenderOperation::OT_LINE_LIST );
      mDrawing = true;
    }
    else if ( prim == DU_DRAW_POINTS )
    {
      mManualObject->begin( mMaterial->getName(),
        Ogre::RenderOperation::OT_POINT_LIST );
      mDrawing = true;
    }
    else if ( prim == DU_DRAW_TRIS )
    {
      mManualObject->begin( mMaterial->getName(),
        Ogre::RenderOperation::OT_TRIANGLE_LIST );
      mDrawing = true;
    }
  }

  void NavigationDebugVisualizer::vertex( const float* pos, unsigned int color )
  {
    if ( !mDrawing )
      return;

    float r, g, b;

    colorToComponents( color, r, g, b );
    mManualObject->position( Math::floatArrayToOgreVec3( pos ) );
    mManualObject->colour( r, g, b );
  }

  void NavigationDebugVisualizer::vertex( const float x, const float y, const float z, unsigned int color )
  {
    if ( !mDrawing )
      return;

    float r, g, b;

    colorToComponents( color, r, g, b );
    mManualObject->position( Vector3( x, y, z ) );
    mManualObject->colour( r, g, b );
  }

  void NavigationDebugVisualizer::vertex( const float* pos, unsigned int color, const float* uv )
  {
    if ( !mDrawing )
      return;

    float r, g, b;

    colorToComponents( color, r, g, b );
    mManualObject->position( Math::floatArrayToOgreVec3( pos ) );
    mManualObject->colour( r, g, b );
  }

  void NavigationDebugVisualizer::vertex( const float x, const float y, const float z, unsigned int color, const float u, const float v )
  {
    if ( !mDrawing )
      return;

    float r, g, b;

    colorToComponents( color, r, g, b );
    mManualObject->position( Vector3( x, y, z ) );
    mManualObject->colour( r, g, b );
  }

  void NavigationDebugVisualizer::end()
  {
    if ( mDrawing )
      mManualObject->end();
    mDrawing = false;
  }

  NavigationDebugVisualizer::~NavigationDebugVisualizer()
  {
    if ( mManualObject )
      mScene->destroyManualObject( mManualObject );
    if ( mNode )
      mScene->destroySceneNode( mNode->getName() );
  }

}