//
    // Created by Matteo Ranzi on 23/01/26.
    //

    /**
     * @file FSM.hpp
     * @brief Finite State Machine (FSM) framework using C++ templates and macros.
     *
     * This file provides a template-based implementation of a finite state machine (FSM)
     * in C++. It includes macros for defining states, events, transitions, and actions,
     * as well as runtime functionality for dispatching events and managing state transitions.
     */

    #ifndef TEMPLATEBASED_FINITESTATEMACHINE_FSM_HPP
    #define TEMPLATEBASED_FINITESTATEMACHINE_FSM_HPP

    #include <iostream>
    #include <type_traits>

    //===========================================================================
    //===================== FSM ELEMENTS DEFINITION =====================

    /**
     * @brief Base class for all FSM states.
     */
    struct FSM_State {};

    /**
     * @brief Base class for all FSM events.
     */
    struct FSM_Event {};

    /**
     * @brief Base class for critical FSM events.
     *
     * Critical events bypass the execution of the current state's exit actions.
     */
    struct FSM_CriticalEvent : FSM_Event {};

    /**
     * @brief Concept to check if a type is derived from FSM_State.
     * @tparam State The type to check.
     */
    template <typename State>
    concept derived_FSM_State = std::is_base_of_v<FSM_State, State>;

    /**
     * @brief Concept to check if a type is derived from FSM_Event.
     * @tparam Event The type to check.
     */
    template <typename Event>
    concept derived_FSM_Event = std::is_base_of_v<FSM_Event, Event>;

    /**
     * @brief Concept to check if a type is derived from FSM_CriticalEvent.
     * @tparam Event The type to check.
     */
    template <typename Event>
    concept derived_FSM_CriticalEvent = std::is_base_of_v<FSM_CriticalEvent, Event>;

    /**
     * @brief Template specialization for defining FSM transitions.
     *
     * This struct is specialized using the NEW_TRANSITION or NEW_GENERIC_TRANSITION macros.
     * @tparam CurrentState The current state type.
     * @tparam Event The event type triggering the transition.
     */
    template <derived_FSM_State CurrentState, derived_FSM_Event Event>
    struct FSM_Transition;

    /**
     * @brief Template specialization for associating names with FSM states and events.
     *
     * This struct is specialized using the NEW_STATE and NEW_EVENT macros.
     * @tparam T The state or event type.
     */
    template<typename T>
    struct FSM_State_Name;

    /**
     * @brief Debug printer for unhandled events.
     *
     * Prints a debug message when an event is not handled in the current state.
     * @tparam State The current state type.
     * @tparam Event The unhandled event type.
     */
    template<derived_FSM_State State, derived_FSM_Event Event>
    struct FSM_Debug_Printer {
        static void print_unhandled_event() {
            std::cout << "[FSM DEBUG]: Unhandled event <"
                      << FSM_State_Name<Event>::name
                      << "> in state <"
                      << FSM_State_Name<State>::name
                      << ">\n";
        }
    };

    //===========================================================================
    //===================== FSM MACROS =====================

    /**
     * @brief Macro to define a new state struct.
     * @param state Name of the state (generates s_state).
     */
    #define NEW_STATE(state) \
        struct s_##state : FSM_State {}; \
        template<> \
        struct FSM_State_Name<s_##state> { \
            static constexpr const char* name = "s_" #state; \
        }

    /**
     * @brief Macro to define a new event struct.
     * @param event Name of the event (generates e_event).
     */
    #define NEW_EVENT(event) \
        struct e_##event : FSM_Event {}; \
        template<> \
        struct FSM_State_Name<e_##event> { \
            static constexpr const char* name = "e_" #event; \
        }

    /**
     * @brief Macro to define a new critical event struct.
     *
     * Critical events bypass the execution of the current state's exit actions.
     * @param event Name of the critical event (generates ce_event).
     */
    #define NEW_CRITICAL_EVENT(event) struct ce_##event : FSM_CriticalEvent {}

    /**
     * @brief Macro to define a new FSM_Transition specialization.
     * @param currentState Current state.
     * @param event Event triggering the transition.
     * @param nextState Next state.
     */
    #define NEW_TRANSITION(currentState, event, nextState) \
    template<> \
    struct FSM_Transition<currentState, event> { \
        static_assert(derived_FSM_State<nextState>); \
        using next_state = nextState; \
    }

    /**
     * @brief Macro to define a new generic FSM_Transition specialization from every state.
     * @param event Event triggering the transition.
     * @param nextState Next state.
     */
    #define NEW_GENERIC_TRANSITION(event, nextState) \
    template<derived_FSM_State State> \
    struct FSM_Transition<State, event> { \
        static_assert(derived_FSM_State<nextState>); \
        using next_state = nextState; \
    }

    //===========================================================================
    //===================== FSM ACTION HOOKS =====================

    /**
     * @brief Template for defining actions to execute when entering a state.
     *
     * This struct is specialized using the NEW_ON_ENTER_ACTION macro.
     * @tparam State The state type.
     */
    template<derived_FSM_State State>
    struct FSM_On_Enter {
        static void exec(){};
    };

    /**
     * @brief Template for defining actions to execute when exiting a state.
     *
     * This struct is specialized using the NEW_ON_EXIT_ACTION macro.
     * @tparam State The state type.
     */
    template<derived_FSM_State State>
    struct FSM_On_Exit {
        static void exec(){};
    };

    /**
     * @brief Template for defining actions to execute on each tick of a state.
     *
     * This struct is specialized using the NEW_ON_TICK_ACTION macro.
     * @tparam State The state type.
     */
    template<derived_FSM_State State>
    struct FSM_On_Tick {
        static void exec(){};
    };

    /**
     * @brief Macro to define a new FSM_On_Enter action for a state.
     * @param state State for which the action is defined.
     * @param code Code to execute on entering the state.
     */
    #define NEW_ON_ENTER_ACTION(state, code) \
    template<> \
    struct FSM_On_Enter<state> { \
        static void exec() { code } \
    }

    /**
     * @brief Macro to define a new FSM_On_Exit action for a state.
     * @param state State for which the action is defined.
     * @param code Code to execute on exiting the state.
     */
    #define NEW_ON_EXIT_ACTION(state, code) \
    template<> \
    struct FSM_On_Exit<state> { \
        static void exec() { code } \
    }

    /**
     * @brief Macro to define a new FSM_On_Tick action for a state.
     * @param state State for which the action is defined.
     * @param code Code to execute on ticking the state.
     */
    #define NEW_ON_TICK_ACTION(state, code) \
    template<> \
    struct FSM_On_Tick<state> { \
        static void exec() { code } \
    }

    //===========================================================================
    //===================== FSM RUNTIME WRAPPER =====================

    /**
     * @brief Concept to check if a state can handle a specific event.
     * @tparam State The state type.
     * @tparam Event The event type.
     */
    template<typename State, typename Event>
    concept can_handle_event = requires {
        typename FSM_Transition<State, Event>::next_state;
    };

    /**
     * @brief FSM runtime wrapper class.
     *
     * This class manages the current state of the FSM and handles event dispatching.
     * @tparam State The initial state of the FSM.
     */
    template<derived_FSM_State State>
    class FSM final {
    public:
        using state = State;

        /**
         * @brief Constructor that executes the on-enter action for the initial state.
         */
        constexpr FSM() noexcept {
            FSM_On_Enter<State>::exec();
        }

        /**
         * @brief Dispatches an event to the FSM.
         *
         * If the event is not handled in the current state, a debug message is printed.
         * If the event is handled, the FSM transitions to the next state.
         * @tparam Event The event type.
         * @param e The event instance.
         * @return A new FSM instance in the next state.
         */
        template<derived_FSM_Event Event>
        [[nodiscard]]
        constexpr auto dispatch(const Event& e) const noexcept {
            if constexpr (!can_handle_event<state, Event>) {
                FSM_Debug_Printer<State, Event>::print_unhandled_event();
                return *this;

            } else {
                if constexpr (!derived_FSM_CriticalEvent<Event>) {
                    FSM_On_Exit<State>::exec();
                }

                using NextState = FSM_Transition<State, Event>::next_state;
                return FSM<NextState>{};
            }
        }
    };

    #endif //TEMPLATEBASED_FINITESTATEMACHINE_FSM_HPP