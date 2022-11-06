#include "Simulator.h"
#include <iostream>
#include <sstream>

using namespace std;

Simulator::Simulator(
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
) : ticks{ticks}, linesToBePrinted{num_lines}, platformPopularities{popularities},
    maxGreenTroon{num_green_trains},
    maxYellowTroon{num_yellow_trains}, maxBlueTroon{num_blue_trains}, num_stations{num_stations} {

    for (auto a: {&linkAdjList,
                  &linkCounters,
                  &linkCurrentDistances,
                  &linkTroons,
                  &platformCounters,
                  &platformTroons}
            ) {
        AllocateSquareMatrix(a, num_stations);
    }

    // allocate the mat to linkAdjList
    for (size_t i = 0; i < num_stations; i++) {
        for (size_t j = 0; j < num_stations; j++) {
            linkAdjList.element[i][j] = mat[i][j];
        }
    }

    // create mapping
    for (size_t i = 0; i < num_stations; i++) {
        const string &stationName = station_names[i];
        stationNameIdMapping[stationName] = i;
        stationIdNameMapping.push_back(stationName);
    }

    PopulateForwardReverseMapping(
            green_station_names.size(),
            green_station_names,
            &terminalGreenForward,
            &terminalGreenReverse,
            &forwardGreenMap,
            &reverseGreenMap
    );

    PopulateForwardReverseMapping(
            yellow_station_names.size(),
            yellow_station_names,
            &terminalYellowForward,
            &terminalYellowReverse,
            &forwardYellowMap,
            &reverseYellowMap
    );

    PopulateForwardReverseMapping(
            blue_station_names.size(),
            blue_station_names,
            &terminalBlueForward,
            &terminalBlueReverse,
            &forwardBlueMap,
            &reverseBlueMap
    );
}

void Simulator::PopulateForwardReverseMapping(
        size_t size,
        const vector<string> &green_station_names,
        size_t *terminalForward,
        size_t *terminalBackward,
        map<size_t, size_t> *forwardMapping,
        map<size_t, size_t> *reverseMapping
) {
    size_t currentStation, nextStation;

    for (size_t i = 0; i < size - 1; i++) {
        currentStation = stationNameIdMapping[green_station_names[i]];
        nextStation = stationNameIdMapping[green_station_names[i + 1]];

        if (i == 0) {
            *terminalForward = currentStation;
        }

        (*forwardMapping)[currentStation] = nextStation;
    }

    for (size_t i = size - 1; i > 0; i--) {
        currentStation = stationNameIdMapping[green_station_names[i]];
        nextStation = stationNameIdMapping[green_station_names[i - 1]];

        if (i == size - 1) {
            *terminalBackward = currentStation;
        }

        (*reverseMapping)[currentStation] = nextStation;
    }
}

void Simulator::Simulate() {
    for (size_t tick = 0; tick < ticks; tick++) {
        // TODO: Fill this with the troon logic

        PrintTroons(tick);
    }

    Clean();
}

void Simulator::PrintTroons(size_t tick) const {
    if (ticks - tick <= linesToBePrinted) {
        stringstream ss;
        ss << tick << ": ";
        for (auto &c: {blueTroons, greenTroons, yellowTroons}) {
            for (auto &t: c) {
                ss << t->GenerateDescription(stationIdNameMapping);
            }
        }

        cout << ss.str() << endl;
    }
}

void Simulator::AllocateSquareMatrix(matrix *m, size_t size) {
    m->element = new uint *[size];
    if (m->element == nullptr) {
        cerr << "Out of Memory \n";
    }

    for (size_t i = 0; i < size; i++) {
        m->element[i] = new uint[size];
        if (m->element[i] == nullptr) {
            cerr << "Out of Memory \n";
        }

        for (size_t j = 0; j < size; j++) {
            m->element[i][j] = 0;
        }
    }
}

void Simulator::Clean() {
    for (auto a: {&linkAdjList,
                  &linkCounters,
                  &linkCurrentDistances,
                  &linkTroons,
                  &platformCounters,
                  &platformTroons}
            ) {
        for (size_t i = 0; i < num_stations; i++) {
            delete[] a->element[i];
        }

        delete[] a->element;
    }
}

