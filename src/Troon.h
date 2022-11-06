#ifndef CS3210_A3_A3_A0196678A_A0219768L_TROON_H
#define CS3210_A3_A3_A0196678A_A0219768L_TROON_H

#include <string>
#include <utility>
#include <vector>
#include "Constants.h"

using std::string;
using std::vector;

class Troon {
public:
    int id;
    Direction direction;
    Line line;
    size_t source;
    size_t destination;
    Location location;

    explicit Troon(
            int id,
            Line line,
            Location location,
            Direction direction
    ) : id{id}, direction{direction}, line{line}, location{location} {};

    void setSourceDestination(size_t sourceP, size_t destinationP) {
        source = sourceP;
        destination = destinationP;
    }

    void setLocation(Location l) {
        location = l;
    }

    void setDirection(Direction d) {
        direction = d;
    }

    string GenerateDescription(const vector<string> &stationIdNameMapping) const;
};

struct TroonLexicographyComparison {
    bool operator()(const Troon *a, const Troon *b) const {
        return std::to_string(a->id) < std::to_string(b->id);
    }
};


#endif //CS3210_A3_A3_A0196678A_A0219768L_TROON_H
