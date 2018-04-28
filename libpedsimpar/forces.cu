#include "ped_agent.h"
#include "ped_vector.h"
#include <thrust/device_vector.h>
#include <thrust/reduce.h>

struct lookaheadForceFunctor : public thrust::binary_function<double2,double2,int>
{
    const double2 e;
    const double2 p;
    const double2 v;

    lookaheadForceFunctor(double2 _e, double2 _p, double2 _v) : e(_e), p(_p), v(_v) {}

    __host__ __device__
    float operator()(const double2& otherp, const double2& otherv) const { 
        const double pi = 3.14159265;
        double distancex = otherp.x - p.x;
        double distancey = otherp.y - p.y;
        double dist2 = distancex*distancex + distancey*distancey;
        if (dist2 < 400) {
            double at2v = atan2(-e.x, -e.y);
            double at2d = atan2(-distancex, -distancey);
            double at2v2 = atan2(-otherv.x, -otherv.y);
            double s = at2d - at2v;
            if (s > pi) s -= 2*pi;
            if (s < -pi) s += 2*pi;
            double vv = at2v - at2v2;
            if (vv > pi) vv -= 2*pi;
            if (vv < -pi) vv += 2*pi;
            if (abs(vv) > 2.5) {
                if ((s < 0) && (s > -0.3))
                    return -1;
                if ((s > 0) && (s < 0.3))
                    return 1;
            }
        }
        return 0;
    }
};

int cudaLookaheadCount(Ped::Tvector e, Ped::Tvector p, Ped::Tvector v, int id, const set<const Ped::Tagent*> &neighbors) {
    int N = neighbors.size() - 1;
    thrust::host_vector<double2> pvec_host(N);
    thrust::host_vector<double2> vvec_host(N);

    int i = 0;
    for (set<const Ped::Tagent*>::iterator iter = neighbors.begin(); iter!=neighbors.end(); ++iter) {
        const Ped::Tagent* other = *iter;

        // don't compute this force for the agent himself
        if (other->getid() == id) continue;
        Ped::Tvector op = other->getPosition();
        Ped::Tvector ov = other->getVelocity();
        pvec_host[i] = make_double2(op.x, op.y);
        vvec_host[i] = make_double2(ov.x, ov.y);
        i++;
    }

    thrust::device_vector<double2> pvec = pvec_host;
    thrust::device_vector<double2> vvec = vvec_host;
    thrust::device_vector<int> result(N);

    thrust::transform(pvec.begin(), vvec.begin(), pvec.end(), result.begin(),
        lookaheadForceFunctor(make_double2(e.x, e.y), make_double2(p.x, p.y), make_double2(v.x, v.y)));

    if (id == 7) {
        cerr << "cuda:";
        for (int i = 0; i < N; i++) {
            //cerr << result[i] << " ";
            cerr << pvec_host[i].x << " " << pvec_host[i].y << " " << vvec_host[i].x << " " << vvec_host[i].y << " " << result[i] << endl;
        }
        cerr << endl;
    }

    int lookaheadCount = thrust::reduce(result.begin(), result.end());
    return lookaheadCount;
}
