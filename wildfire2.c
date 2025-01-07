#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <time.h>
#include "display.h"
#include <unistd.h>

#define SEED 41
#define EMPTY ' '
#define TREE 'Y'
#define BURNING_0 '*'
#define BURNING_1 '1'
#define BURNING_2 '2'
#define BURNING_3 '3'
#define BURNED '.'
#define DEFAULT_P_BURNING 10     // Default burning proportion in percentage
#define DEFAULT_P_CATCH 30       // Default catch-fire probability in percentage
#define DEFAULT_DENSITY 50       // Default forest density in percentage
#define DEFAULT_NEIGHBOR_EFFECT 25 // Default neighbor effect in percentage
#define DEFAULT_GRID_SIZE 10     // Default grid size
#define MAX_GRID_SIZE 40
#define MIN_GRID_SIZE 5
#define MAX_STEP_SIZE 100
#define MICRO_SECONDS 750000

int pBurning = DEFAULT_P_BURNING;
int pCatch = DEFAULT_P_CATCH;
int pDensity = DEFAULT_DENSITY;
int pNeighbor = DEFAULT_NEIGHBOR_EFFECT;
int gridSize = DEFAULT_GRID_SIZE;
int printMode = -1; // By default, overlay mode (-p not specified)
int tot_changes_made = 0;
// Function to display usage/help information
void display_help() {
    fprintf(stderr, "usage: wildfire [options]\n");
    fprintf(stderr, "Simulation Configuration Options:\n");
    fprintf(stderr, " -H  # View simulation options and quit.\n");
    fprintf(stderr, " -bN # proportion of trees that are already burning. 0 < N < 101.\n");
    fprintf(stderr, " -cN # probability that a tree will catch fire. 0 < N < 101.\n");
    fprintf(stderr, " -dN # density: the proportion of trees in the grid. 0 < N < 101.\n");
    fprintf(stderr, " -nN # proportion of neighbors that influence a tree catching fire. -1 < N < 101.\n");
    fprintf(stderr, " -pN # number of states to print before quitting. -1 < N < ...\n");
    fprintf(stderr, " -sN # simulation grid size. 4 < N < 41.\n");
}

// Function to parse arguments
int parse_args(int argc, char* argv[]) {
    int opt;
    while ((opt = getopt(argc, argv, "Hb:c:d:n:p:s:")) != -1) {
        switch (opt) {
            case 'H':
                display_help();
                exit(0);
            case 'b':
                pBurning = (int)strtol(optarg, NULL, 10);
                if (pBurning < 1 || pBurning > 100) {
                    pBurning = DEFAULT_P_BURNING;
                    fprintf(stderr, "Invalid -b value. Set to default %d%%.\n", DEFAULT_P_BURNING);
                }
                break;
            case 'c':
                pCatch = (int)strtol(optarg, NULL, 10);
                if (pCatch < 1 || pCatch > 100) {
                    pCatch = DEFAULT_P_CATCH;
                    fprintf(stderr, "Invalid -c value. Set to default %d%%.\n", DEFAULT_P_CATCH);
                }
                break;
            case 'd':
                pDensity = (int)strtol(optarg, NULL, 10);
                if (pDensity < 1 || pDensity > 100) {
                    pDensity = DEFAULT_DENSITY;
                    fprintf(stderr, "Invalid -d value. Set to default %d%%.\n", DEFAULT_DENSITY);
                }
                break;
            case 'n':
                pNeighbor = (int)strtol(optarg, NULL, 10);
                if (pNeighbor < 0 || pNeighbor > 100) {
                    pNeighbor = DEFAULT_NEIGHBOR_EFFECT;
                    fprintf(stderr, "Invalid -n value. Set to default %d%%.\n", DEFAULT_NEIGHBOR_EFFECT);
                }
                break;
            case 'p':
                printMode = (int)strtol(optarg, NULL, 10);
                if (printMode < 0) {
                    fprintf(stderr, "Invalid -p value. Using default overlay mode.\n");
                    printMode = -1;
                }
                break;
            case 's':
                gridSize = (int)strtol(optarg, NULL, 10);
                if (gridSize < MIN_GRID_SIZE || gridSize > MAX_GRID_SIZE) {
                    gridSize = DEFAULT_GRID_SIZE;
                    fprintf(stderr, "Invalid -s value. Set to default %dx%d.\n", DEFAULT_GRID_SIZE, DEFAULT_GRID_SIZE);
                }
                break;
            default:
                display_help();
                return 1;
        }
    }
    return 0;
    }


int is_burning(int grid[][gridSize], int row, int col){
    if(grid[row][col] == BURNING_0 ||
                  grid[row][col] == BURNING_1 ||
                  grid[row][col] == BURNING_2 ||
                  grid[row][col] == BURNING_3){
        return 1;
    }
    return 0;
}


int check_fires(int grid[][gridSize], int gridSize){
    for(int r = 0; r < gridSize; r++){
        for(int c = 0; c < gridSize; c++){
            if(is_burning(grid, r,c) == 1){
                return 0;
            }
        }
    }
    return 1;
}

void initial_grid(int grid[][gridSize], int gridSize) {
    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize; j++) {
            grid[i][j] = EMPTY;
        }
    }
    int tree_count = (int)((pDensity * gridSize * gridSize) / 100);
    int burning = (int)((pBurning * tree_count) / 100);
    int tree_index[tree_count][2];

    for (int i = 0; i <= tree_count;) {
        int row = rand() % gridSize;
        int col = rand() % gridSize;
        if (grid[row][col] == EMPTY) {
            grid[row][col] = TREE;
            tree_index[i][0] = row;
            tree_index[i][1] = col;
            i++;
        }
    }
    for (int j = 0; j < burning;) {
        int randomTree = rand() % tree_count;
        int row = tree_index[randomTree][0];
        int col = tree_index[randomTree][1];
        if (grid[row][col] == TREE) {
            grid[row][col] = BURNING_1;
            j++;
        }
    }
}

void spread(int initial[][gridSize], int gridSize, int pNeighbor, int pCatch, int * changes_made){
    //first lets make a copy ans store it in a current variable 
    //we will make updattions in the orginal grid itself 
    int current[gridSize][gridSize];
    int neighbor_count;//number of trees in the nneighbors
    int burning_count;//how many of thise trees are burning 
    for(int r = 0; r < gridSize; r++){
        for(int c = 0; c < gridSize; c++){
            current[r][c] = initial[r][c];
            }
    }
    
    for(int r = 0; r < gridSize; r++){
        for(int c = 0; c < gridSize; c++){
            //now lets check for neighbors
            neighbor_count = 0;
            burning_count = 0;
            if(current[r][c] ==  TREE){
                for(int dr = -1; dr <=1; dr++){
                    for(int dc = -1; dc <=1; dc++){
                        if(dr == 0 && dc == 0){continue;}//at the current cell
                        int nr = r + dr;
                        int nc = c + dc;

                        if(nr >= 0 && nr < gridSize && nc >= 0 && nc < gridSize){
                            if(current[nr][nc] == TREE) {neighbor_count++;}
                            if(current[nr][nc] == BURNING_0 || 
                                current[nr][nc] == BURNING_1 ||
                                current[nr][nc] == BURNING_2 || 
                                current[nr][nc] == BURNING_3) {
                                neighbor_count++;
                                burning_count++;
                                }
                        }
                    }   

                }
                //now we have the count and the lets check the ratio
                float neighbor_ratio = 0;
                if(neighbor_count > 0){
                    neighbor_ratio = (float) burning_count/neighbor_count;
                }

                //check susceptibility
                if((neighbor_ratio*100) > pNeighbor){
                    float random_value = (float)rand()/RAND_MAX;
                    if((random_value*100) > pCatch){
                        initial[r][c] = BURNING_1;
                        (*changes_made)++;
                }
            }
        }
    }
    }
}

void update_grid(int initial[][gridSize], int gridSize,int * changes_made){
    for(int r = 0;r < gridSize; r++){
        for(int c = 0; c < gridSize; c++){

            if (initial[r][c] == BURNING_0) initial[r][c] = BURNING_1;
            else if (initial[r][c] == BURNING_1) initial[r][c] = BURNING_2;
            else if (initial[r][c] == BURNING_3){
                initial[r][c] = BURNED;
                (*changes_made)++;
            }
            else if (initial[r][c] == BURNING_2) initial[r][c] = BURNING_3;
    }
}
}

void print_overlay(int grid[][gridSize], int gridSize){
    clear();
    set_cur_pos(0, 0);

    // Print the grid
    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize; j++) {
            if(is_burning(grid,i,j) == 1){
                put(BURNING_0);
            }else{
                put(grid[i][j]);
            }
            put(' ');
        }
        put('\n');
    }
    printf("size %d, pCatch 0.%d, density 0.%d, pBurning 0.%d, pNeighbor 0.%d \n", gridSize, pCatch, pDensity, pBurning, pNeighbor);
   // usleep(MICRO_SECONDS);
}


                
void print_grid(int grid[][gridSize],int gridSize) {
    printf("\n");
     for (int i = 0; i < gridSize; i++) {
         for (int j = 0; j < gridSize; j++) {
             if(is_burning(grid,i,j) == 1){
                 printf("%c ",BURNING_0);
             }else{
             printf("%c ", grid[i][j]);
             }
         }
         printf("\n");
     }
     printf("\n");
     printf("size %d, pCatch 0.%d, density 0.%d, pBurning 0.%d, pNeighbor 0.%d \n", gridSize, pCatch, pDensity, pBurning, pNeighbor);
}

void run_simulation(int grid[][gridSize], int gridsize, int max_time_steps, int pNeighbor, int pCatch, int * tot_changes_made, int overlay){
    
    int time_steps = 1;
    //int changes_made = 0;
    while(time_steps <= max_time_steps){
        int changes_made = 0;
        update_grid(grid, gridSize, &changes_made);
        spread(grid, gridSize, pNeighbor, pCatch, &changes_made);
        if(overlay == -1){grd
            clear();
            print_overlay(grid, gridSize);
        }else{
            print_grid(grid, gridSize);
        }
        *tot_changes_made += changes_made;
        printf("Cycle %d, current Changes %d, cumulative_changes %d \n",time_steps, changes_made, *tot_changes_made);
        time_steps++;
        if(check_fires(grid, gridSize) == 1){
            printf("Fires are out.\n");
            break;

        }
        usleep(MICRO_SECONDS);
    }
    
}


int main(int argc, char* argv[]) {
    srandom(SEED);
    parse_args(argc, argv);
    int grid[gridSize][gridSize];
    initial_grid(grid, gridSize);
    if (printMode != -1) {
        printf("=========================== \n");
        printf("======== Wildfire ========= \n");
        printf("=========================== \n");
        printf("=== Print %d Time Steps === \n", printMode);
        printf("=========================== \n");
        print_grid(grid,gridSize);
        printf("Cycle %d, Current Changes %d, Cumulative Changes %d \n", 0,0,0);
        printf("\n");
        run_simulation(grid, gridSize, printMode, pNeighbor, pCatch, &tot_changes_made, 0);
    }else {
        //overlay mode 
        print_overlay(grid,gridSize);
        run_simulation(grid,gridSize,MAX_STEP_SIZE,pNeighbor,pCatch, &tot_changes_made,printMode);
    }
    return 0;
}

