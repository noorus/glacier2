#include "StdAfx.h"
#include "JSUtil.h"
#include "JSNatives.h"
#include "JSObjectWrapper.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  namespace JS {

    namespace Util {

      //! Expect and extract a Vector3 object as args[arg],
      //! throw JS exception and return null on failure
      Vector3* extractVector3( int arg,
      const FunctionCallbackInfo<v8::Value>& args )
      {
        char error[64];
        if ( args.Length() >= ( arg + 1 ) && args[arg]->IsObject() )
        {
          Local<v8::Object> object = args[arg]->ToObject();
          if ( ObjectWrapper::isWrappedType( object, Wrapped_Vector3 ) )
          {
            Vector3* unwrapped = ObjectWrapper::unwrap<Vector3>( object );
            return unwrapped;
          }
        }
        sprintf_s<64>( error, "Expected object Vector3 as argument %d", arg );
        args.GetIsolate()->ThrowException(
          allocString( error, args.GetIsolate() ) );
        return nullptr;
      }

      //! Expect and extract a Quaternion object as args[arg],
      //! throw JS exception and return null on failure
      Quaternion* extractQuaternion( int arg,
      const FunctionCallbackInfo<v8::Value>& args )
      {
        char error[64];
        if ( args.Length() >= ( arg + 1 ) && args[arg]->IsObject() )
        {
          Local<v8::Object> object = args[arg]->ToObject();
          if ( ObjectWrapper::isWrappedType( object, Wrapped_Quaternion ) )
          {
            Quaternion* unwrapped = ObjectWrapper::unwrap<Quaternion>( object );
            return unwrapped;
          }
        }
        sprintf_s<64>( error, "Expected object Quaternion as argument %d", arg );
        args.GetIsolate()->ThrowException(
          allocString( error, args.GetIsolate() ) );
        return nullptr;
      }

    }

  }

}