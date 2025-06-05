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

    std::pair<bool, std::vector<int>> find_best_improvement(std::vector<int> &solution);

    std::pair<bool, std::vector<int>> find_best_improvement_tolls(const std::vector<int> &solution);

    std::pair<bool, std::vector<int>> tabu_find_best_improvement(
        std::vector<int> &solution,
        std::unordered_map<std::string, int> &tabu_list,
        int tabu, int current_cost, int best_cost_global);

    std::pair<bool, std::vector<int>> tabu_find_best_improvement_tolls(
        const std::vector<int> &solution,
        std::unordered_map<std::string, int> &tabu_list,
        int tabu, int current_cost, int best_cost_global);

    std::vector<int> random_generation();

public:
    Heuristic(
        int solution_size,
        const std::vector<std::vector<int>> &distance,
        const std::vector<std::vector<bool>> &tolls,
        int free_tolls, double tolls_price);

    int evaluate(const std::vector<int> &solution, int free_tolls);

    std::vector<int> nearest_neighbor();

    int local_search(std::vector<int> &solution);

    int grasp();

    int tabu_search();
};

#endif
