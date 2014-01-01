#include "StdAfx.h"
#include "GlacierMemory.h"

// Operators used and required by EASTL
void* operator new[]( size_t size, const char* name, int flags, unsigned debugFlags, const char* file, int line )
{
  return Glacier::Memory::instance().alloc( size );
}

void* operator new[]( size_t size, size_t alignment, size_t alignmentOffset, const char* name, int flags, unsigned debugFlags, const char* file, int line )
{
  return Glacier::Memory::instance().alloc( size, alignment );
}

namespace Glacier {

  Memory::Memory(): mPool( nullptr )
  {
    mPool = nedalloc::nedcreatepool( 10240, 4 );
  }

  Memory::~Memory()
  {
    if ( mPool )
      nedalloc::neddestroypool( mPool );
  }

  void* Memory::alloc( size_t size, size_t alignment )
  {
    return nedalloc::nedpmalloc2( mPool, size, alignment, 0 );
  }

  void* Memory::realloc( void* location, size_t size, size_t alignment )
  {
    return nedalloc::nedprealloc2( mPool, location, size, alignment, 0 );
  }

  void Memory::free( void* location )
  {
    nedalloc::nedpfree2( mPool, location, 0 );
  }

}