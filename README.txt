Snekorama
=========

Goal of Snekorama is to implement a game of Snake using different sets
of technologies. The focus here is not on the game itself, but on the
capabilities of the technologies in implementing a simple concept such
as this one. The rules of the game have been reduced to a minimum so
that the code can be focused on building a small and clean program in a
short time. Each implementation of the game should be as similar from
the player's POV to other implementations as much as the used technology
allows.


Primary Concepts
----------------

* Game runs indefinitely until player dies. There is no win condition.
* No main menu, no options. Program should immediately start
  at the game screen.
* Player is controlled from keyboard and joystick if possible.
* Level is on a 16x16 grid.
* There are 4 bricks randomly placed on the field. Player dies
  when crashing into them.
* Player can also crash into their own tail.
* There is one apple randomly placed on the field. Player scores
  upon eating the apple and their tail grows by 1. When apple is
  eaten, another apple is randomly placed somewhere else on the grid.
* One apple is worth 117 points.
* Player starts at the center of the grid.
* Player can only move orthogonally to the grid and only 1 field at a time.
* When moving outside of the grid bounds, player should appear
  at the other side on the same coordinate of the axis on which
  the movement occurs.
* Upon death, the game can be reset with a single keystroke without
  program restart as if it was started from scratch.
* When player has tail, 180 degrees turns are not possible.
* Player should be allowed to turn 180 degrees when they're just a head.
* Sounds should be played when player steps, consumes an apple or dies.


Assets
------

The assets located in the `assets/` directory should be reused
by all implementations unless that is unfeasible for given technology
or a better alternative is already present in the chosen technology.
For example: score and digits textures need not be used if the technology
makes it really easy to print the text (note: SDL2's TTF expansion
library is not easy enough!)

Unless a given technology requires otherwise, assets should be loaded at
runtime from the directory where the executable is located, despite this
rule standing in contraditiction to the Filesystem Hierarchy Standard.


Copyright Info
==============

Whole work except for the parts listed below is original. Unless stated
otherwise for a specific part of the work, UNLICENSE.txt applies to
the whole source-code and other assets.


Copyright mentions:

- assets/score.bmp, assets/digits.bmp - uses DejaVu Sans fonts
  https://dejavu-fonts.github.io/License.html

- assets/smash.wav - CC BY-SA 3.0 (c) pfranzen
  https://freesound.org/people/pfranzen/sounds/377157/

- cmake/Modules/FindSDL2*.cmake - BSD License
  Copyright details mentioned in the files themselves.
