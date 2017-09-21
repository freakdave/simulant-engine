/* *   Copyright (c) 2011-2017 Luke Benstead https://simulant-engine.appspot.com
 *
 *     This file is part of Simulant.
 *
 *     Simulant is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 *
 *     Simulant is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *     You should have received a copy of the GNU General Public License
 *     along with Simulant.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TEXTURE_H_INCLUDED
#define TEXTURE_H_INCLUDED

#include <cstdint>
#include <memory>
#include <vector>
#include "generic/identifiable.h"
#include "generic/managed.h"
#include "loadable.h"
#include "types.h"
#include "resource.h"
#include "interfaces.h"
#include "interfaces/updateable.h"

namespace smlt {

enum MipmapGenerate {
    MIPMAP_GENERATE_NONE,
    MIPMAP_GENERATE_COMPLETE
};

enum TextureWrap {
    TEXTURE_WRAP_REPEAT,
    TEXTURE_WRAP_CLAMP_TO_EDGE,
    TEXTURE_WRAP_MIRRORED_REPEAT,
    TEXTURE_WRAP_MIRRORED_CLAMP_TO_EDGE
};

enum TextureFilter {
    TEXTURE_FILTER_POINT,
    TEXTURE_FILTER_BILINEAR,
    TEXTURE_FILTER_TRILINEAR
};

enum TextureFormat {
    // Standard formats
    TEXTURE_FORMAT_RGB,
    TEXTURE_FORMAT_RGBA,

    // Dreamcast PVR VQ compressed
    TEXTURE_FORMAT_UNSIGNED_SHORT_5_6_5_VQ,
    TEXTURE_FORMAT_UNSIGNED_SHORT_5_6_5_VQ_TWID,
    TEXTURE_FORMAT_UNSIGNED_SHORT_4_4_4_4_VQ,
    TEXTURE_FORMAT_UNSIGNED_SHORT_4_4_4_4_VQ_TWID,
    TEXTURE_FORMAT_UNSIGNED_SHORT_1_5_5_5_VQ,
    TEXTURE_FORMAT_UNSIGNED_SHORT_1_5_5_5_VQ_TWID,

    // S3TC
    TEXTURE_FORMAT_RGB_S3TC_DXT1_EXT,
    TEXTURE_FORMAT_RGBA_S3TC_DXT1_EXT,
    TEXTURE_FORMAT_RGBA_S3TC_DXT3_EXT,
    TEXTURE_FORMAT_RGBA_S3TC_DXT5_EXT
};

enum TextureTexelType {
    TEXTURE_TEXEL_TYPE_UNSIGNED_BYTE,
    TEXTURE_TEXEL_TYPE_UNSIGNED_SHORT_5_6_5,
    TEXTURE_TEXEL_TYPE_UNSIGNED_SHORT_4_4_4_4,
    TEXTURE_TEXEL_TYPE_UNSIGNED_SHORT_5_5_5_1
};

enum TextureFreeData {
    TEXTURE_FREE_DATA_NEVER,
    TEXTURE_FREE_DATA_AFTER_UPLOAD
};

class NoTextureLockError : public std::runtime_error {
public:
    NoTextureLockError(const std::string& what):
        std::runtime_error(what) {}
};

class TextureLock {
    /*
     * Use when manipulating textures in a thread to prevent the
     * renderer trying to upload your data before you're done
     */

    TextureLock() = default;
    TextureLock(Texture* tex, bool wait=true);

    /* Only Texture can create TextureLock objects */
    friend class Texture;

    Texture* tex_ = nullptr;

public:
    ~TextureLock();

    /* TextureLock isn't copyable */
    TextureLock(const TextureLock& rhs) = delete;
    TextureLock& operator=(const TextureLock& rhs) = delete;

    /* It *is* moveable though */
    TextureLock(TextureLock&& rhs);
    TextureLock& operator=(const TextureLock&& rhs);

    /* TextureLock is false-y if it's not bound to a texture */
    operator bool() const {
        return bool(tex_);
    }
};

typedef std::shared_ptr<TextureLock> TextureLockPtr;


class Texture :
    public Resource,
    public Loadable,
    public generic::Identifiable<TextureID>,
    public Managed<Texture>,
    public Updateable,
    public RenderTarget,
    public std::enable_shared_from_this<Texture> {

public:
    struct BuiltIns {
        static const std::string CHECKERBOARD;
        static const std::string BUTTON;
    };

    typedef std::shared_ptr<Texture> ptr;
    typedef std::vector<uint8_t> Data;

    Texture(TextureID id, ResourceManager* resource_manager):
        Resource(resource_manager),
        generic::Identifiable<TextureID>(id),
        width_(0),
        height_(0) { }

    /*
     * Lock the texture against uploads to the GPU.
     *
     * This is necessary around code which manipulates
     * the texture data buffer (e.g .resize)
     */
    TextureLock lock() {
        return TextureLock(this);
    }

    /*
     * Try to lock the texture data, but don't wait for it.
     * Returns a false-y object if the lock could not be obtained
     */
    TextureLock try_lock() {
        try {
            return TextureLock(this, false);
        } catch(NoTextureLockError& e) {
            return TextureLock();
        }
    }

    void set_texel_type(TextureTexelType type);
    void set_format(TextureFormat format);

    /*
     * Change the width and height, but manually set the data buffer size,
     * mainly used for compressed textures
     */
    void resize(uint32_t width, uint32_t height, uint32_t data_size);

    /*
     * Change the width and height, automatically resizing the data buffer
     * depending on the bytes_per_pixel of the texel_type
     */
    void resize(uint32_t width, uint32_t height);

    /*
     * Flip the data buffer vertically. This will have no effect if the
     * data buffer has been wiped after upload
     */
    void flip_vertically();

    /* Clear the data buffer */
    void free();

    uint32_t width() const { return width_; }
    uint32_t height() const { return height_; }

    /*
     * Returns true if this Texture uses a compressed format
     */
    bool is_compressed() const;

    /* Returns the data size of each texel in bytes */
    std::size_t bytes_per_pixel() const {
        return bits_per_pixel() / 8;
    }

    /* Returns the data size of each texel in bits */
    std::size_t bits_per_pixel() const;

    /*
     * Returns the number of channels that this texture has
     */
    uint8_t channels() const;

    /*
     * Return a const-reference to the internal data buffer
     */
    const Texture::Data& data() const;

    /* Returns a non-const reference to the internal data buffer */
    Texture::Data& data() { return data_; }

    /*
     * Mark the data as changed so it will be reuploaded to the GPU
     * by the renderer
     */
    void mark_data_changed() {
        data_dirty_ = true;
    }

    /*
     * Save a texture to the specified file. Will only work for
     * uncompressed Textures
     */
    void save_to_file(const unicode& filename);

    void set_source(const unicode& source) { source_ = source; }
    unicode source() const { return source_; }

    /* Texture filtering and wrapping */
    void set_texture_filter(TextureFilter filter);

    /* Set the texture wrap modes, either together or per-dimension */
    void set_texture_wrap(TextureWrap wrap_u, TextureWrap wrap_v, TextureWrap wrap_w);
    void set_texture_wrap_u(TextureWrap wrap_u);
    void set_texture_wrap_v(TextureWrap wrap_v);
    void set_texture_wrap_w(TextureWrap wrap_w);

    TextureFilter texture_filter() const {
        return filter_;
    }

    TextureWrap wrap_u() const {
        return wrap_u_;
    }

    TextureWrap wrap_v() const {
        return wrap_v_;
    }

    TextureWrap wrap_w() const {
        return wrap_w_;
    }

    MipmapGenerate mipmap_generation() const {
        return mipmap_generation_;
    }

    void set_mipmap_generation(MipmapGenerate type) {
        mipmap_generation_ = type;
    }

    /*
     * INTERNAL: returns true if the filters are dirty
     */
    bool _params_dirty() const {
        return params_dirty_;
    }

    /*
     * INTERNAL: Clears the params dirty flag
     */
    void _set_params_clean() {
        params_dirty_ = false;
    }

    /*
     * INTERNAL: returns true if the mipmap flag changed
     */
    bool _mipmaps_dirty() const {
        return mipmaps_dirty_;
    }

    /*
     * INTERNAL: clears the mipmap flag
     */
    void _set_mipmaps_clean() {
        mipmaps_dirty_ = false;
    }

    /*
     * INTERNAL: returns true if the data needs re-uploading
     */
    bool _data_dirty() const {
        return data_dirty_;
    }

    /*
     * INTERNAL: clears the dirty data flag
     */
    void _set_data_clean() {
        data_dirty_ = false;
    }


    /* These are overridden to notify the renderer of texture changes */
    bool init() override;
    void cleanup() override;
    void update(float dt) override;

private:
    uint32_t width_;
    uint32_t height_;

    TextureTexelType texel_type_ = TEXTURE_TEXEL_TYPE_UNSIGNED_BYTE;
    TextureFormat format_ = TEXTURE_FORMAT_RGBA;

    unicode source_;

    bool data_dirty_ = true;
    Texture::Data data_;
    TextureFreeData free_data_mode_ = TEXTURE_FREE_DATA_AFTER_UPLOAD;

    bool mipmaps_dirty_ = true;
    MipmapGenerate mipmap_generation_ = MIPMAP_GENERATE_COMPLETE;

    bool params_dirty_ = true;
    TextureFilter filter_ = TEXTURE_FILTER_POINT;
    TextureWrap wrap_u_ = TEXTURE_WRAP_REPEAT;
    TextureWrap wrap_v_ = TEXTURE_WRAP_REPEAT;
    TextureWrap wrap_w_ = TEXTURE_WRAP_REPEAT;

    std::mutex mutex_;

    friend class TextureLock;
};

}

#endif // TEXTURE_H_INCLUDED
