#include "StdAfx.h"
#include "JSNatives.h"
#include "JSObjectWrapper.h"
#include "JSUtil.h"
#include "Engine.h"
#include "Entity.h"
#include "EntityRegistry.h"
#include "Console.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  namespace JS {

    Entity::Entity( Glacier::Entity* entity ): mEntity( entity ),
    ObjectWrapper( Wrapped_Entity )
    {
      // Stubb
    }

    Entity::~Entity()
    {
      unref();
    }

    Entity* Entity::create( Glacier::Entity* entity, Handle<v8::Context> context )
    {
      Isolate* isolate = Isolate::GetCurrent();
      HandleScope handleScope( isolate );
      v8::Context::Scope contextScope( context );

      Local<FunctionTemplate> tpl = FunctionTemplate::New( isolate );

      tpl->SetClassName( Util::allocString( entity->getBaseData().codeClassName.c_str() ) );
      tpl->InstanceTemplate()->SetInternalFieldCount( 1 );

      JS_TEMPLATE_SET( tpl, "toString", jsToString );
      JS_TEMPLATE_SET( tpl, "getName", jsGetName );

      auto instance = new Entity( entity );
      Local<v8::Object> object = tpl->GetFunction()->NewInstance();
      instance->wrap( object );
      instance->ref();

      return instance;
    }

    Glacier::Entity* Entity::getEntity()
    {
      return mEntity;
    }

    void Entity::jsToString( const FunctionCallbackInfo<v8::Value>& args )
    {
      Entity* ptr = unwrap( args.Holder() );
      char result[128];
      sprintf_s<128>( result, "Entity[%s]", ptr->getEntity()->getName().c_str() );
      args.GetReturnValue().Set( Util::allocString( result ) );
    }

    void Entity::jsGetName( const FunctionCallbackInfo<v8::Value>& args )
    {
      Entity* ptr = unwrap( args.Holder() );
      auto entity = ptr->getEntity();

      args.GetReturnValue().Set( Util::allocString( entity->getName() ) );
    }

  }

}