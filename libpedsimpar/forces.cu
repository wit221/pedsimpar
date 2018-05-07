#include "ped_agent.h"
#include "ped_vector.h"
#include <thrust/device_vector.h>
#include <thrust/fill.h>

template<typename T>
struct line2col: public thrust::unary_function<T, T>
{
    T C;
    __host__ __device__ line2col(T C) :
            C(C)
    {
    }

    __host__ __device__ T operator()(T i)
    {
        return i / C;
    }
};

__global__ void kernelLookahead(double2 *e, double2 *p, double2 *v, int* tmp, int N) {
    // compute lookahead counts

    int other = blockIdx.x * blockDim.x + threadIdx.x;
    int me = blockIdx.y * blockDim.y + threadIdx.y;

    if (me >= N || other >= N) {
        return;
    }

    if (me != other) {
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
    }
}

void cudaLookaheadCount(vector<Ped::Tagent*> &agents, vector<int> &counts) {
    int N = agents.size();
    thrust::host_vector<double2> pvec_host(N);
    thrust::host_vector<double2> vvec_host(N);
    thrust::host_vector<double2> evec_host(N);

    thrust::device_vector<int> tmpvec(N*N);
    thrust::fill(tmpvec.begin(), tmpvec.end(), 0);

    int i = 0;
    for (std::vector<Ped::Tagent*>::iterator iter = agents.begin(); iter!=agents.end(); ++iter) {
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
        N
    );

    thrust::device_vector<int> countvec(N);

    thrust::reduce_by_key(
            thrust::make_transform_iterator(thrust::make_counting_iterator(0), line2col<int>(N)),
            thrust::make_transform_iterator(thrust::make_counting_iterator(0), line2col<int>(N)) + (N*N),
            tmpvec.begin(),
            thrust::make_discard_iterator(),
            countvec.begin());

    thrust::copy(countvec.begin(), countvec.end(), counts.begin());

    return;
}
