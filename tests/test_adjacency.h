#pragma once

#include "global.h"
#include "../simulant/meshes/adjacency_info.h"

namespace {

using namespace smlt;

class AdjacencyTests : public SimulantTestCase {
public:
    void test_basic_adjacency_build() {
        auto mesh = window->shared_assets->new_mesh_as_rectangle(1.0, 1.0f).fetch();

        auto adjacency = std::make_shared<AdjacencyInfo>(mesh.get());
        adjacency->rebuild();

        // 2 triangles, with a shared edge
        assert_equal(5u, adjacency->edge_count());

        auto unshared = 0;
        auto shared = 0;

        // Count the shared edges, make sure there is only one
        adjacency->each_edge([&](std::size_t i, const EdgeInfo& edge) {
            if(edge.triangle_count == 2) {
                shared++;
            } else if(edge.triangle_count == 1) {
                unshared++;
            }
        });

        assert_equal(4, unshared);
        assert_equal(1, shared);
    }

    void test_shared_positions_detected() {
        auto mesh = window->shared_assets->new_mesh_as_rectangle(1.0, 1.0f).fetch();

        // Add another vertex, using the same position as the first
        auto i = mesh->shared_data->count();
        mesh->shared_data->move_to_end();
        mesh->shared_data->position(mesh->shared_data->position_at<smlt::Vec3>(1));
        mesh->shared_data->move_next();

        // Add another vertex
        mesh->shared_data->position(10, 10, 10);
        mesh->shared_data->move_next();
        mesh->shared_data->done();

        // Add another triangle
        mesh->first_submesh()->index_data->index(i + 1);
        mesh->first_submesh()->index_data->index(i);
        mesh->first_submesh()->index_data->index(0);
        mesh->first_submesh()->index_data->done();

        auto adjacency = std::make_shared<AdjacencyInfo>(mesh.get());
        adjacency->rebuild();

        auto unshared = 0;
        auto shared = 0;

        // Count the shared edges, make sure there is only one
        adjacency->each_edge([&](std::size_t i, const EdgeInfo& edge) {
            if(edge.triangle_count == 2) {
                shared++;
            } else if(edge.triangle_count == 1) {
                unshared++;
            }
        });

        // Should've detected another shared edges
        assert_equal(2, shared);
    }

    void test_rebuild_on_index_data_change() {
        auto mesh = window->shared_assets->new_mesh_as_rectangle(1.0f, 1.0f).fetch();
        assert_is_not_null(mesh->adjacency_info.get());
        assert_equal(5u, mesh->adjacency_info->edge_count());
    }
};

}