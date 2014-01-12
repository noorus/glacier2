#pragma once

namespace Glacier {

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

}