#pragma once

namespace Glacier {

  class Engine;

  class EngineComponent: public boost::noncopyable {
  protected:
    Engine* mEngine;
  public:
    EngineComponent( Engine* engine );
    virtual void preUpdate( GameTime time );
    virtual void update( GameTime tick, GameTime time );
    virtual void postUpdate( GameTime delta, GameTime time );
    virtual ~EngineComponent();
  };

}