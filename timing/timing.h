#include <string>
#include <vector>
#include <time.h>
#include <iostream>
#include <chrono>
#include <thread>

struct tnode{
 int id;
 std::vector<tnode*> children;
};

class Timing{
private:
int num_timers;
std::vector<std::string> metric_names;
std::vector<double> metric_times;
std::vector<int> metric_counts;
std::vector<std::chrono::time_point<std::chrono::high_resolution_clock>> metric_starts;
tnode *tree;
std::vector<tnode*> tnodes;
void traverse(tnode* node, double ptime, int level);
public:
  Timing();
  int addMetric(std::string metric);
  int addMetric(std::string metric, int parent_id);
  void summary();
  void startTimer(int metric);
  void endTimer(int metric);
};
