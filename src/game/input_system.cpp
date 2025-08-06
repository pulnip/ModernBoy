#include "input/chord.hpp"
#include "game/entity_registry.hpp"
#include "game/input_system.hpp"
#include "physics.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Game;
using namespace ModernBoy::Input;

InputSystem::InputSystem(EntityRegistry& registry,
    ModernBoy::Input::Chord& input)
:registry(registry), input(input){}

void InputSystem::update(DeltaTime dt){
    auto dt_ = dt.count() / 1'000'000.0f;

    for(auto [id, bit, tc, _1]: registry.query<Transform, Player>()){
        if(input.query(KEY_A, Held))
            tc.position -= 10 * dt_ * ground_right(tc.rotation);
        if(input.query(KEY_D, Held))
            tc.position += 10 * dt_ * ground_right(tc.rotation);
        if(input.query(KEY_W, Held))
            tc.position += 10 * dt_ * ground_forward(tc.rotation);
        if(input.query(KEY_S, Held))
            tc.position -= 10 * dt_ * ground_forward(tc.rotation);

        if(input.query(KEY_Q, Held))
            tc.rotation = rotateY(-3.14/2 * dt_) * tc.rotation;
        if(input.query(KEY_E, Held))
            tc.rotation = rotateY( 3.14/2 * dt_) * tc.rotation;
        // if(input.query(KEY_SPACE, Pressed))
        //     jump;
    }

    for(auto [id, bit, tc, _2]: registry.query<Transform, Editor>()){
        if(input.query(KEY_A, Held))
            tc.position -= 10 * dt_ * right(tc.rotation);
        if(input.query(KEY_D, Held))
            tc.position += 10 * dt_ * right(tc.rotation);
        if(input.query(KEY_W, Held))
            tc.position += 10 * dt_ * forward(tc.rotation);
        if(input.query(KEY_S, Held))
            tc.position -= 10 * dt_ * forward(tc.rotation);
        if(input.query(KEY_SPACE, Held))
            tc.position += 10 * dt_ * up(tc.rotation);
        if(input.query(KEY_SHIFT, Held))
            tc.position -= 10 * dt_ * up(tc.rotation);

        Vec2 mouse_move = input.mouseMove();

        if(norm_squared(mouse_move) > 0){
            Vec3 mouse_vec{
                .x = mouse_move.x,
                .y = -mouse_move.y,
                .z = 0
            };

            Vec3 axis = normalize(cross(Vec3{{0, 0, 1}}, mouse_vec));

            float theta = norm(mouse_move) / 10.0f;
            Vec4 mouse_quat = axisAngle(axis, theta * dt_);
    
            tc.rotation = tc.rotation * mouse_quat;
        }

        if(input.query(KEY_Q, Held))
            tc.rotation = tc.rotation * rotateZ( 3.14/2 * dt_);
        if(input.query(KEY_E, Held))
            tc.rotation = tc.rotation * rotateZ(-3.14/2 * dt_);
    }

    for(auto [id, bit, tf, cam]: registry.query<Transform, Camera>()){
        auto m_pos = input.mousePos();
        auto fov_radian = cam.fov * M_PI / 180;

        Ray ray{
            .point = tf.position,
            .dir = normalize(
                m_pos.x * right(tf.rotation) +
                m_pos.y * up(tf.rotation) +
                1/std::tanf(fov_radian/2.0f) * forward(tf.rotation)
            )
        };

        for(auto [id, bit, c_tf, sc, model]: registry.query<Transform ,SphereCollider, Model>()){
            RaycastHit result;
            if(raycastSphere(ray, c_tf.position+sc.position, sc.radius, result))
                model.alpha = 0.5f;
            else
                model.alpha = 1.0;
        }
    }
}