#include "../include/Heuristic.h"
#include <random>
#include <ctime>
#include <numeric>
#include <climits>

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

int Heuristic::local_search(std::vector<int> &solution)
{
    while (true)
    {
        bool improved = false;
        std::vector<int> new_solution;

        auto result = (solution_size == free_tolls)
                          ? find_best_improvement(solution)
                          : find_best_improvement_tolls(solution);

        improved = result.first;
        new_solution = result.second;

        if (!improved)
            break;

        solution = new_solution;
    }

    return evaluate(solution, free_tolls);
}

std::vector<int> Heuristic::random_generation()
{
    std::vector<int> sol(solution_size);
    std::iota(sol.begin(), sol.end(), 0);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(sol.begin(), sol.end(), gen);

    return sol;
}

std::pair<bool, std::vector<int>> Heuristic::find_best_improvement(std::vector<int> &solution)
{
    int solution_size = solution.size();

    int best_i = -1, best_j = -1;
    int best_improvement = 0;

    for (int i = 1; i < solution_size - 1; i++)
        for (int j = i + 1; j < solution_size; j++)
        {
            int prev = solution[i - 1];
            int first_node = solution[i];
            int last_node = solution[j];
            int next_j = (j + 1) % solution_size;
            int after = solution[next_j];

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

        return {true, solution};
    }

    return std::make_pair(false, solution);
}

std::pair<bool, std::vector<int>> Heuristic::find_best_improvement_tolls(const std::vector<int> &solution)
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

std::pair<bool, std::vector<int>> Heuristic::tabu_find_best_improvement(
    std::vector<int> &solution,
    std::unordered_map<std::string, int> &tabu_list,
    int tabu,
    int current_cost,
    int best_cost_global)
{
    int n = solution_size;

    int best_i = -1, best_j = -1;
    int best_improvement = 0;

    for (int i = 0; i < n - 1; ++i)
        for (int j = i + 1; j < n; ++j)
        {
            int prev = solution[(i - 1 + n) % n];
            int first_node = solution[i];
            int last_node = solution[j];
            int after = solution[(j + 1) % n];

            std::string key = std::to_string(first_node) + "-" + std::to_string(last_node);

            int custo_original = distance[prev][first_node] + distance[last_node][after];

            int custo_novo = distance[prev][last_node] + distance[first_node][after];

            int improvement = custo_novo - custo_original;

            // se este arco (first_node→last_node) estiver tabu, só o pulamos se
            // ele não melhorar o melhor global:
            auto it = tabu_list.find(key);
            if (it != tabu_list.end())
            {
                int candidate_cost = current_cost + improvement;
                if (candidate_cost >= best_cost_global)
                {
                    continue;
                }
                // caso contrário (candidate_cost < best_cost_global),
                // deixamos “cair” e aceitamos normalmente este movimento tabu (aspiração)
            }

            if (improvement < best_improvement)
            {
                best_improvement = improvement;
                best_i = i;
                best_j = j;
            }
        }

    // Se achamos alguma melhoria (best_improvement < 0), aplicamos o 2-opt:
    if (best_improvement < 0)
    {
        std::reverse(solution.begin() + best_i, solution.begin() + best_j + 1);

        // Insere no tabu-list a aresta recém-criada (first_node→last_node) com tenure
        // Recalcule quais são os nós na nova solução, pois o trecho já foi invertido:
        int new_first = solution[best_i];
        int new_last = solution[best_j];

        std::string new_key = std::to_string(new_first) + "-" + std::to_string(new_last);

        tabu_list[new_key] = tabu;

        return {true, solution};
    }

    return {false, solution};
}

std::pair<bool, std::vector<int>> Heuristic::tabu_find_best_improvement_tolls(
    const std::vector<int> &solution,
    std::unordered_map<std::string, int> &tabu_list,
    int tabu,
    int current_cost,
    int best_cost_global)
{
    int best_improvement = 0;

    std::vector<int> best_solution;

    int original_value = evaluate(solution, free_tolls);

    for (int i = 1; i < solution_size - 1; i++)
    {
        for (int j = i + 1; j < solution_size; j++)
        {
            std::string key = std::to_string(solution[i]) + "-" + std::to_string(solution[j]);

            std::vector<int> candidate = solution;
            std::reverse(candidate.begin() + i, candidate.begin() + j + 1);

            int new_value = evaluate(candidate, free_tolls);
            int improvement = new_value - original_value;

            // se este arco (first_node→last_node) estiver tabu, só o pulamos se
            // ele não melhorar o melhor global:
            auto it = tabu_list.find(key);
            if (it != tabu_list.end())
            {
                int candidate_cost = current_cost + improvement;
                if (candidate_cost >= best_cost_global)
                {
                    continue;
                }
                // caso contrário (candidate_cost < best_cost_global),
                // deixamos “cair” e aceitamos normalmente este movimento tabu (aspiração)
            }

            if (improvement < best_improvement)
            {
                best_improvement = improvement;
                best_solution = candidate;

                // Insere no tabu-list a aresta recém-criada (first_node→last_node) com tenure
                // Recalcule quais são os nós na nova solução, pois o trecho já foi invertido:
                int new_first = solution[i];
                int new_last = solution[j];

                std::string new_key = std::to_string(new_first) + "-" + std::to_string(new_last);

                tabu_list[new_key] = tabu;
            }
        }
    }

    if (best_improvement < 0)
        return {true, best_solution};

    return {false, solution};
}

int Heuristic::grasp()
{
    int max_iter = solution_size / 10, iter = 0;
    int best_solution = INT_MAX;

    while (iter < max_iter)
    {
        std::mt19937 rng(std::random_device{}());
        std::uniform_int_distribution<int> startDist(0, solution_size - 1);
        int start = startDist(rng);

        std::vector<bool> visited(solution_size, false);
        visited[start] = true;

        std::list<int> partial_solution;
        partial_solution.push_back(start);

        for (int i = 0; i < solution_size - 1; i++)
        {
            int current = partial_solution.back();

            int lb_cost = INT_MAX, up_cost = 0;
            for (int j = 0; j < solution_size; j++)
            {
                if (!visited[j])
                {
                    int d = distance[current][j];
                    if (d < lb_cost)
                        lb_cost = d;
                    if (d > up_cost)
                        up_cost = d;
                }
            }

            int limit = lb_cost + (int)std::lround(0.4 * (up_cost - lb_cost));

            std::vector<int> rcl;
            for (int j = 0; j < solution_size; j++)
            {
                if (!visited[j] && distance[current][j] <= limit)
                {
                    rcl.push_back(j);
                }
            }

            std::uniform_int_distribution<int> dist(0, (int)rcl.size() - 1);
            int best_neighbor = rcl[dist(rng)];

            visited[best_neighbor] = true;
            partial_solution.push_back(best_neighbor);
        }

        std::vector<int> sol(partial_solution.begin(), partial_solution.end());
        int result = local_search(sol);
        if (result < best_solution)
            best_solution = result;

        iter++;
    }

    return best_solution;
}

int Heuristic::tabu_search()
{
    int max_restarts = 20;
    int max_iters_per_restart = 200;

    int tabu = (solution_size * 20) / 100;

    if (tabu < 1)
        tabu = 1;

    std::vector<int> best_solution_global;
    int best_cost_global = std::numeric_limits<int>::max();

    // ========= Loop de “reinicializações” aleatórias =========
    for (int restart = 0; restart < max_restarts; ++restart)
    {
        // 1) Cria uma solução inicial aleatória
        std::vector<int> current_solution = random_generation();
        int current_cost = evaluate(current_solution, free_tolls);

        // 2) Inicializa tabu_list vazio (map: key → tempo restante)
        std::unordered_map<std::string, int> tabu_list;

        // 3) Se for a primeira reinicialização, atualiza best_global
        if (restart == 0 || current_cost < best_cost_global)
        {
            best_solution_global = current_solution;
            best_cost_global = current_cost;
        }

        // ========== Loop de iterações internas ==========
        for (int iter = 0; iter < max_iters_per_restart; ++iter)
        {
            // 3.1) Decrementa todos os tempos do tabu_list. Remove os que cheguem a zero.
            std::vector<std::string> to_erase;
            for (auto &par : tabu_list)
            {
                par.second -= 1;
                if (par.second <= 0)
                    to_erase.push_back(par.first);
            }
            for (const auto &k : to_erase)
                tabu_list.erase(k);

            // 3.2) Busca a melhor troca 2-opt não-tabu para “current_solution”
            auto [move_found, new_sol] = (solution_size == free_tolls)
                                             ? tabu_find_best_improvement(current_solution, tabu_list, tabu, current_cost, best_cost_global)
                                             : tabu_find_best_improvement_tolls(current_solution, tabu_list, tabu, current_cost, best_cost_global);

            if (!move_found)
            {
                // Se não há mais movimentos não-tabu que melhorem, finaliza este restart
                break;
            }

            // Se encontrou move, atualiza current_solution + custo
            current_solution = new_sol;
            current_cost = evaluate(current_solution, free_tolls);

            // 3.3) Testa se é melhor que o global
            if (current_cost < best_cost_global)
            {
                best_cost_global = current_cost;
                best_solution_global = current_solution;
            }
        } // fim loop iterações

        // Após terminar iterações deste restart, a próxima reinicialização irá zerar tabu_list e gerar nova aleatória
    } // fim loop restarts

    return best_cost_global;
}
