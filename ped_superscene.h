#include <set>
#include <vector>
#include <map>
#include <list>

class LIBEXPORT Tsuperscene {

    public:
        Tsuperscene(double left, double top, double width, double height);
        virtual ~Tscene();
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
        int process_id;
        int nprocess;

        long int timestep;

	private:
		vector<Tscene*> scenes;
	};
