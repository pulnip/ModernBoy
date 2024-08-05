#include "Raytracer.hpp"
#include "Shapes.hpp"

using namespace std;
using namespace gsl;
using namespace DirectX::SimpleMath;
using namespace DirectX::Colors;
using namespace ModernBoy;

Raytracer::Raytracer(const ipoint2& resolution)
:resolution(resolution){
    auto sphere = std::make_shared<Sphere>(
        Vector3(0.0f, -0.1f, 1.5f),
        1.0f
    );

	sphere->alpha = 50.0f;
	sphere->reflection = 0.2f;
	sphere->transparency = 0.2f;

	objects.push_back(sphere);

	auto ground = std::make_shared<Square>(
        Vector3(-10.0f, -1.5f, 10.0f), Vector3(10.0f, -1.5f, 10.0f),
        Vector3(10.0f, -1.5f, 0.0f), Vector3(-10.0f, -1.5f, 0.0f)
    );

	auto groundTexture = std::make_shared<Texture>(
        "resource/shadertoy_abstract1.jpg"
    );
    ground->ambTexture = groundTexture;
    ground->difTexture = groundTexture;

    objects.push_back(ground);

	auto square = std::make_shared<Square>(
        Vector3(-10.0f, 10.0f, 10.0f), Vector3(10.0f, 10.0f, 10.0f),
        Vector3(10.0f, -10.0f, 10.0f), Vector3(-10.0f, -10.0f, 10.0f)
    );

	auto squareTexture = std::make_shared<Texture>(
        "resource/back.jpg"
    );
    square->ambTexture = squareTexture;
    square->difTexture = squareTexture;

	objects.push_back(square);

	light = Light{{0.7f, 0.8f, -0.5f}};
}

void Raytracer::render(vector<RGBA>& pixels){
	const Vector3 eyePos(0.0f, 0.0f, -1.5f);

    const auto width=resolution.x;
    const auto height=resolution.y;
#pragma omp parallel for
	for(int i=0; i<height; ++i){
        const auto floor=width*i;
		for(int j=0; j<width; ++j){
            const auto index=floor+j;

			const auto pixelPosWorld = toWorld({
                static_cast<float>(j), static_cast<float>(i)
            });
            auto diff = pixelPosWorld - eyePos;
            diff.Normalize();

			const Ray pixelRay{pixelPosWorld, std::move(diff)};

            const auto traced = traceRay(pixelRay, 5);
            const auto clamped = clamp(traced, {Black.v, White.v});
            pixels[index] = rgbcvt(clamped);
		}
    }
}

Hit Raytracer::closest(const Ray& ray){
	Hit closestHit=objects[0]->shootRay(ray);
    closestHit.object=objects[0];

    const index num=objects.size();
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

Color Raytracer::traceRay(const Ray& ray, const int level){
    if(level < 0){
		return Black.v;
    }

	// Render first hit
	const auto hit = closest(ray);
	if(hit.distance < 0.0f){
        return Black.v;
    }

    // Diffuse
    auto dirToLight = light.pos - hit.point;
    dirToLight.Normalize();
    const auto cosToLight = hit.normal.Dot(dirToLight);
    const float diffuse = std::max(cosToLight, 0.0f);

    // Specular
    const auto reflectDir = 2.0f * cosToLight * hit.normal - dirToLight;

    const auto cosToEye = reflectDir.Dot(-ray.dir);
    const float specular = pow(std::max(cosToEye, 0.0f), hit.object->alpha);

    Color phongColor=hit.object->specular*specular;

    if (hit.object->difTexture){
		phongColor += diffuse * hit.object->diffuse * hit.object->difTexture->getRGB_lerp(hit.uv);
	}
	else{
		phongColor += diffuse * hit.object->diffuse;
	}

    // Ambient
    if(hit.object->ambTexture){
        phongColor += hit.object->ambient * hit.object->ambTexture->getRGB_lerp(hit.uv);
    }
    else{
        phongColor += hit.object->ambient;
    }


    Color color = phongColor * (1.0f - hit.object->reflection - hit.object->transparency);

	if(hit.object->reflection){
		auto reflectedDirection = 2.0f * hit.normal * hit.normal.Dot(-ray.dir) + ray.dir;
        reflectedDirection.Normalize();
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

		const float cosTheta1 = hit.normal.Dot(-ray.dir);
        const auto inside = cosTheta1 < 0.0f;
        // sin($SOURCE) / sin($TARGET)
		float eta = inside ? 1.0f/ior : ior;
		Vector3 normal = inside ? -hit.normal : hit.normal;

		const float sinTheta1 = sqrt(1.0f - cosTheta1 * cosTheta1);
		const float sinTheta2 = sinTheta1 / eta;
		const float cosTheta2 = sqrt(1.0f - sinTheta2 * sinTheta2);

	    Vector3 m = normal.Dot(-ray.dir) * normal + ray.dir;
        m.Normalize();
        const Vector3 a = m * sinTheta2;
		const Vector3 b = -normal * cosTheta2;
		Vector3 refractedDirection = a + b;
        refractedDirection.Normalize();

		Ray refractionRay{
            hit.point + refractedDirection * 1e-4f,
            refractedDirection
        };
		color += traceRay(refractionRay, level - 1) * hit.object->transparency;
	}

	return color;
}

Vector3 Raytracer::toWorld(const Vector2& screenPos
) noexcept{
    const auto scale=2.0f/resolution.y;
    const auto aspect=static_cast<float>(resolution.x)/resolution.y;
    
    const auto newAxisX= scale*Vector4::UnitX;
    const auto newAxisY=-scale*Vector4::UnitY;
    const Vector4 translate=-aspect*Vector4::UnitX + Vector4::UnitY;
    
    const Vector4 extended{screenPos.x, screenPos.y, 0.0f, 1.0f};
    const Matrix transform{
        newAxisX, newAxisY, Vector4::Zero, translate
    };

    auto worldPos=Vector4::Transform(extended, transform);
    return {worldPos.x, worldPos.y, 0.0f};
}
