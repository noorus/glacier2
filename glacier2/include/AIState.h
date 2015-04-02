#pragma once
#include "AIAgent.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  namespace AI {

    class FiniteStateMachine;

    class State {
    protected:
      GameTime mTime;
    public:
      virtual void enter( FiniteStateMachine* machine, Agent* agent ) { mTime = 0.0f; };
      virtual void pause( FiniteStateMachine* machine, Agent* agent ) { leave( machine, agent ); };
      virtual void resume( FiniteStateMachine* machine, Agent* agent ) { enter( machine, agent ); };
      virtual void execute( FiniteStateMachine* machine, Agent* agent, const GameTime delta ) { mTime += delta; };
      virtual void leave( FiniteStateMachine* machine, Agent* agent ) {};
    };

    typedef vector<State*> StateVector;

  }

}