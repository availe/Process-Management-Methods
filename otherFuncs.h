#ifndef PROCESS_MANAGEMENT_METHODS_OTHERFUNCS_H
#define PROCESS_MANAGEMENT_METHODS_OTHERFUNCS_H
#include "iostream"
#include "vector"
#include "iomanip"

struct Process {
    int pID;
    int arrivalTime;
    int priority;
    int burstTime;
    int startTime{};
    int finishTime{};
    int turnaroundTime{};
    int waitTime{};

    Process(const int pID, const int arrivalTime, const int priority, const int burstTime)
            : pID(pID), arrivalTime(arrivalTime), priority(priority), burstTime(burstTime){}
};

void printResults(std::vector<Process>& pv);

#endif //PROCESS_MANAGEMENT_METHODS_OTHERFUNCS_H
