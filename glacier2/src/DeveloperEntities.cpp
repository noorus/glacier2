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

  namespace Entities {

    ENGINE_DECLARE_ENTITY( dev_cube, DevCube );

    DevCube::DevCube( World* world ): Entity( world, &baseData )
    {
      //
    }

    void DevCube::spawn( const Vector3& position, const Quaternion& orientation )
    {
      Entity::spawn( position, orientation );

      PxPhysics& physics = mWorld->getPhysics()->getScene()->getPhysics();
      auto scene = mWorld->getPhysics();

      PxTransform transform;
      transform.p = Math::ogreVec3ToPx( mPosition );
      transform.q = Math::ogreQtToPx( mOrientation );

      PxBoxGeometry geometry;
      geometry.halfExtents.x = 0.125f;
      geometry.halfExtents.y = 0.125f;
      geometry.halfExtents.z = 0.125f;
      mActor = PxCreateDynamic( physics, transform, geometry, *scene->getDefaultMaterial(), 10.0f );
      if ( !mActor )
        ENGINE_EXCEPT( "Could not create physics plane actor" );

      scene->getScene()->addActor( *mActor );

      mMesh = Procedural::BoxGenerator().setSizeX( 0.25f ).setSizeY( 0.25f ).setSizeZ( 0.25f ).realizeMesh();

      mEntity = Locator::getGraphics().getScene()->createEntity( mMesh );
      mEntity->setMaterialName( "Developer/Cube025" );
      mEntity->setCastShadows( false );
      mNode->attachObject( mEntity );
    }

    void DevCube::think()
    {
      PxTransform& transform = mActor->getGlobalPose();
      mNode->setPosition( Math::pxVec3ToOgre( transform.p ) );
      mNode->setOrientation( Math::pxQtToOgre( transform.q ) );
    }

    DevCube::~DevCube()
    {
      if ( mEntity )
        Locator::getGraphics().getScene()->destroyEntity( mEntity );

      mWorld->getPhysics()->getScene()->removeActor( *mActor );
    }

  }

}