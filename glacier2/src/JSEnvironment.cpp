#include "StdAfx.h"
#include "JSNatives.h"
#include "JSObjectWrapper.h"
#include "JSUtil.h"
#include "Engine.h"
#include "ServiceLocator.h"
#include "Environment.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  namespace JS {

    string Environment::className( "Environment" );
    Environment* Environment::instance( nullptr );

    Environment::Environment( Glacier::Environment* environment ):
    mEnvironment( environment ), ObjectWrapper( Wrapped_Environment )
    {
      //
    }

    void Environment::initialize( Glacier::Environment* env, Handle<v8::Context> context )
    {
      Isolate* isolate = Isolate::GetCurrent();
      HandleScope handleScope( isolate );
      v8::Context::Scope contextScope( context );

      Local<FunctionTemplate> tpl = FunctionTemplate::New( isolate );

      tpl->SetClassName( Util::allocString( className.c_str() ) );
      tpl->InstanceTemplate()->SetInternalFieldCount( 1 );

      JS_TEMPLATE_SET( tpl, "setAmbience", jsSetAmbience );

      instance = new Environment( env );
      Local<v8::Object> object = tpl->GetFunction()->NewInstance();
      instance->wrap( object );

      context->Global()->Set( Util::allocString( className.c_str() ), object );
      instance->ref();
    }

    //! \verbatim
    //! Environment.setAmbience( Color lowerHemisphere, Color upperHemisphere )
    //! \endverbatim
    void Environment::jsSetAmbience( const FunctionCallbackInfo<v8::Value>& args )
    {
      v8::Isolate* isolate = args.GetIsolate();
      Environment* ptr = unwrap( args.Holder() );
      HandleScope handleScope( args.GetIsolate() );

      if ( args.Length() != 2 || !args[0]->IsObject() || !args[1]->IsObject() )
      {
        Util::throwException( isolate,
          L"Syntax error: Environment.setAmbience( Color lowerHemisphere, Color upperHemisphere )" );
        return;
      }

      auto env = ptr->getEnvironment();

      auto lowerHemisphere = Util::extractColor( 0, args );
      if ( lowerHemisphere )
        env->setLowerHemisphere( *lowerHemisphere );

      auto upperHemisphere = Util::extractColor( 1, args );
      if ( upperHemisphere )
        env->setUpperHemisphere( *upperHemisphere );
    }

    Glacier::Environment* Environment::getEnvironment()
    {
      return mEnvironment;
    }

    void Environment::shutdown()
    {
      if ( instance )
        instance->unref();
      SAFE_DELETE( instance );
    }

  }

}