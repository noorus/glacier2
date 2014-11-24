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
#include "DeveloperEntities.h"
#include "World.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  using namespace physx;

  ENGINE_DECLARE_ENTITY( cube, Cube );

  Cube::Cube( World* world ): Entity( world, &baseData )
  {
    //
  }

  void Cube::spawn( const Vector3& position, const Quaternion& orientation )
  {
    Entity::spawn( position, orientation );

    PxPhysics& physics = mWorld->getPhysics()->getScene()->getPhysics();
    auto scene = mWorld->getPhysics();

    PxTransform transform;
    transform.p = Math::ogreVec3ToPx( mPosition );
    transform.q = Math::ogreQtToPx( mOrientation );

    PxBoxGeometry geometry;
    geometry.halfExtents.x = 0.5f;
    geometry.halfExtents.y = 0.5f;
    geometry.halfExtents.z = 0.5f;
    mActor = PxCreateDynamic( physics, transform, geometry, *scene->getDefaultMaterial(), 25.0f );
    if ( !mActor )
      ENGINE_EXCEPT( "Could not create physics plane actor" );

    scene->getScene()->addActor( *mActor );

    mMesh = Procedural::BoxGenerator().setSizeX( 1.0f ).setSizeY( 1.0f ).setSizeZ( 1.0f ).realizeMesh();

    mEntity = Locator::getGraphics().getScene()->createEntity( mMesh );
    mEntity->setMaterialName( "ModelViewer/Floor" );
    mEntity->setCastShadows( false );
    mNode->attachObject( mEntity );
  }

  void Cube::think()
  {
    PxTransform& transform = mActor->getGlobalPose();
    mNode->setPosition( Math::pxVec3ToOgre( transform.p ) );
    mNode->setOrientation( Math::pxQtToOgre( transform.q ) );
  }

  Cube::~Cube()
  {
    if ( mEntity )
      Locator::getGraphics().getScene()->destroyEntity( mEntity );

    mWorld->getPhysics()->getScene()->removeActor( *mActor );
  }

}