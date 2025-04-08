# Zen

**Lightweight Modular Framework for Building Console Applications in C with Declarative DSL**

## Overview

Zen is a streamlined framework written in C, designed to facilitate the development of robust and efficient console-based applications. It provides a flexible, modular architecture centered around an interface-based component system and a **powerful Domain-Specific Language (DSL)** for declarative application configuration. **Despite its rich functionality, Zen produces remarkably compact executables, even for complex applications.** Zen aims to offer a solid foundation for projects ranging from simple utilities to complex interactive console applications, including text-based games, with **enhanced color fidelity across different terminal types.**

Key framework capabilities include:

*   **Component-Based Architecture**: Encourages code reusability and maintainability through a flexible composition-based design.
*   **Interface-Driven Component System**: Promotes flexibility and extensibility by defining clear contracts between modules.
*   **Declarative DSL**: Provides an elegant, React-like syntax for defining interfaces, components, and application structure directly in C.
*   **Enhanced Console Screen Rendering**: Offers advanced functionalities for text-based output with color and formatting, including **full RGB color support with automatic conversion for compatibility across various terminals.**
*   **Keyboard Input Handling**: Manages user input from the keyboard for interactive applications.
*   **Memory Management System**: Features a flexible arena allocator for efficient memory use in both static and dynamic allocation contexts.

## Features

*   **Self-Contained Library Design**: Zen is designed as a complete, standalone library (`zen`) that can be compiled and linked to any C project. The repository includes practical examples (`examples`) demonstrating Zen's capabilities in action, from simple applications to complex games. This structure makes it simple to either use Zen as an external dependency in your projects or study the examples to understand its features.

*   **Comprehensive Interface System**: The framework leverages interfaces to define object capabilities and interactions, with a **declarative syntax for effortless implementation**:
    ```c
    // Declarative interface definition
    INTERFACES(arena, object, {
        DRAWABLE(print_function);
        UPDATABLE(update_function);
        EMITTER({
            NEW_EMITTER("signal_name");
        });
        OBSERVER({
            NEW_OBSERVER("signal_name", callback_function);
        });
    });
    ```

*   **Duck-Typing Approach**: Objects are defined by their capabilities (interfaces) rather than their types, allowing for flexible and dynamic component composition:
    ```c
    // Check if object supports specific capabilities
    if (IS_DRAWABLE(object)) {
        DRAW(object, screen, cursor);
    }
    
    if (IS_UPDATABLE(object)) {
        UPDATE(object);
    }
    ```

*   **Declarative Layer Creation**: Easily build application layers with an intuitive DSL:
    ```c
    MAP_LAYER(arena, layer, {
        prepare_screen = prepare_function;
        loop = loop_function;
    }, {
        OBJECT(object1, COORDS(0, 0));
        OBJECT(object2, COORDS(0, 1), {is_main = true;});
    });
    ```

*   **Expressive UI Components**: Create interactive UI elements with minimal code and flexible customization:
    ```c
    // Minimal button declaration with automatic positioning
    BUTTONS_HANDLER({
        BUTTONS_GROUP({
            BUTTON(on_start_click);
            BUTTON(on_settings_click);
            BUTTON(on_exit_click);
        });
    });
    
    // Advanced button customization with optional parameters
    BUTTONS_HANDLER({
        BUTTONS_GROUP({
            // Configure group layout direction and start position
            direction = HORIZONTAL;
            base_coords = COORDS(5, 10);
        }, {
            // Basic button with automatic positioning
            BUTTON(on_start_click);
            
            // Customized button with explicit coordinates
            BUTTON({
                coords = COORDS(7, 12);  // Override automatic positioning
                context = some_data;     // Attach custom data to button
            }, on_settings_click);
            
            // Another customized button
            BUTTON({
                name = "custom_button";  // Custom name for referencing
            }, on_exit_click);
        });
        
        // Multiple button groups in one handler
        BUTTONS_GROUP({
            direction = VERTICAL;
            base_coords = COORDS(20, 5);
        }, {
            BUTTON(on_help_click);
            BUTTON(on_about_click);
        });
    });
    
    // Setting button context by name
    SET_BUTTON_CONTEXT(game->ui, "custom_button", player_data);
    ```

*   **Card Game Support**: Specialized interfaces for implementing card games with drag-and-drop mechanics:
    ```c
    // Define a card handler with giving and taking capabilities
    CARD_HANDLER({
        // Enable card giving capabilities
        CAN_GIVE_CARDS(select_cards_fn, is_same_card_fn, get_cards_fn);
        
        // Enable card taking capabilities
        CAN_TAKE_CARDS(can_place_fn, place_cards_fn);
    });
    ```

*   **Event-Based Communication**: Objects communicate through a signal system using a simple, declarative syntax:
    ```c
    // Define an event emitter
    EMITTER({
        NEW_EMITTER("card_moved");
        NEW_EMITTER("game_won");
    });
    
    // Define an event observer
    OBSERVER({
        NEW_OBSERVER("card_moved", on_card_moved);
        NEW_OBSERVER("game_won", on_game_won);
    });
    
    // Emit an event
    EMIT(object, "card_moved", data);
    ```

*   **Advanced Console Screen Management**: Supports ASCII, Unicode, and **full RGB color rendering**. The framework **automatically detects terminal color capabilities** (Truecolor, 256-color, or basic 16 colors) and **dynamically converts RGB colors to the optimal format** supported by the terminal, and includes **all 140+ standard CSS/X11 color names** (like `COLOR_CHOCOLATE` or `COLOR_SLATE_BLUE`) for easier development.

*   **Optional `tput` Support**: Leverages the `tput` utility (if available) to **query the terminal for advanced capabilities**, ensuring accurate color detection and optimal rendering. If `tput` is not found, a fallback detection method is used.

*   **Keyboard Input Management**: Provides utilities for capturing and processing keyboard input events with raw input mode (no Enter key required).

*   **Flexible Memory Management**: Seamlessly switch between static (zero heap allocations) and dynamic memory models with a single line change, making the framework suitable for everything from embedded systems to complex applications.
    ```c
    // Static allocation - perfect for embedded systems or memory-constrained environments
    size_t buffer[1024*10]; // Static buffer of 40KB
    Arena *arena = arena_new_static(buffer, sizeof(buffer));

    // OR

    // Dynamic allocation - for applications with varying memory needs
    Arena *arena = arena_new_dynamic(1024*1024); // Dynamically allocate 1MB
    ```

*   **FPS and Tick Management**: Separate control of rendering framerate and logic update frequency:
    ```c
    // Set target rendering FPS
    zen_set_target_fps(zen, 60);
    
    // Set logic update frequency (ticks per second)
    zen_set_ticks_per_second(zen, 30);
    
    // Enable FPS display for debugging
    zen_enable_fps_stats(zen);
    ```

## Modular Components

Zen is designed as a collection of modular components that can be used independently:

### Standalone Libraries

* **[Arena Allocator](https://github.com/gooderfreed/arena_c)**: A header-only arena memory allocation library extracted from Zen. Provides efficient memory management with both static and dynamic allocation options.

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

Zen is designed to be lightweight and portable, with minimal external dependencies:

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

# Build the core libraries
make compile

# Build all examples
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
- `make static` - Build with static library (release)
- `make dynamic` - Build with dynamic library (release)
- `make static_debug` - Build with static library (debug)
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
