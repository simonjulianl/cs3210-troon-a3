#ifndef CS3210_A1_A1_A0196678A_A0219768L_SIMULATOR_H
#define CS3210_A1_A1_A0196678A_A0219768L_SIMULATOR_H

#include <vector>
#include <string>
#include <map>
#include <tuple>
#include <set>
#include "Troon.h"
#include "PlatformUnit.h"

using std::vector;
using std::string;
using std::map;
using std::tuple;
using std::set;

using adjacency_matrix = vector<vector<size_t>>;

class Simulator {
private:
    size_t ticks = 0;
    size_t linesToBePrinted = 0;
    int troonIdCounter = 0;

    struct TroonLexicographyComparison {
        bool operator()(const Troon *a, const Troon *b) const {
            return std::to_string(a->id) < std::to_string(b->id);
        }
    };

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

    WaitingArea ***waitingAreaData;
    Link ***linkData;

    vector<Platform *> compactPlatformData;
    vector<WaitingArea *> compactWaitingAreaData;
    vector<Link *> compactLinkData;

    vector<WaitingArea *> terminals;

public:
    Simulator(
            size_t num_stations,
            const vector<string> &station_names,
            const vector<size_t> &popularities,
            const adjacency_matrix &mat,
            const vector<string> &green_station_names,
            const vector<string> &yellow_station_names,
            const vector<string> &blue_station_names,
            size_t ticks,
            size_t num_green_trains,
            size_t num_yellow_trains,
            size_t num_blue_trains,
            size_t num_lines
    );

    void SpawnTroons();

    void Simulate();

    void Clean();

private:
    void
    CreateWaitingPlatformLink(size_t ns, const vector<size_t> &popularities, const adjacency_matrix &mat,
                              const vector<string> &stationIdNameMapping);

    void LinkStation(const size_t &prevId, const size_t &currentId, const size_t &nextId, Line l);

    void AssembleLink(const vector<size_t> &stationNames, Line l);

    void UpdateAllLinks();

    void UpdateAllWA();

    void UpdateWaitingPlatform();

    void PushAllPlatform();
};


#endif //CS3210_A1_A1_A0196678A_A0219768L_SIMULATOR_H
