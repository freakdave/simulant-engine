/* *   Copyright (c) 2011-2017 Luke Benstead https://simulant-engine.appspot.com
 *
 *     This file is part of Simulant.
 *
 *     Simulant is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU Lesser General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 *
 *     Simulant is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU Lesser General Public License for more details.
 *
 *     You should have received a copy of the GNU Lesser General Public License
 *     along with Simulant.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "../generic/managed.h"
#include "../interfaces.h"
#include "../meshes/mesh.h"
#include "../sound.h"

#include "stage_node.h"

namespace smlt {


class GeomCuller;

/**
 * @brief The Geom class
 *
 * A Geom is a fixed piece of geometry, like Actors they are
 * constructed from a mesh, but unlike actors they are completely
 * immovable during their lifetime. This gives partitioners
 * the freedom to split the geometry as necessary for improved performance
 * or even store entirely cached versions of the geometry.
 *
 * Also unlike an actor, a mesh is a requirement.
 */
class Geom :
    public StageNode,
    public virtual Boundable,
    public generic::Identifiable<GeomID>,
    public Source {

public:
    Geom(
        GeomID id,
        Stage* stage,
        SoundDriver *sound_driver,
        MeshID mesh,
        const Vec3& position=Vec3(),
        const Quaternion rotation=Quaternion(),
        uint8_t octree_max_depth=5
    );

    const AABB& aabb() const override;

    void destroy() override;

    RenderPriority render_priority() const { return render_priority_; }

    void clean_up() override {
        StageNode::clean_up();
    }

    Property<Geom, GeomCuller> culler = {this, &Geom::culler_};

    bool init() override;

    void _get_renderables(RenderableFactory* factory, CameraPtr camera, DetailLevel detail_level) override;
private:
    MeshID mesh_id_;
    RenderPriority render_priority_ = RENDER_PRIORITY_MAIN;
    uint8_t octree_max_depth_;

    std::shared_ptr<GeomCuller> culler_;

    AABB aabb_;

    void update(float dt) override {
        update_source(dt);
    }
};

}

