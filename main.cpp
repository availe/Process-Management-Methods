#include <iostream>
#include <algorithm>
#include <vector>
#include <iomanip>
#include <functional>
#include <map>

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

void fcfs(std::vector<Process> pv) {
    // sort all processes by arrival time
    std::sort(pv.begin(), pv.end(), [](const Process& a, const Process& b){
        return a.arrivalTime < b.arrivalTime;
    });

    int currTime = 0;
    // calculate details of all processes, are modified via reference
    for (Process& p : pv) {
        p.startTime = std::max(p.arrivalTime, currTime);
        currTime = p.finishTime = p.startTime + p.burstTime;
        p.turnaroundTime = p.finishTime - p.arrivalTime;
        p.waitTime = p.turnaroundTime - p.burstTime;
    }

    std::cout << "First Come First Serve\n";
    printResults(pv);
}

void sjn(std::vector<Process> pv) {
    // sort all processes by arrival time
    std::sort(pv.begin(), pv.end(), [](const Process &a, const Process &b) {
        return a.arrivalTime < b.arrivalTime;
    });

    int currTime = 0;
    std::vector<Process> remainingQueue = pv, completedQueue;
    while (!remainingQueue.empty()) {
        std::vector<Process> tempQueue; // is used to compare burst time of processes given that their arrivalTime is <= the currTime
        for (Process& p : remainingQueue) {
            if (p.arrivalTime <= currTime) {
                tempQueue.emplace_back(p);
            } else { break; } // vector is presorted, so we can stop searching once we hit the first process that doesn't match criteria
        }

        // if there's nothing to do, make the currTime the arrival time of the next process that arrives
        if (tempQueue.empty()) {
            auto nextProcessIT = std::min_element(remainingQueue.begin(), remainingQueue.end(), [](const Process& a, const Process& b){
                return a.arrivalTime < b.arrivalTime;
            });
            currTime = nextProcessIT->arrivalTime;
            continue;
        }

        // return the shortest process from the tempQueue
        auto shortestProcessIT = std::min_element(tempQueue.begin(), tempQueue.end(), [](const Process &a, const Process &b) {
            return a.burstTime < b.burstTime;
        });

        // we originally got the shortest process in tempQueue, now we need to find same process in remainingQueue
        auto sp = std::find_if(remainingQueue.begin(), remainingQueue.end(), [shortestProcessIT](const Process &p) {
            return p.pID == shortestProcessIT->pID;
        });

        // calculate details of the shortest process, then add it to completedQueue, and delete it from remainingQueue
        sp->startTime = std::max(sp->arrivalTime, currTime);
        currTime = sp->finishTime = sp->startTime + sp->burstTime;
        sp->turnaroundTime = sp->finishTime - sp->arrivalTime;
        sp->waitTime = sp->turnaroundTime - sp->burstTime;
        completedQueue.emplace_back(*sp);
        remainingQueue.erase(sp);
    }

    pv = completedQueue;
    std::cout << "Shortest Job Next\n";
    printResults(pv);
}

void priority(std::vector<Process> pv) {
    std::sort(pv.begin(), pv.end(), [](const Process& a, Process& b){
        return a.arrivalTime < b.arrivalTime;
    });

    // map to hold timeToLive of each process, initially their TTL is equal to their burst time
    // TTL will be decremented by one in each time quantum
    std::map<int, int> timeToLive;
    for (Process& p : pv) {
        timeToLive[p.pID] = p.burstTime;
    }

    int currTime = 0, quantum = 1;
    std::vector<Process> remainingQueue = pv, completedQueue;

    while (!remainingQueue.empty()) {
        std::vector<Process> tempQueue;
        for (Process& p : remainingQueue) {
            if (p.arrivalTime <= currTime) {
                tempQueue.emplace_back(p);
            } else { break; } // vector is presorted, so we can stop searching once we hit the first process that doesn't match criteria
        }

        // if there's nothing to do, make the currTime the arrival time of the next process that arrives
        if (tempQueue.empty()) {
            auto nextProcessIT = std::min_element(remainingQueue.begin(), remainingQueue.end(), [](const Process& a, const Process& b){
                return a.arrivalTime < b.arrivalTime;
            });
            currTime = nextProcessIT->arrivalTime;
            continue;
        }

        auto highestPriorityIT = std::min_element(tempQueue.begin(), tempQueue.end(), [](const Process& a, const Process& b) {
           return a.priority < b.priority;
        });

        // we originally got the shortest process in tempQueue, now we need to find same process in remainingQueue
        auto hp = std::find_if(remainingQueue.begin(), remainingQueue.end(), [highestPriorityIT](const Process &p) {
            return p.pID == highestPriorityIT->pID;
        });

        // if process is highestPriorityIT for first time (startTime hasn't been initialized), set startTime = currTime
        if (hp->startTime == 0) {
            hp->startTime = currTime;
        }

        int& ttl = timeToLive[highestPriorityIT->pID]; // any changes to ttl will also affect the map
        ttl -= quantum;
        currTime += quantum;

        // if highest priority process is done then calculate details and send it to completedQueue
        if (ttl == 0) {
            currTime = hp->finishTime = currTime;
            hp->turnaroundTime = hp->finishTime - hp->arrivalTime;
            hp->waitTime = hp->turnaroundTime - hp->burstTime;

            completedQueue.emplace_back(*hp);
            remainingQueue.erase(hp);
        }
    }

    pv = completedQueue;
    std::cout << "Preemptive Priority\n";
    printResults(pv);
}

int main() {
    std::vector<Process> pvo;
    pvo.emplace_back(1, 2, 3, 3);
    pvo.emplace_back(2, 2, 2, 4);
    pvo.emplace_back(3, 3, 1, 1);
    pvo.emplace_back(4, 5, 3, 2);

    fcfs(pvo);
    sjn(pvo);
    priority(pvo);
    return 0;
}