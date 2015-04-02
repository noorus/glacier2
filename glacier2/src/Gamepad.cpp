#include "StdAfx.h"
#include "InputManager.h"
#include "Engine.h"
#include "Exception.h"
#include "Gamepad.h"
#include "Controllers.h"

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
        mDevice->getController()->beginAction( mDevice, mAction );
      else if ( input == Button_Released )
        mDevice->getController()->endAction( mDevice, mAction );
    }

    Thumbstick::Thumbstick( Device* device, ThumbstickType type, Real multiplier ):
    BaseAxisComponent( device ), mType( type ), mMultiplier( multiplier ), mDirectional( Vector2::ZERO )
    {
      //
    }

    void Thumbstick::onAxisInput( Real value, ThumbstickAxis axis )
    {
      if ( axis == Axis_X )
        mDirectional.x = value;
      else if ( axis == Axis_Y )
        mDirectional.y = value;

      Real length = mDirectional.length();
      if ( length > 1.0f )
        mDirectional /= length;

      if ( mType == Thumbstick_Camera )
        mDevice->getController()->cameraPersistentMovement( mDevice, mDirectional * mMultiplier );
      else if ( mType == Thumbstick_Movement )
        mDevice->getController()->directionalMovement( mDevice, mDirectional );
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

    void Trigger::onAxisInput( Real value, ThumbstickAxis axis )
    {
      //
    }

    // Device
    
    Device::Device( LocalController* local, Nil::Controller* controller ):
    InputDevice( local ), mGamepad( controller ), mFocused( true )
    {
      nButtons = mGamepad->getState().mButtons.size();
      nAxes = mGamepad->getState().mAxes.size();
      nPovs = mGamepad->getState().mPOVs.size();
      nSliders = mGamepad->getState().mSliders.size();

      // 360
      defineButton( 6, Action_Jump );
      defineButton( 4, Action_Crouch );
      defineButton( 8, Action_Run );
      defineThumbstick( 0, 1, Thumbstick_Movement, 1.0f );
      defineThumbstick( 2, 3, Thumbstick_Camera, 4.0f );
      defineDirectional( 0 );
    }

    void Device::prepare()
    {
      //
    }

    void Device::onFocus( const bool focus )
    {
      mFocused = focus;
    }

    Button* Device::defineButton( size_t button, const BindAction& action )
    {
      Button* component = new Button( this, action );
      mButtonMap[button] = component;
      mComponents.push_back( component );
      return component;
    }

    Thumbstick* Device::defineThumbstick( size_t axisX, size_t axisY,
    const ThumbstickType& type, const Real multiplier )
    {
      Thumbstick* component = new Thumbstick( this, type, multiplier );
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
      if ( !mFocused )
        return;

      auto it = mButtonMap.find( button );
      if ( it != mButtonMap.end() )
        (*it).second->onButtonInput( Button_Pressed );
    }

    void Device::onControllerButtonReleased( Nil::Controller* controller,
    const Nil::ControllerState& state, size_t button )
    {
      if ( !mFocused )
        return;

      auto it = mButtonMap.find( button );
      if ( it != mButtonMap.end() )
        (*it).second->onButtonInput( Button_Released );
    }

    void Device::onControllerAxisMoved( Nil::Controller* controller,
    const Nil::ControllerState& state, size_t axis )
    {
      if ( !mFocused )
        return;

      auto it = mAxisMap.find( axis );
      if ( it != mAxisMap.end() )
        (*it).second.component->onAxisInput( state.mAxes[axis].absolute, (*it).second.axis );
    }

    void Device::onControllerSliderMoved( Nil::Controller* controller,
    const Nil::ControllerState& state, size_t slider )
    {
      if ( !mFocused )
        return;
    }

    void Device::onControllerPOVMoved( Nil::Controller* controller,
    const Nil::ControllerState& state, size_t pov )
    {
      if ( !mFocused )
        return;

      auto it = mPovMap.find( pov );
      if ( it != mPovMap.end() )
        (*it).second->onPovInput( state.mPOVs[pov].direction );
    }

    Device::~Device()
    {
      for ( auto component : mComponents )
        delete component;
    }

  }

}