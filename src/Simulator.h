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
    uint32_t **element;
} matrix_uint;

typedef struct {
    int32_t **element;
} matrix_int;

using adjmatrix = vector<vector<size_t>>;

struct TimeId {
    uint32_t time;
    uint32_t id;

    TimeId() : time(0), id(0) {}

    TimeId(uint32_t time, uint32_t id) : time(time), id(id) {}

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
    uint32_t ticks = 0;
    uint32_t linesToBePrinted = 0;
    uint32_t troonIdCounter = 0;

    // Link State
    const adjmatrix *linkLimit;

    matrix_int linkTroons{};
    matrix_uint linkCounters{}, linkCurrentDistances{};

    // Platform State
    matrix_int platformTroons{};
    matrix_uint platformCounters{};
    vector<size_t> platformPopularities;

    // Waiting Area State
    vector<vector<priority_queue<TimeId>>> waitingAreas;

    // Troon State
    set<Troon *, TroonLexicographyComparison> greenTroons;
    set<Troon *, TroonLexicographyComparison> yellowTroons;
    set<Troon *, TroonLexicographyComparison> blueTroons;

    vector<Troon *> troons;

    uint32_t greenTroonCounter = 0;
    uint32_t yellowTroonCounter = 0;
    uint32_t blueTroonCounter = 0;

    uint32_t maxGreenTroon;
    uint32_t maxYellowTroon;
    uint32_t maxBlueTroon;

    uint32_t num_stations;

    // Mapping
    map<string, uint32_t> stationNameIdMapping;
    vector<string> stationIdNameMapping;

    uint32_t terminalGreenForward{};
    uint32_t terminalGreenReverse{};
    uint32_t terminalYellowForward{};
    uint32_t terminalYellowReverse{};
    uint32_t terminalBlueForward{};
    uint32_t terminalBlueReverse{};

    // mapping for the next station in forward and reverse dir for each lane
    map<uint32_t, uint32_t> forwardGreenMap;
    map<uint32_t, uint32_t> reverseGreenMap;
    map<uint32_t, uint32_t> forwardYellowMap;
    map<uint32_t, uint32_t> reverseYellowMap;
    map<uint32_t, uint32_t> forwardBlueMap;
    map<uint32_t, uint32_t> reverseBlueMap;

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
            size_t num_lines,
            int argc,
            char* argv[]
    );

    static void AllocateSquareMatrix0(matrix_uint *m, size_t size);

    static void AllocateSquareMatrix1(matrix_int *m, size_t size);

    void Simulate();

    void SpawnTroons(uint32_t tick);

    void IncrementAllLinks() const;

    void UpdateAllLinks(uint32_t tick);

    void PushAllPlatform();

    void UpdateAllWA();

    void UpdateWaitingPlatform() const;

    void Clean();

    void PrintTroons(uint32_t tick) const;

    void PopulateForwardReverseMapping(
            uint32_t size,
            const vector<string> &green_station_names,
            uint32_t *terminalForward,
            uint32_t *terminalBackward,
            map<uint32_t, uint32_t> *forwardMapping,
            map<uint32_t, uint32_t> *reverseMapping
    );
};


#endif //CS3210_A3_A3_A0196678A_A0219768L_SIMULATOR_H
