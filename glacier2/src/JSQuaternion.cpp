#include "StdAfx.h"
#include "JSNatives.h"
#include "JSObjectWrapper.h"
#include "JSUtil.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  namespace JS {

    const char* cJSQuaternionClass = "Quaternion";
    Eternal<v8::FunctionTemplate> Quaternion::constructor;

    Quaternion::Quaternion( const Ogre::Quaternion& source ):
    Ogre::Quaternion( source )
    {
      //
    }

    Quaternion::~Quaternion()
    {
      //
    }

    void Quaternion::initialize( Handle<ObjectTemplate> exports )
    {
      Isolate* isolate = Isolate::GetCurrent();
      HandleScope handleScope( isolate );

      Local<FunctionTemplate> tpl = FunctionTemplate::New( Quaternion::create );

      tpl->SetClassName( Util::allocString( cJSQuaternionClass ) );
      tpl->InstanceTemplate()->SetInternalFieldCount( 1 );

      JS_TEMPLATE_ACCESSOR( tpl, "w", jsGetW, jsSetW );
      JS_TEMPLATE_ACCESSOR( tpl, "x", jsGetX, jsSetX );
      JS_TEMPLATE_ACCESSOR( tpl, "y", jsGetY, jsSetY );
      JS_TEMPLATE_ACCESSOR( tpl, "z", jsGetZ, jsSetZ );

      JS_TEMPLATE_SET( tpl, "toString", jsToString );
      JS_TEMPLATE_SET( tpl, "multiply", jsMultiply );
      JS_TEMPLATE_SET( tpl, "getRoll", jsGetRoll );
      JS_TEMPLATE_SET( tpl, "getPitch", jsGetPitch );
      JS_TEMPLATE_SET( tpl, "getYaw", jsGetYaw );

      exports->Set( isolate, cJSQuaternionClass, tpl );
      constructor.Set( isolate, tpl );
    }

    //! New JS::Quaternion from Ogre::Quaternion
    Local<v8::Object> Quaternion::newFrom( const Ogre::Quaternion& qtn )
    {
      Local<v8::Function> constFunc = constructor.Get(
        Isolate::GetCurrent() )->GetFunction();
      Local<v8::Object> object = constFunc->NewInstance();
      Quaternion* ret = unwrap<Quaternion>( object );
      ret->w = qtn.w;
      ret->x = qtn.x;
      ret->y = qtn.y;
      ret->z = qtn.z;
      return object;
    }

    //! Quaternion()
    //! Quaternion( Radian rotation, Vector3 axis )
    //! Quaternion( Real w, Real x, Real y, Real z )
    //! Quaternion([Real w, Real x, Real y, Real z])
    //! Quaternion({Real w, Real x, Real y, Real z})
    void Quaternion::create( const FunctionCallbackInfo<v8::Value>& args )
    {
      HandleScope handleScope( args.GetIsolate() );
      if ( !args.IsConstructCall() )
      {
        args.GetIsolate()->ThrowException(
          Util::allocString( "Function called as non-constructor" ) );
        return;
      }
      Ogre::Quaternion qtn( Ogre::Quaternion::IDENTITY );
      if ( args.Length() == 4 )
      {
        qtn.w = args[0]->NumberValue();
        qtn.x = args[1]->NumberValue();
        qtn.y = args[2]->NumberValue();
        qtn.z = args[3]->NumberValue();
      }
      else if ( args.Length() == 2 )
      {
        Vector3* axis = Util::extractVector3( 1, args );
        qtn.FromAngleAxis( Ogre::Radian( args[0]->NumberValue() ), *axis );
      }
      else if ( args.Length() == 1 )
      {
        if ( args[0]->IsArray() )
        {
          v8::Local<v8::Array> arr = v8::Local<v8::Array>::Cast( args[0] );
          if ( arr->Length() == 4 )
          {
            qtn.w = arr->Get( 0 )->NumberValue();
            qtn.x = arr->Get( 1 )->NumberValue();
            qtn.y = arr->Get( 2 )->NumberValue();
            qtn.z = arr->Get( 3 )->NumberValue();
          }
        }
        else if ( args[0]->IsObject() )
        {
          v8::Local<v8::Value> val = args[0]->ToObject()->Get( Util::allocString( "w" ) );
          if ( val->IsNumber() )
            qtn.w = val->NumberValue();
          val = args[0]->ToObject()->Get( Util::allocString( "x" ) );
          if ( val->IsNumber() )
            qtn.x = val->NumberValue();
          val = args[0]->ToObject()->Get( Util::allocString( "y" ) );
          if ( val->IsNumber() )
            qtn.y = val->NumberValue();
          val = args[0]->ToObject()->Get( Util::allocString( "z" ) );
          if ( val->IsNumber() )
            qtn.z = val->NumberValue();
        }
      }
      Quaternion* object = new Quaternion( qtn );
      object->wrap( args.This() );
      args.GetReturnValue().Set( args.This() );
    }

    //! Real Quaternion.w getter
    void Quaternion::jsGetW( Local<v8::String> prop,
    const PropertyCallbackInfo<v8::Value>& info )
    {
      Quaternion* ptr = unwrap<Quaternion>( info.This() );
      info.GetReturnValue().Set( ptr->w );
    }

    //! Real Quaternion.w setter
    void Quaternion::jsSetW( Local<v8::String> prop,
    Local<v8::Value> value, const PropertyCallbackInfo<void>& info )
    {
      Quaternion* ptr = unwrap<Quaternion>( info.This() );
      ptr->w = value->NumberValue();
    }

    //! Real Quaternion.x getter
    void Quaternion::jsGetX( Local<v8::String> prop,
    const PropertyCallbackInfo<v8::Value>& info )
    {
      Quaternion* ptr = unwrap<Quaternion>( info.This() );
      info.GetReturnValue().Set( ptr->x );
    }

    //! Real Quaternion.x setter
    void Quaternion::jsSetX( Local<v8::String> prop,
    Local<v8::Value> value, const PropertyCallbackInfo<void>& info )
    {
      Quaternion* ptr = unwrap<Quaternion>( info.This() );
      ptr->x = value->NumberValue();
    }

    //! Real Quaternion.y getter
    void Quaternion::jsGetY( Local<v8::String> prop,
    const PropertyCallbackInfo<v8::Value>& info )
    {
      Quaternion* ptr = unwrap<Quaternion>( info.This() );
      info.GetReturnValue().Set( ptr->y );
    }

    //! Real Quaternion.y setter
    void Quaternion::jsSetY( Local<v8::String> prop,
    Local<v8::Value> value, const PropertyCallbackInfo<void>& info )
    {
      Quaternion* ptr = unwrap<Quaternion>( info.This() );
      ptr->y = value->NumberValue();
    }

    //! Real Quaternion.z getter
    void Quaternion::jsGetZ( Local<v8::String> prop,
    const PropertyCallbackInfo<v8::Value>& info )
    {
      Quaternion* ptr = unwrap<Quaternion>( info.This() );
      info.GetReturnValue().Set( ptr->z );
    }

    //! Real Quaternion.z setter
    void Quaternion::jsSetZ( Local<v8::String> prop,
    Local<v8::Value> value, const PropertyCallbackInfo<void>& info )
    {
      Quaternion* ptr = unwrap<Quaternion>( info.This() );
      ptr->z = value->NumberValue();
    }

    //! Quaternion.toString
    void Quaternion::jsToString( const FunctionCallbackInfo<v8::Value>& args )
    {
      Quaternion* ptr = unwrap<Quaternion>( args.Holder() );
      char result[128];
      sprintf_s<128>( result, "Quaternion[%f,%f,%f,%f]", ptr->w, ptr->x, ptr->y, ptr->z );
      args.GetReturnValue().Set( Util::allocString( result ) );
    }

    //! Vector3 Quaternion.multiply( Vector3 )
    //! Rotate vector by quaternion
    void Quaternion::jsMultiply( const FunctionCallbackInfo<v8::Value>& args )
    {
      Quaternion* ptr = unwrap<Quaternion>( args.Holder() );
      Vector3* other = Util::extractVector3( 0, args );
      if ( !other )
        return;
      Ogre::Vector3 result = (Ogre::Quaternion)*ptr * (Ogre::Vector3)*other;
      args.GetReturnValue().Set( Vector3::newFrom( result ) );
    }

    //! Radian Quaternion.getRoll()
    void Quaternion::jsGetRoll( const FunctionCallbackInfo<v8::Value>& args )
    {
      Quaternion* ptr = unwrap<Quaternion>( args.Holder() );
      args.GetReturnValue().Set( ptr->getRoll().valueRadians() );
    }

    //! Radian Quaternion.getPitch()
    void Quaternion::jsGetPitch( const FunctionCallbackInfo<v8::Value>& args )
    {
      Quaternion* ptr = unwrap<Quaternion>( args.Holder() );
      args.GetReturnValue().Set( ptr->getPitch().valueRadians() );
    }

    //! Radian Quaternion.getYaw()
    void Quaternion::jsGetYaw( const FunctionCallbackInfo<v8::Value>& args )
    {
      Quaternion* ptr = unwrap<Quaternion>( args.Holder() );
      args.GetReturnValue().Set( ptr->getYaw().valueRadians() );
    }

  }

}