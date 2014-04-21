#pragma once

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  //! \addtogroup Glacier
  //! @{

  //! \addtogroup Engine
  //! @{

  class Engine;

  class EngineComponent: boost::noncopyable {
  protected:
    Engine* mEngine;
  public:
    EngineComponent( Engine* engine );
    virtual Engine* getEngine();
    virtual void componentPreUpdate( GameTime time );
    virtual void componentTick( GameTime tick, GameTime time );
    virtual void componentPostUpdate( GameTime delta, GameTime time );
    virtual ~EngineComponent();
  };

  //! @}

  //! @}

}