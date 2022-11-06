#include "Simulator.h"
#include <iostream>
#include <sstream>
#include <omp.h>

using namespace std;

Simulator::Simulator(
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
) : ticks{ticks}, linesToBePrinted{num_lines}, maxGreenTroon{num_green_trains}, maxYellowTroon{num_yellow_trains},
    maxBlueTroon{num_blue_trains}, num_stations{num_stations} {

    waitingAreaData = new WaitingArea **[num_stations];
    linkData = new Link **[num_stations];
    for (size_t i = 0; i < num_stations; i++) {
        waitingAreaData[i] = new WaitingArea *[num_stations];
        linkData[i] = new Link *[num_stations];
    }

    map<string, size_t> stationNameIdMapping;
    vector<string> stationIdNameMapping(num_stations);

    for (size_t i = 0; i < num_stations; i++) {
        const string &stationName = station_names[i];
        stationNameIdMapping[stationName] = i;
        stationIdNameMapping[i] = stationName;
    }

    CreateWaitingPlatformLink(num_stations, popularities, mat, stationIdNameMapping);

    // Optimization, convert everything to id and pass the array of id
    vector<size_t> green_station(green_station_names.size());
    for (size_t i = 0; i < green_station_names.size(); i++) {
        green_station[i] = stationNameIdMapping[green_station_names[i]];
    }
    vector<size_t> blue_station(blue_station_names.size());
    for (size_t i = 0; i < blue_station_names.size(); i++) {
        blue_station[i] = stationNameIdMapping[blue_station_names[i]];
    }
    vector<size_t> yellow_station(yellow_station_names.size());
    for (size_t i = 0; i < yellow_station_names.size(); i++) {
        yellow_station[i] = stationNameIdMapping[yellow_station_names[i]];
    }

    terminals.push_back(waitingAreaData[green_station[0]][green_station[1]]);
    terminals.push_back(
            waitingAreaData[green_station[green_station.size() - 1]][green_station[green_station.size() - 2]]);
    terminals.push_back(waitingAreaData[yellow_station[0]][yellow_station[1]]);
    terminals.push_back(
            waitingAreaData[yellow_station[yellow_station.size() - 1]][yellow_station[yellow_station.size() -
                                                                                      2]]);
    terminals.push_back(waitingAreaData[blue_station[0]][blue_station[1]]);
    terminals.push_back(waitingAreaData[blue_station[blue_station.size() - 1]][blue_station[blue_station.size() -
                                                                                            2]]);
    AssembleLink(green_station, g);
    AssembleLink(yellow_station, y);
    AssembleLink(blue_station, b);
}

void Simulator::Simulate() {
    for (size_t tick = 0; tick < ticks; tick++) {
        UpdateAllLinks();
        // in the worst case, the platforms need to do 2 jobs, to push the curren troon to link
        // and take another incoming troon
        PushAllPlatform();

        // summon and push
        SpawnTroons();
        UpdateAllWA();
        UpdateWaitingPlatform();

        if (ticks - tick <= linesToBePrinted) {
            std::stringstream ss;
            ss << tick << ": ";
            for (auto &c: {blueTroons, greenTroons, yellowTroons}) {
                for (auto &t: c) {
                    ss << t->GenerateDescription();
                }
            }

            std::cout << ss.str() << std::endl;
        }
    }

    Clean();
}

void Simulator::SpawnTroons() {
    // g -> y -> b
    if (greenTroonCounter < maxGreenTroon) {
        auto t = new Troon{troonIdCounter, g, WAITING_AREA};
        terminals[0]->AddTroon(t);
        troonIdCounter++;
        greenTroonCounter++;
        greenTroons.insert(t);

        if (greenTroonCounter < maxGreenTroon) {
            auto x = new Troon{troonIdCounter, g, WAITING_AREA};
            terminals[1]->AddTroon(x);
            troonIdCounter++;
            greenTroonCounter++;
            greenTroons.insert(x);
        }
    }


    if (yellowTroonCounter < maxYellowTroon) {
        auto t = new Troon{troonIdCounter, y, WAITING_AREA};
        terminals[2]->AddTroon(t);
        troonIdCounter++;
        yellowTroonCounter++;
        yellowTroons.insert(t);

        if (yellowTroonCounter < maxYellowTroon) {
            auto x = new Troon{troonIdCounter, y, WAITING_AREA};
            terminals[3]->AddTroon(x);
            troonIdCounter++;
            yellowTroonCounter++;
            yellowTroons.insert(x);
        }
    }


    if (blueTroonCounter < maxBlueTroon) {
        auto t = new Troon{troonIdCounter, b, WAITING_AREA};
        terminals[4]->AddTroon(t);
        troonIdCounter++;
        blueTroonCounter++;
        blueTroons.insert(t);

        if (blueTroonCounter < maxBlueTroon) {
            auto x = new Troon{troonIdCounter, b, WAITING_AREA};
            terminals[5]->AddTroon(x);
            troonIdCounter++;
            blueTroonCounter++;
            blueTroons.insert(x);
        }
    }
}

void Simulator::PushAllPlatform() {
    for (size_t i = 0; i < compactPlatformData.size(); i++) {
        compactPlatformData[i]->ProcessPushPlatform();
    }
}

void Simulator::UpdateWaitingPlatform() {
    for (size_t i = 0; i < compactPlatformData.size(); i++) {
        compactPlatformData[i]->ProcessWaitPlatform();
    }
}

void Simulator::UpdateAllWA() {
    for (size_t i = 0; i < compactWaitingAreaData.size(); i++) {
        compactWaitingAreaData[i]->ProcessWaitingArea();
    }
}

void Simulator::UpdateAllLinks() {
    for (size_t i = 0; i < compactLinkData.size(); i++) {
        compactLinkData[i]->ProcessLink();
    }
}

void Simulator::CreateWaitingPlatformLink(size_t ns, const vector<size_t> &popularities,
                                          const adjacency_matrix &mat, const vector<string> &stationIdNameMapping) {
    for (size_t source = 0; source < ns; source++) {
        for (size_t destination = 0; destination < ns; destination++) {
            size_t distance = mat[source][destination];
            if (distance == 0) continue;

            const string &sourceStation = stationIdNameMapping[source];
            const string &destinationStation = stationIdNameMapping[destination];

            // Source and destination can be used for debugging
            auto *waitingArea = new WaitingArea{sourceStation, destinationStation};
            auto *platform = new Platform{popularities[source]};
            auto *link = new Link{distance};

            waitingArea->nextPlatform = platform;
            platform->nextLink = link;

            waitingAreaData[source][destination] = waitingArea;
            linkData[source][destination] = link;

            compactLinkData.push_back(link);
            compactPlatformData.push_back(platform);
            compactWaitingAreaData.push_back(waitingArea);
        }
    }
}

void Simulator::AssembleLink(const vector<size_t> &stationIds, Line l) {
    size_t num_trains = stationIds.size();

    // forward direction
    for (size_t i = 1; i < num_trains - 1; i++) {
        LinkStation(stationIds[i - 1], stationIds[i], stationIds[i + 1], l);
    }

    // the end of the forward direction
    LinkStation(stationIds[num_trains - 2], stationIds[num_trains - 1], stationIds[num_trains - 2], l);

    // reverse direction
    for (size_t i = num_trains - 2; i > 0; i--) {
        LinkStation(stationIds[i + 1], stationIds[i], stationIds[i - 1], l);
    }

    LinkStation(stationIds[1], stationIds[0], stationIds[1], l);
}

void
Simulator::LinkStation(const size_t &prevId, const size_t &currentId, const size_t &nextId, Line l) {
    Link *previousLink = linkData[prevId][currentId];
    WaitingArea *currentWaiting = waitingAreaData[currentId][nextId];


    switch (l) {
        case g:
            previousLink->nextWaGreen = currentWaiting;
            break;
        case b:
            previousLink->nextWaBlue = currentWaiting;
            break;
        case y:
            previousLink->nextWaYellow = currentWaiting;
            break;
    }
}

void Simulator::Clean() {
    for (auto &m: compactWaitingAreaData) {
        delete m;
    }

    for (auto &m: compactPlatformData) {
        delete m;
    }

    for (auto &m: compactLinkData) {
        delete m;
    }

    for (auto &m: {blueTroons, yellowTroons, greenTroons}) {
        for (auto &c: m) {
            delete c;
        }
    }

    for (size_t i = 0; i < num_stations; i++) {
        delete[] linkData[i]; // delete sub array
        delete[] waitingAreaData[i];
    }

    delete[] linkData; //delete outer array
    delete[] waitingAreaData;
}
