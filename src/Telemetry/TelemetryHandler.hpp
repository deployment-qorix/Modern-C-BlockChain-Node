#pragma once
#include <string>

class TelemetryHandler {
public:
    explicit TelemetryHandler(const std::string& source);
    void loadData();
    void printLatest() const;

private:
    std::string _source;
    std::string _latestData;
};
