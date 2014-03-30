#include "StdAfx.h"
#include "Physics.h"
#include "Engine.h"
#include "Exception.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  using namespace physx;

  static PxDefaultErrorCallback gDefaultErrorCallback;
  static PxDefaultAllocator gDefaultAllocatorCallback;

  // Physics class ============================================================

  Physics::Physics( Engine* engine ): EngineComponent( engine ),
  mFoundation( nullptr ), mPhysics( nullptr ), mCooking( nullptr ),
  mCudaContextManager( nullptr ), mControllerMgr( nullptr )
  {
    physicsInitialize();
  }

  void Physics::physicsInitialize()
  {
    mEngine->getConsole()->printf( Console::srcPhysics,
      L"Initializing physics engine..." );

    // Print some version stuff
    mEngine->getConsole()->printf( Console::srcPhysics,
      L"Using PhysX %i.%i.%i",
      PX_PHYSICS_VERSION_MAJOR, PX_PHYSICS_VERSION_MINOR,
      PX_PHYSICS_VERSION_BUGFIX );

    // Using default allocator & error callbacks for now
    PxAllocatorCallback* allocator = &gDefaultAllocatorCallback;
    PxErrorCallback* error = &gDefaultErrorCallback;

    // Create foundation
    mFoundation = PxCreateFoundation( PX_PHYSICS_VERSION, *allocator, *error );
    if ( !mFoundation )
      ENGINE_EXCEPT( L"PxCreateFoundation failed" );

    // Create CUDA context manager
    PxCudaContextManagerDesc contextMgrDesc;
    mCudaContextManager = PxCreateCudaContextManager( *mFoundation,
      contextMgrDesc, nullptr );

    if ( mCudaContextManager && !mCudaContextManager->contextIsValid() )
    {
      mCudaContextManager->release();
      mCudaContextManager = nullptr;
    }

    // Default cooking tolerances scale, TODO: adjust later
    PxTolerancesScale scale;

    // Initialize the SDK
    mPhysics = PxCreateBasePhysics( PX_PHYSICS_VERSION, *mFoundation,
      PxTolerancesScale(), false, nullptr );
    if ( !mPhysics )
      ENGINE_EXCEPT( L"PhysX Physics creation failed" );

    // Initialize extensions
    if ( !PxInitExtensions( *mPhysics ) )
      ENGINE_EXCEPT( L"PhysX Extensions initialization failed" );
    PxRegisterHeightFields( *mPhysics );

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
      ENGINE_EXCEPT( L"PhysX Cooking initialization failed" );

    // Print some CUDA information
    if ( mCudaContextManager )
    {
      mEngine->getConsole()->printf( Console::srcPhysics,
        L"CUDA device: %S%s, %dMHz, %dMB",
        mCudaContextManager->getDeviceName(),
        mCudaContextManager->usingDedicatedPhysXGPU() ? " (PhysX PPU)" : "",
        mCudaContextManager->getClockRate() / 1000,
        mCudaContextManager->getDeviceTotalMemBytes() / 1024 / 1024 );
    } else
      mEngine->getConsole()->printf( Console::srcPhysics, L"CUDA unavailable" );

    mEngine->operationContinuePhysics();
  }

  void Physics::componentPreUpdate( GameTime time )
  {
    //
  }

  void Physics::componentTick( GameTime tick, GameTime time )
  {
    //
  }

  void Physics::componentPostUpdate( GameTime delta, GameTime time )
  {
    //
  }

  void Physics::physicsShutdown()
  {
    mEngine->getConsole()->printf( Console::srcPhysics,
      L"Shutting down physics..." );

    mEngine->operationSuspendPhysics();

    if ( mPhysics )
    {
      PxCloseExtensions();
      mPhysics->release();
    }

    SAFE_RELEASE_PHYSX( mCudaContextManager );
    SAFE_RELEASE_PHYSX( mCooking );
    SAFE_RELEASE_PHYSX( mFoundation );
  }

  void Physics::physicsRestart()
  {
    physicsShutdown();
    physicsInitialize();
  }

  Physics::~Physics()
  {
    physicsShutdown();
  }

}