#include "Raytracer.hpp"
#include "Shapes.hpp"

using namespace std;
using namespace gsl;
using namespace glm;
using namespace ModernBoy;

Raytracer::Raytracer(const ivec2& resolution)
:resolution(resolution){
    auto sphere = std::make_shared<Sphere>(
        vec3(0.0f, -0.1f, 1.5f),
        1.0f
    );

	sphere->alpha = 50.0f;
	sphere->reflection = 0.2f;
	sphere->transparency = 0.2f;

	objects.push_back(sphere);

	auto ground = std::make_shared<Square>(
        vec3(-10.0f, -1.5f, 10.0f), vec3(10.0f, -1.5f, 10.0f),
        vec3(10.0f, -1.5f, 0.0f), vec3(-10.0f, -1.5f, 0.0f)
    );

	auto groundTexture = std::make_shared<Texture>(
        "resource/shadertoy_abstract1.jpg"
    );
    ground->ambTexture = groundTexture;
    ground->difTexture = groundTexture;

    objects.push_back(ground);

	auto square = std::make_shared<Square>(
        vec3(-10.0f, 10.0f, 10.0f), vec3(10.0f, 10.0f, 10.0f),
        vec3(10.0f, -10.0f, 10.0f), vec3(-10.0f, -10.0f, 10.0f)
    );

	auto squareTexture = std::make_shared<Texture>(
        "resource/back.jpg"
    );
    square->ambTexture = squareTexture;
    square->difTexture = squareTexture;

	objects.push_back(square);

	light = Light{{0.7f, 0.8f, -0.5f}};
}

void Raytracer::render(vector<fRGBA>& pixels){
	const vec3 eyePos(0.0f, 0.0f, -1.5f);

    const auto width=resolution.x;
    const auto height=resolution.y;
#pragma omp parallel for
	for(index i=0; i<height; ++i){
        const auto floor=width*i;
		for(index j=0; j<width; ++j){
            const auto index=floor+j;

			const vec3 pixelPosWorld = toWorld(vec2(j, i));
			const Ray pixelRay{pixelPosWorld, normalize(pixelPosWorld - eyePos)};

            const auto traced = traceRay(pixelRay, 5);
            const auto clamped = clamp(traced, 0.0f, 1.0f);
            pixels[index] = toRGBA(clamped);
		}
    }
}

Hit Raytracer::closest(const Ray& ray){
	Hit closestHit=objects[0]->shootRay(ray);
    closestHit.object=objects[0];

    const auto num=objects.size();
    for(index i=1; i<num; ++i){
        auto newHit=objects[i]->shootRay(ray);

        const auto newHitValid = newHit.distance >= 0.0f;
        const auto closestHitInvalid = closestHit.distance < 0.0f;
        const auto closer = newHit.distance < closestHit.distance;
    
        if(newHitValid && (closestHitInvalid||closer)){
            closestHit=newHit;
            closestHit.object=objects[i];
        }
    }

    return closestHit;
}

fRGB Raytracer::traceRay(const Ray& ray, const int level){
    if(level < 0){
		return fBLACK;
    }

	// Render first hit
	const auto hit = closest(ray);
	if(hit.distance < 0.0f){
        return fBLACK;
    }

    // Diffuse
    const auto diffHitToLight = light.pos - hit.point;
    const auto dirToLight = normalize(diffHitToLight);
    const auto cosToLight = dot(hit.normal, dirToLight);
    const float diffuse = glm::max(cosToLight, 0.0f);

    // Specular
    const auto reflectDir = 2.0f * cosToLight * hit.normal - dirToLight;

    const auto cosToEye = dot(reflectDir, -ray.dir);
    const float specular = pow(glm::max(cosToEye, 0.0f), hit.object->alpha);

    // Ambient
    const auto ambient = hit.object->ambient;

    fRGB phongColor=hit.object->specular*specular;

    if (hit.object->difTexture){
		phongColor += diffuse * hit.object->diffuse * hit.object->difTexture->getRGB_lerp(hit.uv);
	}
	else{
		phongColor += diffuse * hit.object->diffuse;
	}

    if(hit.object->ambTexture){
        phongColor += hit.object->ambient * hit.object->ambTexture->getRGB_lerp(hit.uv);
    }
    else{
        phongColor += hit.object->ambient;
    }


    fRGB color = phongColor * (1.0f - hit.object->reflection - hit.object->transparency);

	if(hit.object->reflection){
		const auto reflectedDirection = normalize(2.0f * hit.normal * dot(-ray.dir, hit.normal) + ray.dir);
		Ray reflection_ray{hit.point + reflectedDirection * 1e-4f, reflectedDirection}; // add a small vector to avoid numerical issue

		color += traceRay(reflection_ray, level - 1) * hit.object->reflection;
	}

	if(hit.object->transparency){
		// Index of Refraction
        // Air: 1.000'293
        // Water: 1.333
        // Glass: 1.52
        // Diamond: 2.417
        const float ior = 2.417f;

		const float cosTheta1 = dot(-ray.dir, hit.normal);
        const auto inside = cosTheta1 < 0.0f;
        // sin($SOURCE) / sin($TARGET)
		float eta = inside ? 1.0f/ior : ior;
		vec3 normal = inside ? -hit.normal : hit.normal;

		const float sinTheta1 = sqrt(1.0f - cosTheta1 * cosTheta1);
		const float sinTheta2 = sinTheta1 / eta;
		const float cosTheta2 = sqrt(1.0f - sinTheta2 * sinTheta2);

		const vec3 m = normalize(dot(normal, -ray.dir) * normal + ray.dir);
		const vec3 a = m * sinTheta2;
		const vec3 b = -normal * cosTheta2;
		const vec3 refractedDirection = normalize(a + b);

		Ray refractionRay{
            hit.point + refractedDirection * 1e-4f,
            refractedDirection
        };
		color += traceRay(refractionRay, level - 1) * hit.object->transparency;
	}

	return color;
}

vec3 Raytracer::toWorld(const vec2& screenPos){
    const auto scale=2.0f/resolution.y;
    const auto aspect=static_cast<float>(resolution.x)/resolution.y;
    
    const vec3 afX {  scale,   0.0f, 0.0f};
    const vec3 afY {   0.0f, -scale, 0.0f};
    const vec3 move{-aspect,   1.0f, 1.0f};
    
    const vec3 afScreenPos{screenPos, 1.0f};
    const mat3x3 afTransform{
        afX, afY, move
    };
    
    auto afWorldPos=afTransform*afScreenPos;
    afWorldPos.z = 0.0f;
    return afWorldPos;
}
