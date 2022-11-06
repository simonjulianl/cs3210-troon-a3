#ifndef CS3210_A1_A1_A0196678A_A0219768L_SIMULATOR_H
#define CS3210_A1_A1_A0196678A_A0219768L_SIMULATOR_H

#include <vector>
#include <string>

using std::vector;
using std::string;

using matrix = vector<vector<size_t>>;

class Simulator {
public:
    Simulator(
            size_t num_stations,
            const vector<string> &station_names,
            const vector<size_t> &popularities,
            const matrix &mat,
            const vector<string> &green_station_names,
            const vector<string> &yellow_station_names,
            const vector<string> &blue_station_names,
            size_t ticks,
            size_t num_green_trains,
            size_t num_yellow_trains,
            size_t num_blue_trains,
            size_t num_lines
    );

    void Simulate();

    void Clean();
};


#endif //CS3210_A1_A1_A0196678A_A0219768L_SIMULATOR_H
