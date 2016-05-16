#ifndef ENTITY_H
#define ENTITY_H

#include "generic/identifiable.h"
#include "generic/managed.h"
#include "generic/relation.h"
#include "generic/protected_ptr.h"

#include <kazbase/signals.h>

#include "interfaces.h"
#include "object.h"
#include "mesh.h"
#include "sound.h"

#include "utils/parent_setter_mixin.h"
#include "renderers/batching/render_queue.h"
#include "renderers/batching/renderable.h"

namespace kglt {

class SubActor;

class Actor :
    public MeshInterface,
    public virtual BoundableEntity,
    public Managed<Actor>,
    public generic::Identifiable<ActorID>,
    public ParentSetterMixin<MoveableObject>,
    public Source,
    public Protectable {

public:
    Actor(ActorID id, Stage* stage);
    Actor(ActorID id, Stage* stage, MeshID mesh);

    MeshID mesh_id() const { return (mesh_) ? mesh_->id() : MeshID(0); }

    ProtectedPtr<Mesh> mesh() const;

    bool has_mesh() const { return bool(mesh_); }
    void set_mesh(MeshID mesh);

    const VertexData& shared_data() const;

    const uint16_t subactor_count() const {
        return subactors_.size();
    }

    void override_material_id(MaterialID mat);
    void remove_material_id_override();

    SubActor& subactor(uint16_t idx) {
        return *subactors_[idx];
    }

    const std::vector<std::shared_ptr<SubActor> >& _subactors() { return subactors_; }

    void ask_owner_for_destruction();

    RenderPriority render_priority() const { return render_priority_; }
    void set_render_priority(RenderPriority value) { render_priority_ = value;}

    unicode __unicode__() const {
        if(has_name()) {
            return name();
        } else {
            return _u("Actor {0}").format(this->id());
        }
    }

    const AABB aabb() const;
    const AABB transformed_aabb() const;

    void each(std::function<void (uint32_t, SubActor*)> callback);

    typedef sig::signal<void (ActorID, SubActor*)> SubActorCreatedCallback;
    typedef sig::signal<void (ActorID, SubActor*)> SubActorDestroyedCallback;
    typedef sig::signal<void (ActorID, SubActor*, MaterialID, MaterialID)> SubActorMaterialChangedCallback;
    typedef sig::signal<void (ActorID)> MeshChangedCallback;

    SubActorCreatedCallback& signal_subactor_created() {
        return signal_subactor_created_;
    }

    SubActorDestroyedCallback& signal_subactor_destroyed() {
        return signal_subactor_destroyed_;
    }

    SubActorMaterialChangedCallback& signal_subactor_material_changed() {
        return signal_subactor_material_changed_;
    }

    MeshChangedCallback& signal_mesh_changed() { return signal_mesh_changed_; }

private:
    std::shared_ptr<Mesh> mesh_;
    std::vector<std::shared_ptr<SubActor> > subactors_;

    RenderPriority render_priority_;

    SubActorCreatedCallback signal_subactor_created_;
    SubActorDestroyedCallback signal_subactor_destroyed_;
    SubActorMaterialChangedCallback signal_subactor_material_changed_;
    MeshChangedCallback signal_mesh_changed_;

    void do_update(double dt) {
        update_source(dt);
    }

    void clear_subactors();
    void rebuild_subactors();
    sig::connection submesh_created_connection_;
    sig::connection submesh_destroyed_connection_;

    friend class SubActor;
};

class SubActor :
    public SubMeshInterface,
    public virtual BoundableEntity,
    public Managed<SubActor>,
    public Renderable {

public:
    const MaterialID material_id() const;

    const SubMeshID submesh_id() const;

    void override_material_id(MaterialID material);
    void remove_material_id_override();

    const VertexData& vertex_data() const { return submesh().vertex_data(); }
    const IndexData& index_data() const { return submesh().index_data(); }
    const MeshArrangement arrangement() const { return submesh().arrangement(); }

    void _update_vertex_array_object() { submesh()._update_vertex_array_object(); }
    void _bind_vertex_array_object() { submesh()._bind_vertex_array_object(); }

    RenderPriority render_priority() const { return parent_.render_priority(); }
    Mat4 final_transformation() const { return parent_.absolute_transformation(); }
    const bool is_visible() const { return parent_.is_visible(); }

    MeshID instanced_mesh_id() const { return parent_.mesh_id(); }
    SubMeshID instanced_submesh_id() const { return submesh_id(); }

    /* BoundableAndTransformable interface implementation */

    const AABB transformed_aabb() const {
        AABB local = aabb();
        Mat4 transform = parent_.absolute_transformation();

        //Transform local by the transformation matrix of the parent
        kmVec3Transform(&local.min, &local.min, &transform);
        kmVec3Transform(&local.max, &local.max, &transform);

        return local;
    }

    const AABB aabb() const {
        return submesh().aabb();
    }

    SubActor(Actor& parent, SubMesh* submesh):
        parent_(parent),
        submesh_(submesh),
        material_(0) {
    }

private:
    Actor& parent_;
    SubMesh* submesh_ = nullptr;
    MaterialPtr material_;

    SubMesh& submesh();
    const SubMesh& submesh() const;
};

}

#endif // ENTITY_H
