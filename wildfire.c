 /*
  * File:    wildfire.c
  * Author   Jatin Jain
  * description: This assignment develops a two-dimensional computer simulation to model the spread of fire.
  */

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <time.h>
#include "display.h"
#include <unistd.h>


//Contants and states
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
#define MAX_GRID_SIZE 41
#define MIN_GRID_SIZE 4
#define MAX_STEP_SIZE 100
#define MICRO_SECONDS 750000

//Extra credit Default values
#define DAMP_TREE 'D'
#define LIGHTNING_TREE '!'
#define MIN_LIGHTNING -1
#define MAX_LIGHTNING 100
#define MIN_DAMPNESS -1
#define MAX_DAMPNESS 100
//Global Variable 
int pBurning = DEFAULT_P_BURNING;
int pCatch = DEFAULT_P_CATCH;
int pDensity = DEFAULT_DENSITY;
int pNeighbor = DEFAULT_NEIGHBOR_EFFECT;
int gridSize = DEFAULT_GRID_SIZE;
int printMode = -1; // By default, overlay mode (-p not specified)
int tot_changes_made = 0;
//extra credit global variables 
int pLightning = 0;
int pDampness = 0;

// Function: display_help 
// displays usage/help information as stderr
// params: none
// returns: void
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
    fprintf(stderr, " -Ln # (extra-credit) probaility of lightning -1 < N < 101.\n");
    fprintf(stderr, " -DN # (extra-credit) mesure of dampness -1 < N < 101.\n");
}

// Function : parse_arguments
// parses arguments from comand line
// handles bad input
// param argc: int count of cmd line arguments
// param argv: char array, pointer to the values of cmd line input
// returns 1: default case/bad argument
// returns 0: after assigning /handling bad arfuments

int parse_args(int argc, char* argv[]) {
    int opt;
    while ((opt = getopt(argc, argv, "Hb:c:d:n:p:s:L:D:")) != -1) {
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
            case 'L':
                pLightning = (int) strtol(optarg, NULL, 10);
                if(pLightning < MIN_LIGHTNING || pLightning > MAX_LIGHTNING){
                    pLightning = 0;
                    fprintf(stderr, "Invalid -L value, run -H for more info, this is the 1-probability of lightning");
                }
                break;
            case 'D':
                pDampness = (int) strtol(optarg, NULL, 10);
                if(pDampness < MIN_DAMPNESS || pDampness > MAX_DAMPNESS){
                    pDampness = 0;
                    fprintf(stderr, "Invalid -D value, run -H for more infor, running qwith default arfuments");
                }
                break;
            default:
                display_help();
                return 1;
        }
    }
    return 0;
    }


// function is_burning : Checks if the current row/col is in one of the 3 burning states
// param: grid int array of the grid of size gridSize
// returns 1 if true else 0

int is_burning(int grid[][gridSize], int row, int col){
    if(grid[row][col] == BURNING_0 ||
                  grid[row][col] == BURNING_1 ||
                  grid[row][col] == BURNING_2 ||
                  grid[row][col] == BURNING_3){
        return 1;
    }
    return 0;
}

// function check_fires 
// checks for active fires in the grid
//if none found returns 1 else 0
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

// function: intitial_grid
// params: grid is an array of integers
//  gridSize: size of the grid
void initial_grid(int grid[][gridSize], int gridSize) {
    
    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize; j++) {
            grid[i][j] = EMPTY;
       }
    }
    int tree_count = (int)((pDensity * gridSize * gridSize) / 100);
    int burning = (int)((pBurning * tree_count) / 100);
    int tree_index[tree_count][2];
    int num_damp = (int)((pDampness * tree_count)/100);
    int damp_count = 0;
    for (int i = 0; i < tree_count;) {
        int row = rand() % gridSize;
        int col = rand() % gridSize;
        if (grid[row][col] == EMPTY) {
            if( (rand() % 5 ) >= 3){ //just randomly adding damp trees 
                grid[row][col] == DAMP_TREE;
                damp_count++;
            }else{
                grid[row][col] = TREE;
            }
            tree_index[i][0] = row;
            tree_index[i][1] = col;
            i++;
        }
    }
    

    for (int j = 0; j < burning;) {
        if(pBurning == 100){
            int row = tree_index[j][0];
            int col = tree_index[j][1];
            grid[row][col] = BURNING_1;
            j++;
            continue;
        }
        int randomTree = rand() % tree_count;
        int row = tree_index[randomTree][0];
        int col = tree_index[randomTree][1];
        if (grid[row][col] == TREE) {
            grid[row][col] = BURNING_1;
            j++;
        }
    }
//can comment if donot want to add n% damp trees 
    while(damp_count < num_damp){
        int random_tree = rand() % tree_count;
        int row = tree_index[random_tree][0];
        int col = tree_index[random_tree][1];
        if(grid[row][col] == TREE){
            grid[row][col] = DAMP_TREE;
            damp_count++;
        }
    }
}

//function get_neighbor_ratio - returns the ratio of burning tree - Neighbors for given row, col
// returns a floating point number with the ratio value 
float get_neighbor_ratio(int current[][gridSize], int gridSize, int r, int c){
    int neighbor_count = 0;
    int burning_count = 0;
    float neighbor_ratio = 0;
    if(current[r][c] ==  TREE){
        for(int dr = -1; dr <=1; dr++){
            for(int dc = -1; dc <=1; dc++){
                if(dr == 0 && dc == 0){continue;}//at the current cell
                int nr = r + dr;
                int nc = c + dc;

                if(nr >= 0 && nr < gridSize && nc >= 0 && nc < gridSize){
                    if(current[nr][nc] == TREE || current[nr][nc] == DAMP_TREE || current[nr][nc] == LIGHTNING_TREE) {neighbor_count++;}
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
        if(neighbor_count > 0){
            neighbor_ratio = (float) burning_count/neighbor_count;
        }

    }
    return neighbor_ratio;

}

//funbction spread: implements the spread logic as per project guidlines 
// returns: void
// params: initial: int[] this is the initial grid where all changes are made
//  gridSize: int size of the grid
//  pNeighbor: int neighbor influence
//  pCatch: probability of Catching fire
//  hanges_made : int * keeps count of the changes made

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
            int e_pCatch;
            float neighbor_ratio = get_neighbor_ratio(current, gridSize, r,c);
            if(current[r][c] == DAMP_TREE){         
                //Extra Credit
                e_pCatch = pCatch - pDampness; //decrements the probablity of catching fire
                if((neighbor_ratio*100) > pNeighbor){
                    float random_value = (float)rand()/RAND_MAX;
                    if((random_value*100) < e_pCatch){
                        initial[r][c] = BURNING_1;
                        (*changes_made)++;
                    }
                }
            }else if(current[r][c] == TREE){
                e_pCatch = pCatch;
                //check susceptibility for normal tree/damp tree
                if((neighbor_ratio*100) > pNeighbor){
                    float random_value = (float)rand()/RAND_MAX;
                    if((random_value*100) < e_pCatch){
                        initial[r][c] = BURNING_1;
                        (*changes_made)++;
                    }
                }
            } else if(current[r][c] == LIGHTNING_TREE){
                //lighning tree check - extra credit
                // Check susceptibility
                //check if the lightning strike at r,c in the previous state if yea then hecks its susceptibility
                float random_value = (float)rand() / RAND_MAX * 100;
                if ((random_value) < pCatch) {
                    initial[r][c] = BURNING_1;
                    (*changes_made)++;
                }else{
                    initial[r][c] = TREE;
                    (*changes_made)++;
                }
            }
        }
    
    }

    //EXTRA CREDIT lighning simulation
    //will implement here as restoring a current grid will be expensive for memory
    // Lightning strike logic -
    // first iteration lightning strikjes on tree !
    // then next state tree atches fire/not based on susceptibility
    // Y -> ! -> * -> * -> * -> .
    // Y -> ! -> Y
    //here lightning_chance checks if it is less than pLightning
    // as when -L is not called, pLightning = 0 hem=nce to avoid lightnings we will use 1-PLigtning as the chance of striking
    float lightning_chance = (float)rand() / RAND_MAX;
    if ((lightning_chance*100) < pLightning) {
        int lightning_row = rand() % gridSize;
        int lightning_col = rand() % gridSize;
        if (current[lightning_row][lightning_col] == TREE) {
            initial[lightning_row][lightning_col] = LIGHTNING_TREE; // Tree catches fire from lightning
            (*changes_made)++;
        }
    }
}


//function update_grid : changes the states of the fire
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

//function print_overlay implements overlay functionality usinig methods from display.h
void print_overlay(int grid[][gridSize], int gridSize){
    clear();
    //set_cur_pos(0, 0);
    // Print the grid
    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize; j++) 
        {
            set_cur_pos(i,j);
            if(is_burning(grid,i,j) == 1){ put(BURNING_0);}
            else{put(grid[i][j]);}
            //put(' ');
        }
        //put('\n');
    }
    printf("\nsize %d, pCatch %.2f, density %.2f, pBurning %.2f, pNeighbor %.2f \n", gridSize, ((float)pCatch/100), ((float)pDensity/100),((float) pBurning/100), ((float)pNeighbor/100));
   // usleep(MICRO_SECONDS);
}

//implements print_grid in printMode format
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

//function run_simulation implements the simulation logic and calls upon [precipously  defined functions
// check for printMode ad displays output accordingly
// uses usleep() -> which pauses the program fopr n-micro-seconds so the output can be anaalyswed
void run_simulation(int grid[][gridSize], int gridsize, int max_time_steps, int pNeighbor, int pCatch, int * tot_changes_made, int overlay){
    int time_steps = 1;
    //int changes_made = 0;
    while(time_steps <= max_time_steps){
        int changes_made = 0;
        update_grid(grid, gridSize, &changes_made);
        spread(grid, gridSize, pNeighbor, pCatch, &changes_made);
        if(overlay == -1){
            clear();
            print_overlay(grid, gridSize);
        }else{
            print_grid(grid, gridSize);
        }
        *tot_changes_made += changes_made;
        printf("Cycle %d, current Changes %d, cumulative_changes %d, pLigtning %d \n",time_steps, changes_made, *tot_changes_made, pLightning);
        time_steps++;
        if(check_fires(grid, gridSize) == 1){
            printf("Fires are out.\n");
            break;

        }
        usleep(MICRO_SECONDS);
    }
    
}


int main(int argc, char* argv[]) {
    //this is the main method, all instances and function calls made here.
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

