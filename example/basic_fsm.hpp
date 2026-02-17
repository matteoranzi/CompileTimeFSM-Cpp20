//
// Created by Matteo Ranzi on 23/01/26.
//

#ifndef TEMPLATEBASED_FINITESTATEMACHINE_FSM_COMMIT_HPP
#define TEMPLATEBASED_FINITESTATEMACHINE_FSM_COMMIT_HPP

#include "FSM.hpp"

//===========================================================================
//========================== FINITE STATE MACHINE  ==========================

//===========================================================================
// STATES
NEW_STATE(init);
NEW_STATE(idle);
NEW_STATE(tx);
NEW_STATE(rx);
NEW_STATE(error);

//FSM_All_States is a variant that can hold any of the FSM states.
//This allows us to have a single variable that can represent the current state of the FSM, and we can use std::visit to handle events based on the current state.
//FILL WITH ALL THE CREATED STATES
using basicFSM = std::variant<
    FSM<s_init>,
    FSM<s_idle>,
    FSM<s_tx>,
    FSM<s_rx>,
    FSM<s_error>
>;

//===========================================================================
// EVENTS
NEW_EVENT(ready);
NEW_EVENT(startTx);
NEW_EVENT(startRx);
NEW_EVENT(done);

NEW_CRITICAL_EVENT(error);
NEW_CRITICAL_EVENT(reset);

//===========================================================================
// TRANSITIONS
NEW_TRANSITION(s_init, e_ready, s_idle);

NEW_TRANSITION(s_idle, e_startTx, s_tx);
NEW_TRANSITION(s_idle, e_startRx, s_rx);

NEW_TRANSITION(s_tx, e_done, s_idle);
NEW_TRANSITION(s_rx, e_done, s_idle);

NEW_GENERIC_TRANSITION(ce_error, s_error);
NEW_GENERIC_TRANSITION(ce_reset, s_init);

//===========================================================================
// ACTIONS


//s_init
NEW_ON_ENTER_ACTION(s_init, {
    std::cout << "Entering init state\n";
});
NEW_ON_EXIT_ACTION(s_init,
    std::cout << "Exiting init state\n";
);

//s_idle
NEW_ON_ENTER_ACTION(s_idle,
    std::cout << "Entering idle state\n";
);
NEW_ON_EXIT_ACTION(s_idle,
    std::cout << "Exiting idle state\n";
);

//s_tx
NEW_ON_ENTER_ACTION(s_tx,
    std::cout << "Entering tx state\n";
);
NEW_ON_EXIT_ACTION(s_tx,
    std::cout << "Exiting tx state\n";
);

//s_rx
NEW_ON_ENTER_ACTION(s_rx,
    std::cout << "Entering rx state\n";
);
NEW_ON_EXIT_ACTION(s_rx,
    std::cout << "Exiting rx state\n";
);

//s_error
NEW_ON_ENTER_ACTION(s_error,
    std::cout << "Entering error state\n";
);
NEW_ON_EXIT_ACTION(s_error,
    std::cout << "Exiting error state\n";
);

//===========================================================================

#endif //TEMPLATEBASED_FINITESTATEMACHINE_FSM_COMMIT_HPP