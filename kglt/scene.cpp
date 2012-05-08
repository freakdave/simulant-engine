#include "glee/GLee.h"
#include "scene.h"
#include "renderer.h"

namespace kglt {

MeshID Scene::new_mesh() {
    static MeshID counter = 0;
    MeshID id = ++counter;

    meshes_.insert(std::make_pair(id, Mesh::ptr(new Mesh)));

    Mesh& mesh = *meshes_[id];
    mesh.set_parent(this);

    return id;
}

Mesh& Scene::mesh(MeshID m) {
    //FIXME: Assert contains
    return *meshes_[m];
}

SpriteID Scene::new_sprite() {
    static SpriteID counter = 0;
    SpriteID id = ++counter;
    sprites_.insert(std::make_pair(id, Sprite::ptr(new Sprite)));
    
    Sprite& sprite = *sprites_[id];
    sprite.set_parent(this);
    
    return id;
}

Sprite& Scene::sprite(SpriteID s) {
    //FIXME: assert contains
    return *sprites_[s];
}

TextureID Scene::new_texture() {
    static TextureID counter = 0;
    TextureID id = ++counter;
    Texture& tex = textures_[id];
    return id;
}

Texture& Scene::texture(TextureID t) {
    //FIXMe: assert this
    return textures_[t];
}

CameraID Scene::new_camera() {
    static CameraID counter = 0;
    CameraID id = ++counter;

    cameras_.insert(std::make_pair(id, Camera::ptr(new Camera)));

    Camera& cam = *cameras_[id];
    cam.set_parent(this);

    //We always need a camera, so if this is the
    //first one, then make it the current one
    if(cameras_.size() == 1) {
        current_camera_ = id;
    }

    return id;
}

ShaderProgram& Scene::shader(ShaderID s) {
    return shaders_[s];
}

ShaderID Scene::new_shader() {
    static ShaderID counter = 0;
    ShaderID id = counter++; //The first shader should be 0 - or the default shader
    ShaderProgram& shader = shaders_[id];
    return id;
}
    
Camera& Scene::camera(CameraID c) {
    //FIXME: Assert

    if(c == 0) {
        return *cameras_[current_camera_];
    }

    return *cameras_[c];
}

void Scene::init() {
    assert(glGetError() == GL_NO_ERROR);
    ShaderProgram& def = shader(new_shader()); //Create a default shader;
    assert(glGetError() == GL_NO_ERROR);
        
    def.add_and_compile(SHADER_TYPE_VERTEX, kglt::get_default_vert_shader_120());
    def.add_and_compile(SHADER_TYPE_FRAGMENT, kglt::get_default_frag_shader_120());
    def.activate();
}

void Scene::update(double dt) {
	for(Object* child: children_) {
		child->update(dt);
	}
}

void Scene::render() {
    Renderer renderer(render_options);

    renderer.start_render(this);

    this->accept(renderer);

    renderer.finish_render();
}

}
