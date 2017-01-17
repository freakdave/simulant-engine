#pragma once

#include "deps/stb_truetype/stb_truetype.h"
#include "types.h"
#include "generic/managed.h"
#include "generic/identifiable.h"
#include "loadable.h"
#include "resource.h"

struct stbtt_fontinfo;

namespace smlt {

namespace loaders {
    class TTFLoader;
}

enum CharacterSet {
    CHARACTER_SET_LATIN
};

class Font:
    public Managed<Font>,
    public Resource,
    public Loadable,
    public generic::Identifiable<FontID> {

public:
    Font(FontID id, ResourceManager* resource_manager);

    bool is_valid() const { return bool(info_) && texture_; }
    TextureID texture_id() const;

    std::pair<Vec2, Vec2> texture_coordinates_for_character(char32_t c);

private:
    std::unique_ptr<stbtt_fontinfo> info_;
    std::vector<stbtt_bakedchar> char_data_;

    TexturePtr texture_;

    friend class ui::Widget;
    friend class loaders::TTFLoader;
};

}
