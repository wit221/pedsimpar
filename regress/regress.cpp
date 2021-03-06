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
#include <unistd.h>

using namespace std;

void simulate(int numagents, int maxtime) {
    // setup
    Ped::Tscene *pedscene = new Ped::Tscene(); // no quadtree

    // create an output writer which will send output to a visualizer
    Ped::OutputWriter *ow = new Ped::FileOutputWriter();
    //Ped::OutputWriter *ow = new Ped::UDPOutputWriter();
    ow->setScenarioName("Regression/Benchmark Test");
    pedscene->setOutputWriter(ow);

    // create and add obstacles
    pedscene->addObstacle(new Ped::Tobstacle(-100, -50, 100, -50));
    pedscene->addObstacle(new Ped::Tobstacle(-100, 50, 100, 50));
    pedscene->addObstacle(new Ped::Tobstacle(-100, -50, -100, 50));
    pedscene->addObstacle(new Ped::Tobstacle(100, -50, 100, 50));

    // dynamic obstacles
    Ped::Tobstacle *do1 = new Ped::Tobstacle(-40, -5, 40, -5);
    pedscene->addObstacle(do1);
    Ped::Tobstacle *do2 = new Ped::Tobstacle(-40, -5, -100, -50);
    pedscene->addObstacle(do2);
    Ped::Tobstacle *do3 = new Ped::Tobstacle(40, -5, 100, -50);
    pedscene->addObstacle(do3);
    Ped::Tobstacle *do4 = new Ped::Tobstacle(-40, 5, 40, 5);
    pedscene->addObstacle(do4);
    Ped::Tobstacle *do5 = new Ped::Tobstacle(-40, 5, -100, 50);
    pedscene->addObstacle(do5);
    Ped::Tobstacle *do6 = new Ped::Tobstacle(40, 5, 100, 50);
    pedscene->addObstacle(do6);

    // add one waypoint (=destination) with a small radius of 10 at the right
    // end.
    Ped::Twaypoint *w1 = new Ped::Twaypoint(100, 0, 20);
    pedscene->addWaypoint(w1);
    Ped::Twaypoint *w2 = new Ped::Twaypoint(-100, 0, 20);
    pedscene->addWaypoint(w2);

    // create agents
    for (int i = 0; i < numagents; i++) {
        Ped::Tagent *a = new Ped::Tagent();
        a->setWaypointBehavior(Ped::Tagent::BEHAVIOR_ONCE); // only once
        a->setVmax(1.2); // same speed for all agents
        a->setfactorsocialforce(10.0);
        a->setfactorobstacleforce(2.0);
        pedscene->addAgent(a);
    }

    // convenience
    const vector<Ped::Tagent *> &myagents = pedscene->getAllAgents();
    const vector<Ped::Tobstacle *> &myobstacles = pedscene->getAllObstacles();

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
            if (a->reachedDestination())
                notreached--;
        }
        if (timestep >= maxtime)
            notreached = 0; // seems to run forever.
    }

    // cleanup
    for (auto a : pedscene->getAllAgents()) {
        delete a;
    };
    for (auto w : pedscene->getAllWaypoints()) {
        delete w;
    };
    for (auto o : pedscene->getAllObstacles()) {
        delete o;
    };
    delete pedscene;
}

void usage(char* argv0) {
    cout << argv0 << " [-n numagents]" << endl;
    exit(0);
}

int main(int argc, char *argv[]) {
    int numagents = 100;
    int maxtime = 20000;
    int c;
    while ((c = getopt(argc, argv, "hn:t:")) != -1) {
        switch(c) {
            case 'h':
                usage(argv[0]);
                break;
            case 'n':
                numagents = atoi(optarg);
                break;
            case 't':
                maxtime = atoi(optarg);
                break;
            default:
                break;
        }
    }

    if (numagents <= 0 || numagents > 1000) {
        cout << "Invalid number of agents (0 < numagents <= 1000)" << endl;
        exit(0);
    }

    simulate(numagents, maxtime);
    return EXIT_SUCCESS;
}
