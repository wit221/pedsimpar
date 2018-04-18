#include <string>
#include <vector>
#include <time.h>
#include <iostream>
#include <chrono>
#include <thread>

class Timing{
private:
int num_timers;
std::vector<std::string> metric_names;
std::vector<double> metric_times;
std::vector<int> metric_counts;
std::vector<std::chrono::time_point<std::chrono::high_resolution_clock>> metric_starts;

public:
  Timing();
  int addMetric(std::string metric);
  int addMetric(std::string metric, int parent_id);
  void summary();
  void startTimer(int metric);
  void endTimer(int metric);
};
