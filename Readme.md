# Template-Based Finite State Machine Framework

A compile-time, type-safe Finite State Machine (FSM) framework for C++20 that leverages templates, concepts, and macro-based DSL for zero-runtime-overhead state management.

---

## Table of Contents

- [Requirements](#requirements)
- [Overview](#overview)
- [Formal Logic](#formal-logic)
  - [Mathematical Foundation](#mathematical-foundation)
  - [Core Components](#core-components)
  - [Transition Semantics](#transition-semantics)
- [Features](#features)
- [Basic Usage](#basic-usage)
  - [1. Define States](#1-define-states)
  - [2. Define Events](#2-define-events)
  - [3. Define Transitions](#3-define-transitions)
  - [4. Define Actions](#4-define-actions)
  - [5. Create FSM Variant](#5-create-fsm-variant)
  - [6. Run the State Machine](#6-run-the-state-machine)
- [Complete Example](#complete-example)
- [API Reference](#api-reference)
  - [Macros](#macros)
  - [Core Templates](#core-templates)
- [Design Principles](#design-principles)
- [License](#license)

---

## Requirements

- **C++20** or later
- Compiler support for:
  - C++20 Concepts (`std::is_base_of_v`, custom concepts)
  - Template specialization
  - `std::variant`
  - `constexpr` functions
  - Lambda templates with explicit template parameters

### Tested Compilers
- GCC 10+ 
- Clang 13+

---

## Overview

This framework implements a **compile-time finite state machine** where:
- States and events are **types** (zero-size structs)
- Transitions are **template specializations**
- State validation happens at **compile-time**
- Invalid transitions are caught by the compiler
- Runtime overhead is minimized through template metaprogramming

---

## Formal Logic

### Mathematical Foundation

A finite state machine is formally defined as a 5-tuple:

**M = (S, E, δ, s₀, F)**

Where:
- **S**: Finite set of states
- **E**: Finite set of events (input alphabet)
- **δ**: Transition function `δ: S × E → S`
- **s₀**: Initial state (s₀ ∈ S)
- **F**: Set of final/accepting states (F ⊆ S)

### Framework Mapping

This framework implements the FSM model as follows:

| Mathematical Element | C++ Implementation |
|---------------------|-------------------|
| **States (S)** | Types inheriting from `FSM_State` |
| **Events (E)** | Types inheriting from `FSM_Event` |
| **Transition Function (δ)** | Template specialization `FSM_Transition<State, Event>` |
| **Current State** | `FSM<State>` wrapper class |
| **State Change** | `dispatch(event)` returns new `FSM<NextState>` |

### Core Components

#### 1. State Space (S)

States are represented as types:

```cpp
struct FSM_State {};  // Base marker type

struct s_idle : FSM_State {};     // s₁ ∈ S
struct s_running : FSM_State {};   // s₂ ∈ S
```

The concept `derived_FSM_State<T>` ensures type safety:

```cpp
template <typename State>
concept derived_FSM_State = std::is_base_of_v<FSM_State, State>;
```

#### 2. Event Alphabet (E)

Events are represented as types:

```cpp
struct FSM_Event {};  // Base marker type

struct e_start : FSM_Event {};     // e₁ ∈ E
struct e_stop : FSM_Event {};      // e₂ ∈ E
```

**Critical Events** bypass exit actions:

```cpp
struct FSM_CriticalEvent : FSM_Event {};  // For error/reset events

struct ce_reset : FSM_CriticalEvent {};   // Critical event
```

#### 3. Transition Function (δ)

The transition function is implemented via template specialization:

```cpp
template <derived_FSM_State CurrentState, derived_FSM_Event Event>
struct FSM_Transition;  // Primary template (undefined)

// Specialization defines: δ(s_idle, e_start) = s_running
template<>
struct FSM_Transition<s_idle, e_start> {
    using next_state = s_running;
};
```

**Generic Transitions** define transitions from **any state**:

```cpp
// ∀s ∈ S: δ(s, ce_reset) = s_init
template<derived_FSM_State State>
struct FSM_Transition<State, ce_reset> {
    using next_state = s_init;
};
```

### Transition Semantics

#### Deterministic Transitions

For a given state-event pair `(s, e)`, the framework enforces:

1. **At most one transition exists**: Only one template specialization per `<State, Event>` pair
2. **Undefined transitions are handled gracefully**: Returns current state + debug message
3. **Type safety**: Compile-time verification via concepts

#### Transition Execution Model

When `FSM<S>.dispatch(Event)` is called:

```
IF δ(S, Event) is defined:
    IF Event is NOT critical:
        Execute FSM_On_Exit<S>::exec()
    END IF
    
    Let S' = δ(S, Event)  // Next state
    Execute FSM_On_Enter<S'>::exec()
    RETURN FSM<S'>
ELSE:
    Print debug message
    RETURN FSM<S>  // Stay in current state
END IF
```

#### Action Hooks

Three hooks are available per state:

- **FSM_On_Enter<State>**: Executed when entering state
- **FSM_On_Exit<State>**: Executed when leaving state (except for critical events)
- **FSM_On_Tick<State>**: Available for periodic execution (user must call manually)

**Critical Event Behavior**: When a critical event is dispatched, the exit action of the current state is **skipped**, allowing immediate transition to error/reset states.

---

## Features
- **Compile-time type safety**: Invalid transitions caught at compile-time  
- **Zero-runtime overhead**: Templates resolve to direct function calls  
- **Expressive DSL**: Macro-based syntax for readable state machine definitions  
- **Generic transitions**: Define global transitions (e.g., reset from any state)  
- **Critical events**: Emergency transitions that bypass exit actions  
- **Action hooks**: Enter/exit/tick actions per state  
- **Debug support**: Automatic printing of unhandled events  
- **Immutable state**: `dispatch()` returns new state (functional style)  

---

## Basic Usage

### 1. Define States

Use `NEW_STATE(name)` to define states:

```cpp
NEW_STATE(init);      // Creates: struct s_init : FSM_State {}
NEW_STATE(idle);      // Creates: struct s_idle : FSM_State {}
NEW_STATE(running);   // Creates: struct s_running : FSM_State {}
```

### 2. Define Events

Use `NEW_EVENT(name)` for regular events or `NEW_CRITICAL_EVENT(name)` for critical events:

```cpp
NEW_EVENT(start);        // Creates: struct e_start : FSM_Event {}
NEW_EVENT(stop);         // Creates: struct e_stop : FSM_Event {}

NEW_CRITICAL_EVENT(reset);  // Creates: struct ce_reset : FSM_CriticalEvent {}
```

### 3. Define Transitions

Use `NEW_TRANSITION(from, event, to)` for specific transitions:

```cpp
NEW_TRANSITION(s_init, e_start, s_running);
NEW_TRANSITION(s_running, e_stop, s_idle);
```

Use `NEW_GENERIC_TRANSITION(event, to)` for transitions from **any state**:

```cpp
NEW_GENERIC_TRANSITION(ce_reset, s_init);  // Reset from anywhere
```

### 4. Define Actions

Use action macros to define behavior:

```cpp
NEW_ON_ENTER_ACTION(s_idle, {
    std::cout << "System is now idle\n";
});

NEW_ON_EXIT_ACTION(s_running, {
    std::cout << "Shutting down...\n";
});
```

### 5. Create FSM Variant

Define a `std::variant` containing all FSM states:

```cpp
using FSM_States = std::variant<
    FSM<s_init>,
    FSM<s_idle>,
    FSM<s_running>
>;
```

### 6. Run the State Machine

```cpp
FSM_States currentState = FSM<s_init>{};  // Start in init state

// Dispatch events using std::visit
currentState = std::visit([](auto&& state) -> FSM_States {
    return state.dispatch(e_start{});
}, currentState);
```

---

## Complete Example

See the `example/` folder for a full implementation. 

### State Diagram

The example implements a communication system with the following states and transitions:

```
                 ┌─────────────┐
                 │   s_init    │
                 └──────┬──────┘
                        │ e_ready
                        ▼
                 ┌─────────────┐
          ┌─────▶│   s_idle    │◀─────┐
          │      └──────┬──────┘      │
          │             │             │
          │  e_startTx  │  e_startRx  │ e_done
          │             ▼             │
          │      ┌─────────────┐      │
          │      │    s_tx     │──────┘
          │      └─────────────┘
          │             
          │      ┌─────────────┐
          └──────│    s_rx     │
          e_done └─────────────┘
          
                 ┌─────────────┐
                 │   s_error   │◀──── ce_error (from any state)
                 └─────────────┘
                 
     ce_reset (from any state) → s_init
```

### Code Example

Here's a simplified version:

```cpp
#include "FSM.hpp"
#include <variant>

// Define states
NEW_STATE(init);
NEW_STATE(idle);
NEW_STATE(tx);
NEW_STATE(rx);
NEW_STATE(error);

// Define FSM variant
using FSM_States = std::variant<
    FSM<s_init>,
    FSM<s_idle>,
    FSM<s_tx>,
    FSM<s_rx>,
    FSM<s_error>
>;

// Define events
NEW_EVENT(ready);
NEW_EVENT(startTx);
NEW_EVENT(startRx);
NEW_EVENT(done);
NEW_CRITICAL_EVENT(error);
NEW_CRITICAL_EVENT(reset);

// Define transitions
NEW_TRANSITION(s_init, e_ready, s_idle);
NEW_TRANSITION(s_idle, e_startTx, s_tx);
NEW_TRANSITION(s_idle, e_startRx, s_rx);
NEW_TRANSITION(s_tx, e_done, s_idle);
NEW_TRANSITION(s_rx, e_done, s_idle);

NEW_GENERIC_TRANSITION(ce_error, s_error);  // Error from any state
NEW_GENERIC_TRANSITION(ce_reset, s_init);   // Reset from any state

// Define actions
NEW_ON_ENTER_ACTION(s_init, {
    std::cout << "Initializing system...\n";
});

NEW_ON_ENTER_ACTION(s_tx, {
    std::cout << "Transmitting data...\n";
});

NEW_ON_EXIT_ACTION(s_tx, {
    std::cout << "Transmission complete\n";
});

// Usage
int main() {
    FSM_States fsm = FSM<s_init>{};  // Prints: "Initializing system..."
    
    // Dispatch events
    fsm = std::visit([](auto&& state) -> FSM_States {
        return state.dispatch(e_ready{});
    }, fsm);  // init -> idle
    
    fsm = std::visit([](auto&& state) -> FSM_States {
        return state.dispatch(e_startTx{});
    }, fsm);  // idle -> tx (prints "Transmitting data...")
    
    return 0;
}
```

---

## API Reference

### Macros

#### State Definition

```cpp
NEW_STATE(stateName)
```
Creates a state type `s_stateName` inheriting from `FSM_State`.

#### Event Definition

```cpp
NEW_EVENT(eventName)
```
Creates an event type `e_eventName` inheriting from `FSM_Event`.

```cpp
NEW_CRITICAL_EVENT(eventName)
```
Creates a critical event type `ce_eventName` inheriting from `FSM_CriticalEvent`. Critical events skip the current state's exit action.

#### Transition Definition

```cpp
NEW_TRANSITION(currentState, event, nextState)
```
Defines a transition: `currentState` + `event` → `nextState`.

```cpp
NEW_GENERIC_TRANSITION(event, nextState)
```
Defines a generic transition: **any state** + `event` → `nextState`.

#### Action Definition

```cpp
NEW_ON_ENTER_ACTION(state, { /* code */ })
```
Defines code to execute when entering `state`.

```cpp
NEW_ON_EXIT_ACTION(state, { /* code */ })
```
Defines code to execute when exiting `state` (not called for critical events).

```cpp
NEW_ON_TICK_ACTION(state, { /* code */ })
```
Defines code for periodic execution in `state` (must be called manually).

### Core Templates

#### FSM<State>

The runtime wrapper for a state machine state.

```cpp
template<derived_FSM_State State>
class FSM {
public:
    constexpr FSM() noexcept;  // Executes FSM_On_Enter<State>
    
    template<derived_FSM_Event Event>
    [[nodiscard]]
    constexpr auto dispatch(const Event& e) const noexcept;
};
```

**Methods:**
- `dispatch(event)`: Attempts to transition to the next state. Returns new `FSM<NextState>` if transition exists, otherwise returns `*this`.

#### Concepts

```cpp
template <typename State>
concept derived_FSM_State = std::is_base_of_v<FSM_State, State>;
```
Ensures a type is a valid state.

```cpp
template <typename Event>
concept derived_FSM_Event = std::is_base_of_v<FSM_Event, Event>;
```
Ensures a type is a valid event.

```cpp
template <typename Event>
concept derived_FSM_CriticalEvent = std::is_base_of_v<FSM_CriticalEvent, Event>;
```
Ensures a type is a critical event.

```cpp
template<typename State, typename Event>
concept can_handle_event = requires {
    typename FSM_Transition<State, Event>::next_state;
};
```
Checks if a transition exists for the given state-event pair.

---

## Design Principles

1. **Type Safety**: Invalid state machines are rejected at compile-time
2. **Zero Overhead**: Templates and `constexpr` eliminate runtime costs
3. **Immutability**: State machines are immutable; `dispatch()` returns new states
4. **Expressiveness**: Macro DSL provides clear, declarative syntax
5. **Debuggability**: Unhandled events are logged with state/event names
6. **Flexibility**: Generic transitions allow global event handling (e.g., reset, error)

---

## License

This project is provided as-is. Modify and use freely.

---

**Author**: Matteo Ranzi  
**Date**: January 2026


