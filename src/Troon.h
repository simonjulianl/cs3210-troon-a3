#ifndef CS3210_A1_A1_A0196678A_A0219768L_TROON_H
#define CS3210_A1_A1_A0196678A_A0219768L_TROON_H

#include <string>
#include <utility>
#include "Constants.h"

using std::string;

class Troon {
public:
    int id;
    Line line;
    string source;
    string destination;
    Location location;

    explicit Troon(
            int id,
            Line line,
            Location location
    ) : id{id}, line{line}, location{location} {};

    void setSourceDestination(string sourceP, string destinationP) {
        source = std::move(sourceP);
        destination = std::move(destinationP);
    }

    void setLocation(Location l) {
        location = l;
    }

    string GenerateDescription() const;
};


#endif //CS3210_A1_A1_A0196678A_A0219768L_TROON_H
