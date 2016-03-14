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

    DevCube::DevCube( World* world ): Entity( world, &baseData ), mType( DevCube_025 )
    {
      //
    }

    Ogre::MovableObject* DevCube::getMovable()
    {
      return mItem;
    }

    void DevCube::setType( const Type type )
    {
      mType = type;
    }

    void DevCube::spawn( const Vector3& position, const Quaternion& orientation )
    {
      Entity::spawn( position, orientation );

      PxPhysics& physics = mWorld->getPhysics()->getScene()->getPhysics();
      auto scene = mWorld->getPhysics();

      PxTransform transform;
      transform.p = Math::ogreVec3ToPx( mPosition );
      transform.q = Math::ogreQtToPx( mOrientation );

      if ( mType == DevCube_025 )
      {
        PxBoxGeometry geometry;
        geometry.halfExtents.x = 0.125f;
        geometry.halfExtents.y = 0.125f;
        geometry.halfExtents.z = 0.125f;
        mActor = PxCreateDynamic( physics, transform, geometry, *scene->getDefaultMaterial(), 10.0f );
        if ( !mActor )
          ENGINE_EXCEPT( "Could not create physics plane actor" );

        scene->getScene()->addActor( *mActor );

        mMesh = Procedural::BoxGenerator().setSizeX( 0.25f ).setSizeY( 0.25f ).setSizeZ( 0.25f ).realizeMesh();
        mItem = Locator::getGraphics().getScene()->createItem( mMesh );
        mItem->setDatablock( "Developer/Cube025" );
      }
      else if ( mType == DevCube_050 )
      {
        PxBoxGeometry geometry;
        geometry.halfExtents.x = 0.25f;
        geometry.halfExtents.y = 0.25f;
        geometry.halfExtents.z = 0.25f;
        mActor = PxCreateDynamic( physics, transform, geometry, *scene->getDefaultMaterial(), 40.0f );
        if ( !mActor )
          ENGINE_EXCEPT( "Could not create physics plane actor" );

        scene->getScene()->addActor( *mActor );

        mMesh = Procedural::BoxGenerator().setSizeX( 0.5f ).setSizeY( 0.5f ).setSizeZ( 0.5f ).realizeMesh();
        mItem = Locator::getGraphics().getScene()->createItem( mMesh );
        mItem->setDatablock( "Developer/Cube050" );
      }

      mItem->setCastShadows( true );
      mNode->attachObject( mItem );
    }

    void DevCube::think( const GameTime delta )
    {
      PxTransform& transform = mActor->getGlobalPose();
      mNode->setPosition( Math::pxVec3ToOgre( transform.p ) );
      mNode->setOrientation( Math::pxQtToOgre( transform.q ) );
    }

    DevCube::~DevCube()
    {
      if ( mItem )
        Locator::getGraphics().getScene()->destroyItem( mItem );

      if ( !mMesh.isNull() )
        Ogre::MeshManager::getSingleton().remove( mMesh->getHandle() );

      mWorld->getPhysics()->getScene()->removeActor( *mActor );
    }

  }

}