//
// pedsim - A microscopic pedestrian simulation system.
// Copyright (c) by Christian Gloor
//
// To collect the output in a file::
// ./example05 > out.dat
//
// Process output in gnuplot:
// gnuplot> plot "out.dat"

#include "ped_includes.h"

#include <iostream>
#include <sstream>

using namespace std;


int main(int argc, char *argv[]) {

  cout << "# PedSim Example using libpedsim version " << Ped::LIBPEDSIM_VERSION << endl;

  // setup
  Ped::Tscene *pedscene = new Ped::Tscene(); // no quadtree
    
  // add one waypoint (=destination) with a small radius of 10 at the right end.
  Ped::Twaypoint *w1 = new Ped::Twaypoint( 100, 0, 10);
  Ped::Twaypoint *w2 = new Ped::Twaypoint(-100, 0, 10);

  // create and add obstacles
  pedscene->addObstacle(new Ped::Tobstacle(-100, -50,  100, -50));
  pedscene->addObstacle(new Ped::Tobstacle(-100,  50,  100,  50));
  pedscene->addObstacle(new Ped::Tobstacle(-100, -50, -100,  50));
  pedscene->addObstacle(new Ped::Tobstacle( 100, -50,  100,  50));

  // dynamic obstacles
  Ped::Tobstacle *do1 = new Ped::Tobstacle(-40, -5,  40, -5);
  pedscene->addObstacle(do1);
  Ped::Tobstacle *do2 = new Ped::Tobstacle(-40, -5, -100, -50);
  pedscene->addObstacle(do2);
  Ped::Tobstacle *do3 = new Ped::Tobstacle( 40, -5,  100, -50);
  pedscene->addObstacle(do3);
  Ped::Tobstacle *do4 = new Ped::Tobstacle(-40,  5, 40,  5);
  pedscene->addObstacle(do4);
  Ped::Tobstacle *do5 = new Ped::Tobstacle(-40,  5, -100,  50);
  pedscene->addObstacle(do5);
  Ped::Tobstacle *do6 = new Ped::Tobstacle( 40,  5,  100,  50);
  pedscene->addObstacle(do6);

  // create agents
  for (int i = 0; i<500; i++) {
    Ped::Tagent *a = new Ped::Tagent();
    a->setWaypointBehavior(Ped::Tagent::BEHAVIOR_ONCE);  // only once
    a->setVmax(1.2); // same speed for all agents
    a->setfactorsocialforce(10.0);
    a->setfactorobstacleforce(1.0);
    pedscene->addAgent(a);
  }

  // convenience
  const vector<Ped::Tagent*>& myagents = pedscene->getAllAgents();
  const vector<Ped::Tobstacle*>& myobstacles = pedscene->getAllObstacles();

  for (double h = 0; h < 5; h += 0.5) {

    // move obstacle
    do1->setPosition(-40, -5+h,  40, -5+h);
    do2->setPosition(-40, -5+h, -100, -50);
    do3->setPosition( 40, -5+h,  100, -50);
    do4->setPosition(-40,  5-h, 40,  5-h);
    do5->setPosition(-40,  5-h, -100,  50);
    do6->setPosition( 40,  5-h,  100,  50);
	
    long int timestep = 0;

    // reset agents
    for (vector<Ped::Tagent *>::const_iterator it = myagents.begin();
         it != myagents.end(); ++it) {
        if ((*it)->getid() % 2 == 0) {
            (*it)->setPosition(-80 - ((*it)->getid() / 50), -25 + ((*it)->getid() % 50), 0);
            (*it)->addWaypoint(w1);
        } else {
            (*it)->setPosition(80 + ((*it)->getid() / 50), -25 + ((*it)->getid() % 50), 0);
            (*it)->addWaypoint(w2);
        }
    }
    int notreached = myagents.size();
    while (notreached > 0) {
      timestep++;
      notreached = myagents.size();
      pedscene->moveAgents(0.4);
	  
      for (auto a : myagents) {
	if (a->reachedDestination()) notreached--;
      }
      if (timestep >= 20) notreached = 0; // seems to run forever.
    }
	
    cout << "# " << h << " " << timestep << endl;

  }

  // cleanup
  for (auto a : pedscene->getAllAgents()) { delete a; };
  for (auto w : pedscene->getAllWaypoints()) { delete w; };
  for (auto o : pedscene->getAllObstacles()) { delete o; };
  delete pedscene;
  
  return EXIT_SUCCESS;   
}
