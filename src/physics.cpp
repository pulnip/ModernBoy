#include <cmath>
#include "physics.hpp"

using namespace ModernBoy;

constexpr float eps = 1e-6;
constexpr float eps_3d = 1e-5;

Vec2 ModernBoy::support(
    const Vec2 polygon[], int num_vert,
    const Vec2 v
){
    Vec2 point = polygon[0];
    float max_product = dot(point, v);

    for(int i=1; i<num_vert; ++i){
        float product = dot(polygon[i], v);
        if(product > max_product){
            point = polygon[i];
            max_product = product;
        }
    }

    return point;
}
Vec2 ModernBoy::mink_dif(
    const Vec2 poly1[], int num_vert1,
    const Vec2 poly2[], int num_vert2,
    const Vec2 dir
){
    Vec2 poly1_sup = support(poly1, num_vert1,  dir);
    Vec2 poly2_sup = support(poly2, num_vert2, -dir);
    return poly1_sup - poly2_sup;
}
Vec2 ModernBoy::normal(const Vec2 v, const Vec2 dir){
    Vec2 candidate1 = {{-v.y, v.x}};
    Vec2 candidate2 = {{v.y, -v.x}};

    if(dot(candidate1, dir) > 0)
        return candidate1;
    else
        return candidate2;

}
Vec2 ModernBoy::barycenter(const Vec2 poly[], int num_vert){
    if(num_vert == 0)
        return {{0, 0}};

    Vec2 sum = poly[0];
    for(int i=1; i<num_vert; ++i)
        sum = sum + poly[i];
    return sum / num_vert;
}
bool ModernBoy::gjk(
    const Vec2 poly1[], int num_vert1,
    const Vec2 poly2[], int num_vert2
){
    if(poly1 == poly2)
        return true;
    if((num_vert1 < 1) || (num_vert2 < 1))
        return false;
    if((num_vert1==1) && (num_vert2==1))
        return poly1[0]==poly2[0];

    Vec2 zero_dir_normal = barycenter(poly1, num_vert1)
        - barycenter(poly2, num_vert2);
    if(abs(zero_dir_normal.x) <= eps && abs(zero_dir_normal.y) <= eps)
        // fallback
        zero_dir_normal = Vec2{{1, 0}};

    Vec2 point1 = mink_dif(
        poly1, num_vert1,
        poly2, num_vert2,
        zero_dir_normal
    );
    Vec2 point2 = mink_dif(
        poly1, num_vert1,
        poly2, num_vert2,
        -point1
    );

    float initial_dotted = dot(point2-point1, point2);

    if(initial_dotted < 0)
        return false;
    else if(initial_dotted == 0)
        return true;

    int max_iter = num_vert1 + num_vert2 + 5;
    int iter = 0;
    while(true){
        // normal vector of point1-point2 (Origin direction)
        zero_dir_normal = normal(point2-point1, -point1);
        // equivalent to zero_dir_normal = normal(point1-point2, -point2);
        Vec2 point3 = mink_dif(
            poly1, num_vert1,
            poly2, num_vert2,
            zero_dir_normal
        );
        if(dot(zero_dir_normal, point3) < 0)
            return false;

        if((abs(point3.x - point1.x) < eps && abs(point3.y - point1.y) < eps) ||
           (abs(point3.x - point2.x) < eps && abs(point3.y - point2.y) < eps)) {
            return false;
        }

        float c1 = cross(point1-point3, -point3);
        float c2 = cross(point2-point3, -point3);
        // outside of point1-point3 or point2-point3
        if(c1 * c2 > 0){
            if(abs(c1) < abs(c2))
                point2 = point3;
            else
                point1 = point3;
        }
        else
            return true;

        // safe guard about inf loop
        if(++iter > max_iter)
            return false;
    }
}

Vec3 ModernBoy::support(
    const Vec3 polygon[], int num_vert,
    const Vec3 v
){
    Vec3 point = polygon[0];
    float max_product = dot(point, v);

    for(int i=1; i<num_vert; ++i){
        float product = dot(polygon[i], v);
        if(product > max_product){
            point = polygon[i];
            max_product = product;
        }
    }

    return point;
}
Vec3 ModernBoy::mink_dif(
    const Vec3 poly1[], int num_vert1,
    const Vec3 poly2[], int num_vert2,
    const Vec3 dir
){
    Vec3 poly1_sup = support(poly1, num_vert1,  dir);
    Vec3 poly2_sup = support(poly2, num_vert2, -dir);
    return poly1_sup - poly2_sup;
}
Vec3 ModernBoy::dir_normal(
    const Vec3 p1, const Vec3 p2, const Vec3 p3,
    const Vec3 tgt_pt
){
    // plane equation: dot(n, x) + d = 0
    Vec3 n = cross(p2-p1, p3-p1);
    float d = -dot(n, p1);

    if(dot(n, tgt_pt) + d > 0)
        return n;
    else
        return -n;
}
Vec3 ModernBoy::barycenter(const Vec3 poly[], int num_vert){
    if(num_vert == 0)
        return zeros();

    Vec3 sum = poly[0];
    for(int i=1; i<num_vert; ++i)
        sum = sum + poly[i];
    return sum / num_vert;
}

bool inTriangle(Vec3 point1, Vec3 point2, Vec3 point3, Vec3 p_plane){
    Vec3 n = cross(point2-point1, point3-point1);

    // p1-p2-p3 can make plane
    if(norm_squared(n) > eps_3d*eps_3d){
        Vec3 c1 = cross(point2-point1, p_plane-point1);
        Vec3 c2 = cross(point3-point2, p_plane-point2);
        Vec3 c3 = cross(point1-point3, p_plane-point3);

        if( dot(c1, n) > 0 &&
            dot(c2, n) > 0 &&
            dot(c3, n) > 0
        )
            return true;
        return false;
    }

    float dist23_sq = norm_squared(point3-point2);
    float dist31_sq = norm_squared(point1-point3);
    float dist12_sq = norm_squared(point2-point1);
    // p1-p2-p3 cannot make line
    if( dist23_sq < eps_3d*eps_3d &&
        dist31_sq < eps_3d*eps_3d &&
        dist12_sq < eps_3d*eps_3d
    ){
        if(norm_squared(point1-p_plane) < eps_3d*eps_3d)
            return true;
        return false;
    }

    if(dist12_sq < eps_3d*eps_3d)
        point2 = point3;
    // now, point1-point2 can make line.
    Vec3 u = point2 - point1;
    float t_sq = norm_squared(p_plane - point1) / norm_squared(u);
    if(0 <= t_sq && t_sq <= 1)
        return true;
    return false;
}

bool ModernBoy::gjk(
    const Vec3 poly1[], int num_vert1,
    const Vec3 poly2[], int num_vert2
){
    if(poly1 == poly2)
        return true;
    if((num_vert1 < 1) || (num_vert2 < 1))
        return false;
    if((num_vert1==1) && (num_vert2==1))
        return poly1[0]==poly2[0];

    Vec3 zero_dir_normal = barycenter(poly1, num_vert1)
        - barycenter(poly2, num_vert2);
    if( abs(zero_dir_normal.x) <= eps_3d &&
        abs(zero_dir_normal.y) <= eps_3d &&
        abs(zero_dir_normal.z) <= eps_3d
    )
        // fallback
        zero_dir_normal = Vec3{{1, 0, 0}};

    Vec3 point1 = mink_dif(
        poly1, num_vert1,
        poly2, num_vert2,
        zero_dir_normal
    );
    Vec3 point2 = mink_dif(
        poly1, num_vert1,
        poly2, num_vert2,
        -point1
    );

    // normal vector of p1-p2 with Origin direction
    Vec3 op1p2_up_dir = cross(cross(point2-point1, -point1), point2-point1);
    if(norm_squared(op1p2_up_dir) < eps_3d*eps_3d)
        op1p2_up_dir = -point1;

    Vec3 point3 = mink_dif(
        poly1, num_vert1,
        poly2, num_vert2,
        op1p2_up_dir
    );

    int max_iter = num_vert1 + num_vert2 + 10;
    int iter = 0;
    while(true){
        // simplex boundary check for p1-p2-p3 plane
        zero_dir_normal = dir_normal(point1, point2, point3, zeros());
        if(norm_squared(zero_dir_normal) < eps_3d*eps_3d){
            return inTriangle(point1, point2, point3, zeros());
        }
        // if(inTriangle(point1, point2, point3, zeros()))
        //     return true;

        Vec3 point4 = mink_dif(
            poly1, num_vert1,
            poly2, num_vert2,
            zero_dir_normal
        );
        // plane-point4 distance is shorter than
        // plane-Origin distance
        if(dot(zero_dir_normal, point4) < 0)
            return false;

        // <-- current state -->
        //
        //         *  <- Point 4
        //
        //         O
        //
        //         /\   ^ plane normal dir
        //        /  \      (towards Origin)
        //       /____\ <- p1p2p3 plane
        // <------------------->

        // simplex boundary check for new point4
        auto n1 = dir_normal(point2, point3, point4, zeros());
        if(inTriangle(point2, point3, point4, zeros()))
            return true;
        auto n2 = dir_normal(point1, point3, point4, zeros());
        if(inTriangle(point1, point3, point4, zeros()))
            return true;
        auto n3 = dir_normal(point1, point2, point4, zeros());
        if(inTriangle(point1, point2, point4, zeros()))
            return true;

        auto n1_same_dir = dot(zero_dir_normal, n1) > 0;
        auto n2_same_dir = dot(zero_dir_normal, n2) > 0;
        auto n3_same_dir = dot(zero_dir_normal, n3) > 0;
        // Origin is located outside of p2-p3-p4 plane
        if(n1_same_dir)
            point1 = point4;
        // Origin is located outside of p1-p3-p4 plane
        else if(n2_same_dir)
            point2 = point4;
        // Origin is located outside of p1-p2-p4 plane
        else if(n3_same_dir)
            point3 = point4;
        // Origin is located inside of simplex
        else
            return true;

        // safe guard about inf loop
        if(++iter > max_iter)
            return false;
    }
}