#pragma once

template< typename ContextType, typename StateIdType >
class GameState;

template< typename ContextType, typename StateIdType >

class GameStateMachine {

  public:
    using Context = ContextType;
    using StateId = StateIdType;
    using State = GameState<Context, StateId>;

  public:
    virtual ~GameStateMachine(void) {}
    virtual Context & getContext(void) = 0;
    virtual const Context & getContext(void) const = 0;
    virtual void changeState(const StateId & stateId) = 0;

};