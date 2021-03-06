//
//   Copyright (c) 2011-2017 Luke Benstead https://simulant-engine.appspot.com
//
//     This file is part of Simulant.
//
//     Simulant is free software: you can redistribute it and/or modify
//     it under the terms of the GNU Lesser General Public License as published by
//     the Free Software Foundation, either version 3 of the License, or
//     (at your option) any later version.
//
//     Simulant is distributed in the hope that it will be useful,
//     but WITHOUT ANY WARRANTY; without even the implied warranty of
//     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//     GNU Lesser General Public License for more details.
//
//     You should have received a copy of the GNU Lesser General Public License
//     along with Simulant.  If not, see <http://www.gnu.org/licenses/>.
//

#include "renderer.h"

namespace smlt {

void Renderer::register_texture(TextureID tex_id, TexturePtr texture) {
    on_texture_register(tex_id, texture);

    thread::Lock<thread::Mutex> lock(texture_registry_mutex_);
    texture_registry_.insert(std::make_pair(tex_id, std::weak_ptr<Texture>(texture)));
}

void Renderer::unregister_texture(TextureID texture_id, Texture* texture) {
    {
        thread::Lock<thread::Mutex> lock(texture_registry_mutex_);
        texture_registry_.erase(texture_id);
    }

    on_texture_unregister(texture_id, texture);
}

bool Renderer::is_texture_registered(TextureID texture_id) const {
    thread::Lock<thread::Mutex> lock(texture_registry_mutex_);
    return texture_registry_.count(texture_id);
}

void Renderer::pre_render() {
    thread::Lock<thread::Mutex> lock(texture_registry_mutex_);
    for(auto wptr: texture_registry_){
        if(auto ptr = wptr.second.lock()) {
            prepare_texture(ptr);
        }
    }
}

void Renderer::prepare_texture(TexturePtr tex) {
    assert(tex);
    on_texture_prepare(tex);
}


}
