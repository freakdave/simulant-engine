#ifndef TEST_VERTEX_DATA_H
#define TEST_VERTEX_DATA_H

#include "simulant/simulant.h"
#include "kaztest/kaztest.h"

#include "global.h"

class VertexDataTest : public SimulantTestCase {
public:
    void test_offsets() {
        smlt::VertexSpecification spec = {
            smlt::VERTEX_ATTRIBUTE_3F,
            smlt::VERTEX_ATTRIBUTE_3F,
            smlt::VERTEX_ATTRIBUTE_2F
        };

        smlt::VertexData::ptr data = smlt::VertexData::create(spec);

        assert_equal(0, (int32_t) data->specification().position_offset());
        assert_equal(sizeof(float) * 3, data->specification().normal_offset());
        assert_equal(sizeof(float) * 6, data->specification().texcoord0_offset());
    }

    void test_basic_usage() {
        smlt::VertexSpecification spec = smlt::VertexSpecification::POSITION_AND_DIFFUSE;
        spec.texcoord0_attribute = smlt::VERTEX_ATTRIBUTE_2F;

        smlt::VertexData data(spec);

        assert_equal(0u, data.data_size());

        data.position(0, 0, 0);
        data.tex_coord0(1, 1);

        assert_equal(sizeof(float) * 9, data.data_size());
        data.move_next();
        data.position(0, 0, 0);
        data.tex_coord0(2, 2);
        data.done();

        assert_equal(sizeof(float) * 18, data.data_size());
    }
};

#endif // TEST_VERTEX_DATA_H
