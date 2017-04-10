#include "StdAfx.h"
#include "PhysXPhysics.h"
#include "Engine.h"
#include "Exception.h"
#include "ServiceLocator.h"
#include "WorldPrimitives.h"
#include "GlacierMath.h"
#include "PhysicsScene.h"
#include "ServiceLocator.h"
#include "Graphics.h"
#include "EntityRegistry.h"
#include "Entity.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  using namespace physx;

  namespace Primitives {

    Primitive::Primitive( PhysicsScene* scene ):
    mScene( scene ), mActor( nullptr ), mNode( nullptr ), mItem( nullptr )
    {
      //
    }

    Primitive::~Primitive()
    {
      //
    }

    Grid::Grid( PhysicsScene* scene, const Ogre::Plane& plane,
    const Real width, const Real height, const Vector3& position, const Real u, const Real v ):
    Primitive( scene )
    {
      PxPhysics& physics = mScene->getScene()->getPhysics();

      mActor = PxCreatePlane( physics,
        PxPlane( Glacier::Math::ogreVec3ToPx( plane.normal ), plane.d ),
        *mScene->getDefaultMaterial() );
      if ( !mActor )
        ENGINE_EXCEPT( "Could not create physics plane actor" );

      mScene->getScene()->addActor( *mActor );

      auto scm = Locator::getGraphics().getScene();

      obj_ = scm->createManualObject();
      node_ = scm->createSceneNode();
      node_->attachObject( obj_ );
      obj_->setCastShadows( false );
    }

    void Grid::draw()
    {
      obj_->clear();

      auto segments = 128;
      Vector2 dimensions( 128.0f, 128.0f );
      obj_->estimateVertexCount( segments * 2 * 2 );
      obj_->begin( "Debug/PhysicsVisualization", Ogre::OT_LINE_LIST );
      uint32_t index = 0;
      ColourValue color = ColourValue::White;
      color.a = 0.4f;
      for ( size_t i = 0; i < segments; i++ )
      {
        Real p = ( ( dimensions.x / segments ) * i );
        Vector3 pos1( p - ( dimensions.x / 2 ), 0.0f, -( dimensions.y / 2 ) );
        Vector3 pos2( pos1.x, 0.0f, ( dimensions.y / 2 ) );
        obj_->position( pos1 );
        obj_->colour( color );
        obj_->index( index++ );
        obj_->position( pos2 );
        obj_->colour( color );
        obj_->index( index++ );
      }
      for ( size_t i = 0; i < segments; i++ )
      {
        Real p = ( ( dimensions.y / segments ) * i );
        Vector3 pos1( -( dimensions.x / 2 ), 0.0f, p - ( dimensions.y / 2 ) );
        Vector3 pos2( ( dimensions.x / 2 ), 0.0f, pos1.z );
        obj_->position( pos1 );
        obj_->colour( color );
        obj_->index( index++ );
        obj_->position( pos2 );
        obj_->colour( color );
        obj_->index( index++ );
      }
      obj_->end();
    }

    Grid::~Grid()
    {
      auto scm = Locator::getGraphics().getScene();
      scm->destroyManualObject( obj_ );
      scm->destroySceneNode( node_ );
      mScene->getScene()->removeActor( *mActor );
    }

    Box::Box( PhysicsScene* scene, const Vector3& size, const Vector3& position, const Quaternion& orientation ): Primitive( scene )
    {
      PxPhysics& physics = mScene->getScene()->getPhysics();

      PxTransform transform;
      transform.p = Math::ogreVec3ToPx( position );
      transform.q = Math::ogreQtToPx( orientation );

      PxBoxGeometry geometry;
      geometry.halfExtents = Math::ogreVec3ToPx( size / 2.0f );
      mActor = PxCreateStatic( physics, transform, geometry, *scene->getDefaultMaterial() );
      if ( !mActor )
        ENGINE_EXCEPT( "Could not create physics box actor" );

      mScene->getScene()->addActor( *mActor );

      mMesh = Procedural::BoxGenerator().setSize( size ).realizeMesh();

      auto scm = Locator::getGraphics().getScene();
      mNode = scm->getRootSceneNode()->createChildSceneNode();

      mItem = Locator::getGraphics().getScene()->createItem( mMesh );
      mItem->setQueryFlags( SceneQueryFlag_World );
      mItem->setDatablock( "Debug/FOVVisualization" );
      mItem->setCastShadows( true );
      mNode->attachObject( mItem );
      mNode->setPosition( Vector3::ZERO );
      mNode->setDirection( Vector3::NEGATIVE_UNIT_Z, Ogre::Node::TS_WORLD );
      mNode->setPosition( position );
      //mNode->setOrientation( orientation );
    }

    void Box::draw()
    {
      //
    }

    Box::~Box()
    {
      if ( mItem )
        Locator::getGraphics().getScene()->destroyItem( mItem );
      if ( mNode )
      {
        mNode->removeAndDestroyAllChildren();
        Locator::getGraphics().getScene()->destroySceneNode( mNode );
      }
      mScene->getScene()->removeActor( *mActor );
    }

  }


}