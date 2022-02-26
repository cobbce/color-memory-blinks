# Color Memory
Color memory is a game designed for the Blinks platform.

There is no custom setup, the project can be loaded just like any tutorial project in the Blinks developer setup guide.

The project uses multiple files just for organization. This is supported by the Arduino editor loaded as tabs.

The number prefixes in the file names help organize the tabs in the Arduino editor in a logical order.

* ColorMemory.ino is the starting file and declares a few constants and utilities.
* 1_Game - declares game state and shared functionality.
* 2_WaitingToStart - represents idle state in between games. Waits for a button click with 2-6 connected blinks attached to start.
* 3_SetupGame - initialize a new game, assign a color to each tile. Setup leader state to drive game logic.
* 4_PlayGame_Leader - represents the leader tile that generates the sequences and processes player moves
* 4_PlayGame_Tile - represents the game pieces that are assigned turns in the sequence by the leader.
* 5_GameOver - game over state that displays player score
* z_Main - The setup and loop methods that drive the game, plus a state transition function to switch between the various game states described above.

# Playing the Game
Setup:
* Configure between 2-6 blinks surrounding one blink in the center.
* Pieces will idle with a random color while waiting for a game to start.
* Increasing the number of tiles increases difficulty.
* Press the center blink to start the game.
* Reset game by separating the pieces.

Objective:
* Remember and play back an ever increasing sequence of colored tiles.

Play:
* On the first time, the game will light up a single tile
* The player clicks the colored tile to repeat the pattern
* Each turn, the pattern will play again but add a new random tile to the sequence
* Play continues until the player fails to repeat back the entire sequence
* The sequence can grow up to 30 moves (though none of us have made it that far yet!)

Scoring:
After the game ends, the score is displayed in color coded fashion.
All blinks will display the color indicating the player's score
* Red: 1-3 points
* Orange: 4-6 points
* Yellow: 7-9 points
* Green: 10-12 points
* Blue: 13-15 points
* Purple: 16-18 points
* White: 19+ points

The brightness of the tiles indicates what position within the range you scored. For example dim red means one point while bright red means three.

https://www.youtube.com/watch?v=7gd-pVFJtjY
