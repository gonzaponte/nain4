#ifndef N4_MESSENSER_HH
#define N4_MESSENSER_HH

#include <G4GenericMessenger.hh>

namespace nain4 {
  using G4CMD = G4GenericMessenger::G4GenericMessenger::Command;

  struct command {
    G4CMD*   handle;
    G4String name(){ return handle -> GetCommandName(); }

    command(G4CMD* handle, G4String description="") : handle(handle), description_(description) {}

#define VAR_AND_SETTER(NAME)                                 \
    std::optional<G4String> NAME##_ = std::nullopt;          \
    command& NAME(G4String x) { NAME##_ = x; return *this; }

    VAR_AND_SETTER(  dimension)
    VAR_AND_SETTER(       unit)
    VAR_AND_SETTER(description)
    VAR_AND_SETTER(      range)
    VAR_AND_SETTER(    options)
    VAR_AND_SETTER(defaults_to)

    G4bool required_;
    command& required(){required_ =  true; return *this;}
    command& optional(){required_ = false; return *this;}
  }


#define FORWARD(NEW_NAME, TYPE, OLD_NAME) command& NEW_NAME(TYPE v){handle.OLD_NAME(v); return *this;}
    FORWARD(  dimension, G4String, SetUnitCategory)
    FORWARD(       unit, G4String, SetUnit)
    FORWARD(description, G4String, SetGuidance)
    FORWARD(      range, G4String, SetRange)
    FORWARD(    options, G4String, SetCandidates)
    FORWARD( default_to, G4String, SetDefaultValue)

    #undef FORWARD

  private:
    G4CMD& handle;

    inline G4String get_name() { return handle.command->GetCommandName(); }
  };

  struct messenger : public G4GenericMessenger {
    using G4GenericMessenger::G4GenericMessenger;

    template<class VAR>
    command add(G4String name, VAR& variable, G4String doc="") {
      auto g4cmd = DeclareProperty(name, variable, doc);
      return command{g4cmd};
    }
  };

}

#endif // N4_MESSENGER_HH
