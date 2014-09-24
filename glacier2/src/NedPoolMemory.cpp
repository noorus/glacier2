#include "StdAfx.h"
#include "NedPoolMemory.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  const wstring cNedPoolProviderName( L"Nedmalloc Pooled" );

  NedPoolMemory::NedPoolMemory(): mGenericPool( nullptr ),
  mAudioPool( nullptr ), mPhysicsPool( nullptr )
  {
    mGenericPool = glacier_nedalloc::nedcreatepool( 10240, 2 );
    mAudioPool = glacier_nedalloc::nedcreatepool( 10240, 2 );
    mPhysicsPool = glacier_nedalloc::nedcreatepool( 10240, 2 );
  }

  NedPoolMemory::~NedPoolMemory()
  {
    if ( mGenericPool )
      glacier_nedalloc::neddestroypool( mGenericPool );
    if ( mAudioPool )
      glacier_nedalloc::neddestroypool( mAudioPool );
    if ( mPhysicsPool )
      glacier_nedalloc::neddestroypool( mPhysicsPool );
  }

  void* NedPoolMemory::alloc( const Sector sector, size_t size, size_t alignment )
  {
    return glacier_nedalloc::nedpmalloc2( resolvePool( sector ), size, alignment, 0 );
  }

  void* NedPoolMemory::realloc( const Sector sector, void* location, size_t size, size_t alignment )
  {
    return glacier_nedalloc::nedprealloc2( resolvePool( sector ), location, size, alignment, 0 );
  }

  void NedPoolMemory::free( const Sector sector, void* location )
  {
    glacier_nedalloc::nedpfree2( resolvePool( sector ), location, 0 );
  }

  size_t NedPoolMemory::getMemoryUsage( const Sector sector )
  {
    return glacier_nedalloc::nedpmalloc_footprint( resolvePool( sector ) );
  }

  const wstring& NedPoolMemory::getProviderName()
  {
    return cNedPoolProviderName;
  }

}