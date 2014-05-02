#include "StdAfx.h"
#include "NedPoolMemory.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

// Operators used and required by EASTL
void* operator new[]( size_t size, const char* name, int flags, unsigned debugFlags, const char* file, int line )
{
  return malloc( size );
  //return Glacier::Memory::instance().alloc( size );
}

void* operator new[]( size_t size, size_t alignment, size_t alignmentOffset, const char* name, int flags, unsigned debugFlags, const char* file, int line )
{
  return malloc( size );
  // return Glacier::Memory::instance().alloc( size, alignment );
}

namespace Glacier {

  NedPoolMemory::NedPoolMemory(): mPool( nullptr )
  {
    mPool = glacier_nedalloc::nedcreatepool( 10240, 4 );
  }

  NedPoolMemory::~NedPoolMemory()
  {
    if ( mPool )
      glacier_nedalloc::neddestroypool( mPool );
  }

  void* NedPoolMemory::alloc( size_t size, size_t alignment )
  {
    return glacier_nedalloc::nedpmalloc2( mPool, size, alignment, 0 );
  }

  void* NedPoolMemory::realloc( void* location, size_t size, size_t alignment )
  {
    return glacier_nedalloc::nedprealloc2( mPool, location, size, alignment, 0 );
  }

  void NedPoolMemory::free( void* location )
  {
    glacier_nedalloc::nedpfree2( mPool, location, 0 );
  }

}