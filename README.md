# Zen

**A Lightweight, Modular Framework for Building Console Applications in C**

<!--
If the poser bullshit below serves as your criterion for evaluating the quality of the project, know that I, even as an atheist, despite you with every fiber of my godless soul.
-->
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Zen CI](https://github.com/gooderfreed/Zen/actions/workflows/ci.yml/badge.svg)](https://github.com/gooderfreed/Zen/actions/workflows/ci.yml)
[![CodeFactor](https://www.codefactor.io/repository/github/gooderfreed/zen/badge)](https://www.codefactor.io/repository/github/gooderfreed/zen)
<!--
Look, badges! Thats means it`s a *real* project now, right? Right?... gky ffs
-->

**⚠️ Warning: Active Development ⚠️**
Zen is currently under active development. APIs, internal structures, and core concepts may change significantly and without extensive prior notice as the framework evolves. Use in production environments is not recommended at this stage. Feedback and contributions are welcome!

**TL;DR**
*   **What is it?** A lightweight, modular C framework for building console apps (especially interactive/TUI/games).
*   **Key Features:** Component-oriented, interface-based, declarative macros (`INTERFACES`, `MAP_LAYER`), **Arena-based Allocator**, advanced color rendering, Observer/Emitter signals.
*   **Why use it?** Build efficient console apps/games in C with a structured, flexible architecture.
*   **Status:** Actively developed (API may change).
*   **Try it:** `git clone https://github.com/gooderfreed/Zen.git && cd Zen && make` (builds library & all examples).

## Overview

Zen is a streamlined C framework designed for developing robust and efficient console-based applications. It employs a flexible, component-oriented architecture centered around interfaces and utilizes **C macros to provide a declarative API** for configuring application structure and object capabilities.

Zen offers a solid foundation for projects ranging from simple utilities to complex interactive console applications, including text-based games. It emphasizes performance through careful memory management and aims for **good color fidelity across different terminal types**.

Key architectural concepts include:

*   **Component-Oriented Design**: Encourages code reusability and maintainability through composition.
*   **Interface-Based System**: Defines object capabilities and interactions via structures containing function pointers, promoting flexibility and decoupling.
*   **Declarative Macro API**: Provides C macros (like `INTERFACES`, `MAP_LAYER`, `OBSERVER`) that enable a declarative style for defining object interfaces and application structure.
*   **Arena Memory Management**: Leverages an arena allocator for efficient memory handling, suitable for both static and dynamic allocation scenarios.
*   **Console Screen Rendering**: Offers functionalities for text-based output with color (including RGB with automatic fallback) and formatting.
*   **Event System**: Includes a signal/slot (Observer/Emitter) mechanism for inter-object communication.
*   **Input Handling**: Manages keyboard input for interactive applications.

Zen aims to be lightweight, producing compact executables while offering these features.

## Core Concepts

Zen's architecture revolves around a few key concepts that enable its modularity and declarative style.

### 1. Code Structure (`zen/`)

The core library headers are organized into three main directories:

*   `primitives/`: Contains forward declarations (`typedef struct ... TypeName;`) of all major framework structs. This is used to resolve circular dependencies between headers.
*   `components/`: Defines concrete data structures and core functionalities like `Screen` (rendering), `Map` (layered map), `Cursor`, `Arena` (memory), `Container`, `Color`, time managers (`TimeManager`, `TickCounter`, `FrameTimer`), etc.
*   `interfaces/`: Defines the interfaces (contracts) that objects can implement to interact with the framework and each other. Examples include `Drawable`, `Updateable`, `InputHandler`, `Observer`, `Emitter`, `CursorInteractable`, `ButtonHandler`, `CardHandler`.

### 2. Declarative Initialization (`INTERFACES` Macro)

The primary way to create framework-aware objects and attach interfaces is using the `INTERFACES` macro:

```c
#include "my_object.h" // Header defining MyObject and its interface functions

MyObject *create_my_object(Arena *arena) {
    // Allocate object memory from the arena
    MyObject *obj = arena_alloc(arena, sizeof(MyObject));
    obj->some_data = 42;

    // Initialize framework interfaces using the INTERFACES macro
    INTERFACES(arena, obj, {
        // Inside this block, use interface-specific initializer macros
        DRAWABLE(my_object_draw_func);   // Attach Drawable interface
        UPDATEABLE(my_object_update_func); // Attach Updateable interface
        INPUT_HANDLER(my_object_input_func); // Attach InputHandler
        OBSERVER({                       // Attach Observer
            NEW_OBSERVER("some_signal", my_object_on_signal); // Subscribe
        });
        EMITTER({                        // Attach Emitter
            NEW_EMITTER("my_own_signal");    // Declare signal
        });
        // ... other interfaces ...
    });
```

The `INTERFACES` macro handles:
- Setting the object's name
- Managing interface struct allocation via the arena
- Setting capability flags
- Storing function pointers

### 3. Map Layers (`MAP_LAYER`)

Define map layers, their properties (like loop callbacks), and the objects they contain:

```c
// Define a game layer and place objects on it
MapLayer *game_layer = NULL;
MAP_LAYER(arena, game_layer, {
    // Layer properties/callbacks
    prepare_screen = prepare_game_screen;
    loop = game_loop;
}, {
    // Objects placed on the layer using OBJECT(object_ptr, coords, [params])
    OBJECT(player_object, COORDS(10, 5), {is_main = true;});
    OBJECT(enemy_object, COORDS(20, 8));
});
```

### 4. Arena Memory Management

Zen relies heavily on its [Arena-based Allocator](https://github.com/gooderfreed/arena_c) (`components/arena_alloc.h`). **All** dynamic memory within the framework is managed through an Arena instance provided during initialization (`zen_init`).

*   Supports both static (`arena_new_static`) and dynamic (`arena_new_dynamic`) arenas
*   Reduces fragmentation and potentially improves allocation/deallocation speed compared to standard `malloc`/`free`
*   Allows for easy cleanup of entire states using `arena_reset`, **and also supports freeing individual blocks (`arena_free_block`) for memory reuse within the arena.**
*   For dynamic arenas, memory is automatically freed when calling `arena_free` or when the arena is reset

### 5. Signal System (`Observer`/`Emitter`)

Provides a publish-subscribe mechanism for decoupled communication (`interfaces/observer.h`, `interfaces/emitter.h`).

*   Uses string names for signals.
*   Observers subscribe to signals with callbacks using `OBSERVER` / `NEW_OBSERVER`.
*   Emitters declare signals they emit using `EMITTER` / `NEW_EMITTER`.
*   A central registry (managed internally) connects observers to emitters efficiently during a linking phase (`link_observers_to_emitters`).
*   Signals are sent using `emit_signal(emitter_object, "signal_name", data_payload)`.

### 6. Dependency Injection (`*Dependent` Interfaces)

Interfaces like `CoreDependent` and `TickDependent` allow objects to declare dependencies on core framework components (`Zen`, `TickCounter`).

*   The object uses the `CORE_DEPENDENT()` or `TICK_DEPENDENT()` macro during initialization.
*   The framework (during setup) injects the necessary pointers (e.g., `Zen *zen`) into the object's `ObjectInterfaces`.
*   The object can then use helper macros (`GET_CORE()`, `CORE_ACTION()`, `GET_CURRENT_TICK()`) to access core functionality safely.

### 7. Game Loop (Fixed Timestep / Variable Rendering)

Zen implements a standard game loop (`time_manager.h`, `tick_counter.h`, `frame_timer.h`, `updatable.h`) suitable for interactive applications.

*   Logic updates happen at a fixed rate (`ticks_per_second`) via the `Updateable` interface.
*   Rendering happens as fast as possible up to a target FPS (`target_fps`) via the `Drawable` interface.
*   This ensures game logic remains consistent regardless of rendering speed.

## Features

Zen provides a range of components and interfaces (detailed in **Core Concepts**) to facilitate application development:

*   **Core (`Zen` struct, `zen.h`)**: Central orchestrator managing the main loop, components, and event dispatch.
*   **Memory Management**: Integrated **Arena-based Allocator** (`arena_alloc.h`) for efficient static or dynamic memory management.
*   **Screen Rendering (`Screen`, `screen.h`)**: Double-buffered terminal rendering with support for Unicode, `TextEffect`s (bold, etc.), and **RGB color** (using `Color` struct from `color.h`). Includes drawing primitives.
*   **Color Handling (`Color`, `color.h`)**: Simple `uint32_t` based RGB color representation with many predefined `COLOR_*` constants (CSS/X11 names). Color output is **automatically adapted** to terminal capabilities (TrueColor, 256-color, 16-color) detected via internal methods or optionally using the `tput` command if available.
*   **Layered Map (`Map`, `MapLayer`, `map.h`)**: A component for organizing objects (`void*`) in a 2D grid with multiple layers. Supports declarative definition via `MAP_LAYER` / `OBJECT` macros and layer-specific callbacks.
*   **Cursor (`Cursor`, `cursor.h`)**: Manages cursor position, appearance (`CursorConfig`), and interaction state (including a `Container` for selected items).
*   **Generic Container (`Container`, `container.h`)**: A simple dynamic array for `void*`.
*   **Input Handling**: Raw keyboard input reading (`getwchar`), terminal mode setting (`set_noncanonical_mode`), and dispatching via the `InputHandler` interface or `MapLayer` callbacks.
*   **Event System (`Observer`/`Emitter`, `observer.h`/`emitter.h`)**: Signal/slot mechanism for event-based communication using string identifiers.
*   **Game Loop**: Fixed timestep logic updates (`TickCounter`, `Updateable` interface) decoupled from variable FPS rendering (`FrameTimer`, `Drawable` interface).
*   **FPS Counter (`FpsStats`, `fps_stats.h`)**: Optional component to track and display FPS metrics.
*   **Interaction Interfaces**: A suite of interfaces for building interactive elements:
    *   `CursorInteractable`: Defines how an object interacts with cursor movement and placement.
    *   `ButtonHandler`: Handles activation (e.g., Enter/Space press) on objects, often used with `CursorInteractable` for UI buttons. Supports declarative definition via `BUTTONS_HANDLER` / `BUTTON` macros.
    *   `CardHandler`: Specific interface for selecting/moving card-like objects (used in Solitaire).
*   **Dependency Injection**: `CoreDependent` and `TickDependent` interfaces provide controlled access to core systems.

## Roadmap

Here are some of the planned features and improvements for Zen (subject to change):

*   **Refinement & Consistency**: General improvements to existing components and ensuring a consistent API/style across the framework.
*   **Screen Module Extraction**: Enhancing the screen rendering functionality and extracting it into a standalone library.
*   **Input System Enhancement**: Improving the input system, adding an abstraction layer for input locales, and extracting it into a standalone library.
*   **Advanced Signal System**: Implementing a more flexible signal system (e.g., "wide, narrow, deep" signals) for unified inter-object communication.
*   **Request/Response System:** Extend the signal system to support request-response patterns (getting a return value from a signal/call).
*   **Dynamic Context System**: Introducing a system for managing dynamic context at different levels (application, layer).
*   **File I/O Utilities**: Adding helpers or components for file operations.
*   **Resource Bundling & Localization**: Support for packaging resources and internationalization.
*   **Logging System**: Implementing a robust logging system (e.g., logging to a file) for diagnostics and analysis.
*   **Serialization/Deserialization**: Adding mechanisms for saving and loading application state or data.
*   **Theming/Color Schemes**: Developing a system for easily defining and applying color themes.
*   **Socket/Network Communication**: Basic support for network operations.

## Modular Components

Zen is designed as a collection of modular components that can be used independently:

### Standalone Libraries

* **[Arena-based Allocator](https://github.com/gooderfreed/arena_c)**: A header-only arena-based memory allocation library extracted from Zen. Provides efficient memory management with both static and dynamic allocation options.

* **Screen Renderer** *(coming soon)*: A standalone library for terminal rendering with color support and automatic terminal capability detection.

This modular approach allows you to:
* Use only the components you need
* Integrate individual parts into existing projects
* Benefit from Zen's optimized implementations without adopting the entire framework

## Example Applications

The framework includes the following example applications in the `examples` directory to showcase different types of console applications that can be built with Zen:

*   **Solitaire Game**: A complete Klondike Solitaire card game implementation demonstrating advanced framework features like complex game logic, user interface elements (menus, win screen), Unicode card rendering, and terminal-based user interaction.

    ![Klondike Solitaire Implementation](.github/solitaire.png)

*   **Snake Game**: A classic Snake game implementation, illustrating game loop management, real-time input handling, and dynamic object updates within the Zen framework.

    ![Snake Implementation](.github/snake.png)

*   **Rotating 3D Donut**: A visually impressive, rotating 3D donut rendered using ASCII art and grayscale shading. This example demonstrates:

    *   **Advanced Rendering Techniques**: Projection of a 3D object (a torus) onto a 2D terminal screen.
    *   **Real-time Animation**: Smooth rotation of the donut using frame-based updates.
    *   **Grayscale Shading**: Dynamic calculation of brightness levels to create a 3D effect using only background colors.
    *   **Efficient Algorithms**: Optimized calculations for inverse distance (z-buffering) and normalized luminance.

    ![Donut Implementation](.github/donut.png)

## Dependencies

Zen is designed to be lightweight and portable, with minimal external dependencies.

**Required dependencies:**
*  Standard C Library (libc)
*  Math Library (libm)
  
**Optional tools:**
* `tput` - Used for enhanced terminal capability detection. If not available, Zen will fall back to a simpler detection method that works in most environments.

No third-party libraries are required, making Zen easy to integrate into any project or environment.

## Performance and Optimization

Zen is designed with performance and resource efficiency in mind. The framework generates highly optimized executables with minimal overhead:

| Example Application | Executable Size (Static Release) |
|---------------------|----------------------------------|
| Solitaire           | 47.6KB                           |
| Snake               | 35.3KB                           |
| 3D Donut            | 35.3KB                           |
| Simple Demo         | 35.2KB                           |

These compact sizes make Zen suitable for resource-constrained environments while still providing rich functionality and developer-friendly abstractions.

## Building and Using Zen

### Clone and Build

```bash
# Clone the repository
git clone https://github.com/gooderfreed/Zen.git
cd Zen
# Build all Zen and all examples
make
```

### Available Make Targets

To see all available targets:

```bash
make list
```

To build a specific example (e.g., Solitaire):

```bash
cd examples/solitaire
make list     # Show available targets for this example
make static   # Build with static library (release mode)
```

### Build Options for Examples

Each example has multiple build targets:
- `make static`        - Build with static library (release)
- `make dynamic`       - Build with dynamic library (release)
- `make static_debug`  - Build with static library (debug)
- `make dynamic_debug` - Build with dynamic library (debug)

### Using Zen in Your Project

To use Zen in your own project:

1. Build the Zen libraries (`make compile`)
2. Copy the `zen/inc` directory to your project's include path
3. Link against either `libzen.a` (static) or `libzen.so` (dynamic)
4. See the examples directory for implementation patterns

## Getting Started

For a step-by-step guide and a basic code example demonstrating the fundamental usage of Zen, please refer to the [**examples/simple_demo**](examples/simple_demo/) directory. This example provides a clear illustration of how to initialize the core, create objects, implement interfaces, and integrate them into the main application loop.

## License

Licensed under the MIT License. See [LICENSE](LICENSE) for details.

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.
