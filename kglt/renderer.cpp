#include "renderer.h"
#include "scene.h"
#include "window_base.h"

namespace kglt {
	
Renderer::Renderer(Scene& scene):
	scene_(scene) {

	set_perspective_projection(
		45.0, 
		float(scene_.window().width()) / float(scene_.window().height()),
		0.1,
		1000.0
	);
}

Renderer::Renderer(Scene& scene, const RenderOptions& options):
    options_(options),
    scene_(scene) {
		
	set_perspective_projection(
		45.0, 
		float(scene_.window().width()) / float(scene_.window().height()),
		0.1,
		1000.0
	);		
}

void Renderer::start_render() {
    kmMat4* modelview = &modelview_stack_.top();
    scene().camera().apply(modelview);
    
    on_start_render();
}

void Renderer::set_perspective_projection(double fov, double aspect, double near, double far) {
    kmMat4PerspectiveProjection(&projection_stack().top(), fov, aspect, near, far);
}

void Renderer::set_orthographic_projection(double left, double right, double bottom, double top, double near, double far) {
    kmMat4OrthographicProjection(&projection_stack().top(), left, right, bottom, top, near, far);
}

double Renderer::set_orthographic_projection_from_height(double desired_height_in_units, double ratio) {
    double width = desired_height_in_units * ratio;
    set_orthographic_projection(-width / 2.0, width / 2.0, -desired_height_in_units / 2.0, desired_height_in_units / 2.0, -10.0, 10.0);	
    return width;
}

}
