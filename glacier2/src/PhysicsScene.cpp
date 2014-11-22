#include "StdAfx.h"
#include "PhysXPhysics.h"
#include "Engine.h"
#include "Exception.h"
#include "ServiceLocator.h"
#include "PhysicsScene.h"
#include "GlacierMath.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  using namespace physx;

  PhysicsScene::PhysicsScene( PhysXPhysics* physics,
  PxCpuDispatcher* cpuDispatcher, PxGpuDispatcher* gpuDispatcher,
  const float gravity,
  const float restitution, const float staticFriction,
  const float dynamicFriction ):
  mPhysics( physics ), mScene( nullptr ), mCPUDispatcher( cpuDispatcher ),
  mGPUDispatcher( gpuDispatcher )
  {
    PxSceneDesc sceneDescriptor( mPhysics->getPhysics()->getTolerancesScale() );

    PxVec3 g( 0.0f, -gravity, 0.0f );
    mGravity = Glacier::Math::pxVec3ToOgre( g );
    sceneDescriptor.gravity = g;

    sceneDescriptor.cpuDispatcher = mCPUDispatcher;
    sceneDescriptor.gpuDispatcher = mGPUDispatcher;
    sceneDescriptor.filterShader = PxDefaultSimulationFilterShader;

    mScene = mPhysics->getPhysics()->createScene( sceneDescriptor );
    if ( !mScene )
      ENGINE_EXCEPT( "Could not create physics scene" );

    mDefaultMaterial = mPhysics->getPhysics()->createMaterial(
      staticFriction, dynamicFriction, restitution );
  }

  void PhysicsScene::simulationStep( const GameTime delta, const GameTime time )
  {
    mScene->simulate( delta );
  }

  void PhysicsScene::simulationFetchResults()
  {
    mScene->fetchResults( true );
    mScene->getSimulationStatistics( mStatistics );
  }

  float PhysicsScene::setGravity( const float gravity )
  {
    PxVec3 g( 0.0f, -gravity, 0.0f );
    mScene->setGravity( g );
    g = mScene->getGravity();
    mGravity = Glacier::Math::pxVec3ToOgre( g );
    return -mGravity.y;
  }

  float PhysicsScene::setRestitution( const float restitution )
  {
    mDefaultMaterial->setRestitution( restitution );
    return mDefaultMaterial->getRestitution();
  }

  float PhysicsScene::setStaticFriction( const float staticFriction )
  {
    mDefaultMaterial->setStaticFriction( staticFriction );
    return mDefaultMaterial->getStaticFriction();
  }

  float PhysicsScene::setDynamicFriction( const float dynamicFriction )
  {
    mDefaultMaterial->setDynamicFriction( dynamicFriction );
    return mDefaultMaterial->getDynamicFriction();
  }

  PhysicsScene::~PhysicsScene()
  {
    SAFE_RELEASE_PHYSX( mScene );
  }

}