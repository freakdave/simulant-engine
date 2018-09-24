#pragma once

#include "widget.h"

namespace smlt {
namespace ui {


/* An Image widget, useful for health indicators etc.
 *
 * All images have their resize mode set to fixed width and do not
 * allow text.
 */
class Image:
    public Widget,
    public Managed<Image> {

    void clear_layers();

public:
    using Widget::init; // Pull in init to satisfy Managed<Image>
    using Widget::cleanup;

    Image(WidgetID id, UIManager* owner, UIConfig* config);
    virtual ~Image() {}

    /* Set the texture of the Image. By default the image will be sized to the
     * full size of the texture, all set_source_rect to use a subsection of the texture */
    void set_texture_id(const TextureID& texture_id);

    /* Selects the source region of the texture to display in the image */
    void set_source_rect(const Vec2& bottom_left, const Vec2& size);

    void set_resize_mode(ResizeMode resize_mode) override;
};

}
}