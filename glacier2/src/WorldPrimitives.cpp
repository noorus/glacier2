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
    mScene( scene ), mActor( nullptr ), mNode( nullptr )
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
      mEntity->setMaterialName( "Developer/Floor" );
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