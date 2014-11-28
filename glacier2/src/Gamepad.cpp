#include "StdAfx.h"
#include "InputManager.h"
#include "Engine.h"
#include "Exception.h"
#include "ActionManager.h"
#include "ConsoleWindow.h"
#include "Gamepad.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  namespace Gamepad {

    // Components

    Component::Component( Device* device ):
    mDevice( device )
    {
      //
    }

    Button::Button( Device* device, const BindAction& action ):
    Component( device ), mAction( action )
    {
      //
    }

    void Button::onButtonInput( const ButtonInput& input )
    {
      if ( input == Button_Pressed )
        gEngine->getActionManager()->processBindPress( mAction );
      else if ( input == Button_Released )
        gEngine->getActionManager()->processBindRelease( mAction );
    }

    Thumbstick::Thumbstick( Device* device, ThumbstickType type, float multiplier, float threshold ):
    BaseAxisComponent( device ), mType( type ), mMultiplier( multiplier ), mThreshold( threshold ), mDirectional( Vector2::ZERO )
    {
      //
    }

    void Thumbstick::onAxisInput( float value, ThumbstickAxis axis )
    {
      if ( axis == Axis_X )
        mDirectional.x = value;
      else if ( axis == Axis_Y )
        mDirectional.y = value;

      Real length = mDirectional.length();
      if ( length > 1.0f )
        mDirectional /= length;

      if ( mType == Thumbstick_Camera )
      {
        gEngine->getActionManager()->getCameraController()->setPersistentMovement( mMultiplier * mDirectional );
      }
      else if ( mType == Thumbstick_Movement )
      {
        gEngine->getActionManager()->setDirectional( mDirectional );
      }
    }

    Directional::Directional( Device* device ): Component( device )
    {
      //
    }

    void Directional::onPovInput( const Nil::POVDirection direction )
    {
      //
    }

    Trigger::Trigger( Device* device ): BaseAxisComponent( device )
    {
      //
    }

    void Trigger::onAxisInput( float value, ThumbstickAxis axis )
    {
      //
    }

    // Device
    
    Device::Device( Nil::Controller* controller ): mController( controller )
    {
      nButtons = mController->getState().mButtons.size();
      nAxes = mController->getState().mAxes.size();
      nPovs = mController->getState().mPOVs.size();
      nSliders = mController->getState().mSliders.size();

      // 360
      defineButton( 8, Action_Jump );
      defineButton( 6, Action_Crouch );
      defineButton( 7, Action_Run );
      defineThumbstick( 1, 0, Thumbstick_Camera );
      defineThumbstick( 3, 2, Thumbstick_Movement );
      defineDirectional( 0 );
    }

    Button* Device::defineButton( size_t button, const BindAction& action )
    {
      Button* component = new Button( this, action );
      mButtonMap[button] = component;
      mComponents.push_back( component );
      return component;
    }

    Thumbstick* Device::defineThumbstick( size_t axisX, size_t axisY, const ThumbstickType& type )
    {
      Thumbstick* component = new Thumbstick( this, type, 14.0f, 0.25f );
      mAxisMap[axisX] = AxisDefinition( component, Axis_X );
      mAxisMap[axisY] = AxisDefinition( component, Axis_Y );
      mComponents.push_back( component );
      return component;
    }

    Directional* Device::defineDirectional( size_t pov )
    {
      Directional* component = new Directional( this );
      mPovMap[pov] = component;
      mComponents.push_back( component );
      return component;
    }

    Trigger* Device::defineTrigger( size_t axis )
    {
      Trigger* component = new Trigger( this );
      mAxisMap[axis] = AxisDefinition( component );
      mComponents.push_back( component );
      return component;
    }

    void Device::onControllerButtonPressed( Nil::Controller* controller,
    const Nil::ControllerState& state, size_t button )
    {
      auto it = mButtonMap.find( button );
      if ( it != mButtonMap.end() )
        ( *it ).second->onButtonInput( Button_Pressed );
    }

    void Device::onControllerButtonReleased( Nil::Controller* controller,
    const Nil::ControllerState& state, size_t button )
    {
      auto it = mButtonMap.find( button );
      if ( it != mButtonMap.end() )
        ( *it ).second->onButtonInput( Button_Released );
    }

    void Device::onControllerAxisMoved( Nil::Controller* controller,
    const Nil::ControllerState& state, size_t axis )
    {
      auto it = mAxisMap.find( axis );
      if ( it != mAxisMap.end() )
        (*it).second.component->onAxisInput( state.mAxes[axis].absolute, (*it).second.axis );
    }

    void Device::onControllerSliderMoved( Nil::Controller* controller,
    const Nil::ControllerState& state, size_t slider )
    {
      //
    }

    void Device::onControllerPOVMoved( Nil::Controller* controller,
    const Nil::ControllerState& state, size_t pov )
    {
      auto it = mPovMap.find( pov );
      if ( it != mPovMap.end() )
        ( *it ).second->onPovInput( state.mPOVs[pov].direction );
    }

    Device::~Device()
    {
      for ( auto component : mComponents )
        delete component;
    }

  }

}