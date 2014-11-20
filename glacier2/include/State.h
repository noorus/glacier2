#pragma once
#include "Types.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  class Game;

  //! \addtogroup Glacier
  //! @{

  class State: boost::noncopyable {
  protected:
    Game* mGame; //!< Game
    GameTime mTime; //!< Local state time
    wstring mName; //!< State name
    bool mPaused; //!< State paused flag
  public:
    explicit State( const wstring& name );
    virtual const wstring& getName();
    virtual const bool isPaused();
    virtual void initialize( Game* game, GameTime time );
    virtual void pause( GameTime time );
    virtual void update( GameTime tick, GameTime time ) = 0;
    virtual void resume( GameTime time );
    virtual void shutdown( GameTime time );
  };

  typedef vector<State*> StateVector;

  //! @}

}