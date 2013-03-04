#ifndef TEST_VERTEX_DATA_H
#define TEST_VERTEX_DATA_H

#include "kglt/kglt.h"
#include "kglt/kazbase/testing.h"

#include "global.h"

class VertexDataTest : public TestCase {
public:
    void test_offsets() {
        VertexData::ptr data = VertexData::create();

        assert_equal(0, (int32_t) data->position_offset());
        assert_equal(sizeof(float) * 3, data->normal_offset());
    }
};

#endif // TEST_VERTEX_DATA_H
