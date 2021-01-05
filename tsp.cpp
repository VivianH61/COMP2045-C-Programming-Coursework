#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <limits>
#include <cmath>
#include<ctime>
#include<algorithm>
using namespace std;

struct City
{
    int index; //0,1,2,3...
    int id;    //number representing different city in the file
    int x;
    int y;
};

struct Solution
{
    int size;
    float distance_sum = numeric_limits<float>::max();
    vector<City> route;
    //int *travelled = (int *)malloc(sizeof(int) * size); //indicating whether the man has travelled to the city or not, (in the order of index)
    int *travelled;
    Solution(int prob_size) {
        size = prob_size;
        travelled = (int *)malloc(sizeof(int) * size);
    }
    
    ~Solution()
    {
        if (travelled) {
            free(travelled);
        }
    }
};

struct Problem
{
    int size;
    vector<City> cities;
    Solution *best_sln;
    Problem(int input_size, vector<City> input_cities) {
        size = input_size;
        cities = input_cities;
        best_sln = new Solution(input_size);
    }
    ~Problem() {
        delete best_sln;
    }
};

struct less_than_distance_sum
{
    inline bool operator() (Solution* sln1, Solution* sln2)
    {
        return (sln1->distance_sum < sln2->distance_sum);
    }
};


class search_strategy
{
protected:
    Problem * prob;
    void init_prob_from_file(const string &filename);

public:
    float rand_01();
    int rand_int(int min, int max);
    int find_next_city(City cur_city, int* travelled);
    float dist_two_cities(City city1, City city2);
    Solution *greedy_heuristic_search();
    Solution *gen_rand_sln();
    vector<Solution*> roulette_wheel_selection(vector<Solution*> population);
    void partially_mapped_crossover(vector<Solution*> mating_pool);
    void sort_by_dist(vector<Solution*> population);
    void local_search_first_descent(Solution* curt_sln);
    void memetic_algorithm(int max_time);
    void evaluate_solution(Solution* sln);
    int update_best_sln(Solution* curt_sln);
    Solution get_best_sln();
    int get_city_number();
    

    // constructor
    search_strategy(const string &filename) {
        init_prob_from_file(filename);
    }
    // destructor
    ~search_strategy() {
        delete prob;
    }
};

//return a random number between 0 and 1
float search_strategy :: rand_01()
{
    float number;
    number = (float)rand();
    number = number / RAND_MAX;
    return number;
}

//return a random nunber ranging from min to max (inclusive)
int search_strategy :: rand_int(int min, int max)
{
    int div = max - min + 1;
    int val = rand() % div + min;
    return val;
}

void search_strategy :: init_prob_from_file(const string &filename)
{
    int city_count = 0;
    int tmp;

    fstream inFile(filename, std::ios_base::in);
    //skip first line of the file
    string first_line;
    getline(inFile, first_line);
    vector<City> input_cities;

    while (!inFile.eof())
    {
        input_cities.push_back(City());
        inFile >> input_cities[city_count].id;
        inFile >> input_cities[city_count].x;
        inFile >> input_cities[city_count].y;
        input_cities[city_count].index = city_count;
        city_count++;
    }
    prob = new Problem(city_count, input_cities);
    
    inFile.close();
}

int search_strategy :: find_next_city(City cur_city, int* travelled)
{
    int min_index = 0;
    float min_dist = numeric_limits<float>::max();
    float cur_dist;
    int x1, x2, y1, y2;
    x2 = cur_city.x;
    y2 = cur_city.y;
    for (int i = 0; i < prob->size; i++)
    {
        if (travelled[i] == 1) {
            continue;
        }
        x1 = prob->cities[i].x;
        y1 = prob->cities[i].y;
        cur_dist = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2) * 1.0);
        //cout << "cur_dist" << cur_dist << endl;
        if (cur_dist < min_dist) {
            min_index = i;
            min_dist = cur_dist;
        }
        
    }
    return min_index;
}

float search_strategy :: dist_two_cities(City city1, City city2) {
    return sqrt(pow(city1.x - city2.x, 2) + pow(city1.y - city2.y, 2) * 1.0);
}

Solution *search_strategy :: greedy_heuristic_search()
{
    Solution *sln = new Solution(prob->size);
    for (int i = 0; i < prob->size; i++)
    {
        sln->travelled[i] = 0;
    }
    sln->size = prob->size;
    int cur_city_index = 0;
    
    for (int i = 0; i < prob->size; i++)
    {
        sln->travelled[cur_city_index] = 1;
        sln->route.push_back(prob->cities[cur_city_index]);
        cur_city_index = find_next_city(prob->cities[cur_city_index], sln->travelled);
    }
    sln->route.push_back(prob->cities[0]);
    evaluate_solution(sln);

    return sln;
}

// randomly generate a feasible route
Solution *search_strategy :: gen_rand_sln() {
    clock_t cur_time = clock();
    srand(cur_time);
    Solution *sln = new Solution(prob->size);
    // vector<City> init_cities(prob->cities);
    
    // sln->route = init_cities;
    // init_cities.clear();
    sln->route = prob->cities;
    sln->route.push_back(prob->cities[0]);

    int iter = prob->size / 2;
    
    int city1, city2;
    //randomly generate two index and swap them
    for (int i = 0; i < iter; i++) {
        city1 = rand_int(1, prob->size-1);
        city2 = rand_int(1, prob->size-1);
        City tmp = sln->route[city1];
        
        sln->route[city1] = sln->route[city2];
        sln->route[city2] = tmp;
        //swap_two_cities(sln->route[city1], sln->route[city2]);
    }
    evaluate_solution(sln);

    // std::vector<City>::iterator it;
    // for(it = prob->cities.begin(); it != prob->cities.end(); it++){
    //     cout << it->id  << "\t";
    // }
    
    return sln;
}

void search_strategy :: local_search_first_descent(Solution* curt_sln) {
    vector<City> route = curt_sln->route;
    float sub_dist1;
    float sub_dist2;
    //int start_point = rand_int(1, curt_sln->size - 1);
    for (int city1 = 0; city1 < curt_sln->size; city1 ++) {
        for (int city2 = city1 + 1; city2 < curt_sln->size; city2 ++) {
            if (city2 == city1 + 1) {
                sub_dist1 = dist_two_cities(route[city1 - 1], route[city1]) + dist_two_cities(route[city2 + 1], route[city2]);
                sub_dist2 = dist_two_cities(route[city1 - 1], route[city2]) + dist_two_cities(route[city2 + 1], route[city1]);
                
            } else {
                sub_dist1 = dist_two_cities(route[city1-1], route[city1]) + dist_two_cities(route[city1+1], route[city1]) + dist_two_cities(route[city2-1], route[city2]) + dist_two_cities(route[city2+1], route[city2]);
                sub_dist2 = dist_two_cities(route[city1-1], route[city2]) + dist_two_cities(route[city1+1], route[city2]) + dist_two_cities(route[city2-1], route[city1]) + dist_two_cities(route[city2+1], route[city1]);
            
            }
            
            if (sub_dist1 > sub_dist2) {
                City tmp = curt_sln->route[city1];
                curt_sln->route[city1] = curt_sln->route[city2];
                curt_sln->route[city2] = tmp;
                
                
                curt_sln->distance_sum = curt_sln->distance_sum - sub_dist1 + sub_dist2;
                evaluate_solution(curt_sln);
                return;
            }
        }
    }
}

// return the index of the selected solution
vector<Solution*> search_strategy :: roulette_wheel_selection(vector<Solution*> population) {
    float prob;
    float sumup_distance = 0;
    vector<Solution*> mating_pool;
    for (int i = 0; i < population.size(); i++)
    {
        sumup_distance += population[i]->distance_sum;
    }
    
    float acc_prob = 0;
    for (int i = 0; i < population.size(); i++) {
        prob = rand_01();
        for (int j = 0; j < population.size(); j ++) {
            acc_prob += (population[j]->distance_sum);
            if (prob <= acc_prob) {
                mating_pool.push_back(population[j]);
            }
        }
    }
    
    return mating_pool;
}

// 这里也许还需要加上free
// return 1 if current solution is better than the previous best_sln and update the best_sln, 0 otherwise
int search_strategy :: update_best_sln(Solution * curt_sln) {
    if (curt_sln->distance_sum < prob->best_sln->distance_sum) {
        prob->best_sln->route = curt_sln->route;
        prob->best_sln->distance_sum = curt_sln->distance_sum;
        return 1;
    }
    return 0;
}

void search_strategy :: memetic_algorithm(int max_time) {
    clock_t start_time = clock();
    clock_t end_time = clock();
    int pop_size = 50;
    int max_iter = 10000;
    float greedy_sln_ratio = 0.1;
    
    vector<Solution*> population;
    vector<Solution*> mating_pool;
    for (int i = 0; i < pop_size; i ++) {
        if (rand_01() < greedy_sln_ratio) {
            population.push_back(greedy_heuristic_search());
        } else {
            population.push_back(gen_rand_sln());
        }
    }

    for (int i = 0; i < max_iter; i ++) {
        mating_pool = roulette_wheel_selection(population);



        for (int i = 0; i < pop_size; i ++) {
            local_search_first_descent(mating_pool[i]);
        }
        float replace_proportion = 0.3;
        //sort the population(parents) and mating_pool(children) from small distance to high distance
        sort(mating_pool.begin(), mating_pool.end(), less_than_distance_sum());
        sort(population.begin(), population.end(), less_than_distance_sum());
        // replace the worst part of parents with the best part of children
        for (int i = 0; i < population.size() * replace_proportion; i ++) {
            population[population.size() - 1 - i] = mating_pool[i];
        }

        for (int i = 0; i < population.size(); i ++) {
            update_best_sln(population[i]);
        }

        cout << i << ": " << prob->best_sln->distance_sum << endl;
    }

}

//sum up the total distances of the route and update the total_distance property of the input solution
void search_strategy :: evaluate_solution(Solution* sln) {
    sln->distance_sum = 0;
    for (int i = 0; i < sln->size; i ++) {
        sln->distance_sum += dist_two_cities(sln->route[i], sln->route[i+1]);
    }
}

int search_strategy :: get_city_number() {
    return prob->size;
}

int main(int argc, char *argv[])
{
    // if(argc < 2){
    //     cout << "Usage: " << argv[0] << " filename" << endl;
    //     return 0;
    // }
    // string filename = argv[2];
    int max_time;
    string filename = "instance3.txt";
    search_strategy prob_search (filename);
    
    Solution *cur_sln = prob_search.greedy_heuristic_search();
    if (prob_search.get_city_number() < 1000) {
        max_time = 600;
    } else {
        max_time = 1200;
    }

    for (int i = 0; i < cur_sln->size; i++) {
        cout << cur_sln->route[i].id << "\t";
    }

    //Solution *cur_sln = prob_search.gen_rand_sln();
    // for (int i = 0; i < 1500; i ++) {
    //     prob_search.local_search_first_descent(cur_sln);
    //     //prob_search.evaluate_solution(cur_sln);
    //     cout << "i: " << i << " " << cur_sln->distance_sum << endl;
    // }

    
    prob_search.memetic_algorithm(max_time);
    

    // std::vector<City>::iterator it;
    // for(it = cur_sln->route.begin(); it != cur_sln->route.end(); it++){
    //     cout << it->id  << "\t";
    // }

    delete cur_sln;
}
