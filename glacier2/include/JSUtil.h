#pragma once
#include <v8.h>

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  //! \addtogroup Glacier
  //! @{

  //! \addtogroup Scripting
  //! @{

  namespace JS {

    using v8::Isolate;
    using v8::HandleScope;
    using v8::Local;
    using v8::Handle;
    using v8::Persistent;
    using v8::Eternal;
    using v8::PropertyCallbackInfo;
    using v8::ObjectTemplate;
    using v8::FunctionTemplate;
    using v8::FunctionCallbackInfo;
    using v8::WeakCallbackData;

    class Vector3;
    class Quaternion;

    namespace Util {

#     define JS_TEMPLATE_ACCESSOR(tpl,x,y,z) tpl->PrototypeTemplate()->SetAccessor( \
        Util::allocString( x ), y, z )

#     define JS_TEMPLATE_SET(tpl,x,y) tpl->PrototypeTemplate()->Set( \
        Util::allocString( x ), FunctionTemplate::New( y ) )

      //! Create a local JavaScript String instance from given source string.
      inline Local<v8::String> allocString( const wstring& str, Isolate* isolate = nullptr )
      {
        return v8::String::NewFromTwoByte(
          isolate ? isolate : Isolate::GetCurrent(),
          (uint16_t*)str.c_str() );
      }
      
      //! Create a local JavaScript String instance from given source string.
      inline Local<v8::String> allocString( const Ogre::String& str, Isolate* isolate = nullptr )
      {
        return v8::String::NewFromUtf8(
          isolate ? isolate : Isolate::GetCurrent(),
          str.c_str() );
      }

      //! Create a local JavaScript String instance from given source string.
      inline Local<v8::String> allocString( const char* str, Isolate* isolate = nullptr )
      {
        return v8::String::NewFromUtf8(
          isolate ? isolate : Isolate::GetCurrent(),
          str );
      }

      Vector3* extractVector3( int arg, const FunctionCallbackInfo<v8::Value>& args );
      Quaternion* extractQuaternion( int arg, const FunctionCallbackInfo<v8::Value>& args );
      //Ogre::Radian extractRadian( int arg, const FunctionCallbackInfo<v8::Value>& args );

    }

  }

  //! @}

  //! @}

}