# WildfireSim
Wildfire Simulation  This project implements a two-dimensional computer simulation to model the spread of wildfire in a forest environment. 
this is the first project for Mechanics of Programming(Fall 2024), aiming to ecplore and implementdifferent alogorithms and optimization techniques i experimented with and implemented.

**Features:**

* **Realistic Simulation:** 
    * Models the spread of fire based on neighbor influence, probability of catching fire, and initial burning proportion.
    * Includes optional extra credit features:
        * **Dampness:** Trees can have varying levels of dampness, reducing their susceptibility to fire.
        * **Lightning Strikes:** Random lightning strikes can ignite trees, adding an element of unpredictability.
* **User-Friendly Interface:**
    * Command-line interface with clear usage instructions.
    * Customizable parameters:
        * `-b`: Proportion of trees initially burning.
        * `-c`: Probability of a tree catching fire.
        * `-d`: Density of trees in the grid.
        * `-n`: Proportion of neighbors influencing a tree's catching fire.
        * `-p`: Number of states to print before quitting (print mode).
        * `-s`: Simulation grid size.
        * `-L`: Probability of lightning strikes (extra credit).
        * `-D`: Level of tree dampness (extra credit).
* **Visual Output:**
    * Supports both print mode and an overlay display mode for dynamic visualization.

**How to Run:**

1. **Compile:** Compile the source code using a C compiler:
   ```bash
   gcc -o wildfire wildfire.c display.c -lm 
   ```

2. **Run:** Execute the compiled program with optional arguments:
   ```bash
   ./wildfire 
   ```
   For example:
   * `./wildfire -b10 -c50 -d70 -n30 -s20 -L10 -D20` 
      * Sets initial burning proportion to 10%, catch probability to 50%, density to 70%, neighbor influence to 30%, grid size to 20, lightning probability to 10%, and dampness level to 20%.

**Extra Credit:**

* **Dampness:** To enable dampness, use the `-D` flag followed by a value between 0 and 100 representing the dampness level. Higher values reduce the probability of a damp tree catching fire.
* **Lightning Strikes:** To enable lightning strikes, use the `-L` flag followed by a value between 0 and 100 representing the probability of a lightning strike occurring.


**Disclaimer:**

This simulation is a simplified model of wildfire behavior implemenmted for a course project following it's specifications and does not accurately reflect real-world conditions.


