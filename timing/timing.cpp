#include "timing.h"

Timing::Timing(){

}
int Timing::addMetric(std::string metric){
  metric_names.push_back(metric);
  metric_times.push_back(0);
  metric_counts.push_back(0);
  metric_starts.push_back(std::chrono::high_resolution_clock::now());

  return metric_names.size()-1;
}
int Timing::addMetric(std::string metric, int parent_id){

}
void Timing::startTimer(int metric){
  std::chrono::time_point<std::chrono::high_resolution_clock> t_start = std::chrono::high_resolution_clock::now();
  metric_starts[metric] = t_start;
}
void Timing::endTimer(int metric){
  std::chrono::time_point<std::chrono::high_resolution_clock> t_end = std::chrono::high_resolution_clock::now();
  double diff = std::chrono::duration<double, std::milli>(t_end-metric_starts[metric]).count();
  metric_times[metric]+=diff;
  metric_counts[metric]++;
}
void Timing::summary(){
  double total = 0;
  for (int i = 0; i< metric_names.size();i++){
    total+=metric_times[i];
  }
  for (int i = 0; i< metric_names.size();i++){
    printf("%-20s%.2f\%\t%.2fms\n", metric_names[i].c_str(),
  metric_times[i]/total, metric_times[i]);
  }
}
