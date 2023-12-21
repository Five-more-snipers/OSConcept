#include <iostream>
#include <algorithm>
#include <climits>
#include <vector>
#include <iomanip>
#include <queue>
#include <string.h>
using namespace std;

// Improved Round Robin

struct process
{
    int pid;
    int arrival_time;
    int burst_time;
    int start_time;
    int completion_time;
    int turnaround_time;
    int waiting_time;
    int response_time;
};

bool compare1(process p1, process p2)
{
    return p1.arrival_time < p2.arrival_time;
}

bool compare2(process p1, process p2)
{
    return p1.pid < p2.pid;
}

void printGanttChart(const process p[], int n)
{
    cout << "Gantt Chart:" << endl;
    for (int i = 0; i < n; i++)
    {
        cout << "| P" << p[i].pid << " ";
    }
    cout << "|" << endl;
}

int main()
{

    int n;
    int tq;
    struct process p[100];
    float avg_turnaround_time;
    float avg_waiting_time;
    float avg_response_time;
    float cpu_utilisation;
    int total_turnaround_time = 0;
    int total_waiting_time = 0;
    int total_response_time = 0;
    int total_idle_time = 0;
    float throughput;
    int burst_remaining[100];
    int idx;

    cout << setprecision(2) << fixed;

    n = 5;
    int at[] = {0, 6, 8, 9, 10};
    int bt[] = {7, 15, 90, 42, 8};

    for (int i = 0; i < n; i++)
    {
        p[i].arrival_time = at[i];
        p[i].burst_time = bt[i];
        burst_remaining[i] = p[i].burst_time;
        p[i].pid = i + 1;
    }

    sort(p, p + n, compare1);

    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> q;
    int current_time = 0;
    int completed = 0;
    int mark[100];
    memset(mark, 0, sizeof(mark));
    mark[0] = 1;

    while (completed != n)
    {
        int min_remaining_time = INT_MAX;
        int selected_process = -1;

        for (int i = 0; i < n; i++)
        {
            if (burst_remaining[i] > 0 && burst_remaining[i] < min_remaining_time && p[i].arrival_time <= current_time)
            {
                min_remaining_time = burst_remaining[i];
                selected_process = i;
            }
        }

        if (selected_process == -1)
        {
            // No process available, move time forward
            current_time++;
            continue;
        }

        idx = selected_process;

        if (burst_remaining[idx] == p[idx].burst_time)
        {
            p[idx].start_time = max(current_time, p[idx].arrival_time);
            total_idle_time += p[idx].start_time - current_time;
            current_time = p[idx].start_time;
        }

        // IRR-SJF-DTQ logic for time quantum
        int mean = 0;
        for (int j = 0; j < n; j++)
        {
            mean += burst_remaining[j];
        }
        tq = max((mean / (n - completed) + 1) / 2, 1);

        cout << "Time " << current_time << " - Process P" << p[idx].pid << " - Time Quantum: " << tq << endl;

        if (burst_remaining[idx] - tq > 0)
        {
            burst_remaining[idx] -= tq;
            current_time += tq;
        }
        else
        {
            current_time += burst_remaining[idx];
            burst_remaining[idx] = 0;
            completed++;

            p[idx].completion_time = current_time;
            p[idx].turnaround_time = p[idx].completion_time - p[idx].arrival_time;
            p[idx].waiting_time = p[idx].turnaround_time - p[idx].burst_time;
            p[idx].response_time = p[idx].start_time - p[idx].arrival_time;

            total_turnaround_time += p[idx].turnaround_time;
            total_waiting_time += p[idx].waiting_time;
            total_response_time += p[idx].response_time;

            cout << "Process P" << p[idx].pid << " completed at time " << current_time << endl;
        }

        // Check for newly arrived processes
        for (int i = 0; i < n; i++)
        {
            if (burst_remaining[i] > 0 && p[i].arrival_time <= current_time && i != idx)
            {
                q.push({burst_remaining[i], i});
            }
        }
    }

    avg_turnaround_time = (float)total_turnaround_time / n;
    avg_waiting_time = (float)total_waiting_time / n;
    avg_response_time = (float)total_response_time / n;
    cpu_utilisation = ((p[n - 1].completion_time - total_idle_time) / (float)p[n - 1].completion_time) * 100;
    throughput = float(n) / (p[n - 1].completion_time - p[0].arrival_time);

    sort(p, p + n, compare2);

    cout << endl;
    cout << "#P\t"
         << "AT\t"
         << "BT\t"
         << "ST\t"
         << "CT\t"
         << "TAT\t"
         << "WT\t"
         << "RT\t"
         << "\n"
         << endl;

    for (int i = 0; i < n; i++)
    {
        cout << p[i].pid << "\t" << p[i].arrival_time << "\t" << p[i].burst_time << "\t" << p[i].start_time << "\t" << p[i].completion_time << "\t" << p[i].turnaround_time << "\t" << p[i].waiting_time << "\t" << p[i].response_time << "\t"
             << "\n"
             << endl;
    }
    cout << "Average Turnaround Time = " << avg_turnaround_time << endl;
    cout << "Average Waiting Time = " << avg_waiting_time << endl;
    cout << "Average Response Time = " << avg_response_time << endl;
    cout << "CPU Utilization = " << cpu_utilisation << "%" << endl;
    cout << "Throughput = " << throughput << " process/unit time" << endl;
}
