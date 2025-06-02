#ifndef HEURISTIC_H
#define HEURISTIC_H

#include <algorithm>
#include <list>
#include <unordered_map>
#include <vector>
#include <string>

class Heuristic
{
private:
    int solution_size;
    std::vector<std::vector<int>> distance;
    std::vector<std::vector<bool>> tolls;
    int free_tolls;
    double tolls_price;

    // Construir vizinhança com 2-opt
    std::pair<bool, std::vector<int>> find_best_improvement(std::vector<int> &solution, std::unordered_map<std::string, int> &tabu_list, int tabu);

    // Construir vizinhança com 2-opt
    std::pair<bool, std::vector<int>> find_best_improvement_tolls(const std::vector<int> &solution, std::unordered_map<std::string, int> &tabu_list, int tabu);

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

    int aco();
};

#endif
