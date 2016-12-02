# CS3113Assignments: Intro to Game Programming (CS-UY 3113) - Professor Ivan Safrin

Homework1 = Simple excercise of C++ animation using shapes and pictures

Homework2 = Pong:
- Player wins are outputted to the console window
- Uses Keyboard input of "W" and "S" for up and down respectively on
the left; Up arrow and Down arrow for up and down respectively on the
right.
- Spacebar starts the game

Homework3 = Space Invaders: 
- Game is launched to a main menu screen
-- Spacebar starts the game
--- Ship is controlled with Left and Right arrow inputs, Spacebar is attack
--- When you get hit, the Lose Screen launches
--- When you defeat all the ships, the Win Screen launches
-- ESC quits the game from main menu, win screen, and lose screen

Additional Comments:
Textures for ship and enemies created from sheet.png's sprite sheet.
A.I. difficulty starts to get harder when there are less on-screen,
similar to that of the original space invaders, but because the enemy
A.I. randomly selects one enemy to fire based on how many are still
remaining. Hitboxes are a little off for collision, but it's important
to sort of bob-and-weave when attacking the enemies, as not to get hit
(especially when there are few left as they start to attack faster!).

Homework 4 = Added sound to Homework 2 (Pong):

I added music and sounds to my Homework 2 Pong Project.
I used sound files from Super Smash Bros. 64 and Melee.
Colliding with the walls will trigger Shell Hit 2.wav
Colliding with the paddles will trigger Bumper Hit.wav
A win/goal will trigger Crowd Cheering.wav
Background music is Dream Land.mp3 but it sometimes plays at faster speeds?
- Spacebar starts the game
- Controls are: W and S for the left and Up-Arrow and Down-Arrow for the right

Homework 5 = Side-Scrolling 2D Platformer:

I created a side-scrolling platformer for Homework 5 using music and
sound.
I load the level from a tile map as well.

- Controls are Left, Right, and Space.
- Collect the Coin and Key to unlock the Door and Win!

Final Project = 2D Top-Down Perspective Shooter:

Adapted code from my recent Platformer homework assignment.
I'm working on a 2D top-down perspective shooting game (think Hotline Miami).

CONTROLS:
You move with WASD and Aim with J and L. You fire bullets with K.
ESC will quit the window.
Space starts each level.
R will retry when you die.

REQUIREMENTS:
-Have title screen and game states for STATE_MAIN_MENU,
STATE_GAME_LEVEL, STATE_WINNER, STATE_LOSER , STATE_NEXT_LEVEL
-Have a way to quit the game.
-Have music and sound effects.
-Have 3 levels from tile maps.
-Have AI.
-Have animation to cause the player to shrink in size when hit, then
you lose when he disappears.
