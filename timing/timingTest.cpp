#include "timing.h"

#ifndef chrono
#include <chrono>
#include <thread>
#endif
int main(){
  Timing t;
  int TF_ID = t.addMetric("a");
  int SF_ID = t.addMetric("b", TF_ID);
  int LF_ID = t.addMetric("c", TF_ID);
  int zid = t.addMetric("z", LF_ID);
  int B_ID = t.addMetric("d");
  int C_ID = t.addMetric("e", B_ID);
  int D_ID = t.addMetric("f", B_ID);

  t.startTimer(TF_ID);
  t.startTimer(SF_ID);
  std::this_thread::sleep_for(std::chrono::milliseconds(300));
  t.endTimer(SF_ID);

  t.startTimer(LF_ID);
  t.startTimer(zid);
  std::this_thread::sleep_for(std::chrono::milliseconds(50));
  t.endTimer(zid);

  std::this_thread::sleep_for(std::chrono::milliseconds(400));
  t.endTimer(LF_ID);
  t.endTimer(TF_ID);


  t.startTimer(B_ID);
  t.startTimer(C_ID);
  std::this_thread::sleep_for(std::chrono::milliseconds(300));
  t.endTimer(C_ID);

  t.startTimer(D_ID);
  std::this_thread::sleep_for(std::chrono::milliseconds(400));
  t.endTimer(D_ID);
  t.endTimer(B_ID);


  t.summary();
  return 0;
}
