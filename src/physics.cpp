#include <cmath>
#include "physics.hpp"

using namespace ModernBoy;

constexpr float eps = 1e-6;

Vec2 ModernBoy::support(
    Vec2 polygon[], int num_vert,
    Vec2 v
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
    Vec2 poly1[], int num_vert1,
    Vec2 poly2[], int num_vert2,
    Vec2 dir
){
    Vec2 poly1_sup = support(poly1, num_vert1,  dir);
    Vec2 poly2_sup = support(poly2, num_vert2, -dir);
    return poly1_sup - poly2_sup;
}
Vec2 ModernBoy::normal(Vec2 v, Vec2 dir){
    Vec2 candidate1 = {{-v.y, v.x}};
    Vec2 candidate2 = {{v.y, -v.x}};

    if(dot(candidate1, dir) > 0)
        return candidate1;
    else
        return candidate2;

}
Vec2 ModernBoy::barycenter(Vec2 poly[], int num_vert){
    if(num_vert == 0)
        return {{0, 0}};

    Vec2 sum = poly[0];
    for(int i=1; i<num_vert; ++i)
        sum = sum + poly[i];
    return sum / num_vert;
}
bool ModernBoy::gjk(
    Vec2 poly1[], int num_vert1,
    Vec2 poly2[], int num_vert2
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