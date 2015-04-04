#include "StdAfx.h"
#include "AIGoals.h"
#include "Exception.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  namespace AI {

    void Goal::activateIfInactive()
    {
      if ( isInactive() )
        activate();
    }

    void Goal::reactivateIfFailed()
    {
      if ( hasFailed() )
        mStatus = Status_Inactive;
    }

    Goal::Goal( Agent* owner ): mOwner( owner ), mStatus( Status_Inactive )
    {
      //
    }

    Goal::~Goal()
    {
      //
    }

    void Goal::addSubgoal( Goal* goal )
    {
      ENGINE_EXCEPT( "Cannot add a subgoal to an atomic goal" );
    }

  }

}