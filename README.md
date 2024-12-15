# Lights Out - Console Version

This repository contains the implementation of the puzzle game **"Lights Out (special edition)"** as a console application, written in pure C.

## Overview

**Lights Out** is a puzzle game played on a square grid of switches. Each switch can be toggled between "on" and "off" states. The goal of the game is to turn off all the switches. The twist is that toggling a switch also toggles all the switches in the same row and column as the selected switch.

This implementation is designed for educational purposes.

## Features

- Customizable board size.
- Randomized initial board state.
- Computer always builds the optimal solution and shows the number steps in it.
- "Hint" functionality to suggest a move.

### Installation

1. Clone this repository:
   ```sh
   git clone https://github.com/pyataevma/lights_out.git
   ```
2. Navigate to the project directory:
   ```sh
   cd lights-out-console
   ```
3. Compile the source code:
   ```sh
   gcc -o lights_out main.c
   ```

### Running the Game

After compilation, run the game:
```sh
./lights_out
```

Follow the on-screen instructions to play the game.

## Controls

- **Arrow Keys**: Navigate between switches.
- **Spacebar**: Toggle the selected switch.
- **S**: Get a hint for the next move.
- **ESC**: Open the main menu.
- **Q**: Quit the game.

## File Structure

- `main.c`: Contains the main logic and entry point of the application.
- `board.c` / `board.h`: Handles board generation, display, and manipulation.
- `menu.c` / `menu.h`: Implements menu navigation and user interactions.
- `utils.c` / `utils.h`: Utility functions for input handling, encoding, etc.

## Customization

You can customize the game settings by modifying the `Parameters` structure in the `main.c` file. For example:
- Change the default board size.
- Enable or disable step tracking.

## License

This project is licensed under the MIT License. See the `LICENSE` file for details.

## Acknowledgments

- Inspired by the original "Lights Out" game developed by Tiger Electronics.

