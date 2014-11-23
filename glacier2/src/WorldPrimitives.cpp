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

  ENGINE_DECLARE_ENTITY( cube, Cube );

  Cube::Cube(): Entity( &baseData )
  {
    //
  }

  void Cube::spawn( PhysicsScene* scene, const Vector3& position )
  {
    mScene = scene;
    PxPhysics& physics = mScene->getScene()->getPhysics();

    PxTransform transform;
    transform.p = Math::ogreVec3ToPx( position );
    transform.q = PxQuat( PxIdentity );

    PxBoxGeometry geometry;
    geometry.halfExtents.x = 0.5f;
    geometry.halfExtents.y = 0.5f;
    geometry.halfExtents.z = 0.5f;
    mActor = PxCreateDynamic( physics, transform, geometry, *mScene->getDefaultMaterial(), 10.0f );
    if ( !mActor )
      ENGINE_EXCEPT( "Could not create physics plane actor" );

    //mActor->setAngularDamping( 0.5f );
    //mActor->setLinearVelocity( velocity );

    mScene->getScene()->addActor( *mActor );

    mMesh = Procedural::BoxGenerator().setSizeX( 1.0f ).setSizeY( 1.0f ).setSizeZ( 1.0f ).realizeMesh();

    auto scm = Locator::getGraphics().getScene();
    mNode = (PCZSceneNode*)scm->getRootSceneNode()->createChildSceneNode();
    scm->addPCZSceneNode( mNode, scm->getDefaultZone() );

    mEntity = Locator::getGraphics().getScene()->createEntity( mMesh );
    mEntity->setMaterialName( "ModelViewer/Floor" );
    mEntity->setCastShadows( false );
    mNode->attachObject( mEntity );
    mNode->setPosition( position );
    mNode->setOrientation( Quaternion::IDENTITY );
  }

  void Cube::think()
  {
    PxTransform& pose = mActor->getGlobalPose();
    mNode->setPosition( Math::pxVec3ToOgre( pose.p ) );
    mNode->setOrientation( Math::pxQtToOgre( pose.q ) );
  }

  Cube::~Cube()
  {
    if ( mEntity )
      Locator::getGraphics().getScene()->destroyEntity( mEntity );
    if ( mNode )
      Locator::getGraphics().getScene()->destroySceneNode( mNode );

    mScene->getScene()->removeActor( *mActor );
  }

  namespace World {

    Plane::Plane( PhysicsScene* scene, const Ogre::Plane& plane,
    const Real width, const Real height, const Vector3& position ):
    mScene( scene ), mActor( nullptr )
    {
      PxPhysics& physics = mScene->getScene()->getPhysics();

      mActor = PxCreatePlane( physics,
        PxPlane( Glacier::Math::ogreVec3ToPx( plane.normal ), plane.d ),
        *mScene->getDefaultMaterial() );
      if ( !mActor )
        ENGINE_EXCEPT( "Could not create physics plane actor" );

      mScene->getScene()->addActor( *mActor );

      mMesh = Ogre::MeshManager::getSingleton().createPlane( "",
        ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane,
        width, height,
        8, 8, true, 1,
        width, height,
        Vector3::UNIT_Z );

      auto scm = Locator::getGraphics().getScene();
      mNode = (PCZSceneNode*)scm->getRootSceneNode()->createChildSceneNode();
      scm->addPCZSceneNode( mNode, scm->getDefaultZone() );

      mEntity = Locator::getGraphics().getScene()->createEntity( mMesh );
      mEntity->setMaterialName( "ModelViewer/Floor" );
      mEntity->setCastShadows( false );
      mNode->attachObject( mEntity );
      mNode->setPosition( position );
      mNode->setOrientation( Quaternion::IDENTITY );
    }

    Plane::~Plane()
    {
      if ( mEntity )
        Locator::getGraphics().getScene()->destroyEntity( mEntity );
      if ( mNode )
        Locator::getGraphics().getScene()->destroySceneNode( mNode );
      mScene->getScene()->removeActor( *mActor );
    }

  }


}