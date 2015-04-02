#include "StdAfx.h"
#include "AIFiniteStateMachine.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  namespace AI {

    FiniteStateMachine::FiniteStateMachine( Agent* owner ): mOwner( owner )
    {
      //
    }

    void FiniteStateMachine::pushState( State* state )
    {
      if ( !mStates.empty() )
        mStates.back()->pause( mOwner);

      mStates.push_back( state );

      state->enter( mOwner );
    }

    void FiniteStateMachine::popState()
    {
      if ( !mStates.empty() )
      {
        mStates.back()->leave( mOwner );
        mStates.pop_back();
      }

      if ( !mStates.empty() )
        mStates.back()->resume( mOwner );
    }

    void FiniteStateMachine::changeState( State* state )
    {
      if ( !mStates.empty() )
      {
        mStates.back()->leave( mOwner );
        mStates.pop_back();
      }

      mStates.push_back( state );

      state->enter( mOwner );
    }

    void FiniteStateMachine::execute()
    {
      if ( !mStates.empty() )
        mStates.back()->execute( mOwner );
    }

  }

}