#pragma once
#include "Types.h"
#include "EngineComponent.h"
#include "InputManager.h"
#include "ActionManager.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  namespace Gamepad {

    class Device;

    enum ThumbstickAxis {
      Axis_X,
      Axis_Y,
      Axis_Trigger,
      Axis_Unknown
    };

    enum ComponentType {
      Component_Button,
      Component_Thumbstick,
      Component_Directional,
      Component_Trigger // One-axis floating button, such as on Xbox 360
    };

    class Component {
    protected:
      Device* mDevice;
      Component( Device* device );
    public:
      virtual const ComponentType getType() = 0;
    };

    enum ButtonInput {
      Button_Pressed,
      Button_Released
    };

    class Button: public Component {
    protected:
      BindAction mAction;
    public:
      Button( Device* device, const BindAction& action );
      virtual const ComponentType getType() { return Component_Button; }
      virtual void onButtonInput( const ButtonInput& input );
    };

    class BaseAxisComponent: public Component {
    protected:
      BaseAxisComponent( Device* device ): Component( device ) {}
    public:
      virtual void onAxisInput( float value, ThumbstickAxis axis ) = 0;
    };

    enum ThumbstickType {
      Thumbstick_Camera,
      Thumbstick_Movement
    };

    class Thumbstick: public BaseAxisComponent {
    protected:
      ThumbstickType mType; //!< Type of control
      float mThreshold; //!< Minimum value to treat as non-zero
      float mMultiplier; //!< Applied multiplier
      Vector2 mDirectional;
    public:
      Thumbstick( Device* device, ThumbstickType type, float multiplier, float threshold = 0.1f );
      virtual const ComponentType getType() { return Component_Thumbstick; }
      virtual void onAxisInput( float value, ThumbstickAxis axis );
    };

    class Trigger: public BaseAxisComponent {
    protected:
      //
    public:
      Trigger( Device* device );
      virtual const ComponentType getType() { return Component_Trigger; }
      virtual void onAxisInput( float value, ThumbstickAxis axis );
    };

    class Directional: public Component {
    protected:
      //
    public:
      Directional( Device* device );
      virtual const ComponentType getType() { return Component_Directional; }
      virtual void onPovInput( const Nil::POVDirection direction );
    };

    struct AxisDefinition {
      BaseAxisComponent* component;
      ThumbstickAxis axis;
      AxisDefinition(): component( nullptr ), axis( Axis_Unknown ) {}
      AxisDefinition( Thumbstick* c, ThumbstickAxis a ): component( c ), axis( a ) {}
      AxisDefinition( Trigger* c ): component( c ), axis( Axis_Trigger ) {}
    };

    class Device: public Nil::ControllerListener {
    friend class InputHandler;
    public:
      typedef std::list<Component*> ComponentList;
      typedef std::map<size_t, Button*> ButtonMap;
      typedef std::map<size_t, AxisDefinition> AxisMap;
      typedef std::map<size_t, Directional*> PovMap;
    protected:
      Nil::Controller* mController;
      ComponentList mComponents;
      ButtonMap mButtonMap;
      AxisMap mAxisMap;
      PovMap mPovMap;
      size_t nButtons;
      size_t nAxes;
      size_t nPovs;
      size_t nSliders;
      size_t nVectors;
    protected:
      Button* defineButton( size_t button, const BindAction& action );
      Thumbstick* defineThumbstick( size_t axisX, size_t axisY, const ThumbstickType& type );
      Directional* defineDirectional( size_t pov );
      Trigger* defineTrigger( size_t axis );
    protected:
      virtual void onControllerButtonPressed( Nil::Controller* controller,
        const Nil::ControllerState& state, size_t button );
      virtual void onControllerButtonReleased( Nil::Controller* controller,
        const Nil::ControllerState& state, size_t button );
      virtual void onControllerAxisMoved( Nil::Controller* controller,
        const Nil::ControllerState& state, size_t axis );
      virtual void onControllerSliderMoved( Nil::Controller* controller,
        const Nil::ControllerState& state, size_t slider );
      virtual void onControllerPOVMoved( Nil::Controller* controller,
        const Nil::ControllerState& state, size_t pov );
    protected:
      Device( Nil::Controller* controller );
      virtual ~Device();
    };

  }

}