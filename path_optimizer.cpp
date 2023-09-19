#include <iostream>
#include <vector>
#include <cmath>
#include <getopt.h>
#include <iomanip>
#include <queue>
#include <map>
#include "path_optimizer.h"

using namespace std;
vector <Coordinate> coordinates;
vector <Coordinate1> coordinates1;

double calcMSTDist(const Coordinate& cord1, const Coordinate& cord2) {
    if (cord1.loc == Location::Water && cord2.loc == Location::Land) { 
        return numeric_limits<double>::max();
    }
    else if (cord1.loc == Location::Land && cord2.loc == Location::Water){
        return numeric_limits<double>::max();
    }
    else {
        double dx = static_cast<double>(cord1.x) - static_cast<double>(cord2.x);
        double dy = static_cast<double>(cord1.y) - static_cast<double>(cord2.y);
        return dx * dx + dy * dy;
    }
}

double calcTSPDist(const Coordinate1& cord1, const Coordinate1& cord2) {
    double dx = static_cast<double>(cord1.x) - static_cast<double>(cord2.x);
    double dy = static_cast<double>(cord1.y) - static_cast<double>(cord2.y);
    return dx * dx + dy * dy;
}

void computeMST(size_t numPoints) {
    double length = 0;

    vector<bool> addedEdges(numPoints, false);
    vector<size_t> parent(numPoints);
    vector<double> key(numPoints, numeric_limits<double>::max());

    key[0] = 0;
    parent[0] = 0;

    for (size_t i = 0; i < numPoints; i++) {
        size_t minIndex;
        double minVal = numeric_limits<double>::max();

        for (size_t j = 0; j < numPoints; j++) {
            if (!addedEdges[j] && key[j] < minVal) {
                minVal = key[j];
                minIndex = j;
            }
        }

        addedEdges[minIndex] = true;

        for (size_t j = 0; j < numPoints; j++) {
            if (!addedEdges[j]) {
                double temp_weight = calcMSTDist(coordinates[minIndex], coordinates[j]);
                if (temp_weight < key[j]) {
                    parent[j] = minIndex;
                    key[j] = temp_weight;
                }
            }
        }
        length += sqrt(minVal);
    }


    cout << length << '\n';

    for (size_t i = 1; i < numPoints; i++) {
        size_t one = parent[i];
        size_t two = i;

        if (one < two) {
            cout << one << " " << two << '\n';
        }
        else {
            cout << two << " " << one << '\n';
        }
    }
}

vector<size_t> arbitraryInsertion(size_t numPoints) {
    vector<size_t> tour;
    tour.reserve(numPoints);

    tour.push_back(0);
    tour.push_back(1);
    tour.push_back(0);

    for (size_t i = 2; i < numPoints; i++) {
        double minWeight = numeric_limits<double>::max();
        size_t victim = 0;

        for (size_t j = 1; j < tour.size(); j++) {
            double weight = sqrt(calcTSPDist(coordinates1[tour[j-1]], coordinates1[i])) + sqrt(calcTSPDist(coordinates1[i], coordinates1[tour[j]])) - sqrt(calcTSPDist(coordinates1[tour[j-1]], coordinates1[tour[j]]));

            if (weight < minWeight) {
                minWeight = weight;
                victim = j;
            }
        }

        tour.insert(tour.begin() + (int)victim, i);
    }

    tour.pop_back();

    return tour;
}

double computeLowerBound(const vector<size_t>& remainingNodes) {
    size_t numPoints = remainingNodes.size();
    double computeLowerBound = 0;
    
    vector<bool> addedEdges(numPoints, false);
    vector<size_t> parent(numPoints);
    vector<double> key(numPoints, numeric_limits<double>::max());

    key[0] = 0; 
    parent[0] = 0; 

    for (size_t i = 0; i < numPoints; i++) {
        size_t minIndex;
        double minVal = numeric_limits<double>::max();

        for (size_t i = 0; i < numPoints; i++) {
            if (!addedEdges[i] && key[i] < minVal) {
                minVal = key[i];
                minIndex = i;
            }
        }

        addedEdges[minIndex] = true;

        for (size_t i = 0; i < remainingNodes.size(); i++) {
            if (!addedEdges[i]) {
                double tempWeight = calcTSPDist(coordinates1[remainingNodes[minIndex]], coordinates1[remainingNodes[i]]);
                if (tempWeight < key[i]) {
                    parent[i] = minIndex;
                    key[i] = tempWeight;
                }
            }
        }
        computeLowerBound += sqrt(minVal);
    }

    return computeLowerBound;
}

struct OptimalPathFinder {
    vector<size_t> currPath;
    vector<size_t> bestPath;

    vector<size_t> remainingNodes;

    double bestPathLength = numeric_limits<double>::max();
    double currPathLength = 1;

    bool is_promising(vector<size_t> &path, size_t permLength) {
        remainingNodes.clear();
        remainingNodes.reserve(path.size() - permLength);
    
        for (size_t i = permLength; i < path.size(); i++) {
            remainingNodes.push_back(path[i]);
        } 
        
        double lowerB = computeLowerBound(remainingNodes);
        if (lowerB >= bestPathLength) {
            return false;
        }

        double arm1 = numeric_limits<double>::max();
        double arm2 = numeric_limits<double>::max();

        for (size_t i = 0; i < remainingNodes.size(); i++) {
            double arm1Weight = calcTSPDist(coordinates1[currPath[0]], coordinates1[remainingNodes[i]]);
            double arm2Weight = calcTSPDist(coordinates1[currPath[permLength - 1]], coordinates1[remainingNodes[i]]);

            arm1 = min(arm1, arm1Weight);
            arm2 = min(arm2, arm2Weight);
        }
        
        if (currPathLength + sqrt(arm1) + sqrt(arm2) + lowerB < bestPathLength) {
            return true;
        } 

        return false;
    } 

    void generate_perms(vector<size_t> &path, size_t permLength) {
        if (permLength == path.size()) {
            if (currPathLength < bestPathLength) {
        
                currPathLength += sqrt(calcTSPDist(coordinates1[currPath[0]], coordinates1[currPath.back()]));
               
                bestPath = currPath;
                bestPathLength = currPathLength;   

                currPathLength -= sqrt(calcTSPDist(coordinates1[currPath[0]], coordinates1[currPath.back()]));
            }
            return;
        }

        if (!is_promising(path, permLength)) {
            return;
        }

        for (size_t i = permLength; i < path.size(); i++) {
            swap(path[permLength], path[i]);
            
            currPath.push_back(path[permLength]);
            currPathLength += sqrt(calcTSPDist(coordinates1[currPath[currPath.size() - 2]], coordinates1[currPath.back()]));
            
            generate_perms(path, permLength + 1);
    
            currPathLength -= sqrt(calcTSPDist(coordinates1[currPath[currPath.size() - 2]], coordinates1[currPath.back()]));
            currPath.pop_back();
            
            swap(path[permLength], path[i]);
        }  
    }
};


//----------------------------------------------


vector<size_t> optimizePath(size_t numPoints) {
    OptimalPathFinder opt_pf;
    
    opt_pf.bestPath = arbitraryInsertion(numPoints);
    opt_pf.currPath.push_back(0);

    double bestPathWeight= 0;
    for (size_t i = 0; i < opt_pf.bestPath.size() -1; i++) {
        bestPathWeight += sqrt(calcTSPDist(coordinates1[opt_pf.bestPath[i]], coordinates1[opt_pf.bestPath[i + 1]]));
    }
    bestPathWeight += sqrt(calcTSPDist(coordinates1[opt_pf.bestPath.back()], coordinates1[opt_pf.bestPath[0]]));
    opt_pf.bestPathLength = bestPathWeight;

    vector<size_t> path;
    for (size_t i = 0; i < numPoints; i++) {
        path.push_back(i);
    }
   
    opt_pf.generate_perms(path, 1);
    return opt_pf.bestPath;
}

///--------------------------------

struct PathOptimizer {
public:
    void parseOptions(int argc, char** argv);
    void execute();
private:
    OptimizationType optType;
};

void PathOptimizer::parseOptions(int argc, char** argv) {
    int option_index = 0, option = 0;

    opterr = false;

    struct option longOpts[] = {{ "help", no_argument, nullptr, 'h' },
                                { "mode", required_argument, nullptr, 'm' },
                                { nullptr, 0, nullptr, '\0' }};

    while ((option = getopt_long(argc, argv, "hm:", longOpts, &option_index)) != -1) {
        switch (option) {
            case 'h':
                exit(0);
            case 'm':
                if (*optarg == 'M') {
                    type = Algo_Type::MST;
                    break;
                }
                else if (*optarg == 'F') {
                    type = Algo_Type::TSP_FAST;
                    break;
                }
                else if (*optarg == 'O') {
                    type = Algo_Type::TSP_OPTIMAL;
                    break;
                }
                else {
                    cerr << "Error: Invalid mode";
                    exit(1);
                }
            default:
                break;
        }
    }
}

void PathOptimizer::run() {
    size_t num_coordinates = 0;
    int64_t x, y;
    cin >> num_coordinates;

    if (type == Algo_Type::MST) {
        coordinates.reserve(num_coordinates);
        
        for (size_t i = 0; i < num_coordinates; i++) {
            cin >> x;
            cin >> y;

            if (y > 0) {
                coordinates.emplace_back(x,y, Location::Land);
            }
            else if (x < 0 && y < 0) { 
                coordinates.emplace_back(x,y, Location::Water);
            }
            else if (y < 0 && x > 0) {
                coordinates.emplace_back(x,y, Location::Land);
            }
            else {
                coordinates.emplace_back(x,y, Location::Coast);
            }
        }
        computeMST(num_coordinates);
    }
    else if (type == Algo_Type::TSP_FAST) {
        coordinates1.reserve(num_coordinates);
        
        for (size_t i = 0; i < num_coordinates; i++) {
            cin >> x;
            cin >> y;
            coordinates1.emplace_back(x,y);
        }
        
        vector<size_t> tour = arbitraryInsertion(num_coordinates);

        double tour_length = 0;
        for (size_t i = 0; i < tour.size() -1; ++i) {
            tour_length += sqrt(calcTSPDist(coordinates1[tour[i]], coordinates1[tour[i + 1]]));
        }
        tour_length += sqrt(calcTSPDist(coordinates1[tour[tour.size() -1]], coordinates1[tour[0]]));

        cout  << tour_length << '\n';

        for (const auto& node : tour) {
            cout << node << " ";
        }
        cout << '\n';
    }
    else {
       coordinates1.reserve(num_coordinates);
        
        for (size_t i = 0; i < num_coordinates; i++) {
            cin >> x;
            cin >> y;
            coordinates1.emplace_back(x,y);
        }
        vector<size_t> tour = SexyTSP(num_coordinates);

        double tour_length = 0;
        for (size_t i = 0; i < tour.size() -1; ++i) {
            tour_length += sqrt(calcTSPDist(coordinates1[tour[i]], coordinates1[tour[i + 1]]));
        }
        tour_length += sqrt(calcTSPDist(coordinates1[tour[tour.size() -1]], coordinates1[tour[0]]));

        cout  << tour_length << '\n';

        for (const auto& node : tour) {
            cout << node << " ";
        }
        cout << '\n';  
    }
}

int main(int argc, char** argv) {
    std::ios_base::sync_with_stdio(false);
    cerr << fixed << showpoint << setprecision(2);
    cout << std::setprecision(2);
    cout << std::fixed; 

    try {
        PathOptimizer pathOpt;
        pathOpt.parseOptions(argc, argv);
        pathOpt.run();
    }
    catch (runtime_error& e) {
        cerr << e.what() << endl;
        return 1;
    }
    return 0;
}