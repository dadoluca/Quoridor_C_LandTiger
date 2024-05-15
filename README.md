# Quoridor

Quoridor is an abstract board game published in 1997 by Gigamic.

## Rules of the game

Each player is equipped with a token and 8 barriers. The game board is a 7x7 wooden square, with the peculiar feature that the lines dividing and forming the squares are grooved, allowing walls to be inserted.

Each player starts from the centre of their perimeter line (the 4th square), and the goal is to get their token to the opposite perimeter line. The player who achieves this objective first wins.

On their turn, a player has two choices:
1. They can choose to move their token horizontally or vertically.
2. They can choose to place a wall. The wall cannot be jumped over but must be navigated around.

At every step of the game, there are always two players: the moving player (the one that is making the choice), and the opponent. Their roles alternate at every step.
- If two tokens (the moving player and the opponent) are facing each other, the moving player can jump over the opponent and position themselves behind them if there is no barrier behind the opponent.
- It is not allowed to "trap" a player with the help of walls; you must always leave them a way to reach the goal.
- A move must be chosen within 20 seconds, or else it loses the turn.

## Implementation on the LandTiger Board or Emulator

This game is implemented in Keil µVision, using the LANDTIGER emulator and the actual board.

### game.h

- Global struct of type "GameInfo" representing the game state: contains information such as the last movement of the players (32-bit move), the current turn, the current movement mode (whether moving a token or placing a wall), and the number of walls placed by each player (tracking when they run out and updating the count graphically).
- Global struct of type Walls representing the list of walls placed in the game (index is updated as walls are placed and allows implementation of wall controls; each element of the array is a 32-bit move).
- Several defines to make code understanding clearer, such as MAX_WALLS_PER_USER, PLAYER_0, PLAYER_1, TOKEN_MODE, WALL_MODE, DIRECTION_RIGHT, DIRECTION_DOWN, DIRECTION_LEFT, DIRECTION_UP (joystick directions).

### IRQ_RIT.c

- `void joystickMove(int direction)`: Function called for every joystick movement, taking direction as a parameter (e.g., DIRECTION_DOWN), and executing the corresponding movements.
- `confirmEndTurn()`: Function executing end of turn operations for a player, called by joyselect and the end of timer0 (20 seconds).

### Overview of other folders:

- `button_EXINT`: Interrupt handling for buttons (INT0, KEY1, KEY2), disabling interrupts, setting flags, and clearing pending.
- Game logic managed in the RIT with debouncing implementation.
- `Timer`: timer0 for the 20-second turn, every ¼ of a second for efficiency. timer1 to display a player's wall end message for a few seconds.
- `movementLogic`: Implements variations of movements represented in 32-bit unsigned int variable as requested.
- `tokens`: Manages tokens and their graphical part.
- `walls`: Manages walls and their graphical part (e.g., drawing/erasing a wall).
- `victoryStrategy`: Exclusively manages player trapping to prevent placing walls that block a player's victory. It relies on a 0, 1, 2 board matrix. For each wall placement attempt, an algorithm works on this matrix starting from the token's position and explores all reachable cells (with wall checks) and checks if the token reaches the solution row. (cell values: 0 unexplored, 1 reached, 2 explored every neighbor).

This implementation is developed in C.
