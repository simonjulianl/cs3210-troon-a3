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

    for (auto a: {&linkCounters,
                  &linkCurrentDistances,
                  &platformCounters}
            ) {
        AllocateSquareMatrix0(a, num_stations);
    }

    for (auto a: {&linkAdjList,
                  &linkTroons,
                  &platformTroons}
            ) {
        AllocateSquareMatrix1(a, num_stations);
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

    // initialize troon vector size
    size_t maxTroon = maxBlueTroon + maxGreenTroon + maxYellowTroon;
    troons.reserve(maxTroon);

    // initialize waiting areas
    for (size_t i = 0; i < num_stations; i++) {
        vector<priority_queue<TimeId>> v;
        for (size_t j = 0; j < num_stations; j++) {
            priority_queue<TimeId> pq;
            v.push_back(pq);
        }
        waitingAreas.push_back(v);
    }
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
        UpdateAllLinks();

        PushAllPlatform();

        SpawnTroons(tick);

        UpdateAllWA();

        UpdateWaitingPlatform();

        PrintTroons(tick);
    }

    Clean();
}

void Simulator::SpawnTroons(size_t tick) {
    // g -> y -> b
    if (greenTroonCounter < maxGreenTroon) {
        auto t = new Troon{troonIdCounter, g, WAITING_AREA, FORWARD};
        t->setSourceDestination(terminalGreenForward, forwardGreenMap[terminalGreenForward]);
        TimeId temp = {tick, troonIdCounter};
        waitingAreas[terminalGreenForward][forwardGreenMap[terminalGreenForward]].push(temp);
        greenTroons.insert(t);
        troons.push_back(t);
        troonIdCounter++;
        greenTroonCounter++;

        if (greenTroonCounter < maxGreenTroon) {
            auto x = new Troon{troonIdCounter, g, WAITING_AREA, REVERSE};
            x->setSourceDestination(terminalGreenReverse, reverseGreenMap[terminalGreenReverse]);
            TimeId temp = {tick, troonIdCounter};
            waitingAreas[terminalGreenReverse][reverseGreenMap[terminalGreenReverse]].push(temp);
            greenTroons.insert(x);
            troons.push_back(x);
            troonIdCounter++;
            greenTroonCounter++;
        }
    }


    if (yellowTroonCounter < maxYellowTroon) {
        auto t = new Troon{troonIdCounter, y, WAITING_AREA, FORWARD};
        t->setSourceDestination(terminalYellowForward, forwardYellowMap[terminalYellowForward]);
        TimeId temp = {tick, troonIdCounter};
        waitingAreas[terminalYellowForward][forwardYellowMap[terminalYellowForward]].push(temp);
        yellowTroons.insert(t);
        troons.push_back(t);
        troonIdCounter++;
        yellowTroonCounter++;

        if (yellowTroonCounter < maxYellowTroon) {
            auto x = new Troon{troonIdCounter, y, WAITING_AREA, REVERSE};
            x->setSourceDestination(terminalYellowReverse, reverseYellowMap[terminalYellowReverse]);
            TimeId temp = {tick, troonIdCounter};
            waitingAreas[terminalYellowReverse][reverseYellowMap[terminalYellowReverse]].push(temp);
            yellowTroons.insert(x);
            troons.push_back(x);
            troonIdCounter++;
            yellowTroonCounter++;
        }
    }


    if (blueTroonCounter < maxBlueTroon) {
        auto t = new Troon{troonIdCounter, b, WAITING_AREA, FORWARD};
        t->setSourceDestination(terminalBlueForward, forwardBlueMap[terminalBlueForward]);
        TimeId temp = {tick, troonIdCounter};
        waitingAreas[terminalBlueForward][forwardBlueMap[terminalBlueForward]].push(temp);
        blueTroons.insert(t);
        troons.push_back(t);
        troonIdCounter++;
        blueTroonCounter++;

        if (blueTroonCounter < maxBlueTroon) {
            auto x = new Troon{troonIdCounter, b, WAITING_AREA, REVERSE};
            x->setSourceDestination(terminalBlueReverse, reverseBlueMap[terminalBlueReverse]);
            TimeId temp = {tick, troonIdCounter};
            waitingAreas[terminalBlueReverse][reverseBlueMap[terminalBlueReverse]].push(temp);
            blueTroons.insert(x);
            troons.push_back(x);
            troonIdCounter++;
            blueTroonCounter++;
        }
    }
}

void Simulator::UpdateAllLinks() {
    for (size_t i = 0; i < num_stations; i++) {
        for (size_t j = 0; j < num_stations; j++) {

        }
    }
}

void Simulator::PushAllPlatform() {
    for (size_t i = 0; i < num_stations; i++) {
        for (size_t j = 0; j < num_stations; j++) {
            bool isReadyToGo = platformCounters.element[i][j] >= platformPopularities[i];

            if (platformTroons.element[i][j] == -1 || !isReadyToGo) return;

            Troon* troon = troons[platformTroons.element[i][j]];
            if (linkTroons.element[i][j] != -1 || linkCounters.element[i][j] < 1) return;

            platformCounters.element[i][j] = 0;
            linkTroons.element[i][j] = troon->id;
            troon->location = LINK;
            platformTroons.element[i][j] = -1;
        }
    }
}

void Simulator::UpdateAllWA() {
    for (size_t i = 0; i < num_stations; i++) {
        for (size_t j = 0; j < num_stations; j++) {
            if (waitingAreas[i][j].empty() || platformTroons.element[i][j] != -1) return;
            TimeId top = waitingAreas[i][j].top();
            platformTroons.element[i][j] = top.id;
            waitingAreas[i][j].pop();
        }
    }
}

void Simulator::UpdateWaitingPlatform() {
    for (size_t i = 0; i < num_stations; i++) {
        for (size_t j = 0; j < num_stations; j++) {
            if (platformTroons.element[i][j] != -1) {
                platformCounters.element[i][j]++;
            }
        }
    }
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

void Simulator::AllocateSquareMatrix0(matrix0 *m, size_t size) {
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

void Simulator::AllocateSquareMatrix1(matrix1 *m, size_t size) {
    m->element = new int *[size];
    if (m->element == nullptr) {
        cerr << "Out of Memory \n";
    }

    for (size_t i = 0; i < size; i++) {
        m->element[i] = new int[size];
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
                  &linkTroons,
                  &platformTroons}
            ) {
        for (size_t i = 0; i < num_stations; i++) {
            delete[] a->element[i];
        }

        delete[] a->element;
    }

    for (auto a: {&linkCounters,
                  &linkCurrentDistances,
                  &platformCounters}
            ) {
        for (size_t i = 0; i < num_stations; i++) {
            delete[] a->element[i];
        }

        delete[] a->element;
    }
}

