#include "Troon.h"
#include <string>

string Troon::GenerateDescription() const {
    string currentLocation;
    string currentLine;

    switch (this->location) {
        case LINK:
            currentLocation = source + "->" + destination + " ";
            break;
        case PLATFORM:
            currentLocation = source + "% ";
            break;
        case WAITING_AREA:
            currentLocation = source + "# ";
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
