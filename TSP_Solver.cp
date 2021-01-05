// Lanxin HU scylh1 20029781 
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <limits>
#include <cmath>
#include<ctime>
#include<algorithm>
#include <iomanip>

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
    double distance_sum = numeric_limits<double>::max();
    vector<City> route;
    Solution(int prob_size) {
        size = prob_size;
        for (int i = 0; i < prob_size+1; i ++) {
            route.push_back(City());
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
    void init_prob_from_file(fstream & inFile);
    double ** distances = NULL;

public:
    double rand_01();
    int rand_int(int min, int max);
    int find_next_city(int cur_city_index, int* travelled);
    double dist_two_cities(City city1, City city2);
    Solution *greedy_heuristic_search();
    Solution *gen_rand_sln();
    void roulette_wheel_selection(vector<Solution*> population, vector<Solution*> mating_pool);
    void sort_by_dist(vector<Solution*> population);
    void local_search_first_descent(Solution* curt_sln);
    void memetic_algorithm(clock_t start_time, int max_time);
    void evaluate_solution(Solution* sln);
    void twopoint_mutation(Solution* sln);
    int check_feasibility_sln(Solution* sln);
    int update_best_sln(Solution* curt_sln);
    Solution *get_best_sln();
    int get_prob_size();
    bool deep_copy_route(vector<City> &route1, vector<City> &route2);
    bool deep_copy_city(City &city1, City &city2);
    bool deep_copy_sln(Solution* sln1, Solution* sln2);
    

    // constructor
    search_strategy(fstream & inFile) {
        init_prob_from_file(inFile);
    }
    // destructor
    ~search_strategy() {
        for (int i = 0; i < prob->size; i ++) {
            delete [] distances[i];
        }
        delete [] distances;
        delete prob;
    }
};

//return a random number between 0 and 1
double search_strategy :: rand_01()
{
    double number;
    number = (double)rand();
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

bool search_strategy :: deep_copy_route(vector<City> &route1, vector<City> &route2) {
    int size;
    if (route1.size() < route2.size()) {
        size = route1.size();
    } else {
        size = route2.size();
    }
    for (int i = 0; i < size; i ++) {
        route1[i].index = route2[i].index;
        route1[i].id = route2[i].id;
        route1[i].x = route2[i].x;
        route1[i].y = route2[i].y;
    }
    return true;
}

bool search_strategy :: deep_copy_city(City &city1, City &city2) {
    city1.id = city2.id;
    city1.index = city2.index;
    city1.x = city2.x;
    city1.y = city2.y;
}

bool search_strategy :: deep_copy_sln(Solution* sln1, Solution* sln2){
    deep_copy_route(sln1->route, sln2->route);
    sln1->distance_sum = sln2->distance_sum;
}

void search_strategy :: init_prob_from_file(fstream & inFile)
{
    int city_count = 0;
    //skip first line of the file
    string first_line;
    getline(inFile, first_line);
    vector<City> input_cities;

    int input_int;
    int flag = 0;
    while (inFile >> input_int) {
        if (flag == 0) {
            input_cities.push_back(City());
            
            input_cities[city_count].id = input_int;
            input_cities[city_count].index = city_count;
            flag ++;
        } else if (flag == 1) {
            input_cities[city_count].x = input_int;
            flag ++;
        } else if (flag == 2) {
            input_cities[city_count].y = input_int;
            flag = 0;
            city_count ++;
        }
    }
    
    distances = new double *[city_count];
    for (int i = 0; i < city_count; i ++) {
        distances[i] = new double[city_count];
    }

    for (int i = 0; i < city_count; i ++) {
        for (int j = 0; j < city_count; j ++) {
            distances[i][j] = dist_two_cities(input_cities[i], input_cities[j]);
        }
    }
    
    prob = new Problem(city_count, input_cities);
    
    inFile.close();
}

int search_strategy :: find_next_city(int cur_city_index, int* travelled)
{
    int min_index = -1;
    double min_dist = numeric_limits<double>::max();
    double cur_dist;

    for (int i = 1; i < prob->size; i++)
    {
        if (travelled[i] == 1) {
            continue;
        }
      
        cur_dist = distances[cur_city_index][i];
        if (cur_dist < min_dist) {
            min_index = i;
            min_dist = cur_dist;
        }
        
    }
    
    return min_index;
}

double search_strategy :: dist_two_cities(City city1, City city2) {
    return sqrt(pow(city1.x - city2.x, 2) + pow(city1.y - city2.y, 2) * 1.0);
}

Solution *search_strategy :: greedy_heuristic_search()
{
    Solution *sln = new Solution(prob->size);
 
    int *travelled = (int *)malloc(sizeof(int) * prob->size);
    for (int i = 0; i < prob->size; i ++) {
        travelled[i] = 0;
    }
    deep_copy_city(sln->route[0], prob->cities[0]);
    deep_copy_city(sln->route[prob->size], prob->cities[0]);

    int cur_city_index = rand_int(1, prob->size-1);
   
    for (int i = 1; i < prob->size; i++)
    {
        travelled[cur_city_index] = 1;
        deep_copy_city(sln->route[i], prob->cities[cur_city_index]);
        cur_city_index = find_next_city(cur_city_index, travelled);
    }

    free(travelled);
    evaluate_solution(sln);
    return sln;
}

/*randomly select two cities to swap
if the new solution is better than the input solution, update the input solution
do this process for "iter" times, which is determined by the size of the problem*/
void search_strategy :: twopoint_mutation(Solution* curt_sln) {
    int iter = prob->size / 50;
    int city1, city2;
    double sub_dist1, sub_dist2;
    for (int i = 0; i < iter; i ++) {
        city1 = rand_int(1, prob->size-1);
        city2 = rand_int(1, prob->size-1);
        while (city1 == city2) {
            city2 = rand_int(1, prob->size-1);
        }
        if (city2 == city1 + 1) {
            sub_dist1 = distances[curt_sln->route[city1 - 1].index][curt_sln->route[city1].index] + distances[curt_sln->route[city2 + 1].index][curt_sln->route[city2].index];
            
            sub_dist2 = distances[curt_sln->route[city1 - 1].index][curt_sln->route[city2].index] + distances[curt_sln->route[city2 + 1].index][curt_sln->route[city1].index];
            
        } else {
            sub_dist1 = distances[curt_sln->route[city1-1].index][curt_sln->route[city1].index] + distances[curt_sln->route[city1+1].index][curt_sln->route[city1].index] + distances[curt_sln->route[city2-1].index][curt_sln->route[city2].index] + distances[curt_sln->route[city2+1].index][curt_sln->route[city2].index];
            sub_dist2 = distances[curt_sln->route[city1-1].index][curt_sln->route[city2].index] + distances[curt_sln->route[city1+1].index][curt_sln->route[city2].index] + distances[curt_sln->route[city2-1].index][curt_sln->route[city1].index] + distances[curt_sln->route[city2+1].index][curt_sln->route[city1].index];
        }
        
        if (sub_dist1 > sub_dist2) {
            City tmp = curt_sln->route[city1];
            curt_sln->route[city1] = curt_sln->route[city2];
            curt_sln->route[city2] = tmp;
            curt_sln->distance_sum = curt_sln->distance_sum - sub_dist1 + sub_dist2;
        }
    }
    //!!!!!
    evaluate_solution(curt_sln);
}

// randomly generate a feasible route
Solution *search_strategy :: gen_rand_sln() {
    Solution *sln = new Solution(prob->size);
    deep_copy_route(sln->route, prob->cities);
    int iter = prob->size / 2;
    
    int city1, city2;
    //randomly generate two index and swap them
    for (int i = 0; i < iter; i++) {
        //srand((double)cur_time/ CLOCKS_PER_SEC);
        city1 = rand_int(1, prob->size-1);
        city2 = rand_int(1, prob->size-1);
        while (city1 == city2) {
            city2 = rand_int(1, prob->size-1);
        }
        City tmp = sln->route[city1];
        sln->route[city1] = sln->route[city2];
        sln->route[city2] = tmp;
    }
    evaluate_solution(sln);
   
    return sln;
}

int search_strategy :: check_feasibility_sln(Solution* sln) {
    vector<int> city_visited;
    double d = sln->distance_sum;
    evaluate_solution(sln);
    if (d != sln->distance_sum) {
        cout << "d: " << d << " | " << sln->distance_sum << endl;
    }
    
    for (int i = 0; i < sln->size; i ++) {
        if (find(city_visited.begin(), city_visited.end(), sln->route[i].id) != city_visited.end()) {
            cout << "??!!!!Ops!!!!!!" << endl;
            return -1;
        } else {
            city_visited.push_back(sln->route[i].id);
        }
    }
    return 1;
}

Solution *search_strategy :: get_best_sln() {
    return prob->best_sln;
}

void search_strategy :: local_search_first_descent(Solution* curt_sln) {
    double sub_dist1;
    double sub_dist2;
    double pre_d = curt_sln->distance_sum;
    int start_point;
    int search_range = curt_sln->size / 3;
    start_point = rand_int(1, curt_sln->size - search_range - 2);
    
    for (int city1 = start_point; city1 < start_point + search_range; city1 ++) {
        
        for (int city2 = (city1+1); city2 < start_point + search_range; city2 ++) {
            if (city2 == (city1 + 1)) {
                sub_dist1 = distances[curt_sln->route[city1 - 1].index][curt_sln->route[city1].index] + distances[curt_sln->route[city2 + 1].index][curt_sln->route[city2].index];
                sub_dist2 = distances[curt_sln->route[city1 - 1].index][curt_sln->route[city2].index] + distances[curt_sln->route[city2 + 1].index][curt_sln->route[city1].index];
                
            } else {
                sub_dist1 = distances[curt_sln->route[city1-1].index][curt_sln->route[city1].index] + distances[curt_sln->route[city1+1].index][curt_sln->route[city1].index] + distances[curt_sln->route[city2-1].index][curt_sln->route[city2].index] + distances[curt_sln->route[city2+1].index][curt_sln->route[city2].index];  
                sub_dist2 = distances[curt_sln->route[city1-1].index][curt_sln->route[city2].index] + distances[curt_sln->route[city1+1].index][curt_sln->route[city2].index] + distances[curt_sln->route[city2-1].index][curt_sln->route[city1].index] + distances[curt_sln->route[city2+1].index][curt_sln->route[city1].index];
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
void search_strategy :: roulette_wheel_selection(vector<Solution*> population, vector<Solution*> mating_pool) {
    double prob;
    double sumup_distance = 0;

    for (int i = 0; i < population.size(); i++)
    {
        sumup_distance += population[i]->distance_sum;
    }
    
    double acc_prob = 0;
    for (int i = 0; i < population.size(); i++) {
        prob = rand_01();
        for (int j = 0; j < population.size(); j ++) {
            acc_prob += (1 - (population[j]->distance_sum / sumup_distance))/(population.size() - 1);
            if (prob <= acc_prob) {
                
                mating_pool[i]->route = population[j]->route;
                mating_pool[i]->distance_sum = population[j]->distance_sum;
                acc_prob = 0;
                break;
            }
        }
    }    
}

// return 1 if current solution is better than the previous best_sln and update the best_sln, 0 otherwise
int search_strategy :: update_best_sln(Solution * curt_sln) {
    if (curt_sln->distance_sum < prob->best_sln->distance_sum) {
        // prob->best_sln->route = curt_sln->route;
       // prob->best_sln->route.assign(curt_sln->route.begin(), curt_sln->route.end());
        deep_copy_route(prob->best_sln->route, curt_sln->route);
        prob->best_sln->distance_sum = curt_sln->distance_sum;
        return 1;
    }
    return 0;
}

int search_strategy :: get_prob_size() {
    return prob->size;
}

void search_strategy :: memetic_algorithm(clock_t start_time, int max_time) {
    clock_t end_time = clock();
    double time_spent = 0;
    int pop_size = 30;
    int max_iter = 10000;
    double mutation_prob = 0.5;
    double replace_proportion = 0.3;
    
    vector<Solution*> population;
    vector<Solution*> mating_pool;
    
    // initialize population and mating_pool
    for (int i = 0; i < pop_size; i ++) {
        population.push_back(greedy_heuristic_search());
        
        Solution *sln_ptr = new Solution(get_prob_size());
      
        mating_pool.push_back(sln_ptr);
    }
    
    

    for (int i = 0; i < max_iter; i ++) {
        roulette_wheel_selection(population, mating_pool);
        
        for (int i = 0; i < population.size(); i ++) {
            end_time = clock();
            time_spent = (double)(end_time - start_time) / CLOCKS_PER_SEC;
            if (time_spent >= max_time) {
                for (int i = 0; i < pop_size; i ++) {
                    delete population[i];
                    delete mating_pool[i];
                }
                return;
            }
            // mutate by swapping two randomly selected cities
            twopoint_mutation(mating_pool[i]);
            local_search_first_descent(mating_pool[i]);
        }

        
        // sort the population(parents) and mating_pool(children) from small distance to high distance
        sort(mating_pool.begin(), mating_pool.end(), less_than_distance_sum());
        sort(population.begin(), population.end(), less_than_distance_sum());

        // replace the worst part of parents with the best part of children
        for (int i = 0; i < population.size() * replace_proportion; i ++) {
            deep_copy_sln(population[population.size() - 1 - i], mating_pool[i]);
     
        }

        for (int i = 0; i < population.size(); i ++) {
            update_best_sln(population[i]);
        }
        
        //cout << i << "\t" << prob->best_sln->distance_sum << endl;
    }


}

//sum up the total distances of the route and update the total_distance property of the input solution
void search_strategy :: evaluate_solution(Solution* sln) {
    sln->distance_sum = 0;
    for (int i = 0; i < sln->size; i ++) {
        //sln->distance_sum += dist_two_cities(sln->route[i], sln->route[i+1]);
        sln->distance_sum += distances[sln->route[i].index][sln->route[i+1].index];
    }
    
}

int main(int argc, char *argv[])
{
    clock_t start_time = clock();
    if(argc < 2){
        cout << "Usage: " << argv[0] << " filename" << endl;
        return 0;
    }
    string filename = argv[1];

    fstream inFile(filename, std::ios_base::in);
    if (inFile.fail()) {
        cout << "File not found." << endl;
        return 0;
    }
    
    search_strategy prob_search (inFile);

    int max_time = 600;
    if ((prob_search.get_prob_size()) > 1000) {
        max_time = 1200;
    }
    
    prob_search.memetic_algorithm(start_time, max_time);
    
    Solution* best_sln = prob_search.get_best_sln();
    cout << best_sln->distance_sum << "\t";
    cout << best_sln->size << "\t";
    for (int i = 0; i <= best_sln->size; i ++) {
        cout << best_sln->route[i].id << "\t";
    }
    cout << endl;

}
