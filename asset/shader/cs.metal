#import "common.metal"

kernel void expand_points(
    device const Point* inPts   [[buffer(0)]],
    device       Point* outPts  [[buffer(1)]],
    constant      uint& inCount [[buffer(2)]],
    constant      uint& nCopies [[buffer(3)]],
    constant    float3& dir     [[buffer(4)]],
    constant     float& step    [[buffer(5)]],
    uint gid [[thread_position_in_grid]]
){
    if(gid >= inCount)
        return;
    const Point p = inPts[gid];
    const uint base = gid * nCopies;

    for(uint i=0; i<nCopies; ++i){
        Point q;
        q.position = p.position + (step*i)*dir;
        q.color = p.color;
        outPts[base+i] = q;
    }
}