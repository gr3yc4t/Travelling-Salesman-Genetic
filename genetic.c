#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
 
#define DEBUG 0
#define X_MAX 750                   //Grid dimensions
#define Y_MAX 750
#define CITY_COUNT 50               //Number of cities
#define POPULATION_COUNT 256        //Number of individuals
#define ITERATION_COUNT 10000       //Max number of iterations allowed
#define MUTATION_LIKELIHOOD 0.25    //Random mutation probability
 
/*-------------------------------------------------------------*/
 
typedef struct{                 //Contains the coords of a city within the grid
    unsigned x, y;
} city;
 
typedef struct{                 //Strucure of an individual
    int solution[CITY_COUNT];   //Solution vector, the cities are traversed in order
    double fitness;             //Quantifies the goodness of the above solution
} individual;
 
/*-------------------------------------------------------------*/
 
void debug(char *);
double min(individual *); 
void setup_cities(city *);                                      // Generates randomically the cities positions
void generate_initial_population(individual *);                 // Generates casually the initial population, creating random solutions
double get_distance(city *, int, int);                          // Returns the distance between two cities, given a cities_map
double * evaluate_fitness(city *, individual *);                // Computes the various fitness values for every individual in the input, given a cities_map
int * selection(double *);                                      // 
individual * crossover(individual *, int *);                    // 
void mutation(individual *);                                    // Randomically changes some solutions (swapping two elements within a solution) based on the likelihood
void cycle_crossover(individual *, int, int, int *, int *);     // Computes the cycle crossover alghoritm generating two new solutions (int *), given two parent IDs (int, index of individual *)
void swap(int *, int *);                                        // Swaps two cities within a solution
int highest_fitness(individual *);                              // Returns the index of the individual with the best fitness value
 
/*-------------------------------------------------------------*/
 
int main(){
 
    srand(time(NULL));           //Random seed initialization
 
    city cities_map[CITY_COUNT]; //Allocates the cities in the maps
 
    individual * population, * new_gen;
 
    population = malloc(sizeof(individual) * POPULATION_COUNT); //Allocates the population, there are "POPULATION_COUNT" individuals
 
    setup_cities(cities_map);       //Generates (randomically) the cities position
 
    FILE * fd = fopen("data.txt", "w");    //this file will be used in the graphical script to show the various generations
    if (fd == NULL){
        printf("Fatal error: could not open the data file\n");
        return -1;
    }
 
    fprintf(fd, "%d,%d\n", X_MAX, Y_MAX);       //We save the grid size...
    fprintf(fd, "%d\n", CITY_COUNT);        //We save the number of cities generated...

    for (int i = 0; i < CITY_COUNT; i++){   //...and the various coordinates
        fprintf(fd, "%d,%d\n", cities_map[i].x, cities_map[i].y);
    }
 
    generate_initial_population(population);    //Generates casually the initial population, creating random solutions
 
    int best;
    int * candidates;
    double length;
    double * cdf;
 
    for (int i = 0; i < ITERATION_COUNT; i++){
 
        cdf = evaluate_fitness(cities_map, population);    //We obtain the cumulative distribution function associated to the current population
 
        if (i%100 == 0){
 
            best = highest_fitness(population);     //Returns the index of the individual with the best fitness value
           
            length = 1/population[best].fitness;    //The lenght of the solution is the inverse of the fitness (the fitness is defined as 1/lenght)
           
            fprintf(fd, "%d %f ", i, length);       //Save the best lenght...
 
            for (int j = 0; j < CITY_COUNT; j++){
                fprintf(fd, "%d ", population[best].solution[j]);   //... and the solution
            }
            fprintf(fd, "\n");
 
            printf("Current generation: %d | Best length: %f\n", i, length);
        }
 
        candidates = selection(cdf);        //candidates is an integer vector containing the indexes of those that are going to reproduce
       
        new_gen = crossover(population, candidates); //We crossover the population to obtain a new one
 
        free(population);
 
        population = new_gen;
 
        mutation(population);   //We apply the eventual mutation to the population
    }
 
    fclose(fd);
 
    return 0;
}
 
/*-------------------------------------------------------------*/
 
void debug(char *message){
    if (DEBUG) printf("[D] %s\n", message);
}
 
void swap(int * a, int * b){
    int t = *a;
    *a = *b;
    *b = t;
}
 
int highest_fitness(individual * population){
 
    int index = 0;
    double initial = population[0].fitness;
    for (int j = 1; j < POPULATION_COUNT; j++){
        if (population[j].fitness > initial){   //Finds the highest fitness
            initial = population[j].fitness;
            index = j;
        }
    }
    return index;
}
 
void setup_cities(city * cities_map){
 
    for (int i = 0; i < CITY_COUNT; i++){
        cities_map[i].x = rand() % X_MAX;   //Generates the coordinates for the cities between [0, X_MAX) and [0, Y_MAX)
        cities_map[i].y = rand() % Y_MAX;
    }
 
    debug("Random map generation completed");
}
 
void generate_initial_population(individual * population){
 
    int x;
 
    for (int i = 0; i < POPULATION_COUNT; i++){
        for (int j = 0; j < CITY_COUNT; j++){
            x = rand() % (j+1);     //Casual number between [0, j]
            if (x != j){            //The element at position X gets assigned to the element at position J if x<j, otherwise solution[j] is unitizialized
                population[i].solution[j] = population[i].solution[x];
            }
            population[i].solution[x] = j;
        }
    } 
 
    debug("Random population generation completed");
}
 
double get_distance(city * cities_map, int id_1, int id_2){
 
    int x1 = cities_map[id_1].x;
    int y1 = cities_map[id_1].y;
    int x2 = cities_map[id_2].x;
    int y2 = cities_map[id_2].y;
 
    double dist = sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
    return dist;
}
 
double * evaluate_fitness(city * cities_map, individual * population){
 
    int id_1, id_2;
    double total, fitness_sum = 0;
    double * cdf = malloc(sizeof(double) * POPULATION_COUNT);  //Allocate space for the cumulative distribution function of a probability distribution
 
    for (int i = 0; i < POPULATION_COUNT; i++){
 
        total = 0;  //Contains the total distances of one solution
 
        for (int j = 0; j < CITY_COUNT-1; j++){
            id_1 = population[i].solution[j];
            id_2 = population[i].solution[j+1];
            total += get_distance(cities_map, id_1, id_2);  //Caomputes the distance between cities and the total sum
        }
 
        population[i].fitness = 1/total;        //The fitness is 1/total
        fitness_sum += population[i].fitness;   //We use the sum of all the fitness to normalize the probability distribution
    }
 
    cdf[0] = population[0].fitness/fitness_sum;
       
    for (int i = 1; i < POPULATION_COUNT; i++){
        cdf[i] = cdf[i-1] + population[i].fitness/fitness_sum;    //Assign to cumulative distribution the value of the previous element + the current normalized fitness divided by the sum of all the fitnesses
    }
 
    debug("Computed fitness");
   
    return cdf;
}
 
int * selection(double * cdf){
 
    double extraction;
    int pivot, first, last;
    int * candidates = malloc(sizeof(int) * POPULATION_COUNT);
 
    for (int i = 0; i < POPULATION_COUNT; i++){
 
        first = 0;
        last = POPULATION_COUNT;
        extraction = (double)rand()/RAND_MAX;   //rand() generates a number in [0, RAND_MAX] therefore dividing by RAND_MAX we obtain a value in [0, 1]
 
        while (first != last){  //Binary search of the first element of the CDF that has bigger probability than the value randomly generated
            pivot = (first + last)/2;
            if (cdf[pivot] == extraction){
                break;
            }
            if (cdf[pivot] > extraction){
                last = pivot;
            } else {
                first = pivot + 1;
            }
        }
        candidates[i] = first; //candidates[i] contains the index of the extracted individual, the bigger the probability, the more the index appears in candidates (so it reproduces often because it's a good solution)
    }
 
    free(cdf); //It does not serve any purpose anymore
 
    debug("Extracted candidates");
   
    return candidates;
}
 
individual * crossover(individual * population, int * candidates){
 
    individual * new_gen = malloc(sizeof(individual) * POPULATION_COUNT);
 
    for (int i = 0; i < POPULATION_COUNT; i+=2){
        cycle_crossover(population, candidates[i], candidates[i+1], new_gen[i].solution, new_gen[i+1].solution);   //We perform the crossover two by two, choosing as parents two candidates choosen by index, then we save the offspring into new_gen
    }
 
    free(candidates);    //We don't need them anymore
 
    debug("Reproduction performed");
 
    return new_gen;
}
 
void mutation(individual * population){
   
    int x, y, p = RAND_MAX * MUTATION_LIKELIHOOD;
 
    for (int i = 0; i < POPULATION_COUNT; i++){
 
        if (rand() < p){    //In this case p = RAND_MAX*0.25 = RAND_MAX/4, because rand() generates a number in [0, RAND_MAX] and therefore this condition has probability 1/4 of being true
            do {
                x = rand() % CITY_COUNT;
                y = rand() % CITY_COUNT;
            } while (x == y);   //we should not commute x with itself, it's useless
 
            swap(&population[i].solution[x], &population[i].solution[y]);    //Swap the cities
        }
    }
 
    debug("Mutation executed");
}
 
void cycle_crossover(individual * population, int id_1, int id_2, int * child_1, int * child_2){
 
    int parent_1 = id_1, parent_2 = id_2;           //Copy the parent indexes
    int j = 0;                                      //Current column
    int lookup[CITY_COUNT];                         //Addressing vector
    int flags[CITY_COUNT] = {0};                    //Visited columns vector
    int visited = 0;                                //number of visited columns
    int first = population[id_1].solution[0], last; //First and last element of the cycle
 
    for (int i = 0; i < CITY_COUNT; i++){
        lookup[population[id_1].solution[i]] = i;   //this vector is used to obtain the position of an element of id_2 in id_1, to do this we save in lookup[i] the position of the element to search
    }
 
    while (visited < CITY_COUNT){
 
        child_1[j] = population[parent_1].solution[j];  //We copy the cities in the childs
        child_2[j] = population[parent_2].solution[j];
        last = population[id_2].solution[j];
 
        flags[j] = 1;       //J becomes visited
        visited++;
        j = lookup[last];   //search for last in id_1
 
        if (first == last){
            swap(&parent_1, &parent_2);          //We use this every cycle to copy the cities in the childs in reverse order (swapping the parents)
            while (flags[j]) j++;                   //Search for a free column
            first = population[id_1].solution[j];   //Reinitizialize first element of the cycle
        }  
    }
}
