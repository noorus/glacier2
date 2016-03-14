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

    Plane::Plane( PhysicsScene* scene, const Ogre::Plane& plane,
    const Real width, const Real height, const Vector3& position ):
    Primitive( scene )
    {
      PxPhysics& physics = mScene->getScene()->getPhysics();

      mActor = PxCreatePlane( physics,
        PxPlane( Glacier::Math::ogreVec3ToPx( plane.normal ), plane.d ),
        *mScene->getDefaultMaterial() );
      if ( !mActor )
        ENGINE_EXCEPT( "Could not create physics plane actor" );

      mScene->getScene()->addActor( *mActor );

      mMesh = Procedural::PlaneGenerator().setSize( Vector2( width, height ) ).setNormal( plane.normal ).setNumSegX( 8 ).setNumSegY( 8 ).realizeMesh();

      auto scm = Locator::getGraphics().getScene();
      mNode = scm->getRootSceneNode()->createChildSceneNode();

      mItem = scm->createItem( mMesh );
      mItem->setQueryFlags( SceneQueryFlag_World );
      mItem->setDatablock( "Developer/Floor" );
      mItem->setCastShadows( false );
      mNode->attachObject( mItem );
      mNode->setPosition( position );
      mNode->setOrientation( Quaternion::IDENTITY );
    }

    Plane::~Plane()
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
      mItem->setDatablock( "Developer/Floor" );
      mItem->setCastShadows( true );
      mNode->attachObject( mItem );
      mNode->setPosition( position );
      mNode->setOrientation( orientation );
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