#pragma once

#include <G4UnitsTable.hh>

namespace nain4 {

inline void define_unit(const G4String& name, const G4String& symbol, const G4String& category, G4double value) {
  // BuildUnitsTable *must* be called before `new G4UnitDefinition`, otherwise
  // the standard units disappear. We're looking into how this can be mitigated
  // in a nani4 messenger interface.
  if (! G4UnitDefinition::IsUnitDefined(name) && ! G4UnitDefinition::IsUnitDefined(symbol)) {
    new G4UnitDefinition(name, symbol, category, value);
  }
}

} // namespace nain4

namespace n4 { using namespace nain4; }
