#include "Troon.h"

string Troon::GenerateDescription(const vector<string> &stationIdNameMapping) const {
    string currentLocation;
    string currentLine;
    const string& stringSource = stationIdNameMapping[source];
    const string& stringDestination = stationIdNameMapping[destination];

    switch (this->location) {
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

    switch (this->line) {
        case b:
            currentLine = "b";
            break;
        case y:
            currentLine = "y";
            break;
        case g:
            currentLine = "g";
            break;
    }

    return currentLine + std::to_string(id) + "-" + currentLocation;
}
