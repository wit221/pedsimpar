#include "timing.h"
#ifndef chrono
#include <chrono>
#include <thread>
#endif
int main(){
  Timing t;
  int TF_ID = t.addMetric("Total force");
  int SF_ID = t.addMetric("Social force");
  int LF_ID = t.addMetric("Lookahead FORCE");

  t.startTimer(TF_ID);
  t.startTimer(SF_ID);
  std::this_thread::sleep_for(std::chrono::milliseconds(300));
  t.endTimer(SF_ID);

  t.startTimer(LF_ID);
  std::this_thread::sleep_for(std::chrono::milliseconds(400));
  t.endTimer(LF_ID);
  t.endTimer(TF_ID);

  t.summary();
  return 0;
}
