#include "../include/Heuristic.h"

#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>

int main()
{
    std::vector<std::string> paths = {
        "instances/burma14.tsp",
        "instances/berlin52.tsp",
        "instances/st70.tsp",
        "instances/gil262.tsp",
        "instances/gr666.tsp",
        "instances/dsj1000.tsp",
        "instances/t1_burma14.tsp",
        "instances/t2_burma14.tsp",
        "instances/t3_burma14.tsp",
        "instances/t1_berlin52.tsp",
        "instances/t2_berlin52.tsp",
        "instances/t3_berlin52.tsp",
        "instances/t1_st70.tsp",
        "instances/t2_st70.tsp",
        "instances/t3_st70.tsp",
        "instances/t4_st70.tsp",
        "instances/t1_gil262.tsp",
        "instances/t2_gil262.tsp"
    };

    for (const auto &path : paths)
    {
        std::cout << "-------------------------------------" << std::endl;
        std::cout << "Instância: " << path << "\n"
                  << std::endl;

        std::ifstream file(path);
        if (!file)
        {
            std::cerr << path << std::endl;
            std::cerr << "Erro ao abrir o arquivo!" << std::endl;
            return 1;
        }

        std::string line;
        int dimension;
        std::string edgeWeightType;
        int free_tolls, tolls_price, threshold;

        while (std::getline(file, line))
        {
            if (line.find("DIMENSION") != std::string::npos)
            {
                std::stringstream ss(line);
                std::string keyword;

                ss >> keyword >> dimension;
            }
            else if (line.find("EDGE_WEIGHT_TYPE") != std::string::npos)
            {
                std::stringstream ss(line);
                std::string keyword;

                ss >> keyword >> edgeWeightType;
            }
            else if (line.find("FREE_TOLLS") != std::string::npos)
            {
                std::stringstream ss(line);
                std::string keyword;

                ss >> keyword >> free_tolls;
            }
            else if (line.find("TOLLS_PRICE") != std::string::npos)
            {
                std::stringstream ss(line);
                std::string keyword;

                ss >> keyword >> tolls_price;
            }
            else if (line.find("THRESHOLD") != std::string::npos)
            {
                std::stringstream ss(line);
                std::string keyword;

                ss >> keyword >> threshold;
            }
            else if (line.find("NODE_COORD_SECTION") != std::string::npos)
                break;
        }

        std::vector<std::pair<double, double>> nodes;
        while (std::getline(file, line))
        {
            if (line.find("EOF") != std::string::npos)
                break;

            std::stringstream ss(line);
            int id;
            double x, y;

            ss >> id >> x >> y;

            nodes.push_back({x, y});
        }

        std::vector<std::vector<int>> distance(dimension, std::vector<int>(dimension, 0));

        for (int i = 0; i < dimension; i++)
            for (int j = i + 1; j < dimension; j++)
            {
                double i_x = nodes[i].first;
                double j_x = nodes[j].first;
                double i_y = nodes[i].second;
                double j_y = nodes[j].second;

                int distance_ij = std::round(std::sqrt((j_x - i_x) * (j_x - i_x) + (j_y - i_y) * (j_y - i_y)));

                distance[i][j] = distance_ij;
                distance[j][i] = distance_ij;
            }

        std::vector<std::vector<bool>> tolls(dimension, std::vector<bool>(dimension, 0));

        for (int i = 0; i < dimension; i++)
            for (int j = i + 1; j < dimension; j++)
                if (distance[i][j] <= threshold)
                {
                    tolls[i][j] = true;
                    tolls[j][i] = true;
                }

        int start = -1;
        std::vector<int> initial_solution_1(dimension);
        std::generate(initial_solution_1.begin(), initial_solution_1.end(), [&start]()
                      { start += 1; return start; });

        start = -2;
        std::vector<int> initial_solution_2(dimension);
        std::generate(initial_solution_2.begin(), initial_solution_2.begin() + (initial_solution_2.size() / 2), [&start]()
                      { start += 2; return start; });
        start = -1;
        std::generate(initial_solution_2.begin() + (initial_solution_2.size() / 2), initial_solution_2.end(), [&start]()
                      { start += 2; return start; });

        file.close();

        Heuristic heuristic(
            dimension,
            distance,
            tolls,
            free_tolls,
            tolls_price);

        // std::cout << "Grasp: " << heuristic.grasp() << std::endl;

        std::cout << "Tabu Search: " << heuristic.tabu_search() << std::endl;

        std::cout << "-------------------------------------" << std::endl;

        std::cout << std::endl;
    }

    return 0;
}
