//
// pedsim - A microscopic pedestrian simulation system.
// Copyright (c) by Christian Gloor
//

#include "ped_agent.h"
#include "ped_scene.h"
#include "ped_tree.h"

#include <cassert>
#include <cstddef>
#include <iostream>

using namespace std;

/// Description: set intial values
/// \author  chgloor
/// \date    2012-01-28
Ped::Ttree::Ttree(Ped::Tscene *pscene, int pdepth, double px, double py, double pw, double ph) : scene(pscene) {
    // more initializations here. not really necessary to put them into the initializator list, too.
    isleaf = true;
    x = px;
    y = py;
    w = pw;
    h = ph;
    depth = pdepth;
    tree1 = NULL;
    tree2 = NULL;
    tree3 = NULL;
    tree4 = NULL;
    omp_init_nest_lock(const_cast<omp_nest_lock_t*>(&lock));
};


/// Destructor. Deleted this node and all its children. If there are any agents left, they are removed first (not deleted).
/// \author  chgloor
/// \date    2012-01-28
Ped::Ttree::~Ttree() {
    clear();
}


void Ped::Ttree::clear() {
    omp_set_nest_lock(const_cast<omp_nest_lock_t*>(&lock));
    if(isleaf) {
        agents.clear();
    }
    else {
        tree1->clear();
        delete tree1;
        tree2->clear();
        delete tree2;
        tree3->clear();
        delete tree3;
        tree4->clear();
        delete tree4;
        isleaf = true;
    }
    omp_unset_nest_lock(const_cast<omp_nest_lock_t*>(&lock));
    omp_destroy_nest_lock(const_cast<omp_nest_lock_t*>(&lock));
}


/// Adds an agent to the tree. Searches the right node and adds the agent there.
/// If there are too many agents at that node allready, a new child is created.
/// \author  chgloor
/// \date    2012-01-28
/// \param   *a The agent to add
void Ped::Ttree::addAgent(const Ped::Tagent *a) {
  omp_set_nest_lock(const_cast<omp_nest_lock_t*>(&lock));
  #ifdef _OPENMP
  //cerr << a->getid() << " " << omp_get_thread_num() << endl;
  #endif
  if (isleaf) {
    agents.insert(a);
    scene->treehash[a] = this;
  }
  else {
    const Tvector pos = a->getPosition();
    if ((pos.x >= x+w/2) && (pos.y >= y+h/2)) tree3->addAgent(a); // 3
    else if ((pos.x <= x+w/2) && (pos.y <= y+h/2)) tree1->addAgent(a); // 1
    else if ((pos.x >= x+w/2) && (pos.y <= y+h/2)) tree2->addAgent(a); // 2
    else if ((pos.x <= x+w/2) && (pos.y >= y+h/2)) tree4->addAgent(a); // 4
  }

  if (agents.size() > 8) {
    isleaf = false;
    addChildren();
    while (!agents.empty()) {
      const Ped::Tagent *a = (*agents.begin());
      const Tvector pos = a->getPosition();
      if ((pos.x >= x+w/2) && (pos.y >= y+h/2)) tree3->addAgent(a); // 3
      else if ((pos.x <= x+w/2) && (pos.y <= y+h/2)) tree1->addAgent(a); // 1
      else if ((pos.x >= x+w/2) && (pos.y <= y+h/2)) tree2->addAgent(a); // 2
      else if ((pos.x <= x+w/2) && (pos.y >= y+h/2)) tree4->addAgent(a); // 4
      agents.erase(a);
    }
  }
  omp_unset_nest_lock(const_cast<omp_nest_lock_t*>(&lock));
}

void Ped::Ttree::addAgentHelper(const Ped::Tagent *a, omp_nest_lock_t *parentlock) {
  omp_set_nest_lock(const_cast<omp_nest_lock_t*>(&lock));
  if (parentlock != NULL) {
    omp_unset_nest_lock(const_cast<omp_nest_lock_t*>(parentlock));
  }
  #ifdef _OPENMP
  //cerr << a->getid() << " " << omp_get_thread_num() << endl;
  #endif
  if (isleaf) {
    agents.insert(a);
    scene->treehash[a] = this;
    if (agents.size() <= 8) {
      omp_unset_nest_lock(const_cast<omp_nest_lock_t*>(&lock));
      return;
    }
  }
  else {
    const Tvector pos = a->getPosition();
    if ((pos.x >= x+w/2) && (pos.y >= y+h/2)) tree3->addAgentHelper(a, &lock); // 3
    else if ((pos.x <= x+w/2) && (pos.y <= y+h/2)) tree1->addAgentHelper(a, &lock); // 1
    else if ((pos.x >= x+w/2) && (pos.y <= y+h/2)) tree2->addAgentHelper(a, &lock); // 2
    else if ((pos.x <= x+w/2) && (pos.y >= y+h/2)) tree4->addAgentHelper(a, &lock); // 4
  }

  if (agents.size() > 8) {
    isleaf = false;
    addChildren();
    while (!agents.empty()) {
      const Ped::Tagent *a = (*agents.begin());
      const Tvector pos = a->getPosition();
      agents.erase(a);
      if ((pos.x >= x+w/2) && (pos.y >= y+h/2)) tree3->addAgentHelper(a, &lock); // 3
      else if ((pos.x <= x+w/2) && (pos.y <= y+h/2)) tree1->addAgentHelper(a, &lock); // 1
      else if ((pos.x >= x+w/2) && (pos.y <= y+h/2)) tree2->addAgentHelper(a, &lock); // 2
      else if ((pos.x <= x+w/2) && (pos.y >= y+h/2)) tree4->addAgentHelper(a, &lock); // 4
    }
  }
}


/// A little helper that adds child nodes to this node
/// \author  chgloor
/// \date    2012-01-28
void Ped::Ttree::addChildren() {
    omp_set_nest_lock(const_cast<omp_nest_lock_t*>(&lock));
    tree1 = new Ped::Ttree(scene, depth+1, x, y, w/2, h/2);
    tree2 = new Ped::Ttree(scene, depth+1, x+w/2, y, w/2, h/2);
    tree3 = new Ped::Ttree(scene, depth+1, x+w/2, y+h/2, w/2, h/2);
    tree4 = new Ped::Ttree(scene, depth+1, x, y+h/2, w/2, h/2);
    omp_unset_nest_lock(const_cast<omp_nest_lock_t*>(&lock));
}


Ped::Ttree* Ped::Ttree::getChildByPosition(double xIn, double yIn) {
    if((xIn <= x+w/2) && (yIn <= y+h/2))
        return tree1;
    if((xIn >= x+w/2) && (yIn <= y+h/2))
        return tree2;
    if((xIn >= x+w/2) && (yIn >= y+h/2))
        return tree3;
    if((xIn <= x+w/2) && (yIn >= y+h/2))
        return tree4;

    // this should never happen
    return NULL;
}


/// Updates the tree structure if an agent moves. Removes the agent and places it again, if outside boundary.
/// If an this happens, this is O(log n), but O(1) otherwise.
/// \author  chgloor
/// \date    2012-01-28
/// \param   *a the agent to update
void Ped::Ttree::moveAgent(const Ped::Tagent *a) {
  omp_set_nest_lock(const_cast<omp_nest_lock_t*>(&lock));
  const Tvector pos = a->getPosition();
  if ((pos.x < x) || (pos.x > (x+w)) || (pos.y < y) || (pos.y > (y+h))) {
    agents.erase(a);
    omp_unset_nest_lock(const_cast<omp_nest_lock_t*>(&lock));
    scene->placeAgent(a);
  }
   else {
    omp_unset_nest_lock(const_cast<omp_nest_lock_t*>(&lock));
  }
}


bool Ped::Ttree::removeAgent(const Ped::Tagent *a) {
  if(isleaf) {
    omp_set_nest_lock(const_cast<omp_nest_lock_t*>(&lock));
    size_t removedCount = agents.erase(a);
    omp_unset_nest_lock(const_cast<omp_nest_lock_t*>(&lock));
    return (removedCount > 0);
  }
  else {
    const Tvector pos = a->getPosition();
    return getChildByPosition(pos.x, pos.y)->removeAgent(a);
  }
}


/// Checks if this tree node has not enough agents in it to justify more child nodes. It does this by checking all
/// child nodes, too, recursively. If there are not enough children, it moves all the agents into this node, and deletes the child nodes.
/// \author  chgloor
/// \date    2012-01-28
/// \return  the number of agents in this and all child nodes.
int Ped::Ttree::cut() {
    int count = 0;
    if (isleaf) {
        // omp_unset_nest_lock(const_cast<omp_nest_lock_t*>(&lock));
        return agents.size();
    }

    omp_set_nest_lock(const_cast<omp_nest_lock_t*>(&lock));

    count += tree1->cut();
    count += tree2->cut();
    count += tree3->cut();
    count += tree4->cut();
    if (count < 5) {
        assert(tree1->isleaf == true);
        assert(tree2->isleaf == true);
        assert(tree3->isleaf == true);
        assert(tree4->isleaf == true);
        agents.insert(tree1->agents.begin(), tree1->agents.end());
        agents.insert(tree2->agents.begin(), tree2->agents.end());
        agents.insert(tree3->agents.begin(), tree3->agents.end());
        agents.insert(tree4->agents.begin(), tree4->agents.end());
        isleaf = true;
        for (set<const Ped::Tagent*>::iterator it = agents.begin(); it != agents.end(); ++it) {
            const Tagent *a = (*it);
            scene->treehash[a] = this;
        }
        delete tree1;
        delete tree2;
        delete tree3;
        delete tree4;
    }
    omp_unset_nest_lock(const_cast<omp_nest_lock_t*>(&lock));
    return count;
}


/// Returns the set of agents that is stored within this tree node
/// \author  chgloor
/// \date    2012-01-28
/// \return  The set of agents
/// \todo This might be not very efficient, since all childs are checked, too. And then the set (set of pointer, but still) is being copied around.
set<const Ped::Tagent*> Ped::Ttree::getAgents() const {
  omp_set_nest_lock(const_cast<omp_nest_lock_t*>(&lock));
    if (isleaf) {
        omp_unset_nest_lock(const_cast<omp_nest_lock_t*>(&lock));
        return agents;
    }

    set<const Ped::Tagent*> ta;
    set<const Ped::Tagent*> t1 = tree1->getAgents();
    set<const Ped::Tagent*> t2 = tree2->getAgents();
    set<const Ped::Tagent*> t3 = tree3->getAgents();
    set<const Ped::Tagent*> t4 = tree4->getAgents();
    ta.insert(t1.begin(), t1.end());
    ta.insert(t2.begin(), t2.end());
    ta.insert(t3.begin(), t3.end());
    ta.insert(t4.begin(), t4.end());
    omp_unset_nest_lock(const_cast<omp_nest_lock_t*>(&lock));
    return ta;
}


void Ped::Ttree::getAgents(list<const Ped::Tagent*>& outputList) const {
    omp_set_nest_lock(const_cast<omp_nest_lock_t*>(&lock));
    if(isleaf) {
        for(const Ped::Tagent* currentAgent : agents)
            outputList.push_back(currentAgent);
    }
    else {
        tree1->getAgents(outputList);
        tree2->getAgents(outputList);
        tree3->getAgents(outputList);
        tree4->getAgents(outputList);
    }
    omp_unset_nest_lock(const_cast<omp_nest_lock_t*>(&lock));
}


/// Checks if a point x/y is within the space handled by the tree node, or within a given radius r
/// \author  chgloor
/// \date    2012-01-29
/// \return  true if the point is within the space
/// \param   px The x co-ordinate of the point
/// \param   py The y co-ordinate of the point
/// \param   pr The radius
bool Ped::Ttree::intersects(double px, double py, double pr) const {
    if (((px+pr) > x) && ((px-pr) < (x+w)) && ((py+pr) > y) && ((py-pr) < (y+h)))
        return true; // x+-r/y+-r is inside
    else
        return false;
}
