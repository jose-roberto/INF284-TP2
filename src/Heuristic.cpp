#include "../include/Heuristic.h"

Heuristic::Heuristic(
    int solution_size,
    const std::vector<std::vector<int>> &distance,
    const std::vector<std::vector<bool>> &tolls,
    int free_tolls,
    double tolls_price)
    : solution_size(solution_size),
      distance(distance),
      tolls(tolls),
      free_tolls(free_tolls), tolls_price(tolls_price) {}

std::pair<bool, std::vector<int>> Heuristic::find_best_improvement(std::vector<int> &solution, std::vector<int> &tabu_list, int tabu)
{
    int solution_size = solution.size();

    int best_i = -1, best_j = -1;
    int best_improvement = 0;

    for (int i = 0; i < solution_size - 1; i++)
        for (int j = i + 1; j < solution_size; j++)
        {
            if (tabu_list[solution[i]] != 0 || tabu_list[solution[j]] != 0)
                continue;

            int prev = solution[(i - 1) % solution_size];
            int first_node = solution[i];
            int last_node = solution[j];
            int after = solution[(j + 1) % solution_size];

            int original_value = distance[prev][first_node] + distance[last_node][after];

            int new_value = distance[prev][last_node] + distance[first_node][after];

            int improvement = new_value - original_value;

            if (improvement < best_improvement)
            {
                best_improvement = improvement;
                best_i = i;
                best_j = j;
            }
        }

    if (best_improvement < 0)
    {
        std::reverse(solution.begin() + best_i,
                     solution.begin() + best_j + 1);

        tabu_list[solution[best_i]] = tabu;
        
        return {true, solution};
    }

    return std::make_pair(false, solution);
}

std::pair<bool, std::vector<int>> Heuristic::find_best_improvement_tolls(const std::vector<int> &solution, std::vector<int> &tabu_list, int tabu)
{
    int best_improvement = 0;

    std::vector<int> best_solution;

    int original_value = evaluate(solution, free_tolls);

    for (int i = 1; i < solution_size - 1; i++)
    {
        for (int j = i + 1; j < solution_size; j++)
        {
            std::vector<int> candidate = solution;
            std::reverse(candidate.begin() + i, candidate.begin() + j + 1);

            int new_value = evaluate(candidate, free_tolls);
            int improvement = new_value - original_value;

            if (improvement < best_improvement)
            {
                best_improvement = improvement;
                best_solution = candidate;
            }
        }
    }

    if (best_improvement < 0)
        return {true, best_solution};

    return {false, solution};
}

int Heuristic::evaluate(const std::vector<int> &solution, int free_tolls)
{
    int size = solution.size();
    int value = 0;

    for (int i = 0; i < size; i++)
    {
        int a = solution[i];
        int b = solution[(i + 1) % size];
        value += distance[a][b];

        if (tolls[a][b])
        {
            if (free_tolls > 0)
                free_tolls--;
            else
                value += tolls_price;
        }
    }

    return value;
}

std::vector<int> Heuristic::nearest_neighbor()
{
    std::vector<bool> visited(solution_size, false);
    visited[0] = true;

    int best_neighbor = -1;

    std::list<int> partial_solution;
    partial_solution.push_back(0);

    for (int i = 0; i < solution_size - 1; i++)
    {
        int minimum_cost = __INT_MAX__;

        for (int j = 1; j < solution_size; j++)
        {
            if (!visited[j] && (distance[partial_solution.back()][j] <= minimum_cost))
            {
                minimum_cost = distance[partial_solution.back()][j];
                best_neighbor = j;
            }
        }

        visited[best_neighbor] = true;

        partial_solution.push_back(best_neighbor);
    }

    return std::vector<int>(partial_solution.begin(), partial_solution.end());
}

int Heuristic::tabu_search()
{

    std::vector<int> solution = nearest_neighbor();

    int tabu = (solution_size * 20)/100;
    std::vector<int> tabu_list(tabu, 0);

    while (true)
    {
        if (free_tolls == solution_size)
            find_best_improvement(solution, tabu_list, tabu);
        else
            find_best_improvement_tolls(solution, tabu_list, tabu);
    }

    return 0;
}
