#include "StdAfx.h"
#include "JSNatives.h"
#include "JSObjectWrapper.h"
#include "JSUtil.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

// warning C4244: '=' : conversion from double to float, possible loss of data
#pragma warning( disable: 4244 )

namespace Glacier {

  namespace JS {

    string Quaternion::className( "Quaternion" );

    Quaternion::Quaternion( const Ogre::Quaternion& source ):
    Ogre::Quaternion( source ), ObjectWrapper( Wrapped_Quaternion )
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

      tpl->SetClassName( Util::allocString( className.c_str() ) );
      tpl->InstanceTemplate()->SetInternalFieldCount( 1 );

      JS_TEMPLATE_ACCESSOR( tpl, "w", jsGetW, jsSetW );
      JS_TEMPLATE_ACCESSOR( tpl, "x", jsGetX, jsSetX );
      JS_TEMPLATE_ACCESSOR( tpl, "y", jsGetY, jsSetY );
      JS_TEMPLATE_ACCESSOR( tpl, "z", jsGetZ, jsSetZ );

      JS_TEMPLATE_SET( tpl, "toString", jsToString );
      JS_TEMPLATE_SET( tpl, "multiply", jsMultiply );
      JS_TEMPLATE_SET( tpl, "dot", jsDot );
      JS_TEMPLATE_SET( tpl, "norm", jsNorm );
      JS_TEMPLATE_SET( tpl, "normalise", jsNormalise );
      JS_TEMPLATE_SET( tpl, "inverse", jsInverse );
      JS_TEMPLATE_SET( tpl, "unitInverse", jsUnitInverse );
      JS_TEMPLATE_SET( tpl, "getRoll", jsGetRoll );
      JS_TEMPLATE_SET( tpl, "getPitch", jsGetPitch );
      JS_TEMPLATE_SET( tpl, "getYaw", jsGetYaw );

      exports->Set( isolate, className.c_str(), tpl );
      constructor.Set( isolate, tpl );
    }

    //! New JS::Quaternion from Ogre::Quaternion
    Local<v8::Object> Quaternion::newFrom( const Ogre::Quaternion& qtn )
    {
      Local<v8::Function> constFunc = constructor.Get(
        Isolate::GetCurrent() )->GetFunction();
      Local<v8::Object> object = constFunc->NewInstance();
      Quaternion* ret = unwrap( object );
      // Ugly this way, but the Ogre::Quaternion assignment operator silently fails.
      ret->w = qtn.w;
      ret->x = qtn.x;
      ret->y = qtn.y;
      ret->z = qtn.z;
      return object;
    }

    //! \verbatim
    //! Quaternion()
    //! Quaternion( Radian rotation, Vector3 axis )
    //! Quaternion( Real w, Real x, Real y, Real z )
    //! Quaternion([Real w, Real x, Real y, Real z])
    //! Quaternion({Real w, Real x, Real y, Real z})
    //! \endverbatim
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

    //! \verbatim
    //! Real Quaternion.w
    //! \endverbatim
    void Quaternion::jsGetW( Local<v8::String> prop,
    const PropertyCallbackInfo<v8::Value>& info )
    {
      Quaternion* ptr = unwrap( info.This() );
      info.GetReturnValue().Set( ptr->w );
    }

    //! \verbatim
    //! Quaternion.w
    //! \endverbatim
    void Quaternion::jsSetW( Local<v8::String> prop,
    Local<v8::Value> value, const PropertyCallbackInfo<void>& info )
    {
      Quaternion* ptr = unwrap( info.This() );
      ptr->w = value->NumberValue();
    }

    //! \verbatim
    //! Real Quaternion.x
    //! \endverbatim
    void Quaternion::jsGetX( Local<v8::String> prop,
    const PropertyCallbackInfo<v8::Value>& info )
    {
      Quaternion* ptr = unwrap( info.This() );
      info.GetReturnValue().Set( ptr->x );
    }

    //! \verbatim
    //! Quaternion.x
    //! \endverbatim
    void Quaternion::jsSetX( Local<v8::String> prop,
    Local<v8::Value> value, const PropertyCallbackInfo<void>& info )
    {
      Quaternion* ptr = unwrap( info.This() );
      ptr->x = value->NumberValue();
    }

    //! \verbatim
    //! Real Quaternion.y
    //! \endverbatim
    void Quaternion::jsGetY( Local<v8::String> prop,
    const PropertyCallbackInfo<v8::Value>& info )
    {
      Quaternion* ptr = unwrap( info.This() );
      info.GetReturnValue().Set( ptr->y );
    }

    //! \verbatim
    //! Quaternion.y
    //! \endverbatim
    void Quaternion::jsSetY( Local<v8::String> prop,
    Local<v8::Value> value, const PropertyCallbackInfo<void>& info )
    {
      Quaternion* ptr = unwrap( info.This() );
      ptr->y = value->NumberValue();
    }

    //! \verbatim
    //! Real Quaternion.z
    //! \endverbatim
    void Quaternion::jsGetZ( Local<v8::String> prop,
    const PropertyCallbackInfo<v8::Value>& info )
    {
      Quaternion* ptr = unwrap( info.This() );
      info.GetReturnValue().Set( ptr->z );
    }

    //! \verbatim
    //! Quaternion.z
    //! \endverbatim
    void Quaternion::jsSetZ( Local<v8::String> prop,
    Local<v8::Value> value, const PropertyCallbackInfo<void>& info )
    {
      Quaternion* ptr = unwrap( info.This() );
      ptr->z = value->NumberValue();
    }

    //! \verbatim
    //! String Quaternion.toString
    //! \endverbatim
    void Quaternion::jsToString( const FunctionCallbackInfo<v8::Value>& args )
    {
      Quaternion* ptr = unwrap( args.Holder() );
      char result[128];
      sprintf_s<128>( result, "Quaternion[%f,%f,%f,%f]", ptr->w, ptr->x, ptr->y, ptr->z );
      args.GetReturnValue().Set( Util::allocString( result ) );
    }

    //! \verbatim
    //! Vector3 Quaternion.multiply( Vector3 )
    //! Rotate vector by quaternion
    //! \endverbatim
    void Quaternion::jsMultiply( const FunctionCallbackInfo<v8::Value>& args )
    {
      Quaternion* ptr = unwrap( args.Holder() );
      Vector3* other = Util::extractVector3( 0, args );
      if ( !other )
        return;
      Ogre::Vector3 result = (Ogre::Quaternion)*ptr * (Ogre::Vector3)*other;
      args.GetReturnValue().Set( Vector3::newFrom( result ) );
    }

    //! \verbatim
    //! Real Quaternion.dot( Quaternion other )
    //! \endverbatim
    void Quaternion::jsDot( const FunctionCallbackInfo<v8::Value>& args )
    {
      Quaternion* ptr = unwrap( args.Holder() );
      Quaternion* other = Util::extractQuaternion( 0, args );
      if ( !other )
        return;
      args.GetReturnValue().Set( ptr->Dot( *other ) );
    }

    //! \verbatim
    //! Real Quaternion.norm()
    //! \endverbatim
    void Quaternion::jsNorm( const FunctionCallbackInfo<v8::Value>& args )
    {
      Quaternion* ptr = unwrap( args.Holder() );
      args.GetReturnValue().Set( ptr->Norm() );
    }

    //! \verbatim
    //! Real Quaternion.normalise()
    //! \endverbatim
    void Quaternion::jsNormalise( const FunctionCallbackInfo<v8::Value>& args )
    {
      Quaternion* ptr = unwrap( args.Holder() );
      args.GetReturnValue().Set( ptr->normalise() );
    }

    //! \verbatim
    //! Quaternion Quaternion.inverse()
    //! \endverbatim
    void Quaternion::jsInverse( const FunctionCallbackInfo<v8::Value>& args )
    {
      Quaternion* ptr = unwrap( args.Holder() );
      args.GetReturnValue().Set( Quaternion::newFrom( ptr->Inverse() ) );
    }

    //! \verbatim
    //! Quaternion Quaternion.unitInverse()
    //! \endverbatim
    void Quaternion::jsUnitInverse( const FunctionCallbackInfo<v8::Value>& args )
    {
      Quaternion* ptr = unwrap( args.Holder() );
      args.GetReturnValue().Set( Quaternion::newFrom( ptr->UnitInverse() ) );
    }

    //! \verbatim
    //! Radian Quaternion.getRoll()
    //! \endverbatim
    void Quaternion::jsGetRoll( const FunctionCallbackInfo<v8::Value>& args )
    {
      Quaternion* ptr = unwrap( args.Holder() );
      args.GetReturnValue().Set( ptr->getRoll().valueRadians() );
    }

    //! \verbatim
    //! Radian Quaternion.getPitch()
    //! \endverbatim
    void Quaternion::jsGetPitch( const FunctionCallbackInfo<v8::Value>& args )
    {
      Quaternion* ptr = unwrap( args.Holder() );
      args.GetReturnValue().Set( ptr->getPitch().valueRadians() );
    }

    //! \verbatim
    //! Radian Quaternion.getYaw()
    //! \endverbatim
    void Quaternion::jsGetYaw( const FunctionCallbackInfo<v8::Value>& args )
    {
      Quaternion* ptr = unwrap( args.Holder() );
      args.GetReturnValue().Set( ptr->getYaw().valueRadians() );
    }

  }

}