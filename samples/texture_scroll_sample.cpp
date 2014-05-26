#include "kglt/kglt.h"
#include "kglt/shortcuts.h"

#include <kazbase/noise.h>

int main(int argc, char* argv[]) {
    logging::get_logger("/")->add_handler(logging::Handler::ptr(new logging::StdIOHandler));
    
    kglt::Window::ptr window = kglt::Window::create();
    window->set_title("KGLT Sample");

    auto stage = window->stage();

    /**
        Generate a mesh and build a 2D square

        Base objects are always created with new_X() and can
        be destroyed with delete_X(). They are held by the object
        that spawned them. For example, meshes are held by the scene.

        Creating an object gives you an ID, this can then be exchanged
        for a reference to an object.
    */

    ///Shortcut function for loading images
    kglt::TextureID tid = stage->new_texture();
    auto tex = stage->texture(tid);
    tex->resize(512, 512);

    auto perlin = noise::PerlinOctave(5);

    auto& data = tex->data();
    for(int y = 0; y < 512; ++y) {
        for(int x = 0; x < 512; ++x) {
            double X = (double)x/((double)512);
            double Y = (double)y/((double)512);

            double d = perlin.noise(X, Y, 0) + 0.5;
            d = std::min(d, 1.0);
            d = std::max(d, 0.0);

            data[((y * 512) + x) * 4] = int(d * 255.0);
            data[((y * 512) + x) * 4 + 1] = int(d * 255.0);
            data[((y * 512) + x) * 4 + 2] = int(d * 255.0);
            data[((y * 512) + x) * 4 + 3] = 255;
        }
    }

    tex->upload();

    kglt::ActorID actor_id = stage->geom_factory().new_rectangle(2.0, 2.0);
    stage->actor(actor_id)->mesh()->set_texture_on_material(0, tid);

    kglt::MaterialID matid;
    {
        auto mesh = stage->actor(actor_id)->mesh();
        matid = mesh->submesh(mesh->submesh_ids()[0]).material_id();
    }

    /**
        Once we have the reference to a base object, we can
        manipulate it easily
    */
    stage->actor(actor_id)->move_to(0.0f, 0.0f, -2.0f);

    window->camera()->set_orthographic_projection_from_height(2.0, (float) window->width() / (float)window->height());

    while(window->run_frame()) {
        stage->material(matid)->pass(0).texture_unit(0).scroll_x(0.5 * window->delta_time());
    }
	return 0;
}
