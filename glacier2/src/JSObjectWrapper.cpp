#include "StdAfx.h"
#include "JSNatives.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  namespace JS {

    ObjectWrapper::ObjectWrapper(): mJSReferences( 0 )
    {
      mJSIsolate = v8::Isolate::GetCurrent();
    }

    ObjectWrapper::~ObjectWrapper()
    {
      if ( !persistent().IsEmpty() )
      {
        assert( persistent().IsNearDeath() );
        persistent().ClearWeak();
        persistent().Reset();
      }
    }

    void ObjectWrapper::ref()
    {
      assert( !persistent().IsEmpty() );
      persistent().ClearWeak();
      mJSReferences++;
    }

    void ObjectWrapper::unref()
    {
      assert( !persistent().IsEmpty() );
      assert( !persistent().IsWeak() );
      assert( mJSReferences > 0 );
      if ( --mJSReferences == 0 )
        makeWeak();
    }

    void ObjectWrapper::weakCallback(
    const v8::WeakCallbackData<v8::Object, ObjectWrapper>& data )
    {
      v8::Isolate* isolate = data.GetIsolate();
      v8::HandleScope scope( isolate );
      ObjectWrapper* wrapper = data.GetParameter();
      assert( wrapper->mJSReferences == 0 );
      assert( wrapper->mJSHandle.IsNearDeath() );
      assert( data.GetValue() == v8::Local<v8::Object>::New( isolate, wrapper->mJSHandle ) );
      wrapper->mJSHandle.Reset();
      delete wrapper;
    }

  }

}