#pragma once

#include <array>
#include "vec3.h"

namespace smlt {

class AABB {
    /* This was originally a basic struct but for performance reasons it's now a class
     * so that we can store things like pre-calculate corners and know they are kept up-to-date
     * with setters */

    Vec3 min_;
    Vec3 max_;

    mutable std::array<Vec3, 8> corners_;
    mutable bool corners_dirty_ = true;

    void rebuild_corners() const {
        corners_[0] = Vec3(min_.x, min_.y, min_.z);
        corners_[1] = Vec3(max_.x, min_.y, min_.z);
        corners_[2] = Vec3(max_.x, min_.y, max_.z);
        corners_[3] = Vec3(min_.x, min_.y, max_.z);

        corners_[4] = Vec3(min_.x, max_.y, min_.z);
        corners_[5] = Vec3(max_.x, max_.y, min_.z);
        corners_[6] = Vec3(max_.x, max_.y, max_.z);
        corners_[7] = Vec3(min_.x, max_.y, max_.z);

        corners_dirty_ = false;
    }

public:
    void set_min(const Vec3& min) {
        if(min_ != min) {
            min_ = min;
            corners_dirty_ = true;
        }
    }

    void set_min_x(float x) {
        if(x != min_.x) {
            min_.x = x;
            corners_dirty_ = true;
        }
    }

    void set_min_y(float y) {
        if(y != min_.y) {
            min_.y = y;
            corners_dirty_ = true;
        }
    }

    void set_min_z(float z) {
        if(z != min_.z) {
            min_.z = z;
            corners_dirty_ = true;
        }
    }


    void set_max_x(float x) {
        if(x != max_.x) {
            max_.x = x;
            corners_dirty_ = true;
        }
    }

    void set_max_y(float y) {
        if(y != max_.y) {
            max_.y = y;
            corners_dirty_ = true;
        }
    }

    void set_max_z(float z) {
        if(z != max_.z) {
            max_.z = z;
            corners_dirty_ = true;
        }
    }

    void set_max(const Vec3& max) {
        if(max_ != max) {
            max_ = max;
            corners_dirty_ = true;
        }
    }

    const Vec3& min() const { return min_; }
    const Vec3& max() const { return max_; }

    AABB() {
        rebuild_corners();
    }

    AABB(const Vec3& min, const Vec3& max);

    AABB(const Vec3& centre, float width);

    AABB(const Vec3& centre, float xsize, float ysize, float zsize);

    AABB(const Vec3* vertices, const std::size_t count);

    const float width() const {
        return fabs(max_.x - min_.x);
    }

    const float height() const {
        return fabs(max_.y - min_.y);
    }

    const float depth() const  {
        return fabs(max_.z - min_.z);
    }

    const Vec3 dimensions() const {
        return Vec3(width(), height(), depth());
    }

    const float max_dimension() const {
        return std::max(width(), std::max(height(), depth()));
    }

    const float min_dimension() const {
        return std::min(width(), std::min(height(), depth()));
    }

    bool intersects(const AABB& other) const {
        auto acx = (min_.x + max_.x) * 0.5;
        auto acy = (min_.y + max_.y) * 0.5;
        auto acz = (min_.z + max_.z) * 0.5;

        auto bcx = (other.min_.x + other.max_.x) * 0.5;
        auto bcy = (other.min_.y + other.max_.y) * 0.5;
        auto bcz = (other.min_.z + other.max_.z) * 0.5;

        auto arx = (max_.x - min_.x) * 0.5;
        auto ary = (max_.y - min_.y) * 0.5;
        auto arz = (max_.z - min_.z) * 0.5;

        auto brx = (other.max_.x - other.min_.x) * 0.5;
        auto bry = (other.max_.y - other.min_.y) * 0.5;
        auto brz = (other.max_.z - other.min_.z) * 0.5;

        bool x = fabs(acx - bcx) <= (arx + brx);
        bool y = fabs(acy - bcy) <= (ary + bry);
        bool z = fabs(acz - bcz) <= (arz + brz);

        return x && y && z;
    }

    Vec3 centre() const {
        return Vec3(min_) + ((Vec3(max_) - Vec3(min_)) * 0.5f);
    }

    const bool has_zero_area() const {
        /*
         * Returns True if the AABB has two or more zero dimensions
         */
        bool empty_x = width() == 0.0f;
        bool empty_y = height() == 0.0f;
        bool empty_z = depth() == 0.0f;

        return (empty_x && empty_y) || (empty_x && empty_z) || (empty_y && empty_z);
    }

    bool contains_point(const Vec3& p) const {
        if(p.x >= min_.x && p.x <= max_.x &&
           p.y >= min_.y && p.y <= max_.y &&
           p.z >= min_.z && p.z <= max_.z) {
            return true;
        }

        return false;
    }

    /* NOTE: The corners are recalculated when called, so don't hold onto a reference for long */
    const std::array<Vec3, 8>& corners() const {
        if(corners_dirty_) {
            rebuild_corners();
        }

        return corners_;
    }
};

}
