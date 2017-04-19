#include "StdAfx.h"
#include "JSNatives.h"
#include "JSObjectWrapper.h"
#include "JSUtil.h"
#include "Engine.h"
#include "Console.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

// warning C4244: '=' : conversion from double to float, possible loss of data
#pragma warning( disable: 4244 )

namespace Glacier {

  namespace JS {

    string Color::className( "Color" );

    Color::Color( const Ogre::ColourValue& source ):
    Ogre::ColourValue( source ), ObjectWrapper( Wrapped_ColourValue )
    {
      //
    }

    Color::~Color()
    {
      //
    }

    void Color::initialize( Handle<ObjectTemplate> exports )
    {
      Isolate* isolate = Isolate::GetCurrent();
      HandleScope handleScope( isolate );

      Local<FunctionTemplate> tpl = FunctionTemplate::New( isolate, Color::create );

      tpl->SetClassName( Util::allocString( className.c_str() ) );
      tpl->InstanceTemplate()->SetInternalFieldCount( 1 );

      JS_TEMPLATE_ACCESSOR( tpl, "r", jsGetR, jsSetR );
      JS_TEMPLATE_ACCESSOR( tpl, "g", jsGetG, jsSetG );
      JS_TEMPLATE_ACCESSOR( tpl, "b", jsGetB, jsSetB );
      JS_TEMPLATE_ACCESSOR( tpl, "a", jsGetA, jsSetA );

      JS_TEMPLATE_SET( tpl, "toString", jsToString );
      JS_TEMPLATE_SET( tpl, "add", jsAdd );
      JS_TEMPLATE_SET( tpl, "subtract", jsSubtract );
      JS_TEMPLATE_SET( tpl, "multiply", jsMultiply );
      JS_TEMPLATE_SET( tpl, "saturate", jsSaturate );
      JS_TEMPLATE_SET( tpl, "saturateCopy", jsSaturateCopy );

      exports->Set( isolate, className.c_str(), tpl );
      constructor.Set( isolate, tpl );
    }

    //! New JS::Color from Ogre::ColourValue
    Local<v8::Object> Color::newFrom( const Ogre::ColourValue& color )
    {
      Local<v8::Function> constFunc = constructor.Get(
        Isolate::GetCurrent() )->GetFunction();
      Local<v8::Object> object = constFunc->NewInstance();
      Color* ret = unwrap( object );
      ret->r = color.r;
      ret->g = color.g;
      ret->b = color.b;
      ret->a = color.a;
      return object;
    }

    //! \verbatim
    //! Color()
    //! Color( Real r, Real g, Real b, Real a = 1.0 )
    //! Color([Real r, Real g, Real b, Real a])
    //! Color({Real r, Real g, Real b, Real a})
    //! \endverbatim
    void Color::create( const FunctionCallbackInfo<v8::Value>& args )
    {
      HandleScope handleScope( args.GetIsolate() );
      if ( !args.IsConstructCall() )
      {
        args.GetIsolate()->ThrowException(
          Util::allocString( "Function called as non-constructor" ) );
        return;
      }
      Ogre::ColourValue clr( Ogre::ColourValue::Black );
      clr.a = 1.0f;
      if ( args.Length() >= 3 )
      {
        clr.r = args[0]->NumberValue();
        clr.g = args[1]->NumberValue();
        clr.b = args[2]->NumberValue();
        if ( args.Length() > 3 )
          clr.a = args[3]->NumberValue();
      }
      else if ( args.Length() == 1 )
      {
        if ( args[0]->IsNumber() )
        {
          clr.r = clr.g = clr.b = clr.a = args[0]->NumberValue();
        }
        else if ( args[0]->IsArray() )
        {
          v8::Local<v8::Array> arr = v8::Local<v8::Array>::Cast( args[0] );
          if ( arr->Length() >= 3 )
          {
            clr.r = arr->Get( 0 )->NumberValue();
            clr.g = arr->Get( 1 )->NumberValue();
            clr.b = arr->Get( 2 )->NumberValue();
            if ( arr->Length() > 3 )
              clr.a = arr->Get( 3 )->NumberValue();
          }
        }
        else if ( args[0]->IsObject() )
        {
          auto obj = args[0]->ToObject();
          v8::Local<v8::Value> val = obj->Get( Util::allocString( "r" ) );
          if ( val->IsNumber() )
            clr.r = val->NumberValue();
          val = obj->Get( Util::allocString( "g" ) );
          if ( val->IsNumber() )
            clr.g = val->NumberValue();
          val = obj->Get( Util::allocString( "b" ) );
          if ( val->IsNumber() )
            clr.b = val->NumberValue();
          val = obj->Get( Util::allocString( "a" ) );
          if ( val->IsNumber() )
            clr.a = val->NumberValue();
        }
      }
      Color* object = new Color( clr );
      object->wrap( args.This() );
      args.GetReturnValue().Set( args.This() );
    }

    //! \verbatim
    //! Real Color.r
    //! \endverbatim
    void Color::jsGetR( Local<v8::String> prop,
      const PropertyCallbackInfo<v8::Value>& info )
    {
      Color* ptr = unwrap( info.This() );
      info.GetReturnValue().Set( ptr->r );
    }

    //! \verbatim
    //! Color.r
    //! \endverbatim
    void Color::jsSetR( Local<v8::String> prop,
      Local<v8::Value> value, const PropertyCallbackInfo<void>& info )
    {
      Color* ptr = unwrap( info.This() );
      ptr->r = value->NumberValue();
    }

    //! \verbatim
    //! Real Color.g
    //! \endverbatim
    void Color::jsGetG( Local<v8::String> prop,
      const PropertyCallbackInfo<v8::Value>& info )
    {
      Color* ptr = unwrap( info.This() );
      info.GetReturnValue().Set( ptr->g );
    }

    //! \verbatim
    //! Color.g
    //! \endverbatim
    void Color::jsSetG( Local<v8::String> prop,
      Local<v8::Value> value, const PropertyCallbackInfo<void>& info )
    {
      Color* ptr = unwrap( info.This() );
      ptr->g = value->NumberValue();
    }

    //! \verbatim
    //! Real Color.b
    //! \endverbatim
    void Color::jsGetB( Local<v8::String> prop,
      const PropertyCallbackInfo<v8::Value>& info )
    {
      Color* ptr = unwrap( info.This() );
      info.GetReturnValue().Set( ptr->b );
    }

    //! \verbatim
    //! Color.b
    //! \endverbatim
    void Color::jsSetB( Local<v8::String> prop,
      Local<v8::Value> value, const PropertyCallbackInfo<void>& info )
    {
      Color* ptr = unwrap( info.This() );
      ptr->b = value->NumberValue();
    }

    //! \verbatim
    //! Real Color.a
    //! \endverbatim
    void Color::jsGetA( Local<v8::String> prop,
      const PropertyCallbackInfo<v8::Value>& info )
    {
      Color* ptr = unwrap( info.This() );
      info.GetReturnValue().Set( ptr->a );
    }

    //! \verbatim
    //! Color.A
    //! \endverbatim
    void Color::jsSetA( Local<v8::String> prop,
      Local<v8::Value> value, const PropertyCallbackInfo<void>& info )
    {
      Color* ptr = unwrap( info.This() );
      ptr->a = value->NumberValue();
    }

    //! \verbatim
    //! Color Color.add( value, ... )
    //! Color Color.add( Color, ... )
    //! \endverbatim
    void Color::jsAdd( const FunctionCallbackInfo<v8::Value>& args )
    {
      auto ptr = unwrap( args.Holder() );
      ColourValue val = *ptr;

      for ( size_t i = 0; i < args.Length(); i++ )
      {
        if ( args[i]->IsNumber() ) {
          auto otherval = (Real)args[i]->NumberValue();
          ColourValue other( otherval, otherval, otherval, otherval );
          val += other;
        }
        else {
          auto other = Util::extractColor( i, args );
          if ( !other )
            return;
          val += (ColourValue)*other;
        }
      }
      args.GetReturnValue().Set( Color::newFrom( val ) );
    }

    //! \verbatim
    //! Color Color.subtract( value, ... )
    //! Color Color.subtract( Color, ... )
    //! \endverbatim
    void Color::jsSubtract( const FunctionCallbackInfo<v8::Value>& args )
    {
      auto ptr = unwrap( args.Holder() );
      ColourValue val = *ptr;

      for ( size_t i = 0; i < args.Length(); i++ )
      {
        if ( args[i]->IsNumber() ) {
          auto otherval = (Real)args[i]->NumberValue();
          ColourValue other( otherval, otherval, otherval, otherval );
          val -= other;
        }
        else {
          auto other = Util::extractColor( i, args );
          if ( !other )
            return;
          val -= (ColourValue)*other;
        }
      }
      args.GetReturnValue().Set( Color::newFrom( val ) );
    }

    //! \verbatim
    //! Color Color.multiply( scalar )
    //! \endverbatim
    void Color::jsMultiply( const FunctionCallbackInfo<v8::Value>& args )
    {
      auto ptr = unwrap( args.Holder() );
      ColourValue val = *ptr;

      if ( !args[0]->IsNumber() ) {
        args.GetIsolate()->ThrowException( JS::Util::allocString( "Expected scalar argument" ) );
        return;
      }

      val *= (Real)args[0]->NumberValue();

      args.GetReturnValue().Set( Color::newFrom( val ) );
    }
    //! \verbatim
    //! Color.saturate()
    //! \endverbatim
    void Color::jsSaturate( const FunctionCallbackInfo<v8::Value>& args )
    {
      Color* ptr = unwrap( args.Holder() );
      ptr->saturate();
    }

    //! \verbatim
    //! Color Color.saturateCopy()
    //! \endverbatim
    void Color::jsSaturateCopy( const FunctionCallbackInfo<v8::Value>& args )
    {
      Color* ptr = unwrap( args.Holder() );
      args.GetReturnValue().Set( Color::newFrom( ptr->saturateCopy() ) );
    }

    //! \verbatim
    //! String Color.toString
    //! \endverbatim
    void Color::jsToString( const FunctionCallbackInfo<v8::Value>& args )
    {
      Color* ptr = unwrap( args.Holder() );
      char result[128];
      sprintf_s<128>( result, "Color[%f,%f,%f,%f]", ptr->r, ptr->g, ptr->b, ptr->a );
      args.GetReturnValue().Set( Util::allocString( result ) );
    }

  }

}