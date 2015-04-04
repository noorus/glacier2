#include "StdAfx.h"
#include "AIGoals.h"
#include "Exception.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  namespace AI {

    void ThinkGoal::arbitrate()
    {
      Real best = 0.0f;
      GoalEvaluator* mostDesirable = nullptr;
      for ( auto evaluator : mEvaluators )
      {
        Real desirability = evaluator->calculateDesirability( mOwner );
        if ( desirability >= best )
        {
          best = desirability;
          mostDesirable = evaluator;
        }
      }
      if ( !mostDesirable )
        ENGINE_EXCEPT( "No evaluator selected" );
      mostDesirable->setGoal( mOwner );
    }

    void ThinkGoal::activate()
    {
      arbitrate();
      mStatus = Status_Active;
    }

    Goal::Status ThinkGoal::process()
    {
      activateIfInactive();
      auto subStatus = processSubgoals();
      if ( subStatus == Status_Completed || subStatus == Status_Failed )
        mStatus = Status_Inactive;
      return mStatus;
    }

    void ThinkGoal::terminate()
    {
      //
    }

  }

}