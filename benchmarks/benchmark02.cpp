#include "ped_includes.h"

#include <iostream>
#include <sstream>
#include <cstdlib>

using namespace std;


int random(int min, int max) //range : [min, max)
{
   static bool first = true;
   if (first)
   {
      srand( time(NULL) ); //seeding for the first time only!
      first = false;
   }
   return min + rand() % (( max + 1 ) - min);
}

int main(int argc, char *argv[]) {

  cout << "# Benchmark 02 " << Ped::LIBPEDSIM_VERSION << endl;

  //sim params
  int width;
  int height;

  int num_waypoints = 2;
  int num_agents = 100;
  int num_obstacles = 1;
  int num_iter = 1000;
  double h = 0.4;

  // create scene
  Ped::Tscene *pedscene = new Ped::Tscene(0, width, 0, height); // no quadtree

  //create and add obstacles
  for (int o = 0; o< num_obstacles; o++){
    obstacles[o] = new Ped::Tobstacle(random(0, width), random(0, height),
     random(0, width), random(0, height));
    pedscene->addObstacle(obstacles[o]);
  }

  //create waypoints
  Ped::Twaypoint* waypoints [num_waypoints];
  for (int w = 0; w<num_waypoints; w++){
    waypoints[w] = new Ped::Twaypoint(random(0, width), random(0, height), random(5, 25));
  }

  //create agents
  for (int a = 0; a<num_agents; a++){
    Ped::Tagent *a = new Ped::Tagent();
    for (int w = 0; w<num_waypoints; w++){
      a->addWaypoint(waypoints[w]);
    }
    a->setPosition(random(0, width), random(0, height), 0);
    pedscene->addAgent(a);
  }

  //make simulation
  for (int step = 0; step<iters; step++){
    pedscene->moveAgents(h);
  }
}

  // cleanup
  for (auto a : pedscene->getAllAgents()) { delete a; };
  for (auto w : pedscene->getAllWaypoints()) { delete w; };
  for (auto o : pedscene->getAllObstacles()) { delete o; };
  delete pedscene;

  return EXIT_SUCCESS;
}
