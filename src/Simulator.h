#ifndef CS3210_A3_A3_A0196678A_A0219768L_SIMULATOR_H
#define CS3210_A3_A3_A0196678A_A0219768L_SIMULATOR_H

#include <vector>
#include <string>
#include <set>
#include <queue>
#include <map>
#include "Troon.h"

using std::set;
using std::vector;
using std::string;
using std::priority_queue;
using std::map;

typedef struct {
    uint **element;
} matrix;

using adjmatrix = vector<vector<size_t>>;

struct TimeId {
    int time;
    int id;

    TimeId(int time, int id) : time(time), id(id) {}

    bool operator<(const struct TimeId &other) const {
        if (other.time == time) {
            return other.id < id;
        } else {
            return other.time < time;
        }
    }
};

class Simulator {
private:
    size_t ticks = 0;
    size_t linesToBePrinted = 0;
    int troonIdCounter = 0;

    // Link State
    matrix linkAdjList{}, linkCounters{}, linkCurrentDistances{}, linkTroons{};

    // Platform State
    matrix platformCounters{}, platformTroons{};
    vector<size_t> platformPopularities;

    // Waiting Area State
    vector<vector<priority_queue<TimeId>>> waitingAreas;

    // Troon State
    set<Troon *, TroonLexicographyComparison> greenTroons;
    set<Troon *, TroonLexicographyComparison> yellowTroons;
    set<Troon *, TroonLexicographyComparison> blueTroons;

    size_t greenTroonCounter = 0;
    size_t yellowTroonCounter = 0;
    size_t blueTroonCounter = 0;

    size_t maxGreenTroon;
    size_t maxYellowTroon;
    size_t maxBlueTroon;

    size_t num_stations;

    // Mapping
    map<string, size_t> stationNameIdMapping;
    vector<string> stationIdNameMapping;

    size_t terminalGreenForward;
    size_t terminalGreenReverse;
    size_t terminalYellowForward;
    size_t terminalYellowReverse;
    size_t terminalBlueForward;
    size_t terminalBlueReverse;

    // mapping for the next station in forward and reverse dir for each lane
    map<size_t, size_t> forwardGreenMap;
    map<size_t, size_t> reverseGreenMap;
    map<size_t, size_t> forwardYellowMap;
    map<size_t, size_t> reverseYellowMap;
    map<size_t, size_t> forwardBlueMap;
    map<size_t, size_t> reverseBlueMap;

public:
    Simulator(
            size_t num_stations,
            const vector<string> &station_names,
            const vector<size_t> &popularities,
            const adjmatrix &mat,
            const vector<string> &green_station_names,
            const vector<string> &yellow_station_names,
            const vector<string> &blue_station_names,
            size_t ticks,
            size_t num_green_trains,
            size_t num_yellow_trains,
            size_t num_blue_trains,
            size_t num_lines
    );

    static void AllocateSquareMatrix(matrix *m, size_t size);

    void Simulate();

    void Clean();

    void PrintTroons(size_t tick) const;

    void PopulateForwardReverseMapping(
            size_t num_stations,
            const vector<string> &green_station_names,
            size_t *terminalForward,
            size_t *terminalBackward,
            map<size_t, size_t> *forwardMapping,
            map<size_t, size_t> *reverseMapping
    );
};


#endif //CS3210_A3_A3_A0196678A_A0219768L_SIMULATOR_H
