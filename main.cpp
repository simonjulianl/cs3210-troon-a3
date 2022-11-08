#include <fstream>
#include <iostream>
#include <vector>
#include <queue>
#include <map>
#include <algorithm>
#include <set>
#include <sstream>

using namespace std;

using adjacency_matrix = std::vector<std::vector<size_t>>;

#define WAITING_AREA 0
#define PLATFORM 1
#define LINK 2

#define GREEN 0
#define YELLOW 1
#define BLUE 2

struct Troon {
    size_t arrivalTime = 0;
    size_t id = 0;
    size_t src = 0;
    size_t dest = 0;
    size_t location = 0;
    size_t line = 0;
};

struct staticLinkState {
    size_t popularity = 0;
    size_t distance = 0;

    size_t nextLinkGreen = 0;
    size_t nextLinkYellow = 0;
    size_t nextLinkBlue = 0;

    size_t srcId = 0;
    size_t destId = 0;

    size_t id = 0;
};

struct TroonComparison {
    bool operator()(const Troon *a, const Troon *b) const {
        if (a->arrivalTime == b->arrivalTime) {
            return a->id < b->id;
        } else {
            return a->arrivalTime < b->arrivalTime;
        }
    }
};

class dynamicLinkState { // per node
public:
    staticLinkState state;

    size_t platformCounter = 0;
    size_t linkCounter = 0;
    size_t linkDistance = 0;

    Troon *troonAtPlatform = new Troon();
    Troon *troonAtLink = new Troon();

    priority_queue<Troon *, std::deque<Troon *>, TroonComparison> *waitingArea = new priority_queue<Troon *, std::deque<Troon *>, TroonComparison>();
};

void convertStationNamesToId(const vector<string> &station_names, vector<size_t> &station_id);

void populateStaticData(const vector<size_t> &popularities, const adjacency_matrix &mat,
                        const vector<size_t> &station_id, map<pair<size_t, size_t>, size_t> &tempLinkMapping);

void assembleGreenLine(const vector<size_t> &green_station_id, map<pair<size_t, size_t>, size_t> &tempLinkMapping,
                       bool isReverse);

void assembleYellowLine(const vector<size_t> &yellow_station_id, map<pair<size_t, size_t>, size_t> &tempLinkMapping,
                        bool isReverse);

void assembleBlueLine(const vector<size_t> &blue_station_id, map<pair<size_t, size_t>, size_t> &tempLinkMapping,
                      bool isReverse);

void initialization(size_t num_stations, const vector<string> &station_names, const vector<size_t> &popularities,
                    const vector<string> &green_station_names, const vector<string> &yellow_station_names,
                    const vector<string> &blue_station_names, const adjacency_matrix &mat);

void processLink(dynamicLinkState *dstate, size_t tick);

void processPushPlatform(dynamicLinkState *dstate);

void processWaitingArea(dynamicLinkState *dstate);

void processWaitPlatform(dynamicLinkState *dstate);

string generateTroonDescription(const Troon &t);

void spawnTroons(vector<dynamicLinkState *> &graphStateDynamic, size_t num_green_trains, size_t num_yellow_trains,
                 size_t num_blue_trains, size_t t);

void printTroons(vector<dynamicLinkState *> &graphStateDynamic, size_t ticks, size_t num_lines, size_t t);

// mapping
map<string, uint32_t> stationNameIdMapping;
vector<string> stationIdNameMapping;

// link states
size_t graphCounter = 0;
vector<staticLinkState> graphState;
size_t terminalGreenForward;
size_t terminalGreenReverse;
size_t terminalYellowForward;
size_t terminalYellowReverse;
size_t terminalBlueForward;
size_t terminalBlueReverse;

// troon state
size_t greenTroonCounter = 0;
size_t blueTroonCounter = 0;
size_t yellowTroonCounter = 0;
size_t troonIdCounter = 0;

struct TroonLexicographyComparison {
    bool operator()(const Troon *a, const Troon *b) const {
        return generateTroonDescription(*a) < generateTroonDescription(*b);
    }
};

void simulate(
        size_t num_stations,
        const vector<string> &station_names,
        const std::vector<size_t> &popularities,
        const adjacency_matrix &mat,
        const vector<string> &green_station_names,
        const vector<string> &yellow_station_names,
        const vector<string> &blue_station_names, size_t ticks,
        size_t num_green_trains, size_t num_yellow_trains,
        size_t num_blue_trains, size_t num_lines,
        int argc,
        char **argv
) {

    // Initialization
    initialization(
            num_stations,
            station_names,
            popularities,
            green_station_names,
            yellow_station_names,
            blue_station_names,
            mat
    );

#ifdef DEBUG
    for (auto &c: graphState) {
        cout << c.id << " " << c.srcId << " " << c.destId << " Y : " << c.nextLinkYellow << " G : " << c.nextLinkGreen
             << "  B : " << c.nextLinkBlue << " Distance: " << c.distance << endl;
    }
    cout << terminalGreenForward << " " << terminalGreenReverse << endl;
    cout << terminalYellowForward << " " << terminalYellowReverse << endl;
    cout << terminalBlueForward << " " << terminalBlueReverse << endl;
#endif

    // initialize per node
    vector<dynamicLinkState *> graphStateDynamic(graphState.size()); // to be initialized in each node

    for (size_t i = 0; i < graphState.size(); i++) {
        auto *d = new dynamicLinkState();
        d->state = graphState[i];

        graphStateDynamic[i] = d;
    }

    for (size_t t = 0; t < ticks; t++) {
//        for (size_t i = 0; i < graphStateDynamic.size(); i++) {
//            processLink(graphStateDynamic[i], t);
//            processPushPlatform(graphStateDynamic[i]);
//        }

        // master only
        spawnTroons(graphStateDynamic, num_green_trains, num_yellow_trains, num_blue_trains, t);
        cout << "testing manual: " << graphStateDynamic[terminalGreenForward]->waitingArea->top()->id << endl;
        printf("Address of x is %p\n", (void *) graphStateDynamic[terminalGreenForward]->waitingArea);
        printf("Address of x is %p\n", (void *) graphStateDynamic[terminalGreenForward]->waitingArea->top());

        cout << "testing manual: " << graphStateDynamic[terminalGreenReverse]->waitingArea->top()->id << endl;
        printf("Address of y is %p\n", (void *) graphStateDynamic[terminalGreenReverse]->waitingArea);
        printf("Address of y is %p\n", (void *) graphStateDynamic[terminalGreenReverse]->waitingArea->top());
        cout << "testing manual: " << graphStateDynamic[terminalYellowForward]->waitingArea->top()->id << endl;
        cout << "testing manual: " << graphStateDynamic[terminalYellowReverse]->waitingArea->top()->id << endl;
        cout << "testing manual: " << graphStateDynamic[terminalBlueForward]->waitingArea->top()->id << endl;
        cout << "testing manual: " << graphStateDynamic[terminalBlueReverse]->waitingArea->top()->id << endl;

        for (auto c: graphStateDynamic) {
            // per node in OpenMPI
            processWaitingArea(c);
            processWaitPlatform(c);
        }

        // master only
        printTroons(graphStateDynamic, ticks, num_lines, t);
    }
}

void printTroons(vector<dynamicLinkState *> &graphStateDynamic, size_t ticks, size_t num_lines, size_t t) {
    set<Troon *, TroonLexicographyComparison> troons;

    // TODO: Collect all the troons from all the nodes
    for (auto &c: graphStateDynamic) {
        if (c->troonAtLink != nullptr) {
            troons.insert(c->troonAtLink);
        }

        if (c->troonAtPlatform != nullptr) {
            troons.insert(c->troonAtPlatform);
        }

        auto *new_pq = new priority_queue<Troon *, std::deque<Troon *>, TroonComparison>();

        while (!c->waitingArea->empty()) {
            Troon *troon = c->waitingArea->top();
            new_pq->push(troon);
            troons.insert(troon);
            c->waitingArea->pop();
        }

        delete c->waitingArea;
        c->waitingArea = new_pq;
    }

    if (ticks - t <= num_lines) {
        stringstream ss;
        ss << t << ": ";
        for (auto &troon: troons) {
            ss << generateTroonDescription(*troon);
        }

        cout << ss.str() << endl;
    }
}

void spawnTroons(vector<dynamicLinkState *> &graphStateDynamic, size_t num_green_trains, size_t num_yellow_trains,
                 size_t num_blue_trains, size_t t) {
    // TODO: Send these troons to other nodes
    if (greenTroonCounter < num_green_trains) {
        Troon *troon = new Troon{
                t,
                troonIdCounter++,
                graphStateDynamic[terminalGreenForward]->state.srcId,
                graphStateDynamic[terminalGreenForward]->state.destId,
                WAITING_AREA,
                GREEN
        };

        graphStateDynamic[terminalGreenForward]->waitingArea->push(troon);
        greenTroonCounter++;
    }
    printf("Address of x is %p\n", (void *) graphStateDynamic[terminalGreenForward]->waitingArea);
    printf("Address of x is %p\n", (void *) graphStateDynamic[terminalGreenForward]->waitingArea->top());

    cout << "testing manual: " << graphStateDynamic[terminalGreenForward]->waitingArea->top()->id << endl;

    if (greenTroonCounter < num_green_trains) {
        Troon *troon = new Troon{
                t,
                troonIdCounter++,
                graphStateDynamic[terminalGreenReverse]->state.srcId,
                graphStateDynamic[terminalGreenReverse]->state.destId,
                WAITING_AREA,
                GREEN
        };

        graphStateDynamic[terminalGreenReverse]->waitingArea->push(troon);
        greenTroonCounter++;
    }

    printf("Address of y is %p\n", (void *) graphStateDynamic[terminalGreenReverse]->waitingArea);
    printf("Address of y is %p\n", (void *) graphStateDynamic[terminalGreenReverse]->waitingArea->top());

    cout << "testing manual: " << graphStateDynamic[terminalGreenReverse]->waitingArea->top()->id << endl;

    if (yellowTroonCounter < num_yellow_trains) {
        auto *troon = new Troon{
                t,
                troonIdCounter++,
                graphStateDynamic[terminalYellowForward]->state.srcId,
                graphStateDynamic[terminalYellowForward]->state.destId,
                WAITING_AREA,
                YELLOW
        };

        graphStateDynamic[terminalYellowForward]->waitingArea->push(troon);
        yellowTroonCounter++;
    }

    cout << "testing manual: " << graphStateDynamic[terminalYellowForward]->waitingArea->top()->id << endl;

    if (yellowTroonCounter < num_yellow_trains) {
        auto *troon = new Troon{
                t,
                troonIdCounter++,
                graphStateDynamic[terminalYellowReverse]->state.srcId,
                graphStateDynamic[terminalYellowReverse]->state.destId,
                WAITING_AREA,
                YELLOW
        };

        graphStateDynamic[terminalYellowReverse]->waitingArea->push(troon);
        yellowTroonCounter++;
    }

    cout << "testing manual: " << graphStateDynamic[terminalYellowReverse]->waitingArea->top()->id << endl;

    if (blueTroonCounter < num_blue_trains) {
        auto *troon = new Troon{
                t,
                troonIdCounter++,
                graphStateDynamic[terminalBlueForward]->state.srcId,
                graphStateDynamic[terminalBlueForward]->state.destId,
                WAITING_AREA,
                BLUE
        };

        graphStateDynamic[terminalBlueForward]->waitingArea->push(troon);
        blueTroonCounter++;
    }

    cout << "testing manual: " << graphStateDynamic[terminalBlueForward]->waitingArea->top()->id << endl;

    if (blueTroonCounter < num_blue_trains) {
        auto *troon = new Troon{
                t,
                troonIdCounter++,
                graphStateDynamic[terminalBlueReverse]->state.srcId,
                graphStateDynamic[terminalBlueReverse]->state.destId,
                WAITING_AREA,
                BLUE
        };

        graphStateDynamic[terminalBlueReverse]->waitingArea->push(troon);
        blueTroonCounter++;
    }

    cout << "testing manual: " << graphStateDynamic[terminalBlueReverse]->waitingArea->top()->id << endl << endl;
    printf("Address of x is %p\n", (void *) graphStateDynamic[terminalGreenReverse]->waitingArea);
    printf("Address of x is %p\n", (void *) graphStateDynamic[terminalGreenReverse]->waitingArea->top());
}

void processLink(vector<dynamicLinkState *> &graphStateDynamic, dynamicLinkState *dstate, size_t tick) {
    if (dstate->troonAtLink == nullptr) {
        dstate->linkCounter++;
        return;
    }

    // TODO: send and receive the troon to the necessary node for OpenMPI
    if (dstate->linkDistance == (dstate->state.distance - 1)) {
        Troon *currTroon = dstate->troonAtLink;
        currTroon->arrivalTime = tick;
        currTroon->location = WAITING_AREA;

        switch (currTroon->line) {
            case GREEN: // G
                currTroon->src = graphStateDynamic[dstate->state.nextLinkGreen]->state.srcId;
                currTroon->dest = graphStateDynamic[dstate->state.nextLinkGreen]->state.destId;
                graphStateDynamic[dstate->state.nextLinkGreen]->waitingArea->push(currTroon);
                break;
            case YELLOW: // Y
                currTroon->src = graphStateDynamic[dstate->state.nextLinkYellow]->state.srcId;
                currTroon->dest = graphStateDynamic[dstate->state.nextLinkYellow]->state.destId;
                graphStateDynamic[dstate->state.nextLinkYellow]->waitingArea->push(currTroon);
                break;
            case BLUE: // B
                currTroon->src = graphStateDynamic[dstate->state.nextLinkBlue]->state.srcId;
                currTroon->dest = graphStateDynamic[dstate->state.nextLinkBlue]->state.destId;
                graphStateDynamic[dstate->state.nextLinkBlue]->waitingArea->push(currTroon);
                break;

        }

        dstate->linkCounter = 0;
        dstate->linkDistance = 0;
        dstate->troonAtLink = nullptr;
    } else {
        dstate->linkDistance++;
    }

}

void processPushPlatform(dynamicLinkState *dstate) {
    size_t maxCounter = dstate->state.popularity + 2;
    bool isReadyToGo = dstate->platformCounter >= maxCounter;
    bool isLinkSafeToEnter = dstate->troonAtLink == nullptr && dstate->linkCounter >= 1;

    if (!isReadyToGo || !isLinkSafeToEnter || !dstate->troonAtPlatform) {
        return;
    }

    dstate->platformCounter = 0;
    dstate->troonAtLink = dstate->troonAtPlatform;
    dstate->troonAtLink->location = LINK;
    dstate->troonAtPlatform = nullptr;
}

void processWaitingArea(dynamicLinkState *dstate) {
    bool hasTroonAtPlatform = dstate->troonAtPlatform != nullptr;
    if (dstate->waitingArea->empty() || hasTroonAtPlatform) {
        return;
    }

    Troon *troon = dstate->waitingArea->top();
    troon->location = PLATFORM;
    dstate->troonAtPlatform = troon;
    dstate->waitingArea->pop();
}

void processWaitPlatform(dynamicLinkState *dstate) {
    if (dstate->troonAtPlatform != nullptr) {
        dstate->platformCounter++;
    }
}

void initialization(size_t num_stations, const vector<string> &station_names, const vector<size_t> &popularities,
                    const vector<string> &green_station_names, const vector<string> &yellow_station_names,
                    const vector<string> &blue_station_names, const adjacency_matrix &mat) {// Create station mapping
    for (size_t i = 0; i < num_stations; i++) {
        const string &stationName = station_names[i];
        stationNameIdMapping[stationName] = i;
        stationIdNameMapping.push_back(stationName);
    }

    std::vector<size_t> green_station_id;
    std::vector<size_t> yellow_station_id;
    std::vector<size_t> blue_station_id;

    convertStationNamesToId(green_station_names, green_station_id);
    convertStationNamesToId(yellow_station_names, yellow_station_id);
    convertStationNamesToId(blue_station_names, blue_station_id);

    std::map<std::pair<size_t, size_t>, size_t> tempLinkMapping;
    // initialize static link mapping
    // populate line green forward direction
    populateStaticData(popularities, mat, green_station_id, tempLinkMapping);
    populateStaticData(popularities, mat, yellow_station_id, tempLinkMapping);
    populateStaticData(popularities, mat, blue_station_id, tempLinkMapping);

    // reverse mapping
    std::reverse(green_station_id.begin(), green_station_id.end());
    std::reverse(yellow_station_id.begin(), yellow_station_id.end());
    std::reverse(blue_station_id.begin(), blue_station_id.end());

    // populate reverse mapping
    populateStaticData(popularities, mat, green_station_id, tempLinkMapping);
    populateStaticData(popularities, mat, yellow_station_id, tempLinkMapping);
    populateStaticData(popularities, mat, blue_station_id, tempLinkMapping);

    // assemble the graph
    assembleGreenLine(green_station_id, tempLinkMapping, true);
    std::reverse(green_station_id.begin(), green_station_id.end());
    assembleGreenLine(green_station_id, tempLinkMapping, false);

    assembleYellowLine(yellow_station_id, tempLinkMapping, true);
    std::reverse(yellow_station_id.begin(), yellow_station_id.end());
    assembleYellowLine(yellow_station_id, tempLinkMapping, false);

    assembleBlueLine(blue_station_id, tempLinkMapping, true);
    std::reverse(blue_station_id.begin(), blue_station_id.end());
    assembleBlueLine(blue_station_id, tempLinkMapping, false);
}

void assembleGreenLine(const vector<size_t> &green_station_id, map<pair<size_t, size_t>, size_t> &tempLinkMapping,
                       bool isReverse) {
    size_t currentLink, nextLink, nextTwoStation;
    for (size_t i = 0; i < green_station_id.size() - 1; i++) {
        size_t currentStation = green_station_id[i];
        size_t nextStation = green_station_id[i + 1];
        currentLink = tempLinkMapping[std::make_pair(currentStation, nextStation)];

        if (i == green_station_id.size() - 2) { // terminal
            nextLink = tempLinkMapping[std::make_pair(nextStation, currentStation)];
            if (isReverse) {
                terminalGreenForward = nextLink;
            } else {
                terminalGreenReverse = nextLink;
            }
        } else {
            nextTwoStation = green_station_id[i + 2];
            nextLink = tempLinkMapping[std::make_pair(nextStation, nextTwoStation)];
        }

        graphState[currentLink].nextLinkGreen = nextLink;
    }
}

void assembleYellowLine(const vector<size_t> &yellow_station_id, map<pair<size_t, size_t>, size_t> &tempLinkMapping,
                        bool isReverse) {
    size_t currentLink, nextLink, nextTwoStation;
    for (size_t i = 0; i < yellow_station_id.size() - 1; i++) {
        size_t currentStation = yellow_station_id[i];
        size_t nextStation = yellow_station_id[i + 1];
        currentLink = tempLinkMapping[std::make_pair(currentStation, nextStation)];

        if (i == yellow_station_id.size() - 2) { // terminal
            nextLink = tempLinkMapping[std::make_pair(nextStation, currentStation)];
            if (isReverse) {
                terminalYellowForward = nextLink;
            } else {
                terminalYellowReverse = nextLink;
            }
        } else {
            nextTwoStation = yellow_station_id[i + 2];
            nextLink = tempLinkMapping[std::make_pair(nextStation, nextTwoStation)];
        }

        graphState[currentLink].nextLinkYellow = nextLink;
    }
}

void assembleBlueLine(const vector<size_t> &blue_station_id, map<pair<size_t, size_t>, size_t> &tempLinkMapping,
                      bool isReverse) {
    size_t currentLink, nextLink, nextTwoStation;
    for (size_t i = 0; i < blue_station_id.size() - 1; i++) {
        size_t currentStation = blue_station_id[i];
        size_t nextStation = blue_station_id[i + 1];
        currentLink = tempLinkMapping[std::make_pair(currentStation, nextStation)];

        if (i == blue_station_id.size() - 2) { // terminal
            nextLink = tempLinkMapping[std::make_pair(nextStation, currentStation)];
            if (isReverse) {
                terminalBlueForward = nextLink;
            } else {
                terminalBlueReverse = nextLink;
            }
        } else {
            nextTwoStation = blue_station_id[i + 2];
            nextLink = tempLinkMapping[std::make_pair(nextStation, nextTwoStation)];
        }

        graphState[currentLink].nextLinkYellow = nextLink;
    }
}

void populateStaticData(
        const vector<size_t> &popularities,
        const adjacency_matrix &mat,
        const vector<size_t> &station_id,
        map<pair<size_t, size_t>, size_t> &tempLinkMapping
) {
    for (size_t i = 0; i < station_id.size() - 1; i++) {
        size_t currentStation = station_id[i];
        size_t nextStation = station_id[i + 1];

        if (tempLinkMapping.find(std::make_pair(currentStation, nextStation)) == tempLinkMapping.end()) {
            staticLinkState s = {};
            s.popularity = popularities[currentStation];
            s.srcId = currentStation;
            s.destId = nextStation;
            s.distance = mat[currentStation][nextStation];
            s.id = graphCounter++;

            tempLinkMapping[std::make_pair(currentStation, nextStation)] = s.id;
            graphState.push_back(s);
        }
    }
}

void convertStationNamesToId(const vector<string> &station_names, vector<size_t> &station_id) {
    for (const auto &green_station_name: station_names) {
        station_id.push_back(stationNameIdMapping[green_station_name]);
    }
}

string generateTroonDescription(const Troon &t) {
    string currentLocation;
    string currentLine;
    const string &stringSource = stationIdNameMapping[t.src];
    const string &stringDestination = stationIdNameMapping[t.dest];

    switch (t.location) {
        case LINK:
            currentLocation = stringSource + "->" + stringDestination + " ";
            break;
        case PLATFORM:
            currentLocation = stringSource + "% ";
            break;
        case WAITING_AREA:
            currentLocation = stringSource + "# ";
            break;
    }

    switch (t.line) {
        case GREEN:
            currentLine = "g";
            break;
        case YELLOW:
            currentLine = "y";
            break;
        case BLUE:
            currentLine = "b";
            break;
    }

    return currentLine + std::to_string(t.id) + "-" + currentLocation;
}

vector<string> extract_station_names(string &line) {
    constexpr char space_delimiter = ' ';
    vector<string> stations{};
    line += ' ';
    size_t pos;
    while ((pos = line.find(space_delimiter)) != string::npos) {
        stations.push_back(line.substr(0, pos));
        line.erase(0, pos + 1);
    }
    return stations;
}

int main(int argc, char **argv) {
    using std::cout;

    if (argc < 2) {
        std::cerr << argv[0] << " <input_file>\n";
        std::exit(1);
    }

    std::ifstream ifs(argv[1], std::ios_base::in);
    if (!ifs.is_open()) {
        std::cerr << "Failed to open " << argv[1] << '\n';
        std::exit(2);
    }

    // Read S
    size_t S;
    ifs >> S;

    // Read station names.
    string station;
    std::vector<string> station_names{};
    station_names.reserve(S);
    for (size_t i = 0; i < S; ++i) {
        ifs >> station;
        station_names.emplace_back(station);
    }

    // Read P popularity
    size_t p;
    std::vector<size_t> popularities{};
    popularities.reserve(S);
    for (size_t i = 0; i < S; ++i) {
        ifs >> p;
        popularities.emplace_back(p);
    }

    // Form adjacency mat
    adjacency_matrix mat(S, std::vector<size_t>(S));
    for (size_t src{}; src < S; ++src) {
        for (size_t dst{}; dst < S; ++dst) {
            ifs >> mat[src][dst];
        }
    }

    ifs.ignore();

    string stations_buf;

    std::getline(ifs, stations_buf);
    auto green_station_names = extract_station_names(stations_buf);

    std::getline(ifs, stations_buf);
    auto yellow_station_names = extract_station_names(stations_buf);

    std::getline(ifs, stations_buf);
    auto blue_station_names = extract_station_names(stations_buf);

    // N time ticks
    size_t N;
    ifs >> N;

    // g,y,b number of trains per line
    size_t g, y, b;
    ifs >> g;
    ifs >> y;
    ifs >> b;

    size_t num_lines;
    ifs >> num_lines;

    simulate(S, station_names, popularities, mat, green_station_names,
             yellow_station_names, blue_station_names, N, g, y, b, num_lines, argc, argv);

    return 0;
}
