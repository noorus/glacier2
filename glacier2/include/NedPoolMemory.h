#pragma once
#include "Utilities.h"
#include "Services.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  class NedPoolMemory: public Memory {
  protected:
    glacier_nedalloc::nedpool* mGenericPool;
    glacier_nedalloc::nedpool* mAudioPool;
    glacier_nedalloc::nedpool* mPhysicsPool;
    inline glacier_nedalloc::nedpool* resolvePool( const Sector sector )
    {
      if ( sector == Sector_Audio )
        return mAudioPool;
      if ( sector == Sector_Physics )
        return mPhysicsPool;
      return mGenericPool;
    }
  public:
    NedPoolMemory();
    ~NedPoolMemory();
    virtual void* alloc( const Sector sector, size_t size, size_t alignment = 0Ui64 );
    virtual void* realloc( const Sector sector, void* location, size_t size, size_t alignment = 0Ui64 );
    virtual void free( const Sector sector, void* location );
    virtual size_t getMemoryUsage( const Sector sector );
    virtual const wstring& getProviderName();
  };

}