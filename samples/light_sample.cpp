#include "simulant/simulant.h"
#include "simulant/shortcuts.h"
#include "simulant/extra.h"

using namespace smlt;
using namespace smlt::extra;

class GameScene : public smlt::Scene<GameScene> {
public:
    GameScene(WindowBase& window):
        smlt::Scene<GameScene>(window) {}

    void do_load() {
        prepare_basic_scene(stage_id_, camera_id_);
        auto stage = window->stage(stage_id_);

        camera_id_.fetch()->set_perspective_projection(
            Degrees(45.0),
            float(window->width()) / float(window->height()),
            0.1,
            1000.0
        );

        stage->set_ambient_light(smlt::Colour(0.2, 0.2, 0.2, 1.0));

        actor_id_ = stage->new_actor_with_mesh(stage->assets->new_mesh_as_cube(2.0));
        stage->actor(actor_id_)->move_to(0.0, 0.0, -5.0);

        smlt::TextureID texture = stage->assets->new_texture_from_file("sample_data/crate.png");
        stage->actor(actor_id_)->mesh()->set_texture_on_material(0, texture);

        // Test Camera::look_at function
        stage->camera(camera_id_)->look_at(stage->actor(actor_id_)->absolute_position());

        {
            auto light = stage->new_light_as_point(Vec3(5, 0, -5), smlt::Colour::GREEN).fetch();
            light->set_attenuation_from_range(20.0);

            auto light2 = stage->new_light_as_point(Vec3(-5, 0, -5), smlt::Colour::BLUE).fetch();
            light2->set_attenuation_from_range(30.0);

            auto light3 = stage->new_light_as_point(Vec3(0, -15, -5), smlt::Colour::RED).fetch();
            light3->set_attenuation_from_range(50.0);

            stage->new_light_as_directional(Vec3(1, 0, 0), smlt::Colour::YELLOW);
        }

        for(auto i = 0; i < window->joypad_count(); ++i) {
            window->joypad(i).axis_while_nonzero_connect(JOYPAD_AXIS_X, [&](AxisRange v, JoypadAxis) {
                actor_id_.fetch()->rotate_global_y_by(smlt::Degrees(v * 360.0f));
            });
        }
    }

    void fixed_update(float dt) {
        window->stage(stage_id_)->actor(actor_id_)->rotate_x_by(smlt::Degrees(dt * 20.0));
        window->stage(stage_id_)->actor(actor_id_)->rotate_y_by(smlt::Degrees(dt * 15.0));
        window->stage(stage_id_)->actor(actor_id_)->rotate_z_by(smlt::Degrees(dt * 25.0));
    }

private:
    CameraID camera_id_;
    StageID stage_id_;
    ActorID actor_id_;
};

class LightingSample: public smlt::Application {
public:
    LightingSample():
        Application("Simulant Light Sample") {
    }

private:
    bool do_init() {
        register_scene<GameScene>("main");
        return true;
    }
};

int main(int argc, char* argv[]) {
    LightingSample app;
    return app.run();
}

