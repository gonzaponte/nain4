// clang-format off
#pragma once

#include <n4-run-manager.hh>

#include <G4LogicalVolumeStore.hh>
#include <G4PVPlacement.hh>
#include <G4PhysicalVolumeStore.hh>
#include <G4SolidStore.hh>
#include <G4VisAttributes.hh>



// Disable GCC's shadow warnings
// This happens in constructors where it is a C++ idiom to have
// constructor parameters match memeber names.
// This has been reported to gcc's bug tracker
// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=78147
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"

////////////////////////////////////////////////////////////////////////////////////////////////////
// Note 1
//
// + G4 forces you to use its G4Exception function, whose return type is void.
//
// + C++'s ternary operator treats throw expressions as a special case.
//
// + Hiding the throw expression inside G4Exception, disables this special
//   treatment, which results in a type mismatch between the void and whatever
//   values are present in the rest of the ternary operator
//
// + So we go through the following convolutions to satisfy the type system:
//
//   1. use throw at the top-level
//   2. use G4Exception in the argument to throw
//   3. but throw does not accept void
//   4. so use comma operator to give acceptable overall expression type (c-string)
//   5. but the actual value of the string doesn't matter, just its type.

#define FATAL(description) G4Exception((__FILE__ + (":" + std::to_string(__LINE__))).c_str(), "666", FatalException, description)

namespace nain4 {

using std::make_optional;
using std::nullopt;
using std::optional;

inline auto volume(G4VSolid* solid, G4Material* material) { return new G4LogicalVolume{solid, material, solid->GetName()}; }

// Create logical volume from solid and material
template<class SOLID, class NAME, class... ArgTypes>
G4LogicalVolume* volume(NAME name, G4Material* material, ArgTypes&&... args) {
  auto solid = new SOLID{std::forward<NAME>(name), std::forward<ArgTypes>(args)...};
  return volume(solid, material);
}

// Here for now, as it admits volumes. Needs to find a better home if we ever overload it.
G4LogicalVolume* envelope_of(G4LogicalVolume* original);
G4LogicalVolume* envelope_of(G4LogicalVolume* original, G4String name);

// --------------------------------------------------------------------------------
// Utilies for concisely retrieving things from stores
// clang-format off
#define NAME     (G4String const& name)
#define NAME_VRB (G4String const& name, G4bool verbose=true)
#define IA inline auto
IA find_logical  NAME_VRB { return G4LogicalVolumeStore ::GetInstance()->GetVolume          (name, verbose); }
IA find_physical NAME_VRB { return G4PhysicalVolumeStore::GetInstance()->GetVolume          (name, verbose); }
IA find_solid    NAME_VRB { return G4SolidStore         ::GetInstance()->GetSolid           (name, verbose); }
IA find_particle NAME     { return G4ParticleTable:: GetParticleTable()->FindParticle       (name         ); }
IA event_number  ()       { return run_manager::get().here_be_dragons() -> GetCurrentRun() -> GetNumberOfEvent(); }
#undef IA
#undef NAME
#undef NAME_VRB

// Remove all, logical/physical volumes, solids and assemblies.
inline void clear_geometry() { G4RunManager::GetRunManager() -> ReinitializeGeometry(true); }

// --------------------------------------------------------------------------------

class place {
public:
  place(G4LogicalVolume* child)  : child(child ? make_optional(child) : nullopt) {}
  place(place const&) = default;

  place& trans    (G4Transform3D& transform_){ return transform(transform_);                                    }
  place& transform(G4Transform3D& transform_){ transformation =      transform_ * transformation; return *this; }

  place& rotate  (G4RotationMatrix& r)  { transformation = HepGeom::Rotate3D{r} * transformation; return *this; }
  place& rotate_x(double delta       )  { auto rot = G4RotationMatrix{}; rot.rotateX(delta); return rotate(rot);}
  place& rotate_y(double delta       )  { auto rot = G4RotationMatrix{}; rot.rotateY(delta); return rotate(rot);}
  place& rotate_z(double delta       )  { auto rot = G4RotationMatrix{}; rot.rotateZ(delta); return rotate(rot);}

  place& rot     (G4RotationMatrix& r)  { return rotate      (r); }
  place& rot_x   (double delta       )  { return rotate_x(delta); }
  place& rot_y   (double delta       )  { return rotate_y(delta); }
  place& rot_z   (double delta       )  { return rotate_z(delta); }

  place& at  (double x, double y, double z) { transformation = HepGeom::Translate3D{x,y,z} * transformation; return *this; }
  place& at  (G4ThreeVector p             ) { return at(p.x(), p.y(), p.z()); }
  place& at_x(double x                    ) { return at(  x  ,   0  ,   0  ); }
  place& at_y(          double y          ) { return at(  0  ,   y  ,   0  ); }
  place& at_z(                    double z) { return at(  0  ,   0  ,   z  ); }

  place& copy_no(int         n)           { copy_number = n      ; return *this; }
  place& in(G4LogicalVolume* parent_)     { parent      = parent_; return *this; }
  place& in(G4PVPlacement*   parent_)     { return in(parent_ -> GetLogicalVolume()); }
  place& in(n4::place&       parent_)     { return in(parent_ .  get_logical()     ); }
  place& name(G4String       label_)      { label       = label_ ; return *this; }

  place&      check_overlaps           () {  local_check_overlaps_ = true ; return *this; }
  void static check_overlaps_switch_on () { global_check_overlaps_ = true ; }
  void static check_overlaps_switch_off() { global_check_overlaps_ = false; }

  place  clone() const                                           { return *this; }
  G4PVPlacement* operator()()                                    { return now(); }
  G4PVPlacement* now();

  G4LogicalVolume* get_logical();

private:
  optional<G4LogicalVolume*>  child;
  optional<G4LogicalVolume*>  parent;
  optional<G4String>          label;
  optional<int>               copy_number;
  G4Transform3D               transformation = HepGeom::Transform3D::Identity;
  bool         local_check_overlaps_ = false;
  static bool global_check_overlaps_;
};


// --------------------------------------------------------------------------------
// keyword-argument construction of G4VisAttributes
// TODO: G4VisAttributes does NOT have virtual destructor, do we get UB by subclassing?
class vis_attributes : public G4VisAttributes {
public:
  using G4VisAttributes::G4VisAttributes; // Inherit ALL constructors
  using MAP = std::map<G4String, G4AttDef>;
#define FORWARD(NEW_NAME, TYPE, OLD_NAME) vis_attributes& NEW_NAME(TYPE v){ this->OLD_NAME(v); return *this; }
  FORWARD(visible                       , bool     , SetVisibility)
  FORWARD(daughters_invisible           , bool     , SetDaughtersInvisible)
  FORWARD(colour                        , G4Colour , SetColour)
  FORWARD(color                         , G4Color  , SetColor)
  FORWARD(line_style                    , LineStyle, SetLineStyle)
  FORWARD(line_width                    , G4double , SetLineWidth)
  FORWARD(force_wireframe               , bool     , SetForceWireframe)
  FORWARD(force_solid                   , bool     , SetForceSolid)
  FORWARD(force_aux_edge_visible        , bool     , SetForceAuxEdgeVisible)
  FORWARD(force_line_segments_per_circle, G4int    , SetForceLineSegmentsPerCircle)
  FORWARD(start_time                    , G4double , SetStartTime)
  FORWARD(  end_time                    , G4double , SetEndTime)
  FORWARD(att_values, const std::vector<G4AttValue>*, SetAttValues)
  FORWARD(att_defs  , const                     MAP*, SetAttDefs)
#undef FORWARD
};

// --------------------------------------------------------------------------------
// stream redirection utilities

// redirect to arbitrary stream or buffer
class redirect {
public:
  redirect(std::ios& stream, std::streambuf* new_buffer);
  redirect(std::ios& stream, std::ios&       new_stream);
  ~redirect();
private:
  std::streambuf* original_buffer;
  std::ios&       stream;
};

// redirect to /dev/null
class silence {
public:
  explicit silence(std::ios& stream);
  ~silence();
private:
  std::streambuf* original_buffer;
  std::ios&       stream;
  std::ofstream   dev_null;
};

} // namespace nain4

namespace n4 { using namespace nain4; }

#pragma GCC diagnostic pop
