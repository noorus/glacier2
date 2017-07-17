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
      JS_TEMPLATE_SET( tpl, "setSun", jsSetSun );

      instance = new Environment( env );
      Local<v8::Object> object = tpl->GetFunction()->NewInstance();
      instance->wrap( object );

      context->Global()->Set( Util::allocString( className.c_str() ), object );
      instance->ref();
    }

    //! \verbatim
    //! Environment.setAmbience( Color lowerHemisphere, Color upperHemisphere, float scale )
    //! \endverbatim
    void Environment::jsSetAmbience( const FunctionCallbackInfo<v8::Value>& args )
    {
      v8::Isolate* isolate = args.GetIsolate();
      Environment* ptr = unwrap( args.Holder() );
      HandleScope handleScope( args.GetIsolate() );

      if ( args.Length() != 3 || !args[0]->IsObject() || !args[1]->IsObject() || !args[2]->IsNumber() )
      {
        Util::throwException( isolate,
          L"Syntax error: Environment.setAmbience( Color lowerHemisphere, Color upperHemisphere, float scale )" );
        return;
      }

      auto env = ptr->getEnvironment();

      auto lowerHemisphere = Util::extractColor( 0, args );
      if ( lowerHemisphere )
        env->setLowerHemisphere( *lowerHemisphere );

      auto upperHemisphere = Util::extractColor( 1, args );
      if ( upperHemisphere )
        env->setUpperHemisphere( *upperHemisphere );

      env->setHemisphereScale( (Real)args[2]->NumberValue() );
    }

    //! \verbatim
    //! Environment.setSun( float power, Color diffuse, Color specular, Vector3 direction )
    //! \endverbatim
    void Environment::jsSetSun( const FunctionCallbackInfo<v8::Value>& args )
    {
      v8::Isolate* isolate = args.GetIsolate();
      Environment* ptr = unwrap( args.Holder() );
      HandleScope handleScope( args.GetIsolate() );

      if ( args.Length() != 4 || !args[0]->IsNumber() || !args[1]->IsObject() || !args[2]->IsObject() || !args[3]->IsObject() )
      {
        Util::throwException( isolate,
          L"Syntax error: Environment.setSun( float power, Color diffuse, Color specular, Vector3 direction )" );
        return;
      }

      auto env = ptr->getEnvironment();

      auto power = (Real)args[0]->NumberValue();
      auto diffuse = Util::extractColor( 1, args );
      auto specular = Util::extractColor( 2, args );
      auto direction = Util::extractVector3( 3, args );

      if ( diffuse && specular && direction )
        env->setSun( power, *diffuse, *specular, *direction );
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