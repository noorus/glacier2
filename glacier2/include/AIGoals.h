#pragma once
#include "Types.h"

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

namespace Glacier {

  namespace AI {

    class Agent;

    class Goal {
    public:
      enum Status {
        Status_Active,
        Status_Inactive,
        Status_Completed,
        Status_Failed
      };
    protected:
      Agent* mOwner;
      Status mStatus;
      void activateIfInactive();
      void reactivateIfFailed();
    public:
      Goal( Agent* owner );
      virtual ~Goal();
      virtual void activate() = 0;
      virtual Status process() = 0;
      virtual void terminate() = 0;
      virtual void addSubgoal( Goal* goal );
      const bool isActive() const throw() { return ( mStatus == Status_Active ); }
      const bool isInactive() const throw() { return ( mStatus == Status_Inactive ); }
      const bool isComplete() const throw() { return ( mStatus == Status_Completed ); }
      const bool hasFailed() const throw() { return ( mStatus == Status_Failed ); }
    };

    typedef std::list<Goal*> GoalList;

    class CompositeGoal: public Goal {
    protected:
      GoalList mSubgoals;
      Status processSubgoals();
    public:
      CompositeGoal( Agent* owner );
      virtual ~CompositeGoal();
      virtual void addSubgoal( Goal* goal );
      virtual void removeAllSubgoals();
    };

    class GoalEvaluator {
    public:
      virtual const Real calculateDesirability( Agent* agent ) = 0;
      virtual void setGoal( Agent* agent ) = 0;
    };

    typedef std::vector<GoalEvaluator*> GoalEvaluatorVector;

    class ThinkGoal: public CompositeGoal {
    protected:
      GoalEvaluatorVector mEvaluators;
    public:
      void arbitrate();
      void activate();
      Status process();
      void terminate();
    };

  }

}