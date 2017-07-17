#pragma once
#include <v8.h>
#include "JSObjectWrapper.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  //! \addtogroup Glacier
  //! @{

  //! \addtogroup Scripting
  //! @{

  class Console;
  class Entity;
  class Environment;

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

    //! \class Console
    //! A JavaScript-wrapped Console.
    //! \sa Glacier::Console
    //! \sa ObjectWrapper
    class Console: public ObjectWrapper<Console> {
    protected:
      static Console* instance;
      Glacier::Console* mConsole;
      explicit Console( Glacier::Console* console );
      //! JavaScript Console.print.
      static void jsPrint( const FunctionCallbackInfo<v8::Value>& args );
      //! JavaScript Console.getVariable.
      static void jsGetVariable( const FunctionCallbackInfo<v8::Value>& args );
      //! JavaScript Console.setVariable.
      static void jsSetVariable( const FunctionCallbackInfo<v8::Value>& args );
      //! JavaScript Console.execute.
      static void jsExecute( const FunctionCallbackInfo<v8::Value>& args );
    public:
      Glacier::Console* getConsole();
      static void initialize( Glacier::Console* console, Handle<v8::Context> context );
      static void shutdown();
    };

    //! \class Colors
    //! A JavaScript-wrapped color service.
    //! \sa Glacier::Colors
    //! \sa ObjectWrapper
    class Colors: public ObjectWrapper<Colors> {
    protected:
      static Colors* instance;
      Colors();
      //! JavaScript Colors.set.
      static void jsSet( const FunctionCallbackInfo<v8::Value>& args );
    public:
      static void initialize( Handle<v8::Context> context );
      static void shutdown();
    };

    //! \class Environment
    //! A JavaScript-wrapped Environment.
    //! \sa Glacier::Environment
    //! \sa ObjectWrapper
    class Environment: public ObjectWrapper<Environment> {
    protected:
      static Environment* instance;
      Glacier::Environment* mEnvironment;
      explicit Environment( Glacier::Environment* environment );
      //! JavaScript Environment.setAmbience
      static void jsSetAmbience( const FunctionCallbackInfo<v8::Value>& args );
    public:
      static void initialize( Glacier::Environment* environment, Handle<v8::Context> context );
      Glacier::Environment* getEnvironment();
      static void shutdown();
    };

    class Entity: public ObjectWrapper<Entity> {
    protected:
      Glacier::Entity* mEntity;
      explicit Entity( Glacier::Entity* entity );
      static void jsToString( const FunctionCallbackInfo<v8::Value>& args );
      static void jsGetName( const FunctionCallbackInfo<v8::Value>& args );
    public:
      ~Entity();
      Glacier::Entity* getEntity();
      static Entity* create( Glacier::Entity* entity, Handle<v8::Context> context );
    };

    //! \class Material
    //! A JavaScript-wrapped material.
    //! \sa Glacier::Materials
    //! \sa ObjectWrapper
    /*class Material: public ObjectWrapper<Material> {
    protected:
      //! Constructor.
      //! \param source Source Ogre::Vector3 to initialize from.
      explicit Material( const Ogre::Vector3& source );

      //! Destructor.
      ~Material();

      //! My JavaScript-exported constructor function.
      static void create( const FunctionCallbackInfo<v8::Value>& args );

      //! Magic JavaScript toString method.
      //! Returns a compact string representation of the object.
      static void jsToString( const FunctionCallbackInfo<v8::Value>& args );
    public:

      //! Initializes my JavaScript object template in given object namespace.
      static void initialize( Handle<ObjectTemplate> exports );

      //! Creates a new JavaScript-compatible Vector3 from given source.
      //! \param  vector The Ogre::Vector3 to copy from.
      static Local<v8::Object> newFrom( const Ogre::Vector3& vector );
    };*/

    //! \class Color
    //! A JavaScript-wrapped ColourValue.
    //! \sa Ogre::ColourValue
    //! \sa ObjectWrapper
    class Color: public Ogre::ColourValue, public ObjectWrapper<Color> {
    protected:
      //! Constructor.
      //! \param source Source Ogre::ColourValue to initialize from.
      explicit Color( const Ogre::ColourValue& source );
      //! Destructor.
      ~Color();
      //! My JavaScript-exported constructor function.
      static void create( const FunctionCallbackInfo<v8::Value>& args );

      //! JavaScript-exported get accessor for the "red" component.
      static void jsGetR( Local<v8::String> prop,
        const PropertyCallbackInfo<v8::Value>& info );

      //! JavaScript-exported set accessor for the "red" component.
      static void jsSetR( Local<v8::String> prop, Local<v8::Value> value,
        const PropertyCallbackInfo<void>& info );

      //! JavaScript-exported get accessor for the "green" component.
      static void jsGetG( Local<v8::String> prop,
        const PropertyCallbackInfo<v8::Value>& info );

      //! JavaScript-exported set accessor for the "green" component.
      static void jsSetG( Local<v8::String> prop, Local<v8::Value> value,
        const PropertyCallbackInfo<void>& info );

      //! JavaScript-exported get accessor for the "blue" component.
      static void jsGetB( Local<v8::String> prop,
        const PropertyCallbackInfo<v8::Value>& info );

      //! JavaScript-exported set accessor for the "blue" component.
      static void jsSetB( Local<v8::String> prop, Local<v8::Value> value,
        const PropertyCallbackInfo<void>& info );

      //! JavaScript-exported get accessor for the "alpha" component.
      static void jsGetA( Local<v8::String> prop,
        const PropertyCallbackInfo<v8::Value>& info );

      //! JavaScript-exported set accessor for the "alpha" component.
      static void jsSetA( Local<v8::String> prop, Local<v8::Value> value,
        const PropertyCallbackInfo<void>& info );

      //! Magic JavaScript toString method.
      //! Returns a compact string representation of the object.
      static void jsToString( const FunctionCallbackInfo<v8::Value>& args );

      //! JavaScript "add" call.
      static void jsAdd( const FunctionCallbackInfo<v8::Value>& args );

      //! JavaScript "subtract" call.
      static void jsSubtract( const FunctionCallbackInfo<v8::Value>& args );

      //! JavaScript "multiply" call.
      static void jsMultiply( const FunctionCallbackInfo<v8::Value>& args );

      //! JavaScript "saturate" call.
      static void jsSaturate( const FunctionCallbackInfo<v8::Value>& args );

      //! JavaScript "saturateCopy" call.
      static void jsSaturateCopy( const FunctionCallbackInfo<v8::Value>& args );
    public:

      //! Initializes my JavaScript object template in given object namespace.
      static void initialize( Handle<ObjectTemplate> exports );

      //! Creates a new JavaScript-compatible Color from given source.
      //! \param  color The Ogre::ColourValue to copy from.
      static Local<v8::Object> newFrom( const Ogre::ColourValue& color );
    };

    //! \class Vector3
    //! A JavaScript-wrapped Vector3.
    //! \sa Ogre::Vector3
    //! \sa ObjectWrapper
    class Vector3: public Ogre::Vector3, public ObjectWrapper<Vector3> {
    protected:
      //! Constructor.
      //! \param source Source Ogre::Vector3 to initialize from.
      explicit Vector3( const Ogre::Vector3& source );
      //! Destructor.
      ~Vector3();
      //! My JavaScript-exported constructor function.
      static void create( const FunctionCallbackInfo<v8::Value>& args );

      //! JavaScript-exported get accessor for the "x" component.
      static void jsGetX( Local<v8::String> prop,
        const PropertyCallbackInfo<v8::Value>& info );

      //! JavaScript-exported set accessor for the "x" component.
      static void jsSetX( Local<v8::String> prop, Local<v8::Value> value,
        const PropertyCallbackInfo<void>& info );

      //! JavaScript-exported get accessor for the "y" component.
      static void jsGetY( Local<v8::String> prop,
        const PropertyCallbackInfo<v8::Value>& info );

      //! JavaScript-exported set accessor for the "y" component.
      static void jsSetY( Local<v8::String> prop, Local<v8::Value> value,
        const PropertyCallbackInfo<void>& info );

      //! JavaScript-exported get accessor for the "z" component.
      static void jsGetZ( Local<v8::String> prop,
        const PropertyCallbackInfo<v8::Value>& info );

      //! JavaScript-exported set accessor for the "z" component.
      static void jsSetZ( Local<v8::String> prop, Local<v8::Value> value,
        const PropertyCallbackInfo<void>& info );

      //! Magic JavaScript toString method.
      //! Returns a compact string representation of the object.
      static void jsToString( const FunctionCallbackInfo<v8::Value>& args );

      //! JavaScript equals.
      static void jsEquals( const FunctionCallbackInfo<v8::Value>& args );

      //! JavaScript "add" call.
      static void jsAdd( const FunctionCallbackInfo<v8::Value>& args );

      //! JavaScript "subtract" call.
      static void jsSubtract( const FunctionCallbackInfo<v8::Value>& args );

      //! JavaScript "multiply" call.
      static void jsMultiply( const FunctionCallbackInfo<v8::Value>& args );

      //! JavaScript "length" call.
      //! \copydoc Ogre::Vector3::length()
      static void jsLength( const FunctionCallbackInfo<v8::Value>& args );

      //! JavaScript "squaredLength" call.
      //! \copydoc Ogre::Vector3::squaredLength()
      static void jsSquaredLength( const FunctionCallbackInfo<v8::Value>& args );

      //! JavaScript "distance" call.
      //! \copydoc Ogre::Vector3::distance()
      static void jsDistance( const FunctionCallbackInfo<v8::Value>& args );

      //! JavaScript "squaredDistance" call.
      //! \copydoc Ogre::Vector3::squaredDistance()
      static void jsSquaredDistance( const FunctionCallbackInfo<v8::Value>& args );

      //! JavaScript "dotProduct" call.
      //! \copydoc Ogre::Vector3::dotProduct()
      static void jsDotProduct( const FunctionCallbackInfo<v8::Value>& args );

      //! JavaScript "absDotProduct" call.
      //! \copydoc Ogre::Vector3::absDotProduct()
      static void jsAbsDotProduct( const FunctionCallbackInfo<v8::Value>& args );

      //! JavaScript "normalise" call.
      //! \copydoc Ogre::Vector3::normalise()
      //! \param  args The arguments.
      static void jsNormalise( const FunctionCallbackInfo<v8::Value>& args );

      //! JavaScript "crossProduct" call.
      //! \copydoc Ogre::Vector3::crossProduct()
      static void jsCrossProduct( const FunctionCallbackInfo<v8::Value>& args );

      //! JavaScript "midPoint" call.
      //! \copydoc Ogre::Vector3::midPoint()
      static void jsMidPoint( const FunctionCallbackInfo<v8::Value>& args );

      //! JavaScript "makeFloor" call.
      //! \copydoc Ogre::Vector3::makeFloor()
      static void jsMakeFloor( const FunctionCallbackInfo<v8::Value>& args );

      //! JavaScript "makeCeil" call.
      //! \copydoc Ogre::Vector3::makeCeil()
      static void jsMakeCeil( const FunctionCallbackInfo<v8::Value>& args );

      //! JavaScript "perpendicular" call.
      //! \copydoc Ogre::Vector3::perpendicular()
      static void jsPerpendicular( const FunctionCallbackInfo<v8::Value>& args );

      //! JavaScript "randomDeviant" call.
      //! \copydoc Ogre::Vector3::randomDeviant()
      static void jsRandomDeviant( const FunctionCallbackInfo<v8::Value>& args );

      //! JavaScript "angleBetween" call.
      //! \copydoc Ogre::Vector3::angleBetween()
      static void jsAngleBetween( const FunctionCallbackInfo<v8::Value>& args );

      //! JavaScript "getRotationTo" call.
      //! \copydoc Ogre::Vector3::getRotationTo()
      static void jsGetRotationTo( const FunctionCallbackInfo<v8::Value>& args );

      //! JavaScript "isZeroLength" call.
      //! \copydoc Ogre::Vector3::isZeroLength()
      static void jsIsZeroLength( const FunctionCallbackInfo<v8::Value>& args );

      //! JavaScript "normalisedCopy" call.
      //! \copydoc Ogre::Vector3::normalisedCopy()
      static void jsNormalisedCopy( const FunctionCallbackInfo<v8::Value>& args );
    public:

      //! Initializes my JavaScript object template in given object namespace.
      static void initialize( Handle<ObjectTemplate> exports );

      //! Creates a new JavaScript-compatible Vector3 from given source.
      //! \param  vector The Ogre::Vector3 to copy from.
      static Local<v8::Object> newFrom( const Ogre::Vector3& vector );
    };

    //! \class Quaternion
    //! A JavaScript-wrapped Quaternion.
    //! \sa Ogre::Quaternion
    //! \sa ObjectWrapper
    class Quaternion: public Ogre::Quaternion, public ObjectWrapper<Quaternion> {
    protected:
      //! Constructor.
      //! \param source Source Ogre::Quaternion to initialize from.
      explicit Quaternion( const Ogre::Quaternion& source );

      //! Destructor.
      ~Quaternion();

      //! My JavaScript-exported constructor function.
      static void create( const FunctionCallbackInfo<v8::Value>& args );

      //! JavaScript-exported get accessor for the "w" component.
      static void jsGetW( Local<v8::String> prop,
        const PropertyCallbackInfo<v8::Value>& info );

      //! JavaScript-exported set accessor for the "w" component.
      static void jsSetW( Local<v8::String> prop, Local<v8::Value> value,
        const PropertyCallbackInfo<void>& info );

      //! JavaScript-exported get accessor for the "x" component.
      static void jsGetX( Local<v8::String> prop,
        const PropertyCallbackInfo<v8::Value>& info );

      //! JavaScript-exported set accessor for the "x" component.
      static void jsSetX( Local<v8::String> prop, Local<v8::Value> value,
        const PropertyCallbackInfo<void>& info );

      //! JavaScript-exported get accessor for the "y" component.
      static void jsGetY( Local<v8::String> prop,
        const PropertyCallbackInfo<v8::Value>& info );

      //! JavaScript-exported set accessor for the "y" component.
      static void jsSetY( Local<v8::String> prop, Local<v8::Value> value,
        const PropertyCallbackInfo<void>& info );

      //! JavaScript-exported get accessor for the "z" component.
      static void jsGetZ( Local<v8::String> prop,
        const PropertyCallbackInfo<v8::Value>& info );

      //! JavaScript-exported set accessor for the "z" component.
      static void jsSetZ( Local<v8::String> prop, Local<v8::Value> value,
        const PropertyCallbackInfo<void>& info );

      //! Magic JavaScript toString method.
      //! Returns a compact string representation of the object.
      static void jsToString( const FunctionCallbackInfo<v8::Value>& args );

      //! JavaScript "multiply" call.
      static void jsMultiply( const FunctionCallbackInfo<v8::Value>& args );

      //! JavaScript "dot" call.
      //! \copydoc Ogre::Quaternion::Dot()
      static void jsDot( const FunctionCallbackInfo<v8::Value>& args );

      //! JavaScript "norm" call.
      //! \copydoc Ogre::Quaternion::Norm()
      static void jsNorm( const FunctionCallbackInfo<v8::Value>& args );

      //! JavaScript "normalise" call.
      //! \copydoc Ogre::Quaternion::normalise()
      static void jsNormalise( const FunctionCallbackInfo<v8::Value>& args );

      //! JavaScript "inverse" call.
      //! \copydoc Ogre::Quaternion::Inverse()
      static void jsInverse( const FunctionCallbackInfo<v8::Value>& args );

      //! JavaScript "unitInverse" call.
      //! \copydoc Ogre::Quaternion::UnitInverse()
      static void jsUnitInverse( const FunctionCallbackInfo<v8::Value>& args );

      //! JavaScript "getRoll" call.
      //! \copydoc Ogre::Quaternion::getRoll()
      static void jsGetRoll( const FunctionCallbackInfo<v8::Value>& args );

      //! JavaScript "getPitch" call.
      //! \copydoc Ogre::Quaternion::getPitch()
      static void jsGetPitch( const FunctionCallbackInfo<v8::Value>& args );

      //! JavaScript "getYaw" call.
      //! \copydoc Ogre::Quaternion::getYaw()
      static void jsGetYaw( const FunctionCallbackInfo<v8::Value>& args );
    public:

      //! Initializes my JavaScript object template in given object namespace.
      static void initialize( Handle<ObjectTemplate> exports );

      //! Creates a new JavaScript-compatible Quaternion from given source.
      //! \param qtn Source Ogre::Quaternion to copy from.
      static Local<v8::Object> newFrom( const Ogre::Quaternion& qtn );
    };

  }

  //! @}

  //! @}

}