#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

// Command-line argument values
int burn_threshold = 50;
int combustion_prob = 70;
int density = 90;
int neighbor_influence = 50;
int grid_size = 10;
int simulation_steps = 20;

// Parse command-line arguments
void parse_args(int argc, char *argv[]) {
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            char option = argv[i][1];
            int value = atoi(&argv[i][2]);  // Convert to integer

            switch (option) {
                case 'b':
                    burn_threshold = value;
                    break;
                case 'c':
                    combustion_prob = value;
                    break;
                case 'd':
                    density = value;
                    break;
                case 'n':
                    neighbor_influence = value;
                    break;
                case 's':
                    grid_size = value;
                    break;
                case 'p':
                    simulation_steps = value;
                    break;
                default:
                    printf("Unknown option: -%c\n", option);
                    exit(1);
            }
        }
    }
}

// Function to simulate spontaneous combustion
bool spontaneous_combustion() {
    return (rand() % 100) < grid_size;
}
// Function to check if a tree catches fire due to neighbors
bool catches_fire_from_neighbors(int burning_neighbors) {
    int fire_prob = (burning_neighbors * neighbor_influence) / 100;
    return (rand() % 100) < fire_prob;
}

// Initialize forest grid based on density
void initialize_forest(int forest[][grid_size], int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            // Initialize trees based on the density percentage
            forest[i][j] = (rand() % 100) < density ? 1 : 0;  // 1 = tree, 0 = empty space
        }
    }
}

// Print the forest grid (for visualization)
void print_forest(int forest[][grid_size], int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            printf(forest[i][j] ? "Y" : " ");
        }
        printf("\n");
    }
}

// Function to simulate one step of the fire spreading
void simulate_step(int forest[][grid_size], int size) {
    int next_forest[grid_size][grid_size];

    // Copy current forest state to the next forest state
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            next_forest[i][j] = forest[i][j];  // Start with same state
        }
    }

    // Process each tree and decide if it will catch fire
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (forest[i][j] == 1) {  // If there is a tree
                int burning_neighbors = 0;

                // Check neighbors (for simplicity, checking 4 adjacent neighbors)
                if (i > 0 && forest[i-1][j] == 2) burning_neighbors++;  // Up
                if (i < size-1 && forest[i+1][j] == 2) burning_neighbors++;  // Down
                if (j > 0 && forest[i][j-1] == 2) burning_neighbors++;  // Left
                if (j < size-1 && forest[i][j+1] == 2) burning_neighbors++;  // Right

                // Decide if the tree catches fire
                if (catches_fire_from_neighbors(burning_neighbors)) {
                    next_forest[i][j] = 2;  // Tree catches fire
                } else if (spontaneous_combustion()) {
                    next_forest[i][j] = 2;  // Spontaneous combustion
                }
            }
        }
    }

    // Copy the next forest state back to the current forest
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            forest[i][j] = next_forest[i][j];
        }
    }
}

int main(int argc, char *argv[]) {
    srand(time(NULL));  // Seed for randomness

    // Parse command-line arguments
    parse_args(argc, argv);

    // Forest grid size (for example, 100x100)
    int size = grid_size;
    int forest[grid_size][grid_size];

    // Initialize the forest based on density
    initialize_forest(forest, size);

    printf("Initial Forest State:\n");
    print_forest(forest, size);

    // Run the simulation for the specified number of steps
    for (int step = 0; step < simulation_steps; step++) {
        printf("\nStep %d:\n", step + 1);
        simulate_step(forest, size);
        print_forest(forest, size);
        printf("\n");
    }

    return 0;
}

