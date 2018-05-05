#include <mpi.h>

#include "ped_superscene.h"
#include "ped_scene.h"
#include "ped_agent.h"
#include "ped_obstacle.h"
#include "ped_tree.h"
#include "ped_waypoint.h"
#include "ped_outputwriter.h"

#include <cstddef>
#include <algorithm>
#include <stack>

using namespace std;



Ped::Tsuperscene::Tsuperscene(double left, double top, double width, double height,
                              int iters, int h) {
  //init MPI
  int process_count;
  int process_id;

  MPI_Init(NULL, NULL);
  MPI_Comm_size(MPI_COMM_WORLD, &process_count);
  MPI_Comm_rank(MPI_COMM_WORLD, &process_id);

  bool is_master = process_id == 0;

  //determine own boundaries (horizontal bands)
  double my_height = height/process_count;
  double my_top = my_height*process_id;
  //create scene
  s = new Tscene(left, my_top, width, my_height);
}

/// \param   *a A pointer to the Tagent to add.
void Ped::Tsuperscene::addAgent(Ped::Tagent *a) {
  //determine which processor to tell to add agent
  Tvector pod = a->getPosition();
  if (withinScene(s, pos)) s->addAgent(a);
}

void Ped::Tsuperscene::addObstacle(Ped::Tobstacle *o) {

    //determine which processor to send to.
    //problem: obstacle may be across multiple processors, it's a line

    // for now, just add the obstacle
    s->addObstacle(o);
}
void Ped::Tsuperscene::addWaypoint(Ped::Twaypoint* w) {
  s->addWaypoint(w);
}
void Ped::Tsuperscene::startSim(int iters, double h){
  s->startSim(iters, h);
}
