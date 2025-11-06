# PA2: Aquarium

This project is a fun multi-directionaly iteractive evolution game . This repository is part of a Programming Assignment for the Advanced Programming Course. The purpose of this project is to creatively help the student create practical programming skills by implementing changes to the base code that require strong understanding of the concepts discussed in class.

Base code developed by: Max R. Berrios Cruz

PS: Any blame or criticism is not to be put on the author, blame the staff instead.

Thank you and happy coding!

Att. NOT Max R. Berrios Cruz

# Setup
---

    Clone this project inside an OpenFrameworks installation's "apps/myApps/" directory
    Build & Run the project

Submitting Assignment

To submit this assignment, create a Release with the branch that holds all the changes you have added. If no Release is made, the last commit to the master branch will be graded. Don't forget that you must also fill out an evaluation form in Moodle to consider the project submitted.
Grading

## Team Info
| Student Number | Student Name | Student Email |
|-|-|-|

Letizia M. Mayor Pena 802238798 letizia.mayor@upr.edu 
|(802237534)| (Loalis M. Feliciano Medina)| (loalis.feliciano@upr.edu) |
|(STUDENT NUMBER HERE)| (STUDENT NAME HERE)| (STUDENT EMAIL HERE) |

---

Any project that doesn't compile will receive a 0.

You MUST put you & your partner's information in this README file otherwise there will be a 5pts penalty!

If a partner has no commits in the repositories, they will receive a 0.

# Student Notes

Stage 1 – Explore & Stabilize

Implemented the base aquarium system (Aquarium, Creature, NPCreature, PlayerCreature) with movement and collision logic.

Added collision handling (resolveBounce) and border bounce to prevent creatures from leaving the aquarium.

Created the HUD to display score, power, and lives.

Configured level progression and connected the scoring system to the player’s interactions.

Added background music and basic sound effects for gameplay.

Tested collision detection (DetectAquariumCollisions) and life loss mechanics to ensure stability and correct behavior.

Stage 2 – Evolve the Ecosystem

Implemented the Power-Up (1-UP) system with sound and temporary speed boost effects.

Added two new species:

JellyFish: vertical bobbing movement.

PufferFish: horizontal zig-zag movement with border collision.

Updated AquariumSpriteManager and SpawnCreature() to handle new species.

Adjusted Power-Up frequency and duration to make gameplay more dynamic.

Designed new level populations:

Level 0: 10 NPC Fish

Level 1: 8 NPC Fish, 3 JellyFish, 

Level 2: 4 NPC Fish, 3 BiggerFish, 2 PufferFish, 4 JellyFish

Stage 3 – Streamline Repopulation

Refactored repopulation logic into a single inherited implementation in AquariumLevel::Repopulate().

Removed repetitive Repopulate() functions from Level_0, Level_1, and Level_2.

Ensured backward compatibility so all levels repopulate correctly using the new base implementation.

Verified transitions between levels and automatic respawning of creatures.

Bonus Features

Added soun for eating fish.

Added dual player movement (WASD + arrow keys).

Implemented key “M” to mute/unmute global sound.

Repositioned the title screen text to the top-left corner.

Increased Power-Up spawn rate to make them appear faster.

Performed full testing for collisions, power-ups, repopulation, and sound to ensure smooth gameplay.



