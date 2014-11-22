#include "StdAfx.h"
#include "PhysXPhysics.h"
#include "Engine.h"
#include "Exception.h"
#include "ServiceLocator.h"
#include "PhysicsScene.h"
#include "PhysicsDebugVisualizer.h"
#include "Graphics.h"
#include "GlacierMath.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  using namespace physx;

  inline void colorToComponents( PxU32 clr, float& r, float& g, float& b )
  {
    r = PxF32( (clr)& 0xff ) / 255.0f;
    g = PxF32( ( clr >> 8 ) & 0xff ) / 255.0f;
    b = PxF32( ( clr >> 16 ) & 0xff ) / 255.0f;
  }

  PhysicsDebugVisualizer::PhysicsDebugVisualizer( Engine* engine ):
  mEngine( engine )
  {
    assert( mEngine && mEngine->getGraphics() );
    mScene = mEngine->getGraphics()->getScene();

    mNode = (PCZSceneNode*)mScene->getRootSceneNode()->createChildSceneNode();
    mManualObject = mScene->createManualObject();

    mMaterial = Ogre::MaterialManager::getSingleton().getByName(
      "Debug/PhysicsVisualization", "Bootload" );

    mNode->attachObject( mManualObject );
    mManualObject->setRenderQueueGroup( Ogre::RENDER_QUEUE_SKIES_LATE );
    mManualObject->setCastShadows( false );

    mAlpha = 0.8f;
  }

  void PhysicsDebugVisualizer::clearDebugScene()
  {
    mManualObject->clear();
  }

  void PhysicsDebugVisualizer::drawDebugScene( const PxRenderBuffer* renderable )
  {
    if ( !renderable )
      return;

    float r, g, b;

    // process points
    auto count = renderable->getNbPoints();
    const PxDebugPoint* point = renderable->getPoints();
    mManualObject->estimateVertexCount( count );
    mManualObject->begin( mMaterial->getName(),
      Ogre::RenderOperation::OT_POINT_LIST );
    while ( count-- ) {
      colorToComponents( point->color, r, g, b );
      mManualObject->position( Math::pxVec3ToOgre( point->pos ) );
      mManualObject->colour( r, g, b );
      point++;
    }
    mManualObject->end();

    // process lines
    count = renderable->getNbLines();
    const PxDebugLine* line = renderable->getLines();
    mManualObject->estimateVertexCount( count * 2 );
    mManualObject->begin( mMaterial->getName(),
      Ogre::RenderOperation::OT_LINE_LIST );
    while ( count-- ) {
      colorToComponents( line->color0, r, g, b );
      mManualObject->position( Math::pxVec3ToOgre( line->pos0 ) );
      mManualObject->colour( r, g, b );
      colorToComponents( line->color1, r, g, b );
      mManualObject->position( Math::pxVec3ToOgre( line->pos1 ) );
      mManualObject->colour( r, g, b );
      line++;
    }
    mManualObject->end();

    // process triangles
    count = renderable->getNbTriangles();
    const PxDebugTriangle* triangle = renderable->getTriangles();
    mManualObject->estimateVertexCount( count * 3 );
    mManualObject->begin( mMaterial->getName(),
      Ogre::RenderOperation::OT_TRIANGLE_LIST );
    while ( count-- ) {
      colorToComponents( triangle->color0, r, g, b );
      mManualObject->position( Math::pxVec3ToOgre( triangle->pos0 ) );
      mManualObject->colour( r, g, b, mAlpha );
      colorToComponents( triangle->color1, r, g, b );
      mManualObject->position( Math::pxVec3ToOgre( triangle->pos1 ) );
      mManualObject->colour( r, g, b, mAlpha );
      colorToComponents( triangle->color2, r, g, b );
      mManualObject->position( Math::pxVec3ToOgre( triangle->pos2 ) );
      mManualObject->colour( r, g, b, mAlpha );
      triangle++;
    }
    mManualObject->end();
  }

  PhysicsDebugVisualizer::~PhysicsDebugVisualizer()
  {
    if ( mManualObject )
      mScene->destroyManualObject( mManualObject );
    if ( mNode )
      mScene->destroySceneNode( mNode->getName() );
  }

}