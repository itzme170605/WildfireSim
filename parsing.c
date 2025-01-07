#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void usage(const char *program_name) {
    fprintf(stderr, "usage: %s [options]\n", program_name);
    fprintf(stderr, "By default, the simulation runs in overlay display mode.\n");
    fprintf(stderr, "The -pN option makes the simulation run in print mode for up to N states.\n\n");
    fprintf(stderr, "Simulation Configuration Options:\n");
    fprintf(stderr, " -H  # View simulation options and quit.\n");
    fprintf(stderr, " -bN # proportion that a tree is already burning. 0 < N < 101.\n");
    fprintf(stderr, " -cN # probability that a tree will catch fire. 0 < N < 101.\n");
    fprintf(stderr, " -dN # density: the proportion of trees in the grid. 0 < N < 101.\n");
    fprintf(stderr, " -nN # proportion of neighbors that influence a tree catching fire. -1 < N < 101.\n");
    fprintf(stderr, " -pN # number of states to print before quitting. -1 < N < 10001.\n");
    fprintf(stderr, " -sN # simulation grid size. 4 < N < 41.\n");
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    int proportion_burning = -1, prob_catch_fire = -1, density = -1;
    int neighbor_influence = -1, print_steps = -1, grid_size = -1;
    int opt;

    // Initialize getopt to recognize options
    while ((opt = getopt(argc, argv, "b:c:d:n:p:s:H")) != -1) {
        switch (opt) {
            case 'b':
                proportion_burning = atoi(optarg);
                if (proportion_burning < 1 || proportion_burning > 100) {
                    fprintf(stderr, "(-bN) proportion already burning must be an integer in [1...100].\n");
                    usage(argv[0]);
                }
                break;
            case 'c':
                prob_catch_fire = atoi(optarg);
                if (prob_catch_fire < 1 || prob_catch_fire > 100) {
                    fprintf(stderr, "(-cN) probability a tree will catch fire must be an integer in [1...100].\n");
                    usage(argv[0]);
                }
                break;
            case 'd':
                density = atoi(optarg);
                if (density < 1 || density > 100) {
                    fprintf(stderr, "(-dN) density of trees in the grid must be an integer in [1...100].\n");
                    usage(argv[0]);
                }
                break;
            case 'n':
                neighbor_influence = atoi(optarg);
                if (neighbor_influence < 0 || neighbor_influence > 100) {
                    fprintf(stderr, "(-nN) %%neighbors influence catching fire must be an integer in [0...100].\n");
                    usage(argv[0]);
                }
                break;
            case 'p':
                print_steps = atoi(optarg);
                if (print_steps < 0 || print_steps > 10000) {
                    fprintf(stderr, "(-pN) number of states to print must be an integer in [0...10000].\n");
                    usage(argv[0]);
                }
                break;
            case 's':
                grid_size = atoi(optarg);
                if (grid_size < 5 || grid_size > 40) {
                    fprintf(stderr, "(-sN) simulation grid size must be an integer in [5...40].\n");
                    usage(argv[0]);
                }
                break;
            case 'H':
                usage(argv[0]); // Print help message and exit
                break;
            default:
                fprintf(stderr, "Unknown flag.\n");
                usage(argv[0]); // Print usage message for unknown flags
                break;
        }
    }

    // Additional logic to check if all required flags are set
    // Or default values could be assumed if no flag is provided
    if (proportion_burning == -1 || prob_catch_fire == -1 || density == -1 || 
        neighbor_influence == -1 || print_steps == -1 || grid_size == -1) {
        fprintf(stderr, "Some required settings are missing.\n");
        usage(argv[0]);
    }

    // Now you can proceed with the rest of your simulation logic
    printf("Proportion burning: %d\n", proportion_burning);
    printf("Probability of catching fire: %d\n", prob_catch_fire);
    printf("Density of trees: %d\n", density);
    printf("Neighbor influence: %d\n", neighbor_influence);
    printf("Print steps: %d\n", print_steps);
    printf("Grid size: %d\n", grid_size);

    return 0;
}

