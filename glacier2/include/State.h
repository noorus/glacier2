#pragma once
#include "Types.h"

namespace Glacier {

  class Game;

  class State: boost::noncopyable {
  protected:
    Game* mGame; //!< Game
    GameTime mTime; //!< Local state time
  public:
    virtual void initialize( Game* game, GameTime time );
    virtual void pause( GameTime time ) = 0;
    virtual void update( GameTime tick, GameTime time ) = 0;
    virtual void resume( GameTime time ) = 0;
    virtual void shutdown( GameTime time );
  };

  typedef vector<State*> StateVector;

}