#include "PlatformUnit.h"

using namespace std;

void Platform::ProcessWaitPlatform() {
    if (HasTroon()) {
        currentCounter += 1;
    }
}


void Platform::ProcessPushPlatform() {
    bool isReadyToGo = currentCounter >= maxCounter;

    if (!currentTroon || !isReadyToGo || !nextLink->SafeToGo()) return;

    currentCounter = 0;
    nextLink->AddTroon(currentTroon);
    currentTroon = nullptr;
}

void Platform::AddTroon(Troon *t) {
    currentTroon = t;
    t->setLocation(PLATFORM);
}

bool Platform::HasTroon() const {
    return currentTroon != nullptr;
}

void Link::ProcessLink() {
    if (!currentTroon) {
        currentCounter++;
        return;
    }

    if (currentDistance == (actualDistance - 1)) {
        switch (currentTroon->line) {
            case g:
                nextWaGreen->AddTroon(currentTroon);
                break;
            case y:
                nextWaYellow->AddTroon(currentTroon);
                break;
            case b:
                nextWaBlue->AddTroon(currentTroon);
                break;
        }
        currentTroon = nullptr;
        currentCounter = 0;
        currentDistance = 0;
    } else {
        currentDistance++;
    }
}

void Link::AddTroon(Troon *t) {
    currentTroon = t;
    currentTroon->setLocation(LINK);
}

bool Link::SafeToGo() const {
    return currentCounter >= 1 && currentTroon == nullptr;
}

void WaitingArea::AddTroon(Troon *troon) {
    troon->setSourceDestination(source, destination);
    troon->setLocation(WAITING_AREA);

    troonPq.push(troon);
}

void WaitingArea::ProcessWaitingArea() {
    while (!troonPq.empty()) {
        Troon *temp = troonPq.top();
        troonQ.push_back(temp);
        troonPq.pop();
    }
    if (troonQ.empty() || nextPlatform->HasTroon()) return;

    Troon *top = troonQ.front();
    nextPlatform->AddTroon(top);
    troonQ.pop_front();
}
