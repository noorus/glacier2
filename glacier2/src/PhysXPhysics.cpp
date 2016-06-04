#include "StdAfx.h"
#include "PhysXPhysics.h"
#include "Engine.h"
#include "Exception.h"
#include "ServiceLocator.h"
#include "PhysicsScene.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  using namespace physx;

  PxDefaultCpuDispatcher* gDispatcher = nullptr;

  void* PhysXPhysics::Allocator::allocate( size_t size, const char* typeName,
  const char* filename, int line )
  {
    return Locator::getMemory().alloc( Memory::Sector_Physics, size, 16 );
  }

  void PhysXPhysics::Allocator::deallocate( void* ptr )
  {
    Locator::getMemory().free( Memory::Sector_Physics, ptr );
  }

  // CVars

  ENGINE_DECLARE_CONVAR( px_threads,
    L"Number of PhysX dispatch threads.", 2 );
  ENGINE_DECLARE_CONVAR( px_cuda,
    L"Enable CUDA utilisation in physics.", true );
  ENGINE_DECLARE_CONVAR( px_gravity,
    L"World gravity in metres per second.", 9.81f );
  ENGINE_DECLARE_CONVAR( px_restitution,
    L"Default restitution coefficient for world materials.", 0.095f );
  ENGINE_DECLARE_CONVAR( px_staticfriction,
    L"Default static friction coefficient for world materials.", 12.5f );
  ENGINE_DECLARE_CONVAR( px_dynamicfriction,
    L"Default dynamic friction coefficient for world materials.", 0.9f );

  // Physics class ============================================================

  PhysXPhysics::PhysXPhysics( Engine* engine ): EngineComponent( engine ),
    mFoundation( nullptr ), mPhysics( nullptr ), mCooking( nullptr ),
    mCPUDispatcher( nullptr )
  {
    initialize();
  }

  void PhysXPhysics::reportError( PxErrorCode::Enum code,
  const char* message, const char* file, int line )
  {
    switch ( code )
    {
      case PxErrorCode::eDEBUG_INFO:
        mEngine->getConsole()->printf( Console::srcPhysics,
          L"PhysX Debug info: %S", message );
      break;
      case PxErrorCode::eDEBUG_WARNING:
        mEngine->getConsole()->printf( Console::srcPhysics,
          L"PhysX Debug warning: %S", message );
      break;
      case PxErrorCode::eINVALID_PARAMETER:
        mEngine->getConsole()->errorPrintf( Console::srcPhysics,
          L"PhysX Invalid parameter: %S", message );
      break;
      case PxErrorCode::eINVALID_OPERATION:
        mEngine->getConsole()->errorPrintf( Console::srcPhysics,
          L"PhysX Invalid operation: %S", message );
      break;
      case PxErrorCode::eOUT_OF_MEMORY:
        mEngine->getConsole()->errorPrintf( Console::srcPhysics,
          L"PhysX Out of memory: %S", message );
        mEngine->triggerFatalError( Engine::Fatal_MemoryAllocation );
      break;
      case PxErrorCode::eINTERNAL_ERROR:
        mEngine->getConsole()->errorPrintf( Console::srcPhysics,
          L"PhysX Internal error: %S", message );
      break;
      case PxErrorCode::eABORT:
        mEngine->getConsole()->errorPrintf( Console::srcPhysics,
          L"PhysX Abort: %S", message );
        mEngine->triggerFatalError( Engine::Fatal_Generic );
      break;
      case PxErrorCode::ePERF_WARNING:
        mEngine->getConsole()->printf( Console::srcPhysics,
          L"PhysX Performance warning: %S", message );
      break;
    }
  }

  void PhysXPhysics::initialize()
  {
    // Print some version stuff
    mEngine->getConsole()->printf( Console::srcPhysics,
      L"Initializing PhysX v%i.%i.%i",
      PX_PHYSICS_VERSION_MAJOR, PX_PHYSICS_VERSION_MINOR,
      PX_PHYSICS_VERSION_BUGFIX );

    // Set memory allocator and error handler
    PxAllocatorCallback* allocator = &mAllocator;
    PxErrorCallback* error = this;

    // Create foundation
    mFoundation = PxCreateFoundation( PX_PHYSICS_VERSION, *allocator, *error );
    if ( !mFoundation )
      ENGINE_EXCEPT( "PxCreateFoundation failed" );

    // Default cooking tolerances scale, TODO: adjust later
    PxTolerancesScale scale;

    // Initialize the SDK
    mPhysics = PxCreateBasePhysics( PX_PHYSICS_VERSION, *mFoundation,
      scale, false, nullptr );
    if ( !mPhysics )
      ENGINE_EXCEPT( "PhysX Physics creation failed" );

    // Initialize extensions
    if ( !PxInitExtensions( *mPhysics ) )
      ENGINE_EXCEPT( "PhysX Extensions initialization failed" );
    PxRegisterHeightFields( *mPhysics );

    // Create CPU dispatcher
    mCPUDispatcher = PxDefaultCpuDispatcherCreate( g_CVar_px_threads.getInt() );

    // Initialize cooking parameters
    PxCookingParams cookingParams( scale );
    cookingParams.meshWeldTolerance = 0.001f;
    cookingParams.meshPreprocessParams = PxMeshPreprocessingFlags(
      PxMeshPreprocessingFlag::eWELD_VERTICES |
      PxMeshPreprocessingFlag::eREMOVE_UNREFERENCED_VERTICES |
      PxMeshPreprocessingFlag::eREMOVE_DUPLICATED_TRIANGLES );

    // Create cooker
    mCooking = PxCreateCooking( PX_PHYSICS_VERSION, *mFoundation, cookingParams );
    if ( !mCooking )
      ENGINE_EXCEPT( "PhysX Cooking initialization failed" );

    mEngine->operationContinuePhysics();
  }

  PxPhysics* PhysXPhysics::getPhysics()
  {
    return mPhysics;
  }

  PhysicsScene* PhysXPhysics::createScene()
  {
    PxCpuDispatcher* cpuDispatcher = mCPUDispatcher;
    PxGpuDispatcher* gpuDispatcher = nullptr;

    float gravity = g_CVar_px_gravity.getFloat();
    float restitution = g_CVar_px_restitution.getFloat();
    float staticFriction = g_CVar_px_staticfriction.getFloat();
    float dynamicFriction = g_CVar_px_dynamicfriction.getFloat();

    auto scene = new PhysicsScene( this,
      cpuDispatcher, gpuDispatcher,
      gravity, restitution, staticFriction, dynamicFriction );

    mScenes.push_back( scene );

    return scene;
  }

  physx::PxCooking * PhysXPhysics::getCooking()
  {
    return mCooking;
  }

  void PhysXPhysics::destroyScene( PhysicsScene* scene )
  {
    mScenes.remove( scene );
    SAFE_DELETE( scene );
  }

  void PhysXPhysics::componentPreUpdate( GameTime time )
  {
    //
  }

  void PhysXPhysics::componentTick( GameTime tick, GameTime time )
  {
    for ( auto scene : mScenes )
    {
      scene->simulationStep( tick, time );
      scene->simulationFetchResults();
      scene->post();
    }
  }

  void PhysXPhysics::componentPostUpdate( GameTime delta, GameTime time )
  {
    //
  }

  void PhysXPhysics::shutdown()
  {
    mEngine->getConsole()->printf( Console::srcPhysics,
      L"Shutting down PhysX" );

    mEngine->operationSuspendPhysics();

    assert( mScenes.empty() );

    SAFE_RELEASE_PHYSX( mCPUDispatcher );

    if ( mPhysics )
    {
      PxCloseExtensions();
      mPhysics->release();
    }

    SAFE_RELEASE_PHYSX( mCooking );
    SAFE_RELEASE_PHYSX( mFoundation );
  }

  void PhysXPhysics::restart()
  {
    shutdown();
    initialize();
  }

  PhysXPhysics::~PhysXPhysics()
  {
    shutdown();
  }

}