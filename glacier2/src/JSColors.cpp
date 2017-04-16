#include "StdAfx.h"
#include "JSNatives.h"
#include "JSObjectWrapper.h"
#include "JSUtil.h"
#include "Engine.h"
#include "ServiceLocator.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  namespace JS {

    string Colors::className( "Colors" );
    Colors* Colors::instance( nullptr );

    Colors::Colors(): ObjectWrapper( Wrapped_Colors )
    {
      //
    }

    void Colors::initialize( Handle<v8::Context> context )
    {
      Isolate* isolate = Isolate::GetCurrent();
      HandleScope handleScope( isolate );
      v8::Context::Scope contextScope( context );

      Local<FunctionTemplate> tpl = FunctionTemplate::New( isolate );

      tpl->SetClassName( Util::allocString( className.c_str() ) );
      tpl->InstanceTemplate()->SetInternalFieldCount( 1 );

      JS_TEMPLATE_SET( tpl, "set", jsSet );

      instance = new Colors();
      Local<v8::Object> object = tpl->GetFunction()->NewInstance();
      instance->wrap( object );

      context->Global()->Set( Util::allocString( className.c_str() ), object );
      instance->ref();
    }

    void Colors::shutdown()
    {
      if ( instance )
        instance->unref();
      SAFE_DELETE( instance );
    }

    //! \verbatim
    //! Colors.set( Int index, Int value )
    //! Colors.set( Int index, Color value )
    //! \endverbatim
    void Colors::jsSet( const FunctionCallbackInfo<v8::Value>& args )
    {
      v8::Isolate* isolate = args.GetIsolate();
      HandleScope handleScope( isolate );

      Colors* colors = unwrap( args.Holder() );

      if ( args.Length() != 2 || !args[0]->IsNumber() || ( !args[1]->IsNumber() && !args[1]->IsObject() ) )
      {
        Util::throwException( isolate,
          L"Syntax error: Colors.set( Int index, [Int,Color] value )" );
        return;
      }

      uint32_t index = args[0]->Int32Value();
      if ( index >= Glacier::Colors::Color_MAX )
      {
        Util::throwException( isolate,
          L"Colors.set: Color index %d out of bounds (%d)",
          index, Glacier::Colors::Color_MAX );
        return;
      }

      if ( args[1]->IsObject() )
      {
        auto clr = Util::extractColor( 1, args );
        if ( clr )
          Locator::getColors().set( ( Glacier::Colors::Value )index, *clr );
      }
      else
        Locator::getColors().set( (Glacier::Colors::Value)index, args[1]->Int32Value() );
    }

  }

}