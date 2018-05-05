#ifndef _ped_superscene_h_
#define _ped_superscene_h_ 1

//disable warnings on 255 char debug symbols
#pragma warning (disable : 4786)
//disable warnings on extern before template instantiation
#pragma warning (disable : 4231)

#ifdef _WIN32
#ifdef _DLL
#    define LIBEXPORT __declspec(dllexport)
#    define EXPIMP_TEMPLATE
#else
#    define LIBEXPORT __declspec(dllimport)
#    define EXPIMP_TEMPLATE extern
#endif
#else
#    define LIBEXPORT
#    define EXPIMP_TEMPLATE
#endif

#include <set>
#include <vector>
#include <map>
#include <list>

using namespace std;
namespace Ped {
  class OutputWriter;
  class Tagent;
  class Tobstacle;
  class Twaypoint;
  class Tscene;
}

// EXPIMP_TEMPLATE template class LIBEXPORT std::vector<Ped::Tagent*>;
// EXPIMP_TEMPLATE template class LIBEXPORT std::vector<Ped::Tobstacle*>;
// EXPIMP_TEMPLATE template class LIBEXPORT std::vector<Ped::Twaypoint*>;
// EXPIMP_TEMPLATE template class LIBEXPORT std::vector<Ped::OutputWriter*>;

namespace Ped {

class LIBEXPORT Tsuperscene {

    public:
        Tsuperscene(double left, double top, double width, double height,
        int iters, int h);
        virtual ~Tsuperscene();
        virtual void clear();
        virtual void addAgent(Tagent* a);
        virtual void addObstacle(Tobstacle* o);
        virtual void addWaypoint(Twaypoint* w);
        virtual bool removeAgent(Tagent* a);
        virtual bool removeObstacle(Tobstacle* o);
        virtual bool removeWaypoint(Twaypoint* w);
        virtual void cleanup();
        virtual void moveAgents(double h);

    protected:
        Tscene s;
        int process_id;
        int nprocess;
        int iters;
        int h;
        long int timestep;

	private:
		vector<Tscene*> scenes;
	};

}
#endif
