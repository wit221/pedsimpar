#include "ped_agent.h"
#include "ped_vector.h"
#include <thrust/device_vector.h>
#include <thrust/fill.h>

__global__ void kernelLookahead(double2 *e, double2 *p, double2 *v, int* tmp, int* count, int N) {
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


__global__ void kernelSocial(double2 *p, double2 *v, double* tmpx, double* tmpy, double* socialx, double* socialy, int N) {
    int other = blockIdx.x * blockDim.x + threadIdx.x;
    int me = blockIdx.y * blockDim.y + threadIdx.y;

    if (me >= N || other >= N) {
        return;
    }

    if (me != other) {
        const double lambdaImportance = 2.0;
        const double gamma = 0.35;
        const double n = 2;
        const double n_prime = 3;
        const double m_pi = 3.14159265358979323846264338327950288;

        double2 diff = make_double2(p[other].x - p[me].x, p[other].y - p[me].y);
        if (diff.x*diff.x + diff.y*diff.y <= 64.0) {
            double difflength = hypot(diff.x, diff.y);
            double2 diffDirection = make_double2(diff.x / difflength, diff.y / difflength);
            double2 velDiff = make_double2(v[me].x - v[other].x, v[me].y - v[other].y);
            double2 interactionVector = make_double2(lambdaImportance * velDiff.x + diffDirection.x,
                                                     lambdaImportance * velDiff.y + diffDirection.y);
            double interactionLength = hypot(interactionVector.x, interactionVector.y);
            double2 interactionDirection = make_double2(interactionVector.x / interactionLength,
                                                        interactionVector.y / interactionLength);
            double angleThis = atan2(interactionDirection.y, interactionDirection.x);
            double angleOther = atan2(diffDirection.y, diffDirection.x);
            double theta = angleOther - angleThis;
            if (theta > M_PI) theta -= 2 * m_pi;
            else if(theta <= -M_PI) theta += 2 * m_pi;

            int thetaSign = (theta == 0) ? (0) : (theta / abs(theta));
            double B = gamma * interactionLength;

            double forceVelocityAmount = -exp(-difflength/B - (n_prime*B*theta)*(n_prime*B*theta));
            double forceAngleAmount = -thetaSign * exp(-difflength/B - (n*B*theta)*(n*B*theta));

            double2 forceVelocity = make_double2(forceVelocityAmount * interactionDirection.x,
                                                 forceVelocityAmount * interactionDirection.y);
            double2 forceAngle = make_double2(forceAngleAmount * -interactionDirection.y,
                                              forceAngleAmount * interactionDirection.x);

            tmpx[me*N + other] = forceVelocity.x + forceAngle.x;
            tmpy[me*N + other] = forceVelocity.y + forceAngle.y;
        }
    }

    __syncthreads();
    // now reduce
    // TODO make parallel
    if (other != 0) {
        return;
    }
    double2 total;
    total.x = 0.0;
    total.y = 0.0;
    for (int i = 0; i < N; i++) {
        total.x += tmpx[me*N + i];
        total.y += tmpy[me*N + i];
    }
    /*
    if (me == 517 && other == 0) {
        printf("inside %.10lf %.10lf\n", total.x, total.y);
    }
    */

    socialx[me] = total.x;
    socialy[me] = total.y;
}


void cudaLookaheadSocial(vector<Ped::Tagent*> &agents, vector<int> &counts, vector<double> &socialx, vector<double> &socialy) {
    int N = agents.size();
    thrust::host_vector<double2> pvec_host(N);
    thrust::host_vector<double2> vvec_host(N);
    thrust::host_vector<double2> evec_host(N);

    thrust::device_vector<int> tmpvec(N*N);
    thrust::fill(tmpvec.begin(), tmpvec.end(), 0);
    thrust::device_vector<int> countvec(N);

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
        thrust::raw_pointer_cast(countvec.data()),
        N
    );

    thrust::device_vector<double> tmpvec2x(N*N);
    thrust::device_vector<double> tmpvec2y(N*N);
    thrust::fill(tmpvec2x.begin(), tmpvec2x.end(), 0.0);
    thrust::fill(tmpvec2y.begin(), tmpvec2y.end(), 0.0);
    thrust::device_vector<double> socialvecx(N);
    thrust::device_vector<double> socialvecy(N);

    kernelSocial<<<gridDim, blockDim>>>(
        thrust::raw_pointer_cast(pvec.data()),
        thrust::raw_pointer_cast(vvec.data()),
        thrust::raw_pointer_cast(tmpvec2x.data()),
        thrust::raw_pointer_cast(tmpvec2y.data()),
        thrust::raw_pointer_cast(socialvecx.data()),
        thrust::raw_pointer_cast(socialvecy.data()),
        N
    );

    thrust::copy(countvec.begin(), countvec.end(), counts.begin());
    thrust::copy(socialvecx.begin(), socialvecx.end(), socialx.begin());
    thrust::copy(socialvecy.begin(), socialvecy.end(), socialy.begin());


    /*if (id == 7) {
        cerr << "cuda:";
        for (int i = 0; i < N; i++) {
            //cerr << result[i] << " ";
            cerr << pvec_host[i].x << " " << pvec_host[i].y << " " << vvec_host[i].x << " " << vvec_host[i].y << " " << result[i] << endl;
        }
        cerr << endl;
    }*/

    return;
}
