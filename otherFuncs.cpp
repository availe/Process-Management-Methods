#include "otherFuncs.h"

void printResults(std::vector<Process>& pv) {
    int width = 20;
    std::sort(pv.begin(), pv.end(), [](const Process& a, const Process& b) {
        return a.pID < b.pID;
    });

    // the format we'll be using to print out the tables
    std::vector<std::pair<std::string, const int Process::*>> properties = {
            {"pID:", &Process::pID},
            {"Arrival Time", &Process::arrivalTime},
            {"Priority", &Process::priority},
            {"Burst Time", &Process::burstTime},
            {"Start Time", &Process::startTime},
            {"Finish Time", &Process::finishTime},
            {"TAT", &Process::turnaroundTime},
            {"Wait Time", &Process::waitTime}

    };

    // print header row of table
    for (const auto& prop: properties) {
        std::cout << std::setw(width) << std:: left << prop.first;
    }
    std::cout << "\n";

    // print data rows of table
    for (const auto& process: pv) {
        for (const auto& prop: properties) {
            std::cout << std::setw(width) << std::left << process.*(prop.second);
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}