#include <n4-shape-boolean.hh>

#include <G4UnionSolid.hh>
#include <G4SubtractionSolid.hh>
#include <G4IntersectionSolid.hh>

namespace nain4 {

G4VSolid* shape_boolean::solid() const {
  if (op == BOOL_OP::ADD) { return new G4UnionSolid       {name_, a, b, transformation}; }
  if (op == BOOL_OP::SUB) { return new G4SubtractionSolid {name_, a, b, transformation}; }
  if (op == BOOL_OP::INT) { return new G4IntersectionSolid{name_, a, b, transformation}; }
  // Unreachable
  return nullptr;
}

} // namespace nain4
