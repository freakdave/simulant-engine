#include "transformable.h"

namespace smlt {

void Transformable::lock_rotation(bool value) {
    rotation_locked_ = value;
}

void Transformable::lock_translation(bool value) {
    translation_locked_ = value;
}

void Transformable::move_to(const smlt::Vec3& pos) {
    if(translation_locked_) return;

    set_position(pos);
}

void Transformable::move_to(const smlt::Vec2& pos) {
    move_to(Vec3(pos, 0));
}

void Transformable::move_to(float x, float y, float z) {
    move_to(Vec3(x, y, z));
}

void Transformable::move_to(float x, float y) {
    move_to(Vec2(x, y));
}

void Transformable::move_by(const smlt::Vec3& pos) {
    if(translation_locked_) return;

    set_position(
        Vec3(
            position_.x + pos.x,
            position_.y + pos.y,
            position_.z + pos.z
        )
    );
}

void Transformable::move_by(const smlt::Vec2& pos) {
    move_by(Vec3(pos, 0));
}

void Transformable::move_by(float x, float y, float z) {
    move_by(Vec3(x, y, z));
}

void Transformable::move_by(float x, float y) {
    move_by(Vec2(x, y));
}

void Transformable::rotate_to(const smlt::Degrees& angle, float axis_x, float axis_y, float axis_z) {
    rotate_to(angle, Vec3(axis_x, axis_y, axis_z));
}

void Transformable::rotate_to(const smlt::Degrees& angle, const smlt::Vec3& axis) {
    Quaternion new_rot;
    kmQuaternionRotationAxisAngle(&new_rot, &axis, angle.value_);
    set_rotation(new_rot);
}

void Transformable::rotate_to(const smlt::Quaternion& rotation) {
    if(rotation_locked_) return;

    set_rotation(rotation);
}

void Transformable::rotate_x_by(const smlt::Degrees& angle) {
    rotate_around(right(), angle);
}

void Transformable::rotate_y_by(const smlt::Degrees& angle) {
    rotate_around(up(), angle);
}

void Transformable::rotate_z_by(const smlt::Degrees& angle) {
    rotate_around(forward(), angle);
}

void Transformable::scale_x_by(const float scale) {
    set_scaling(Vec3(scaling_.x * scale, scaling_.y, scaling_.z));
}

void Transformable::scale_y_by(const float scale) {
    set_scaling(Vec3(scaling_.x, scaling_.y * scale, scaling_.z));
}

void Transformable::scale_z_by(const float scale) {
    set_scaling(Vec3(scaling_.x, scaling_.y, scaling_.z * scale));
}

void Transformable::scale_by(float x) {
    set_scaling(Vec3(scaling_.x * x, scaling_.y * x, scaling_.z * x));
}

void Transformable::scale_by(const Vec3& x) {
    scale_x_by(x.x);
    scale_y_by(x.y);
    scale_z_by(x.z);
}

void Transformable::rotate_around(const smlt::Vec3& axis, const smlt::Degrees& degrees) {
    if(rotation_locked_) return;

    Quaternion rot;
    kmQuaternionRotationAxisAngle(&rot, &axis, kmDegreesToRadians(degrees.value_));

    Quaternion tmp = rotation_;

    tmp = rot * tmp;
    tmp.normalize();

    set_rotation(tmp);
}

void Transformable::rotate_global_x_by(const Degrees &degrees) {
    const static Vec3 X(1, 0, 0);
    rotate_around(X, degrees);
}

void Transformable::rotate_global_y_by(const Degrees &degrees) {
    const static Vec3 Y(0, 1, 0);
    rotate_around(Y, degrees);
}

void Transformable::rotate_global_z_by(const Degrees &degrees) {
    const static Vec3 Z(0, 0, 1);
    rotate_around(Z, degrees);
}

Quaternion Transformable::calc_look_at_rotation(const smlt::Vec3& target) {
    Vec3 up(0, 1, 0);

    Quaternion final;
    Vec3 dir = (target - position_).normalized();
    kmQuaternionLookRotation(&final, &dir, &up);
    final.normalize();
    kmQuaternionInverse(&final, &final);
    return final;
}

void Transformable::look_at(const smlt::Vec3& target) {
    set_rotation(calc_look_at_rotation(target));
}

void Transformable::look_at(float x, float y, float z) {
    look_at(Vec3(x, y, z));
}

Vec3 Transformable::right() const {
    Vec3 result;
    kmQuaternionGetRightVec3(&result, &rotation_);
    return result;
}

Vec3 Transformable::up() const {
    Vec3 result;
    kmQuaternionGetUpVec3(&result, &rotation_);
    return result;
}

Vec3 Transformable::forward() const {
    Vec3 result;
    kmQuaternionGetForwardVec3RH(&result, &rotation_);
    return result;
}

void Transformable::constrain_to_aabb(const AABB& region) {
    constraint_.reset(new AABB(region));
}

bool Transformable::is_constrained() const {
    return bool(constraint_);
}

void Transformable::remove_constraint() {
    constraint_.reset();
}

void Transformable::move_forward_by(float amount) {
    move_by(forward() * amount);
}

void Transformable::move_right_by(float amount) {
    move_by(right() * amount);
}

void Transformable::move_up_by(float amount) {
    move_by(up() * amount);
}


void Transformable::set_position(const Vec3 &p) {
    if(p.x != position_.x || p.y != position_.y || p.z != position_.z) {
        auto tmp = position_;
        position_ = p;
        on_position_set(tmp, position_);
        signal_transformation_changed_();
    }
}

void Transformable::set_rotation(const Quaternion& q) {
    if(q.x != rotation_.x || q.y != rotation_.y || q.z != rotation_.z || q.w != rotation_.w) {
        auto tmp = rotation_;
        rotation_ = q;
        on_rotation_set(tmp, rotation_);
        signal_transformation_changed_();
    }
}

void Transformable::set_scaling(const Vec3 &s) {
    if(s.x != scaling_.x || s.y != scaling_.y || s.z != scaling_.z) {
        auto tmp = scaling_;
        scaling_ = s;
        on_scaling_set(tmp, scaling_);
        signal_transformation_changed_();
    }
}


}