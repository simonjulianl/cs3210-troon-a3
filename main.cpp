#include <fstream>
#include <iostream>
#include <vector>
#include <queue>
#include <map>
#include <algorithm>

using namespace std;

using adjacency_matrix = std::vector<std::vector<size_t>>;

struct Troon {
    size_t arrivalTime;
    size_t id;
    size_t src;
    size_t dest;
    size_t line; // 0=G, 1=Y, 2=B

    bool operator<(const struct Troon &other) const {
        if (other.arrivalTime == arrivalTime) {
            return other.id < id;
        } else {
            return other.arrivalTime < arrivalTime;
        }
    }
};

struct staticLinkState {
    size_t popularity = 0;

    size_t nextLinkGreen = 0;
    size_t nextLinkYellow = 0;
    size_t nextLinkBlue = 0;

    size_t srcId = 0;
    size_t destId = 0;

    size_t id = 0;
};

struct dynamicLinkState {
    priority_queue<Troon> waitingArea;
    size_t platformCounter = 0;
    size_t linkCounter = 0;
};

void convertStationNamesToId(const vector<string> &station_names, vector<size_t> &station_id);

void populateStaticData(const vector<size_t> &popularities,
                        const vector<size_t> &station_id, map<pair<size_t, size_t>, size_t> &tempLinkMapping);

void assembleGreenLine(const vector<size_t> &green_station_id, map<pair<size_t, size_t>, size_t> &tempLinkMapping);

void assembleYellowLine(const vector<size_t> &yellow_station_id, map<pair<size_t, size_t>, size_t> &tempLinkMapping);

void assembleBlueLine(const vector<size_t> &blue_station_id, map<pair<size_t, size_t>, size_t> &tempLinkMapping);

void initialization(size_t num_stations, const vector<string> &station_names, const vector<size_t> &popularities,
                    const vector<string> &green_station_names, const vector<string> &yellow_station_names,
                    const vector<string> &blue_station_names);

// mapping
map<string, uint32_t> stationNameIdMapping;
vector<string> stationIdNameMapping;

// link states
size_t graphCounter = 0;
vector<staticLinkState> graphState;
vector<dynamicLinkState> graphStateDynamic; // to be initialized in each node

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
            blue_station_names
    );

#ifdef DEBUG

    for (auto &c: graphState) {
        cout << c.id << " " << c.srcId << " " << c.destId << " Y : " << c.nextLinkYellow << " G : " << c.nextLinkGreen
             << "  B : " << c.nextLinkBlue << endl;
    }
#endif

}

void initialization(size_t num_stations, const vector<string> &station_names, const vector<size_t> &popularities,
                    const vector<string> &green_station_names, const vector<string> &yellow_station_names,
                    const vector<string> &blue_station_names) {// Create station mapping
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
    populateStaticData(popularities, green_station_id, tempLinkMapping);
    populateStaticData(popularities, yellow_station_id, tempLinkMapping);
    populateStaticData(popularities, blue_station_id, tempLinkMapping);

    // reverse mapping
    std::reverse(green_station_id.begin(), green_station_id.end());
    std::reverse(yellow_station_id.begin(), yellow_station_id.end());
    std::reverse(blue_station_id.begin(), blue_station_id.end());

    // populate reverse mapping
    populateStaticData(popularities, green_station_id, tempLinkMapping);
    populateStaticData(popularities, yellow_station_id, tempLinkMapping);
    populateStaticData(popularities, blue_station_id, tempLinkMapping);

    // assemble the graph
    assembleGreenLine(green_station_id, tempLinkMapping);
    std::reverse(green_station_id.begin(), green_station_id.end());
    assembleGreenLine(green_station_id, tempLinkMapping);

    assembleYellowLine(yellow_station_id, tempLinkMapping);
    std::reverse(yellow_station_id.begin(), yellow_station_id.end());
    assembleYellowLine(yellow_station_id, tempLinkMapping);

    assembleBlueLine(blue_station_id, tempLinkMapping);
    std::reverse(blue_station_id.begin(), blue_station_id.end());
    assembleBlueLine(blue_station_id, tempLinkMapping);
}

void assembleGreenLine(const vector<size_t> &green_station_id, map<pair<size_t, size_t>, size_t> &tempLinkMapping) {
    size_t currentLink, nextLink, nextTwoStation;
    for (size_t i = 0; i < green_station_id.size() - 1; i++) {
        size_t currentStation = green_station_id[i];
        size_t nextStation = green_station_id[i + 1];
        currentLink = tempLinkMapping[std::make_pair(currentStation, nextStation)];

        if (i == green_station_id.size() - 2) { // terminal
            nextLink = tempLinkMapping[std::make_pair(nextStation, currentStation)];
        } else {
            nextTwoStation = green_station_id[i + 2];
            nextLink = tempLinkMapping[std::make_pair(nextStation, nextTwoStation)];
        }

        graphState[currentLink].nextLinkGreen = nextLink;
    }
}

void assembleYellowLine(const vector<size_t> &yellow_station_id, map<pair<size_t, size_t>, size_t> &tempLinkMapping) {
    size_t currentLink, nextLink, nextTwoStation;
    for (size_t i = 0; i < yellow_station_id.size() - 1; i++) {
        size_t currentStation = yellow_station_id[i];
        size_t nextStation = yellow_station_id[i + 1];
        currentLink = tempLinkMapping[std::make_pair(currentStation, nextStation)];

        if (i == yellow_station_id.size() - 2) { // terminal
            nextLink = tempLinkMapping[std::make_pair(nextStation, currentStation)];
        } else {
            nextTwoStation = yellow_station_id[i + 2];
            nextLink = tempLinkMapping[std::make_pair(nextStation, nextTwoStation)];
        }

        graphState[currentLink].nextLinkYellow = nextLink;
    }
}

void assembleBlueLine(const vector<size_t> &blue_station_id, map<pair<size_t, size_t>, size_t> &tempLinkMapping) {
    size_t currentLink, nextLink, nextTwoStation;
    for (size_t i = 0; i < blue_station_id.size() - 1; i++) {
        size_t currentStation = blue_station_id[i];
        size_t nextStation = blue_station_id[i + 1];
        currentLink = tempLinkMapping[std::make_pair(currentStation, nextStation)];

        if (i == blue_station_id.size() - 2) { // terminal
            nextLink = tempLinkMapping[std::make_pair(nextStation, currentStation)];
        } else {
            nextTwoStation = blue_station_id[i + 2];
            nextLink = tempLinkMapping[std::make_pair(nextStation, nextTwoStation)];
        }

        graphState[currentLink].nextLinkYellow = nextLink;
    }
}

void populateStaticData(
        const vector<size_t> &popularities,
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
