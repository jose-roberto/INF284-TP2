#ifndef HEURISTIC_H
#define HEURISTIC_H

#include <iostream>
#include <algorithm>
#include <list>
#include <unordered_map>
#include <vector>
#include <string>
#include <unordered_set>

class Heuristic
{
private:
    int solution_size;
    std::vector<std::vector<int>> distance;
    std::vector<std::vector<bool>> tolls;
    int free_tolls;
    double tolls_price;

    // Construir vizinhança com 2-opt
    std::pair<bool, std::vector<int>> find_best_improvement(std::vector<int> &solution);

    // Construir vizinhança com 2-opt
    std::pair<bool, std::vector<int>> find_best_improvement_tolls(const std::vector<int> &solution);

    std::vector<int> random_generation();

    void ordered_crossover(std::vector<int> &population_a, std::vector<int> &population_b);

    void mutation(std::vector<std::vector<int>> &population, int mutation_rate);

public:
    Heuristic(
        int solution_size,
        const std::vector<std::vector<int>> &distance,
        const std::vector<std::vector<bool>> &tolls,
        int free_tolls, double tolls_price);

    int evaluate(const std::vector<int> &solution, int free_tolls);

    // Construir solução inicial
    std::vector<int> nearest_neighbor();

    int tabu_search();
    // Provavelmente o "tabu" será aplicado sobre as cidades, mas talvez será sobre os movimentos

    int genetic_algorithm();

    int grasp();

    int local_search(std::vector<int> &solution);
};

#endif
