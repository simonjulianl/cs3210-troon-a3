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
) : ticks{static_cast<uint32_t>(ticks)}, linesToBePrinted{static_cast<uint32_t>(num_lines)},
    platformPopularities{popularities},
    maxGreenTroon{static_cast<uint32_t>(num_green_trains)},
    maxYellowTroon{static_cast<uint32_t>(num_yellow_trains)}, maxBlueTroon{static_cast<uint32_t>(num_blue_trains)},
    num_stations{static_cast<uint32_t>(num_stations)} {

    for (auto a: {&linkCounters,
                  &linkCurrentDistances,
                  &platformCounters}
            ) {
        AllocateSquareMatrix0(a, num_stations);
    }

    for (auto a: {&linkTroons,
                  &platformTroons}
            ) {
        AllocateSquareMatrix1(a, num_stations);
    }

    // allocate the mat to linkAdjList
    linkLimit = &mat;

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
        uint32_t size,
        const vector<string> &green_station_names,
        uint32_t *terminalForward,
        uint32_t *terminalBackward,
        map<uint32_t, uint32_t> *forwardMapping,
        map<uint32_t, uint32_t> *reverseMapping
) {
    uint32_t currentStation, nextStation;

    for (uint32_t i = 0; i < size - 1; i++) {
        currentStation = stationNameIdMapping[green_station_names[i]];
        nextStation = stationNameIdMapping[green_station_names[i + 1]];

        if (i == 0) {
            *terminalForward = currentStation;
        }

        (*forwardMapping)[currentStation] = nextStation;
    }

    for (uint32_t i = size - 1; i > 0; i--) {
        currentStation = stationNameIdMapping[green_station_names[i]];
        nextStation = stationNameIdMapping[green_station_names[i - 1]];

        if (i == size - 1) {
            *terminalBackward = currentStation;
        }

        (*reverseMapping)[currentStation] = nextStation;
    }
}

void Simulator::Simulate() {
    for (uint32_t tick = 0; tick < ticks; tick++) {
        IncrementAllLinks();

        UpdateAllLinks(tick);

        PushAllPlatform();

        SpawnTroons(tick);

        UpdateAllWA();

        UpdateWaitingPlatform();

        PrintTroons(tick);
    }

    Clean();
}

void Simulator::SpawnTroons(uint32_t tick) {
    // g -> y -> b
    TimeId temp;
    if (greenTroonCounter < maxGreenTroon) {
        auto t = new Troon{troonIdCounter, g, WAITING_AREA, FORWARD};
        t->setSourceDestination(terminalGreenForward, forwardGreenMap[terminalGreenForward]);
        temp = {tick, troonIdCounter};
        waitingAreas[terminalGreenForward][forwardGreenMap[terminalGreenForward]].push(temp);
        greenTroons.insert(t);
        troons.push_back(t);
        troonIdCounter++;
        greenTroonCounter++;

        if (greenTroonCounter < maxGreenTroon) {
            auto x = new Troon{troonIdCounter, g, WAITING_AREA, REVERSE};
            x->setSourceDestination(terminalGreenReverse, reverseGreenMap[terminalGreenReverse]);
            temp = {tick, troonIdCounter};
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
        temp = {tick, troonIdCounter};
        waitingAreas[terminalYellowForward][forwardYellowMap[terminalYellowForward]].push(temp);
        yellowTroons.insert(t);
        troons.push_back(t);
        troonIdCounter++;
        yellowTroonCounter++;

        if (yellowTroonCounter < maxYellowTroon) {
            auto x = new Troon{troonIdCounter, y, WAITING_AREA, REVERSE};
            x->setSourceDestination(terminalYellowReverse, reverseYellowMap[terminalYellowReverse]);
            temp = {tick, troonIdCounter};
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
        temp = {tick, troonIdCounter};
        waitingAreas[terminalBlueForward][forwardBlueMap[terminalBlueForward]].push(temp);
        blueTroons.insert(t);
        troons.push_back(t);
        troonIdCounter++;
        blueTroonCounter++;

        if (blueTroonCounter < maxBlueTroon) {
            auto x = new Troon{troonIdCounter, b, WAITING_AREA, REVERSE};
            x->setSourceDestination(terminalBlueReverse, reverseBlueMap[terminalBlueReverse]);
            temp = {tick, troonIdCounter};
            waitingAreas[terminalBlueReverse][reverseBlueMap[terminalBlueReverse]].push(temp);
            blueTroons.insert(x);
            troons.push_back(x);
            troonIdCounter++;
            blueTroonCounter++;
        }
    }
}

//TODO: Parallelize this with OpenMPI
void Simulator::IncrementAllLinks() const {
    for (size_t i = 0; i < num_stations; i++) {
        for (size_t j = 0; j < num_stations; j++) {
            if (linkTroons.element[i][j] == -1) {
                linkCounters.element[i][j]++;
            }
        }
    }
}

void Simulator::UpdateAllLinks(uint32_t tick) {
    for (uint32_t i = 0; i < num_stations; i++) {
        for (uint32_t j = 0; j < num_stations; j++) {
            if (linkTroons.element[i][j] == -1) continue;

            if (linkCurrentDistances.element[i][j] >= (*linkLimit)[i][j] - 1) {
                Troon *curr = troons[linkTroons.element[i][j]];
                curr->location = WAITING_AREA;
                uint32_t source = j;
                uint32_t destination = 0;
                switch (curr->line) {
                    case g:
                        if (curr->direction == FORWARD) {
                            if (j == terminalGreenReverse) {
                                destination = reverseGreenMap[j];
                                curr->setSourceDestination(j, destination);
                                curr->direction = REVERSE;
                            } else {
                                destination = forwardGreenMap[j];
                                curr->setSourceDestination(j, destination);
                            }
                        } else if (curr->direction == REVERSE) {
                            if (j == terminalGreenForward) {
                                destination = forwardGreenMap[j];
                                curr->setSourceDestination(j, destination);
                                curr->direction = FORWARD;
                            } else {
                                destination = reverseGreenMap[j];
                                curr->setSourceDestination(j, destination);
                            }
                        }
                        break;
                    case y:
                        if (curr->direction == FORWARD) {
                            if (j == terminalYellowReverse) {
                                destination = reverseYellowMap[j];
                                curr->setSourceDestination(j, destination);
                                curr->direction = REVERSE;
                            } else {
                                destination = forwardYellowMap[j];
                                curr->setSourceDestination(j, destination);
                            }
                        } else if (curr->direction == REVERSE) {
                            if (j == terminalYellowForward) {
                                destination = forwardYellowMap[j];
                                curr->setSourceDestination(j, destination);
                                curr->direction = FORWARD;
                            } else {
                                destination = reverseYellowMap[j];
                                curr->setSourceDestination(j, destination);
                            }
                        }
                        break;
                    case b:
                        if (curr->direction == FORWARD) {
                            if (j == terminalBlueReverse) {
                                destination = reverseBlueMap[j];
                                curr->setSourceDestination(j, destination);
                                curr->direction = REVERSE;
                            } else {
                                destination = forwardBlueMap[j];
                                curr->setSourceDestination(j, destination);
                            }
                        } else if (curr->direction == REVERSE) {
                            if (j == terminalBlueForward) {
                                destination = forwardBlueMap[j];
                                curr->setSourceDestination(j, destination);
                                curr->direction = FORWARD;
                            } else {
                                destination = reverseBlueMap[j];
                                curr->setSourceDestination(j, destination);
                            }
                        }
                        break;
                }

                linkCounters.element[i][j] = 0;
                linkCurrentDistances.element[i][j] = 0;
                linkTroons.element[i][j] = -1;

                TimeId temp = {tick, curr->id};
                waitingAreas[source][destination].push(temp);
            } else {
                linkCurrentDistances.element[i][j]++;
            }
        }
    }
}

void Simulator::PushAllPlatform() {
    for (size_t i = 0; i < num_stations; i++) {
        for (size_t j = 0; j < num_stations; j++) {
            bool isReadyToGo = platformCounters.element[i][j] >= platformPopularities[i] + 2;
            //cout << i << "," << j << ":" << platformCounters.element[i][j] << "popularity: "<< platformPopularities[i] << "\n";

            if (platformTroons.element[i][j] == -1 || !isReadyToGo) continue;

            Troon *troon = troons[platformTroons.element[i][j]];
            if (linkTroons.element[i][j] != -1 || linkCounters.element[i][j] < 1) continue;
            //cout << i << "," << j << ":" << linkTroons.element[i][j] << "counter: "<< linkCounters.element[i][j] << "\n";

            platformCounters.element[i][j] = 0;
            linkTroons.element[i][j] = static_cast<int32_t>(troon->id);
            troon->location = LINK;
            platformTroons.element[i][j] = -1;
        }
    }
}

void Simulator::UpdateAllWA() {
    for (size_t i = 0; i < num_stations; i++) {
        for (size_t j = 0; j < num_stations; j++) {
            //cout << i << "," << j << ":" <<waitingAreas[i][j].size() << "platform: "<< platformTroons.element[i][j] << "\n";
            if (waitingAreas[i][j].empty() || platformTroons.element[i][j] != -1) continue;
            TimeId top = waitingAreas[i][j].top();
            platformTroons.element[i][j] = static_cast<int32_t>(top.id);
            troons[top.id]->location = PLATFORM;
            waitingAreas[i][j].pop();
        }
    }
}

//TODO: Parallelize this with OpenMPI
void Simulator::UpdateWaitingPlatform() const {
    for (size_t i = 0; i < num_stations; i++) {
        for (size_t j = 0; j < num_stations; j++) {
            if (platformTroons.element[i][j] != -1) {
                platformCounters.element[i][j]++;
            }
        }
    }
}

void Simulator::PrintTroons(uint32_t tick) const {
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

void Simulator::AllocateSquareMatrix0(matrix_uint *m, size_t size) {
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

void Simulator::AllocateSquareMatrix1(matrix_int *m, size_t size) {
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
            m->element[i][j] = -1;
        }
    }
}

void Simulator::Clean() {
    for (auto a: {&linkTroons,
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
