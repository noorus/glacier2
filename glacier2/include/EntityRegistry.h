#pragma once
#include "Types.h"
#include "Utilities.h"
#include <map>

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  class World;
  class Entity;

  typedef Entity* ( *fnEntityFactory )( World* world );

  //! \struct EntityBaseData
  //! \brief Static base data for every derived entity class.
  //! This data is shared by all instances of a specific entity class.
  //! Declared for an entity in the ENGINE_DECLARE_ENTITY macro.
  struct EntityBaseData {
    const string className;
    const uint32_t flags;
  };

  //! \struct EntityRecord
  //! \brief Record structure used by EntityRegistry to track entity classes.
  struct EntityRecord {
    string name; //!< In-game entity class name, such as "prop_static"
    string className; //!< Entity class name in code, such as "PropStatic"
    fnEntityFactory factory; //!< Pointer to the factory function
    EntityRecord( const string& name_, const string& class_,
      fnEntityFactory factory_ ): name( name_ ), className( class_ ),
      factory( factory_ ) {}
  };

  typedef std::map<string, EntityRecord*> EntityRecordMap;

  //! \class EntityRegistry
  //! \brief Manages entity class registrations in the engine.
  //! \warning Never call declare directly! Use ENGINE_DECLARE_ENTITY.
  class EntityRegistry: public Singleton<EntityRegistry> {
  friend class Engine;
  protected:
    EntityRecordMap mRecords;
  public:
    ~EntityRegistry();
    void declare( const string& name, const string& className,
      fnEntityFactory factory );
    EntityRecord* lookup( const string& name );
    void clear();
  };

  class World;

  //! \class EntityFactories
  //! \brief Contains entity factory functions for all entities.
  //! \brief Must be a friend of all registered entity classes!
  //! \warning You should never need to use this class directly.
  class EntityFactories {
  public:
    template <class T>
    static Entity* factory( World* world ) {
      return static_cast<Entity*>( new T( world ) );
    }
  };

  //! \class EntityRegistrar
  //! \brief Declares an entity to the EntityRegistry on construction.
  //! \brief Only called through the ENGINE_DECLARE_ENTITY macro.
  //! \warning Don't use directly for anything!
  template <class T>
  class EntityRegistrar {
  public:
    explicit EntityRegistrar( const string& name, const string& className,
    fnEntityFactory factory )
    {
      EntityRegistry::instance().declare( name, className, factory );
    }
  };

  //! \def ENGINE_DECLARE_ENTITY( realName, className )
  //! \brief A macro that declares an entity class to the engine.
  //! \param realName The in-game class name, such as prop_static
  //! \param className The entity class in code, such as PropStatic
# define ENGINE_DECLARE_ENTITY( realName, className )\
  EntityBaseData className##::baseData = { #realName, 0 };\
  static EntityRegistrar<##className##> g_EntityRegistrar_##realName(\
  #realName, #className, &EntityFactories::factory<##className##> )

}