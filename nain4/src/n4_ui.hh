#ifndef nain4_ui_hh
#define nain4_ui_hh

#include <G4Types.hh>
#include <G4String.hh>

#include <G4UImanager.hh>
#include <optional>


namespace nain4 {

class ui {
public:
  ui(const std::string& program_name, int argc, char** argv);
  void run();
  void run_macro(const G4String& filename) { g4_ui.ApplyCommand("/control/execute " + filename    ); }
  void beam_on  (      G4int            n) { g4_ui.ApplyCommand("/run/beamOn " + std::to_string(n)); }
  void apply    (const G4String& command ) { g4_ui.ApplyCommand(command);                            }

private:
  std::optional<G4int   >    n_events;
  std::optional<G4String> early_macro;
  std::optional<G4String>  late_macro;
  std::optional<G4String>   vis_macro;

  int    argc;
  char** argv;

  G4UImanager& g4_ui;
};

} // namespace nain4

namespace n4{ using namespace nain4; }

#endif // nain4_ui_hh
