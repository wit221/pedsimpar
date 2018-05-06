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
  int width = 400;
  int height = 600;

  int num_waypoints = 2;
  int num_agents = 20;
  int num_obstacles = 1;
  int num_iter = 10;
  double h = 0.4;

  Ped::Twaypoint* waypoints [num_waypoints];
  Ped::Tobstacle* obstacles [num_obstacles];


  // create scene
  Ped::Tscene *pedscene = new Ped::Tscene(0, width, 0, height); // no quadtree

  //create and add obstacles
  for (int o = 0; o< num_obstacles; o++){
    obstacles[o] = new Ped::Tobstacle(random(1, width-2), random(1, height-2),
     random(1, width-2), random(1, height-2));
    pedscene->addObstacle(obstacles[o]);
  }

  //create waypoints
  for (int w = 0; w<num_waypoints; w++){
    waypoints[w] = new Ped::Twaypoint(random(50, width-50), random(50, height-50), random(5, 25));
  }
  //create agents
  for (int a = 0; a<num_agents; a++){
    Ped::Tagent *agent = new Ped::Tagent();
    for (int w = 0; w<num_waypoints; w++){
      agent->addWaypoint(waypoints[w]);
    }
    cout << "hello\n";
    agent->setWaypointBehavior(Ped::Tagent::BEHAVIOR_ONCE);  // only once
    agent->setVmax(1.2); // same speed for all agents
    agent->setfactorsocialforce(10.0);
    agent->setfactorobstacleforce(1.0);
    agent->setPosition(random(5, width-5), random(5, height-5), 0);
    pedscene->addAgent(agent);
  }

  //make simulation
  for (int step = 0; step<num_iter; step++){
    pedscene->moveAgents(h);
  }

  // cleanup
  for (auto a : pedscene->getAllAgents()) { delete a; };
  for (auto w : pedscene->getAllWaypoints()) { delete w; };
  for (auto o : pedscene->getAllObstacles()) { delete o; };
  delete pedscene;

  return EXIT_SUCCESS;
}
