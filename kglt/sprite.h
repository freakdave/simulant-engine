#ifndef SPRITE_H
#define SPRITE_H

#include "types.h"
#include "interfaces.h"
#include "generic/managed.h"
#include "generic/identifiable.h"
#include "utils/parent_setter_mixin.h"
#include "sound.h"
#include "object.h"

namespace kglt {

class Sprite :
    public Managed<Sprite>,
    public generic::Identifiable<SpriteID>,
    public ParentSetterMixin<MoveableObject>,
    public KeyFrameAnimated,
    public Source {

public:
    //Ownable interface (inherited through ParentSetterMixin)
    void ask_owner_for_destruction() override;

    bool init() override;
    void cleanup() override;
    void update(double dt) override;

    Sprite(SpriteID id, Stage* stage);

    void set_render_dimensions(float width, float height);
    void set_render_dimensions_from_width(float width);
    void set_render_dimensions_from_height(float height);

    void set_spritesheet(
        TextureID texture_id,
        uint32_t frame_width,
        uint32_t frame_height,
        uint32_t margin=0, uint32_t spacing=0,
        std::pair<uint32_t, uint32_t> padding=std::make_pair(0, 0)
    );

    void flip_vertically(bool value=true);
    void flip_horizontally(bool value=true);

    //Printable interface
    unicode __unicode__() const override {
        return _u("Sprite {0}").format(this->id());
    }

    kglt::ActorID actor_id() const { return actor_id_; }
private:
    float frame_width_ = 0;
    float frame_height_ = 0;
    float sprite_sheet_margin_ = 0;
    float sprite_sheet_spacing_ = 0;
    std::pair<uint32_t, uint32_t> sprite_sheet_padding_;
    float render_width_ = 1.0;
    float render_height_ = -1;
    ActorID actor_id_;
    MeshID mesh_id_;
    MaterialID material_id_;

    float image_width_ = 0;
    float image_height_ = 0;

    void update_texture_coordinates();

    bool flipped_vertically_ = false;
    bool flipped_horizontally_ = false;

    void refresh_animation_state() {
        update_texture_coordinates();
    }
};

}
#endif // SPRITE_H
