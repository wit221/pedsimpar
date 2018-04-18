#include "timing.h"

Timing::Timing(){
  tree = new tnode;
}
int Timing::addMetric(std::string metric){
  metric_names.push_back(metric);
  metric_times.push_back(0);
  metric_counts.push_back(0);
  metric_starts.push_back(std::chrono::high_resolution_clock::now());
  //create node, add to master node.
  tnode *node = new tnode;
  node->id = metric_names.size()-1;
  tnodes.push_back(node);
  tree->children.push_back(node);
  return node->id;
}
int Timing::addMetric(std::string metric, int parent_id){
  metric_names.push_back(metric);
  metric_times.push_back(0);
  metric_counts.push_back(0);
  metric_starts.push_back(std::chrono::high_resolution_clock::now());
  //create node, add to parent node.
  tnode *node = new tnode;
  node->id = metric_names.size()-1;
  tnodes.push_back(node);
  tnodes[parent_id]->children.push_back(node);
  return node->id;
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
  //add up 1st level times
  for (int i = 0; i< tree->children.size();i++){
    total+=metric_times[tree->children[i]->id];
  }
  for (int i = 0; i< tree->children.size();i++){
    traverse(tree->children[i], total, 0);
  }
}
void Timing::traverse(tnode* node, double ptime, int level){
  std::string indent = "";
  std::string format = "%-20s%.2f\%\t%.2fms\n";
  for (int i = 0; i< level; i++) indent += "\t";
  format = indent+format;
  printf(format.c_str(), metric_names[node->id].c_str(),
  metric_times[node->id]/ptime*100, metric_times[node->id]);

  for (int i = 0; i< node->children.size();i++){
    traverse(node->children[i], metric_times[node->id], level+1);
  }
}
// void Timing::summary_basic(){
//   double total = 0;
//   for (int i = 0; i< metric_names.size();i++){
//     total+=metric_times[i];
//   }
//   for (int i = 0; i< metric_names.size();i++){
//     printf("%-20s%.2f\%\t%.2fms\n", metric_names[i].c_str(),
//   metric_times[i]/total, metric_times[i]);
//   }
// }
