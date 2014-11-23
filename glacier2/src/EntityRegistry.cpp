#include "StdAfx.h"
#include "Engine.h"
#include "Exception.h"
#include "EntityManager.h"
#include "EntityRegistry.h"
#include "Entity.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  void EntityRegistry::declare( const string& name, const string& className,
  fnEntityFactory factory )
  {
    assert( !name.empty() && factory );
    assert( mRecords.find( name ) == mRecords.end() );

    auto record = new EntityRecord( name, className, factory );
    mRecords[name] = record;
  }

  EntityRecord* EntityRegistry::lookup( const string& name )
  {
    auto iterator = mRecords.find( name );
    if ( iterator == mRecords.end() )
      return nullptr;
    return (*iterator).second;
  }

  void EntityRegistry::clear()
  {
    for ( auto record : mRecords )
      delete record.second;
    mRecords.clear();
  }

  EntityRegistry::~EntityRegistry()
  {
    clear();
  }

}