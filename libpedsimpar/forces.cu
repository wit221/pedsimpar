#include "ped_agent.h"
#include "ped_vector.h"
#include <thrust/device_vector.h>

__global__ void kernelLookahead(double2 *e, double2 *p, double2 *v, int* tmp, int* count, int N) {
    // compute lookahead counts

    int other = blockIdx.x * blockDim.x + threadIdx.x;
    int me = blockIdx.y * blockDim.y + threadIdx.y;

    if (me == other || me >= N || other >= N) {
        return;
    }

    const double pi = 3.14159265;
    double distancex = p[other].x - p[me].x;
    double distancey = p[other].y - p[me].y;
    double dist2 = distancex*distancex + distancey*distancey;
    if (dist2 < 400) {
        double at2v = atan2(-e[me].x, -e[me].y);
        double at2d = atan2(-distancex, -distancey);
        double at2v2 = atan2(-v[other].x, -v[other].y);
        double s = at2d - at2v;
        if (s > pi) s -= 2*pi;
        if (s < -pi) s += 2*pi;
        double vv = at2v - at2v2;
        if (vv > pi) vv -= 2*pi;
        if (vv < -pi) vv += 2*pi;
        if (abs(vv) > 2.5) {
            if ((s < 0) && (s > -0.3))
                tmp[me*N + other] = -1;
            if ((s > 0) && (s < 0.3))
                tmp[me*N + other] = 1;
        }
    }

    __syncthreads();
    // now reduce
    // TODO make parallel
    if (other != 0) {
        return;
    }
    int total = 0;
    for (int i = 0; i < N; i++) {
        total += tmp[me*N + i];
    }
    count[me] = total;
}

void cudaLookaheadCount(const vector<Ped::Tagent*> &agents, vector<int> &counts) {
    int N = agents.size();
    thrust::host_vector<double2> pvec_host(N);
    thrust::host_vector<double2> vvec_host(N);
    thrust::host_vector<double2> evec_host(N);

    thrust::device_vector<int> tmpvec(N*N);
    thrust::fill(thrust::device, tmpvec.begin(), tmpvec.end(), 0);
    thrust::device_vector<int> countvec(N);

    int i = 0;
    for (auto iter = agents.begin(); iter!=agents.end(); ++iter) {
        const Ped::Tagent* agent = *iter;
        Ped::Tvector op = agent->getPosition();
        Ped::Tvector ov = agent->getVelocity();
        Ped::Tvector oe = agent->getDesiredDirection();
        pvec_host[i] = make_double2(op.x, op.y);
        vvec_host[i] = make_double2(ov.x, ov.y);
        evec_host[i] = make_double2(oe.x, oe.y);
        i++;
    }

    thrust::device_vector<double2> pvec = pvec_host;
    thrust::device_vector<double2> vvec = vvec_host;
    thrust::device_vector<double2> evec = evec_host;

    dim3 blockDim(32, 32);
    dim3 gridDim((N + 32 - 1)/32, (N + 32 - 1)/32);
    kernelLookahead<<<gridDim, blockDim>>>(
        thrust::raw_pointer_cast(evec.data()),
        thrust::raw_pointer_cast(pvec.data()),
        thrust::raw_pointer_cast(vvec.data()),
        thrust::raw_pointer_cast(tmpvec.data()),
        thrust::raw_pointer_cast(countvec.data()),
        N
    );

    /*if (id == 7) {
        cerr << "cuda:";
        for (int i = 0; i < N; i++) {
            //cerr << result[i] << " ";
            cerr << pvec_host[i].x << " " << pvec_host[i].y << " " << vvec_host[i].x << " " << vvec_host[i].y << " " << result[i] << endl;
        }
        cerr << endl;
    }*/

    return 0;
}
