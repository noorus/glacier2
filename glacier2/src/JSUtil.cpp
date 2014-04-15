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
        if ( args.Length() < ( arg + 1 ) || !args[arg]->IsObject() )
        {
          // TODO argument %d
          args.GetIsolate()->ThrowException(
            v8::String::NewFromUtf8( args.GetIsolate(),
            "Argument must be a Vector3" ) );
          return nullptr;
        }
        // TODO actually verify object type..
        Vector3* that = Vector3::unwrap<Vector3>( args[arg]->ToObject() );
        return that;
      }

      //! Expect and extract a Quaternion object as args[arg],
      //! throw JS exception and return null on failure
      Quaternion* extractQuaternion( int arg,
      const FunctionCallbackInfo<v8::Value>& args )
      {
        if ( args.Length() < ( arg + 1 ) || !args[arg]->IsObject() )
        {
          // TODO argument %d
          args.GetIsolate()->ThrowException(
            v8::String::NewFromUtf8( args.GetIsolate(),
            "Argument must be a Quaternion" ) );
          return nullptr;
        }
        // TODO actually verify object type..
        Quaternion* that = Quaternion::unwrap<Quaternion>( args[arg]->ToObject() );
        return that;
      }

    }

  }

}