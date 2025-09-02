#include "TelemetryHandler.hpp"
#include <iostream>

TelemetryHandler::TelemetryHandler(const std::string& source)
    : _source(source) {}

void TelemetryHandler::loadData() {
    _latestData = "Telemetry data loaded from: " + _source;
}

void TelemetryHandler::printLatest() const {
    std::cout << "[Telemetry] " << _latestData << std::endl;
}
