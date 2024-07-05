#include "Raytracer.hpp"
#include "Shapes.hpp"

using namespace std;
using namespace gsl;
using namespace glm;
using namespace ModernBoy;

Raytracer::Raytracer(const ivec2& resolution)
:resolution(resolution){
    auto sphere1 = std::make_shared<Sphere>(vec3(0.0f, -0.1f, 1.5f), 1.0f);

	sphere1->ambient = vec3(0.2f);
	sphere1->diffuse = vec3(0.0f, 0.0f, 1.0f);
	sphere1->specular = vec3(0.0f);
	sphere1->alpha = 50.0f;
	sphere1->reflection = 0.2f;
	sphere1->transparency = 0.2f;

	objects.push_back(sphere1);

	auto groundTexture = std::make_shared<Texture>("resource/shadertoy_abstract1.jpg");

	auto ground = std::make_shared<Square>(
        vec3(-10.0f, -1.5f, 10.0f), vec3(10.0f, -1.5f, 10.0f),
        vec3(10.0f, -1.5f, 0.0f), vec3(-10.0f, -1.5f, 0.0f),
        vec2(0.0f, 0.0f), vec2(1.0f, 0.0f),
        vec2(1.0f, 1.0f), vec2(0.0f, 1.0f)
    );

	ground->ambient = vec3(1.0f);
	ground->diffuse = vec3(1.0f);
	ground->specular = vec3(1.0f);
	ground->alpha = 10.0f;
	ground->reflection = 0.0f;
	ground->ambTexture = groundTexture;
	ground->difTexture = groundTexture;

    objects.push_back(ground);

	auto squareTexture = std::make_shared<Texture>("resource/back.jpg");
	auto square = std::make_shared<Square>(
        vec3(-10.0f, 10.0f, 10.0f), vec3(10.0f, 10.0f, 10.0f),
        vec3(10.0f, -10.0f, 10.0f), vec3(-10.0f, -10.0f, 10.0f),
		vec2(0.0f, 0.0f), vec2(1.0f, 0.0f),
        vec2(1.0f, 1.0f), vec2(0.0f, 1.0f)
    );

	square->ambient = vec3(1.0f);
	square->diffuse = vec3(0.0f);
	square->specular = vec3(0.0f);
	square->alpha = 10.0f;
	square->reflection = 0.0f;
	square->ambTexture = squareTexture;
	square->difTexture = squareTexture;

	objects.push_back(square);

	light = Light{{0.0f, 0.3f, -0.5f}}; // 화면 뒷쪽
}

void Raytracer::render(vector<vec4>& pixels){
    std::fill(pixels.begin(), pixels.end(), vec4{0.0f, 0.0f, 0.0f, 1.0f});

	const vec3 eyePos(0.0f, 0.0f, -1.5f);

    const auto width=resolution.x;
    const auto height=resolution.y;
#pragma omp parallel for
	for (index i=0; i<height; ++i){
        const auto floor=width*i;
		for (index j=0; j<width; ++j){
            const auto index=floor+j;

			const vec3 pixelPosWorld = toWorld(vec2(j, i));
			Ray pixelRay{pixelPosWorld, normalize(pixelPosWorld - eyePos)};

            pixels[index] = vec4(clamp(traceRay(pixelRay, 5), 0.0f, 1.0f), 1.0f);
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

constexpr fRGB black(0.0f);

fRGB Raytracer::traceRay(const Ray& ray, const int level){
    if (level < 0){
		return black;
    }

	// Render first hit
	const auto hit = closest(ray);
	if (hit.distance < 0.0f){
        return black;
    }

    // Diffuse
    const auto diffHitToLight = light.pos - hit.point;
    const auto dirToLight = glm::normalize(diffHitToLight);
    const auto cosToLight = glm::dot(hit.normal, dirToLight);
    const float diffuse = glm::max(cosToLight, 0.0f);

    // Specular
    const auto reflectDir = 2.0f * cosToLight * hit.normal - dirToLight;

    const auto cosToEye = glm::dot(reflectDir, -ray.dir);
    const float specular = glm::pow(glm::max(cosToEye, 0.0f), hit.object->alpha);

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

	if (hit.object->reflection){
		const auto reflectedDirection = glm::normalize(2.0f * hit.normal * dot(-ray.dir, hit.normal) + ray.dir);
		Ray reflection_ray{hit.point + reflectedDirection * 1e-4f, reflectedDirection}; // add a small vector to avoid numerical issue

		color += traceRay(reflection_ray, level - 1) * hit.object->reflection;
	}

	if (hit.object->transparency){
		const float ior = 1.5f; // Index of refraction (glass: 1.5, water: 1.3)

		float eta; // sinTheta1 / sinTheta2
		vec3 normal;

		if (glm::dot(ray.dir, hit.normal) < 0.0f){
			eta = ior;
			normal = hit.normal;
		}
		else{
			eta = 1.0f / ior;
			normal = -hit.normal;
		}

		const float cosTheta1 = glm::dot(-ray.dir, normal);
		const float sinTheta1 = sqrt(1.0f - cosTheta1 * cosTheta1);
		const float sinTheta2 = sinTheta1 / eta;
		const float cosTheta2 = sqrt(1.0f - sinTheta2 * sinTheta2);

		const vec3 m = glm::normalize(dot(normal, -ray.dir) * normal + ray.dir);
		const vec3 a = m * sinTheta2;
		const vec3 b = -normal * cosTheta2;
		const vec3 refractedDirection = glm::normalize(a + b);

		Ray refractionRay{
            hit.point + refractedDirection * 1e-4f,
            refractedDirection
        };
		color += traceRay(refractionRay, level - 1) * hit.object->transparency;
	}

	return color;
}

vec3 Raytracer::toWorld(const vec2& screenPos){
    const auto aspect=static_cast<float>(resolution.x)/resolution.y;
    const auto scale=2.0f/resolution.y;
    
    const vec3 afScreenPos{screenPos.x, screenPos.y, 1.0f};
    const mat3x3 afTransfrom_T{
        {scale,   0.0f, -aspect},
        { 0.0f, -scale,    1.0f},
        { 0.0f,   0.0f,    1.0f}
    };
    const auto afTransform=transpose(afTransfrom_T);
    const auto afWorldPos=afTransform*afScreenPos;
    const vec3 v3ext{afWorldPos.x, afWorldPos.y, 0.0f};

    return v3ext;
}
