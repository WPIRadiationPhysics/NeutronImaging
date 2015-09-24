#include "EventAction.hh"

// Useful module inserts
EventAction::EventAction() : G4UserEventAction() {}
EventAction::~EventAction() {}
void EventAction::BeginOfEventAction(const G4Event* /*event*/) {}
void EventAction::EndOfEventAction(const G4Event* /*event*/) {}
