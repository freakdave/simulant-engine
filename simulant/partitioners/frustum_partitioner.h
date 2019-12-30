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

#include "../partitioner.h"

namespace smlt {

class SubActor;

class FrustumPartitioner : public Partitioner {
public:
    FrustumPartitioner(Stage* ss):
        Partitioner(ss) {}

    void lights_and_geometry_visible_from(
        CameraID camera_id,
        std::vector<LightID> &lights_out,
        std::vector<StageNode*> &geom_out
    );

private:
    void apply_staged_write(const UniqueIDKey& key, const StagedWrite& write);

    std::set<UniqueIDKey> all_nodes_;
};

}
