#include <iostream>
#include <variant>

#include "basic_fsm.hpp"

FSM_States processInputEvent(const FSM_States& fsm, int inputEvent);

[[noreturn]]
int main() {
    FSM_States fsm = FSM<s_init>{};

    while(true) {
        int inputEvent;
        std::cout << "Enter event (0: ready, 1: startTx, 2: startRx, 3: done, 4: error, 5: reset): ";
        std::cin >> inputEvent;

        fsm = processInputEvent(fsm, inputEvent);

    }
}

FSM_States processInputEvent(const FSM_States& fsm, const int inputEvent) {
    return std::visit([&]<typename State>(State&& state) -> FSM_States {

        switch (inputEvent) {
        case 0:
            return state.dispatch(e_ready{});
        case 1:
            return state.dispatch(e_startTx{});
        case 2:
            return state.dispatch(e_startRx{});
        case 3:
            return state.dispatch(e_done{});
        case 4:
            return state.dispatch(ce_error{});
        case 5:
            return state.dispatch(ce_reset{});
        default:
            std::cout << "Invalid event\n";
            return state;
        }
    }, fsm);
}