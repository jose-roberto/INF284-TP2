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

// std::pair<bool, std::vector<int>> Heuristic::find_best_improvement(std::vector<int> &solution, std::unordered_map<std::string, int> &tabu_list, int tabu)
// {
//     int solution_size = solution.size();

//     int best_i = -1, best_j = -1;
//     int best_improvement = 0;

//     for (int i = 0; i < solution_size - 1; i++)
//         for (int j = i + 1; j < solution_size; j++)
//         {
//             int prev = solution[(i - 1) % solution_size];
//             int first_node = solution[i];
//             int last_node = solution[j];
//             int after = solution[(j + 1) % solution_size];

//             std::string key = first_node + " " + last_node;
//             if (tabu_list.find(key) != tabu_list.end())
//                 continue;

//             int original_value = distance[prev][first_node] + distance[last_node][after];

//             int new_value = distance[prev][last_node] + distance[first_node][after];

//             int improvement = new_value - original_value;

//             if (improvement < best_improvement)
//             {
//                 best_improvement = improvement;
//                 best_i = i;
//                 best_j = j;
//             }
//         }

//     if (best_improvement < 0)
//     {
//         std::reverse(solution.begin() + best_i,
//                      solution.begin() + best_j + 1);

//         if (tabu_list.size() < tabu)
//         {
//             std::string key = solution[best_i] + " " + solution[best_j];
//             tabu_list[key] = tabu;
//         }

//         return {true, solution};
//     }

//     return std::make_pair(false, solution);
// }

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

// std::pair<bool, std::vector<int>>
// Heuristic::find_best_improvement(std::vector<int> &solution,
//                                  std::unordered_map<std::string, int> &tabu_list,
//                                  int tabu_tenure)
// {
//     int n = solution_size;

//     int best_i = -1, best_j = -1;
//     // Queremos um delta negativo (pois “improvement” = custo_novo – custo_antigo).
//     int best_delta = 0;

//     // Percorremos todas as trocas i<j (2-opt) na rota “solution”
//     for (int i = 0; i < n - 1; ++i)
//     {
//         for (int j = i + 1; j < n; ++j)
//         {
//             // Índice “prev” e “after” com wrap-around seguro:
//             int prev = solution[(i - 1 + n) % n];
//             int first_node = solution[i];
//             int last_node = solution[j];
//             int after = solution[(j + 1) % n];

//             // Monta a chave “first_node‐last_node” para o tabu-list
//             std::string key = std::to_string(first_node) + "-" + std::to_string(last_node);

//             // Se está tabu, pula (não avalia esse movimento)
//             if (tabu_list.find(key) != tabu_list.end())
//                 continue;

//             // Custo atual = aresta(prev→first_node) + aresta(last_node→after)
//             int custo_original = distance[prev][first_node] + distance[last_node][after];
//             // Custo se trocarmos: aresta(prev→last_node) + aresta(first_node→after)
//             int custo_novo = distance[prev][last_node] + distance[first_node][after];
//             int delta = custo_novo - custo_original;

//             // Se encontramos um delta melhor (mais negativo), armazenamos i,j
//             if (delta < best_delta)
//             {
//                 best_delta = delta;
//                 best_i = i;
//                 best_j = j;
//             }
//         }
//     }

//     // Se achamos alguma melhoria (best_delta < 0), aplicamos o 2-opt:
//     if (best_delta < 0)
//     {
//         // Inverte o trecho de i até j:
//         std::reverse(solution.begin() + best_i, solution.begin() + best_j + 1);

//         // Insere no tabu-list a aresta recém-criada (first_node→last_node) com tenure
//         // Recalcule quais são os nós na nova solução, pois o trecho já foi invertido:
//         int new_first = solution[best_i];
//         int new_last = solution[best_j];
//         std::string new_key = std::to_string(new_first) + "-" + std::to_string(new_last);

//         tabu_list[new_key] = tabu_tenure;
//         return {true, solution};
//     }

//     // Se não encontrou nenhuma troca válida, devolve false e mantém “solution” intacta
//     return {false, solution};
// }

// std::pair<bool, std::vector<int>> Heuristic::find_best_improvement_tolls(const std::vector<int> &solution, std::unordered_map<std::string, int> &tabu_list, int tabu)
// {
//     int best_improvement = 0;

//     std::vector<int> best_solution;

//     int original_value = evaluate(solution, free_tolls);

//     for (int i = 1; i < solution_size - 1; i++)
//     {
//         for (int j = i + 1; j < solution_size; j++)
//         {
//             std::vector<int> candidate = solution;
//             std::reverse(candidate.begin() + i, candidate.begin() + j + 1);

//             int new_value = evaluate(candidate, free_tolls);
//             int improvement = new_value - original_value;

//             if (improvement < best_improvement)
//             {
//                 best_improvement = improvement;
//                 best_solution = candidate;
//             }
//         }
//     }

//     if (best_improvement < 0)
//         return {true, best_solution};

//     return {false, solution};
// }

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
        int minimum_cost = INT_MAX;

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

std::vector<int> Heuristic::random_generation()
{
    std::vector<int> sol(solution_size);
    std::iota(sol.begin(), sol.end(), 0);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(sol.begin(), sol.end(), gen);

    return sol;
}

// int Heuristic::tabu_search()
// {
//     int max_restarts = 100;
//     int max_iters_per_restart = 100;

//     // Número de moves tabu permitidos simultaneamente (20% de n, por exemplo)
//     int tabu_tenure = (solution_size * 20) / 100;
//     if (tabu_tenure < 1)
//         tabu_tenure = 1;

//     std::vector<int> best_solution_global;
//     int best_cost_global = std::numeric_limits<int>::max();

//     // ========= Loop de “reinicializações” aleatórias =========
//     for (int restart = 0; restart < max_restarts; ++restart)
//     {
//         // 1) Cria uma solução inicial aleatória
//         std::vector<int> current_solution = random_generation();
//         int current_cost = evaluate(current_solution, free_tolls);

//         // 2) Inicializa tabu_list vazio (map: key → tempo restante)
//         std::unordered_map<std::string, int> tabu_list;

//         // 3) Se for a primeira reinicialização, atualiza best_global
//         if (restart == 0 || current_cost < best_cost_global)
//         {
//             best_solution_global = current_solution;
//             best_cost_global = current_cost;
//         }

//         // ========== Loop de iterações internas ==========
//         for (int iter = 0; iter < max_iters_per_restart; ++iter)
//         {
//             // 3.1) Decrementa todos os tempos do tabu_list. Remove os que cheguem a zero.
//             std::vector<std::string> to_erase;
//             for (auto &par : tabu_list)
//             {
//                 par.second -= 1;
//                 if (par.second <= 0)
//                     to_erase.push_back(par.first);
//             }
//             for (const auto &k : to_erase)
//                 tabu_list.erase(k);

//             // 3.2) Busca a melhor troca 2-opt não-tabu para “current_solution”
//             auto [move_found, new_sol] = find_best_improvement(current_solution,
//                                                                tabu_list,
//                                                                tabu_tenure);
//             if (!move_found)
//             {
//                 // Se não há mais movimentos não-tabu que melhorem, finaliza este restart
//                 break;
//             }

//             // Se encontrou move, atualiza current_solution + custo
//             current_solution = new_sol;
//             current_cost = evaluate(current_solution, free_tolls);

//             // 3.3) Testa se é melhor que o global
//             if (current_cost < best_cost_global)
//             {
//                 best_cost_global = current_cost;
//                 best_solution_global = current_solution;
//             }
//         } // fim loop iterações

//         // Após terminar iterações deste restart, a próxima reinicialização irá zerar tabu_list e gerar nova aleatória
//     } // fim loop restarts

//     // No final, best_cost_global contém o melhor custo encontrado
//     // Se quiser, podemos imprimir a rota:
//     // std::cout << "Melhor custo = " << best_cost_global << "\n";
//     // for (int x : best_solution_global) std::cout << x << " ";
//     // std::cout << "\n";

//     return best_cost_global;
// }

void Heuristic::ordered_crossover(std::vector<int> &population_a, std::vector<int> &population_b)
{
    int crossover_size = ((solution_size * 40) / 100);

    std::unordered_set<int> dna_a, dna_b;
    dna_a.insert(population_a.begin() + crossover_size - 1, population_a.begin() + (2 * crossover_size) - 1);
    dna_b.insert(population_b.begin() + crossover_size - 1, population_b.begin() + (2 * crossover_size) - 1);

    std::vector<int> remaining_dna_a, remaining_dna_b;
    remaining_dna_a = population_a;
    remaining_dna_b = population_b;

    int aux_a = 0;
    for (int i = 0; i < solution_size; i++)
    {
        if (i >= crossover_size - 1 && i < crossover_size * 2 - 1)
            continue;

        while (dna_a.find(remaining_dna_b[aux_a]) != dna_a.end())
            aux_a++;

        population_a[i] = remaining_dna_b[aux_a];
        aux_a++;
    }

    int aux_b = 0;
    for (int i = 0; i < solution_size; i++)
    {
        if (i >= crossover_size - 1 && i < crossover_size * 2 - 1)
            continue;

        while (dna_b.find(remaining_dna_a[aux_b]) != dna_b.end())
            aux_b++;

        population_b[i] = remaining_dna_a[aux_b];
        aux_b++;
    }
}

void Heuristic::mutation(std::vector<std::vector<int>> &population, int n_mutations)
{
    std::vector<int> randomly_generated = random_generation();
    std::vector<int> targets(randomly_generated.begin(), randomly_generated.begin() + n_mutations);

    for (int i = 0; i < n_mutations; i++)
    {
        std::vector<int> solution_target = population[targets[i]];

        std::vector<int> randomly_generated = random_generation();

        int a = randomly_generated[0];
        int b = randomly_generated[1];

        std::swap(solution_target[a], solution_target[b]);
        population[targets[i]] = solution_target;
    }
}

int Heuristic::genetic_algorithm()
{
    int population_size = (solution_size * 200) / 100;

    std::vector<std::vector<int>> population;

    for (int i = 0; i < population_size; i++)
        population.push_back(random_generation());

    int generations = (population_size * 500) / 100;


    while (generations > 0)
    {
        for (int i = 0; i < population_size; i += 2)
            ordered_crossover(population[i], population[i + 1]);

        int n_mutations = (population_size * 5) / 100;

        mutation(population, n_mutations);

        generations--;
    }

    return 0;
}

int Heuristic::grasp() {
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> startDist(0, solution_size - 1);
    int start = startDist(rng);

    std::vector<bool> visited(solution_size, false);
    visited[start] = true;

    std::list<int> partial_solution;
    partial_solution.push_back(start);

    for (int i = 0; i < solution_size - 1; i++) {
        int current = partial_solution.back();

        int lb_cost = INT_MAX, up_cost = 0;
        for (int j = 0; j < solution_size; j++) {
            if (!visited[j]) {
                int d = distance[current][j];
                if (d < lb_cost) lb_cost = d;
                if (d > up_cost)  up_cost = d;
            }
        }

        int limit = lb_cost + (int)std::lround(0.4 * (up_cost - lb_cost));

        std::vector<int> rcl;
        for (int j = 0; j < solution_size; j++) {
            if (!visited[j] && distance[current][j] <= limit) {
                rcl.push_back(j);
            }
        }

        std::uniform_int_distribution<int> dist(0, (int)rcl.size() - 1);
        int best_neighbor = rcl[ dist(rng) ];

        visited[best_neighbor] = true;
        partial_solution.push_back(best_neighbor);
    }

    std::vector<int> sol(partial_solution.begin(), partial_solution.end());
    return local_search(sol);
}
