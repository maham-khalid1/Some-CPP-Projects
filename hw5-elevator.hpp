// starter code for HW5 - elevator problem
#ifndef HW5_ELEVATOR_HPP
#define HW5_ELEVATOR_HPP

#include <iostream>
#include <thread>
#include <mutex>
#include <queue>
#include <chrono>
#include <random>
#include <atomic>
#include <vector>
#include <limits>

using namespace std;

const int NUM_FLOORS = 50;
const int NUM_ELEVATORS = 6;
const int MAX_OCCUPANCY = 10;
const int MAX_WAIT_TIME = 5000; // milliseconds

mutex mtx;
mutex global_mtx; // for information shared by both the elevator and person
mutex cout_mtx;
queue<pair<int, int>> global_queue; // global elevator queue (floor, destination)
queue<pair<int, int>> elevator_queues[NUM_ELEVATORS];
atomic<int> num_people_serviced(0);
vector<int> elevator_positions(NUM_ELEVATORS, 0);
vector<bool> elevator_directions(NUM_ELEVATORS, true);
vector<int> global_passengers_serviced(NUM_ELEVATORS, 0);
int npeople; // global variable for the number of people to be serviced

void elevator(int id)
{
    int curr_floor = 0;
    int dest_floor = 0;
    int occupancy = 0;

    // lord have mercy on my sOUL istg i will S C R E A M
    while (true)
    {
        std::unique_lock<std::mutex> lock(global_mtx);
        // at least one person on elevator, someone gets off
        if (!elevator_queues[id].empty() && occupancy < MAX_OCCUPANCY)
        {
            curr_floor = elevator_positions[id];
            dest_floor = elevator_queues[id].front().second;
            if (curr_floor != dest_floor)
            {
                occupancy++;
            }
            else if (curr_floor == dest_floor)
            {
                // Person gets off
                elevator_queues[id].pop();
                occupancy--;
                global_passengers_serviced[id]++;
                num_people_serviced++;
            }
        }
        // at least one person waiting, someone gets on
        else if (!global_queue.empty() && occupancy < MAX_OCCUPANCY)
        {
            curr_floor = elevator_positions[id];
            if (curr_floor == global_queue.front().first)
            {
                // curr_floor = global_queue.front().first;
                dest_floor = global_queue.front().second;
                elevator_positions[id] = curr_floor;
                elevator_queues[id].push({global_queue.front().first, global_queue.front().second});
                global_queue.pop();
                occupancy++;
            }
        }
        // someone has to get off
        else if (occupancy >= MAX_OCCUPANCY)
        {
            curr_floor = elevator_positions[id];
            dest_floor = elevator_queues[id].front().second;
            if (curr_floor == dest_floor)
            {
                elevator_queues[id].pop();
                occupancy--;
                global_passengers_serviced[id]++;
                num_people_serviced++;
                elevator_positions[id] = dest_floor;
            }
        }

        if (dest_floor != curr_floor)
        {
            cout_mtx.lock();
            std::cout << "Elevator " << id << " moving from floor " << curr_floor << " to floor " << dest_floor << endl;
            cout_mtx.unlock();
            if (dest_floor < curr_floor)
            {
                elevator_directions[id] = false;
                curr_floor = dest_floor;
            }
            else
            {
                elevator_directions[id] = true;
                curr_floor = dest_floor;
            }
            this_thread::sleep_for(chrono::milliseconds(100));
            elevator_positions[id] = curr_floor;
        }

        else if (occupancy > 0)
        {
            curr_floor = elevator_positions[id];
            dest_floor = elevator_queues[id].front().second;
            if (curr_floor == dest_floor)
            {
                elevator_queues[id].pop();
                occupancy--;
                global_passengers_serviced[id]++;
                num_people_serviced++;
                elevator_positions[id] = dest_floor;
            }
            // continue;
        }

        if (num_people_serviced >= npeople && elevator_queues[id].empty() && occupancy == 0)
        {
            cout_mtx.lock();
            std::cout << "Elevator " << id << " serviced " << global_passengers_serviced[id] << " people " << endl
                      << flush;
            cout_mtx.unlock();
            break;
        }

        lock.unlock();
    }
}

void person(int id)
{
    static atomic<int> counter(0);        // static counter for unique IDs
    int person_id = counter.fetch_add(1); // Increment and assign ID atomically

    int curr_floor = rand() % NUM_FLOORS;
    int dest_floor = rand() % NUM_FLOORS;
    while (dest_floor == curr_floor)
    {
        dest_floor = rand() % NUM_FLOORS;
    }

    cout_mtx.lock();
    std::cout << "Person " << id << " wants to go from floor " << curr_floor << " to floor " << dest_floor << endl;
    cout_mtx.unlock();

    mtx.lock();
    global_queue.push({curr_floor, dest_floor});
    mtx.unlock();

    cout_mtx.lock();
    std::cout << "Person " << id << " on floor " << curr_floor << " requested elevator service to go to floor " << dest_floor << endl;
    cout_mtx.unlock();

    int elev_id = rand() % NUM_ELEVATORS;
    global_mtx.lock();
    elevator_queues[elev_id].push({curr_floor, dest_floor});
    global_mtx.unlock();

    cout_mtx.lock();
    std::cout << "Person " << id << " entered elevator " << elev_id << endl;
    cout_mtx.unlock();

    // Simulate moving towards the destination floor
    while (curr_floor != dest_floor)
    {
        this_thread::sleep_for(chrono::milliseconds(1000));
    }

    cout_mtx.lock();
    std::cout << "Person " << id << " arrived at floor " << curr_floor << endl
              << flush;
    cout_mtx.unlock();
}

#endif // HW5_ELEVATOR_HPP
