#include "StdAfx.h"
#include "AIGoals.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  namespace AI {

    Goal::Status CompositeGoal::processSubgoals()
    {
      while ( !mSubgoals.empty() && ( mSubgoals.front()->isComplete() || mSubgoals.front()->hasFailed() ) )
      {
        auto goal = mSubgoals.front();
        goal->terminate();
        delete goal;
        mSubgoals.pop_front();
      }
      if ( mSubgoals.empty() )
        return Status_Completed;
      auto status = mSubgoals.front()->process();
      if ( status == Status_Completed && mSubgoals.size() > 1 )
        return Status_Active;
      return status;
    }

    CompositeGoal::CompositeGoal( Agent* owner ): Goal( owner )
    {
      //
    }

    CompositeGoal::~CompositeGoal()
    {
      removeAllSubgoals();
    }

    void CompositeGoal::addSubgoal( Goal* goal )
    {
      mSubgoals.push_front( goal );
    }

    void CompositeGoal::removeAllSubgoals()
    {
      for ( auto goal : mSubgoals )
      {
        goal->terminate();
        delete goal;
      }
      mSubgoals.clear();
    }

  }

}