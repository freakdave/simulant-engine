#ifndef KGLT_SELECTION_RENDERER_H
#define KGLT_SELECTION_RENDERER_H

#include <map>

#include "kglt/renderer.h"

namespace kglt {

class SelectionRenderer : public Renderer {
public:
	typedef std::tr1::shared_ptr<SelectionRenderer> ptr;
		
    SelectionRenderer(const RenderOptions& options=RenderOptions()):
        Renderer(options),
		selected_mesh_id_(0) {}
	
    void visit(Mesh& mesh);
    void visit(Text& text) {} //Dunno if this should be selectable..

	MeshID selected_mesh() const { return selected_mesh_id_; }
	
    bool pre_visit(Object& obj) {
	    //If this is a mesh, and the entire branch is not selectable,
	    //then bail out
        if(Mesh* m = dynamic_cast<Mesh*>(&obj)) {
	        if(!m->branch_selectable()) {	            
	            return false;
	        }
	    }
	    
	    Renderer::pre_visit(obj);
	    
	    return true;
	}
	
private:
    void on_start_render(Scene& scene);
    void on_finish_render(Scene& scene);

    uint8_t r_count, g_count, b_count;	
    
    std::map<std::tuple<float, float, float>, MeshID> colour_mesh_lookup_;
    MeshID selected_mesh_id_;
};


}
#endif
