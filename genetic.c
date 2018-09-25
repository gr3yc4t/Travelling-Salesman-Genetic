#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
 
#define DEBUG 0
#define X_MAX 700                   //Dimensione griglia
#define Y_MAX 700
#define CITY_COUNT 25               //Numero di città
#define POPULATION_COUNT 256        //Numero di individui
#define ITERATION_COUNT 10000       //Numero di generazioni
#define MUTATION_LIKELIHOOD 0.25    //probabilità di mutazione in [0,1]
 
/*-------------------------------------------------------------*/
 
typedef struct{                 //Coordinate città
    unsigned x, y;
} city;
 
typedef struct{                 //Struttura individuo
    int solution[CITY_COUNT];   //Vettore solution, ovvero l'ordine di visita delle città
    double fitness;             //Bontà della soluzione
} individual;
 
/*-------------------------------------------------------------*/
 
void debug(char *);
double min(individual *);
void setup_cities(city *);
void generate_initial_population(individual *);
double get_distance(city *, int, int);
double * evaluate_fitness(city *, individual *);
int * selection(double *);
individual * crossover(individual *, int *);
void mutation(individual *);
void cycle_crossover(individual *, int, int, int *, int *);
void scambia(int *, int *);
int highest_fitness(individual * population);
 
/*-------------------------------------------------------------*/
 
int main(){
 
    srand(time(NULL));           //Inizializzazione del generatore di numeri pseudo casuali
 
    city cities_map[CITY_COUNT]; //Mappa delle città
 
    individual * population, * new_gen;
 
    population = malloc(sizeof(individual) * POPULATION_COUNT); //population contiene POPULATION_COUNT individui
 
    setup_cities(cities_map);       //Generazione iniziale della mappa
 
    FILE * fd;
 
    fd = fopen("data.txt", "w");    //Questo file contiene le informazioni da utilizzare per la grafica
    if (fd == NULL){
        printf("Errore apertura file\n");
        return -1;
    }
 
    fprintf(fd, "%d\n", CITY_COUNT);        //Salviamo il numero di città
 
    for (int i = 0; i < CITY_COUNT; i++){   //E le loro coordinate
        fprintf(fd, "%d,%d\n", cities_map[i].x, cities_map[i].y);
    }
 
    generate_initial_population(population);    //Genera la popolazione iniziale, partiamo da soluzioni casuali per ogni individuo
 
    int best;
    int * candidati;
    double length;
    double * ripartizione;
 
    for (int i = 0; i < ITERATION_COUNT; i++){
 
        ripartizione = evaluate_fitness(cities_map, population);    //Otteniamo la distribuzione di probabilità (ripartizione) associata alla popolazione corrente
 
        if (i%100 == 0){
 
            best = highest_fitness(population);     //Troviamo l'indice dell'individuo con la soluzione migliore
           
            length = 1/population[best].fitness;    //la lunghezza del percorso è 1/fitness essendo fitness calcolato come 1/lunghezza
           
            fprintf(fd, "%d %f ", i, length);       //Salviamo la lunghezza del percorso
 
            for (int j = 0; j < CITY_COUNT; j++){
                fprintf(fd, "%d ", population[best].solution[j]);   //E l'ordine delle città
            }
            fprintf(fd, "\n");
 
            printf("Generazione attuale: %d | Distanza migliore: %f\n", i, length);
        }
 
        candidati = selection(ripartizione);        //canditati è un vettore di interi contenente gli indici degli individui scelti secondo la distribuzione ripartizione
       
        new_gen = crossover(population, candidati); //Incrociamo gli individui ottenendo la nuova popolazione new_gen
 
        free(population);
 
        population = new_gen;
 
        mutation(population);   //Applichiamo la mutazione sulla popolazione
    }
 
    fclose(fd);
 
    return 0;
}
 
/*-------------------------------------------------------------*/
 
void debug(char *message){
    if (DEBUG) printf("[D] %s\n", message);
}
 
void scambia(int * a, int * b){
    int t = *a;
    *a = *b;
    *b = t;
}
 
int highest_fitness(individual * population){
 
    int index = 0;
    double initial = population[0].fitness;
    for (int j = 0; j < POPULATION_COUNT; j++){
        if (population[j].fitness > initial){   //Troviamo il fitness massimo e restituiamo l'indice
            initial = population[j].fitness;
            index = j;
        }
    }
    return index;
}
 
void setup_cities(city * cities_map){
 
    for (int i = 0; i < CITY_COUNT; i++){
        cities_map[i].x = rand() % X_MAX;   //Genera le coordinate per ogni città in [0, X_MAX) e [0, Y_MAX)
        cities_map[i].y = rand() % Y_MAX;
    }
 
    debug("Generazione della mappa completata");
}
 
void generate_initial_population(individual * population){
 
    int x;
 
    for (int i = 0; i < POPULATION_COUNT; i++){
        for (int j = 0; j < CITY_COUNT; j++){
            x = rand() % (j+1);     //Numero casuale in [0, j]
            if (x != j){            //All'elemento alla posizione j viene assegnato l'elemento alla posizione x (se x strettamente minore di j altrimenti assegneremmo a solution[j] un valore non inizializzato)
                population[i].solution[j] = population[i].solution[x];
            }
            population[i].solution[x] = j;
        }
    }
 
    debug("Generazione della popolazione iniziale completata");
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
    double * ripartizione = malloc(sizeof(double) * POPULATION_COUNT);  //Allochiamo spazio per la distribuzione di probabilità
 
    for (int i = 0; i < POPULATION_COUNT; i++){
 
        total = 0;  //Contiene la distanza totale tra le città
 
        for (int j = 0; j < CITY_COUNT-1; j++){
            id_1 = population[i].solution[j];
            id_2 = population[i].solution[j+1];
            total += get_distance(cities_map, id_1, id_2);  //Calcola la distanza tra due città e la somma alla totale
        }
 
        population[i].fitness = 1/total;        //A questo punto il fitness è 1/distanza totale
        fitness_sum += population[i].fitness;   //La somma dei fitness viene utilizzata per la "normalizzazione"
    }
 
    ripartizione[0] = population[0].fitness/fitness_sum;
       
    for (int i = 1; i < POPULATION_COUNT; i++){
        ripartizione[i] = ripartizione[i-1] + population[i].fitness/fitness_sum;    //Assegna al valore ripartizione il valore dell'elemento precedente + il fitness corrente normalizzato (diviso per la loro somma)
    }
 
    debug("Fitness calcolati");
   
    return ripartizione;
}
 
int * selection(double * ripartizione){
 
    double extraction;
    int pivot, first, last;
    int * candidati = malloc(sizeof(int) * POPULATION_COUNT);
 
    for (int i = 0; i < POPULATION_COUNT; i++){
 
        first = 0;
        last = POPULATION_COUNT;
        extraction = (double)rand()/RAND_MAX;   //rand() genera un numero in [0, RAND_MAX] quindi dividendo per RAND_MAX otteniamo un numero in [0, 1]
 
        while (first != last){  //Eseguiamo una ricerca binaria del primo elemento del vettore ripartizione con probabilità maggiore di quella estratta
            pivot = (first + last)/2;
            if (ripartizione[pivot] == extraction){
                break;
            }
            if (ripartizione[pivot] > extraction){
                last = pivot;
            } else {
                first = pivot + 1;
            }
        }
        candidati[i] = first; //candidati[i] contiene l'indice dell'individuo estratto, maggiore è la sua probabilità in ripartizione, maggiore è la probabilità di essere in candidati, anche ripetutamente
    }
 
    free(ripartizione); //Non serve più a questo punto
 
    debug("Candidati estratti");
   
    return candidati;
}
 
individual * crossover(individual * population, int * candidati){
 
    individual * new_gen = malloc(sizeof(individual) * POPULATION_COUNT);
 
    for (int i = 0; i < POPULATION_COUNT; i+=2){
        cycle_crossover(population, candidati[i], candidati[i+1], new_gen[i].solution, new_gen[i+1].solution);  //Eseguiamo il crossover a due a due scegliendo come genitori due candidati scelti tramite l'indice, salviamo i figli in new_gen
    }
 
    free(candidati);    //Non servono più
 
    debug("Riproduzione effettuata");
 
    return new_gen;
}
 
void mutation(individual * population){
   
    int x, y, p = RAND_MAX * MUTATION_LIKELIHOOD;
 
    for (int i = 0; i < POPULATION_COUNT; i++){
 
        if (rand() < p){    //In questo caso p = RAND_MAX*0.25 = RAND_MAX/4, poiché rand() genera un numero in [0, RAND_MAX] la condizione rand() < p ha 1 probabilità su 4 di verificarsi
            do {
                x = rand() % CITY_COUNT;
                y = rand() % CITY_COUNT;
            } while (x == y);   //evitiamo che x sia uguale a y
 
            scambia(&population[i].solution[x], &population[i].solution[y]);    //Scambiamo le 2 città
        }
    }
 
    debug("Mutazioni eseguite");
}
 
void cycle_crossover(individual * population, int id_1, int id_2, int * child_1, int * child_2){
 
    int parent_1 = id_1, parent_2 = id_2;           //Copiamo gli indici dei genitori
    int j = 0;                                      //Colonna corrente
    int lookup[CITY_COUNT];                         //Vettore di indirizzamento
    int flags[CITY_COUNT] = {0};                    //Vettore delle colonne visitate
    int visited = 0;                                //Numero di colonne visitate
    int first = population[id_1].solution[0], last; //primo e ultimo elemento del ciclo
 
    for (int i = 0; i < CITY_COUNT; i++){
        lookup[population[id_1].solution[i]] = i;   //Questo vettore viene utilizzato per ottenere la posizione di un elemento del vettore id_2 nel vettore id_1, per farlo salviamo in lookup[i] la posizione dell'elemento da cercare
    }
 
    while (visited < CITY_COUNT){
 
        child_1[j] = population[parent_1].solution[j];  //Copiamo le città nei figli
        child_2[j] = population[parent_2].solution[j];
        last = population[id_2].solution[j];
 
        flags[j] = 1;       //Impostiamo la colonna j come visitata
        visited++;
        j = lookup[last];   //Cerchiamo last in id_1
 
        if (first == last){
            scambia(&parent_1, &parent_2);          //Una volta terminato il ciclo scambiamo i genitori in modo da copiare le città al contrario
            for (j = 0; flags[j]; j++);             //Cerchiamo una colonna libera
            first = population[id_1].solution[j];   //Reinizializziamo il primo elemento del ciclo
        }  
    }
}
