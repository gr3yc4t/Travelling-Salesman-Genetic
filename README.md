# Travelling-Salesman-Genetic
A small genetic algorithm developed in C with the objective of solving the Travelling Salesman Problem.
The `genetic.c` file contains some explanation of how the program works.
The `progetto_algoritmi.pdf` file contains a detailed explanation of the code, the algorithms used and an analisys of the spatial and time complexity (in italian).

## Compiling and usage
To compile and execute, use the commands:
```
gcc -o genetic.exe genetic.c -lm
./genetic.exe
```
If DEBUG is disabled, you will see only a couple of generations with the scores.
To draw the result of the execution, after the program ended use processing to run `graphic.pde`.
This script will replicate the various generations and show graphically how the generations improved.

## Possible improvements
* Choosing better hyper parameters, for a given problem, can reduce the required number of iterations and avoid local minimums
* Changing the cycle crossover (CX) to a better crossover function. The CX is one of the slowest crossovers for genetic algorithms (as showed [here](https://arxiv.org/ftp/arxiv/papers/1203/1203.3097.pdf), but it's the easiest to implement. Other crossovers also have a better complexity.
* Changing the way in which the population is extracted for the reproduction can improve a lot when it comes to execution times. Extracting an index from the repartition array takes O(n) time, but using something like the [Vose's Alias Method](http://www.keithschwarz.com/darts-dice-coins/) can reduce this time to O(1). This extraction is done for every member of the population array, so this improvement would make the program a lot faster, allowing for more iterations.
* Many other small improvements can be made, here only the most important ones were explained.

## Developers
This program was the final project of the course "Algorithms and data structures" at Università Politecnica delle Marche (A.Y. 2016-2017) and was developed by Alessandro Cingolani, Giacomo Astolfi, Orazio Edoardo, Cristian Federiconi, Federica Massacci and Luca Luzi.
