# MiniCore

**Lightweight Modular Framework for Building Console Applications in C with Declarative DSL**

## Overview

MiniCore is a streamlined framework written in C, designed to facilitate the development of robust and efficient console-based applications. It provides a flexible, modular architecture centered around an interface-based component system and a **powerful Domain-Specific Language (DSL)** for declarative application configuration. **Despite its rich functionality, MiniCore produces remarkably compact executables, even for complex applications.** MiniCore aims to offer a solid foundation for projects ranging from simple utilities to complex interactive console applications, including text-based games, now with **enhanced color fidelity across different terminal types.**

Key framework capabilities include:

*   **Modular "Object-Oriented" Architecture**: Encourages code reusability and maintainability through a component-based design.
*   **Interface-Driven Component System**:  Promotes flexibility and extensibility by defining clear contracts between modules.
*   **Declarative DSL**: Provides an elegant, React-like syntax for defining interfaces, components, and application structure directly in C.
*   **Enhanced Console Screen Rendering**:  Offers advanced functionalities for text-based output with color and formatting, including **full RGB color support with automatic conversion for compatibility across various terminals.**
*   **Keyboard Input Handling**:  Manages user input from the keyboard for interactive applications.

## Features

*   **Modular Core Design**: Clear separation between the core framework (`core`) and example implementations (`impl`), promoting a clean and organized project structure.
*   **Comprehensive Interface System**: The framework leverages interfaces to define object capabilities and interactions, with a **declarative syntax for effortless implementation**:
    ```c
    // Declarative interface definition
    INTERFACES(arena, object, {
        DRAWABLE(print_function);
        UPDATEABLE(update_function);
        EMITTER({
            NEW_EMITTER("signal_name");
        });
        OBSERVER({
            NEW_OBSERVER("signal_name", callback_function);
        });
    });
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
*   **Event-Based Communication**: Objects communicate through a signal system using a simple, declarative syntax.
*   **Advanced Console Screen Management**:  Supports ASCII, Unicode, and **full RGB color rendering**. The framework **automatically detects terminal color capabilities** (Truecolor, 256-color, or basic 16 colors) and **dynamically converts RGB colors to the optimal format** supported by the terminal, and includes **all 140+ standard CSS/X11 color names** (like `COLOR_CHOCOLATE` or `COLOR_SLATE_BLUE`) for easier development.
*   **Optional `tput` Support**:  Leverages the `tput` utility (if available) to **query the terminal for advanced capabilities**, ensuring accurate color detection and optimal rendering.  If `tput` is not found, a fallback detection method is used.
*   **Keyboard Input Management**:  Provides utilities for capturing and processing keyboard input events.
*   **Memory Efficiency Focus**:  Emphasizes memory control with arena allocation, minimizing dynamic memory allocations by default for predictable performance.

## Example Implementations

The framework includes the following example implementations within the `impl` directory to showcase different types of console applications that can be built with MiniCore:

*   **Solitaire Game**:  A complete Klondike Solitaire card game implementation demonstrating advanced framework features like complex game logic, user interface elements (menus, win screen), Unicode card rendering, and terminal-based user interaction.

    ![Klondike Solitaire Implementation](.github/solitaire.png)

*   **Snake Game**: A classic Snake game implementation, illustrating game loop management, real-time input handling, and dynamic object updates within the MiniCore framework.

    ![Snake Implementation](.github/snake.png)

*   **Rotating 3D Donut**: A visually impressive, rotating 3D donut rendered using ASCII art and grayscale shading. This example demonstrates:

    *   **Advanced Rendering Techniques:**  Projection of a 3D object (a torus) onto a 2D terminal screen.
    *   **Real-time Animation:** Smooth rotation of the donut using frame-based updates.
    *   **Grayscale Shading:**  Dynamic calculation of brightness levels to create a 3D effect using only background colors.
    *   **Efficient Algorithms:** Optimized calculations for inverse distance (z-buffering) and normalized luminance.

    ![Donut Implementation](.github/donut.png)

## Dependencies

MiniCore is designed to be lightweight and portable, with minimal external dependencies:

**Required dependencies:**
*  Standard C Library (libc)
*  Math Library (libm)
  
**Optional tools:**
* `tput` - Used for enhanced terminal capability detection. If not available, MiniCore will fall back to a simpler detection method that works in most environments.

No third-party libraries are required, making MiniCore easy to integrate into any project or environment.

## Performance and Optimization

MiniCore is designed with performance and resource efficiency in mind. The framework generates highly optimized executables with minimal overhead:

| Example Application | Executable Size |
|---------------------|-----------------|
| Solitaire           | 51.7KB          |
| Snake               | 35.3KB          |
| 3D Donut            | 31.2KB          |
| Simple Demo         | 31.1KB          |

These compact sizes make MiniCore suitable for resource-constrained environments while still providing rich functionality and developer-friendly abstractions.

## Building

**Clone the repository**

```bash
git clone https://github.com/gooderfreed/MiniCore.git
cd MiniCore
```

**Build the project**

To build all implementations:

```bash
make
```

To build a specific implementation (e.g., Solitaire):

```bash
cd impl/solitaire
make
```

## Usage Example

For a step-by-step guide and a basic code example demonstrating the fundamental usage of MiniCore, please refer to the [**impl/simple_demo**](impl/simple_demo/) directory. This example provides a clear illustration of how to initialize the core, create objects, implement interfaces, and integrate them into the main application loop. It serves as an excellent starting point for understanding the framework's core concepts and building your own console applications.


## License

Licensed under the MIT License. See [LICENSE](LICENSE) for details.

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.
