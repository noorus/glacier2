#include "StdAfx.h"
#include "Navigation.h"
#include "Engine.h"
#include "Exception.h"
#include "ServiceLocator.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  void* Navigation::allocator( int size, rcAllocHint hint )
  {
    return Locator::getMemory().alloc( Memory::Sector_Navigation, size );
  }

  void Navigation::deallocator( void* ptr )
  {
    Locator::getMemory().free( Memory::Sector_Navigation, ptr );
  }

  Navigation::Navigation( Engine* engine ): EngineComponent( engine )
  {
    rcAllocSetCustom( allocator, deallocator );
  }

  Navigation::~Navigation()
  {
    //
  }

}