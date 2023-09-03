#ifndef N4_MESSENSER_HH
#define N4_MESSENSER_HH

#include <G4Types.hh>
#include <G4GenericMessenger.hh>

namespace nain4 {
#define G4CMD G4GenericMessenger::G4GenericMessenger::Command

  struct cmd_config {
  private:
    friend struct messenger;

    G4CMD* handle;
    G4String name;
    cmd_config(G4CMD* handle, G4String name) : handle(handle), name(name) {}

#define VAR_AND_SETTER(NAME, TYPE)                          \
    std::optional<TYPE> NAME##_ = std::nullopt;             \
    cmd_config& NAME(TYPE x) { NAME##_ = x; return *this; }

    VAR_AND_SETTER(  dimension, G4String)
    VAR_AND_SETTER(       unit, G4String)
    VAR_AND_SETTER(description, G4String)
    VAR_AND_SETTER(      range, G4String)
    VAR_AND_SETTER(    options, G4String)
    VAR_AND_SETTER(defaults_to, G4String)
    VAR_AND_SETTER(   required, G4bool  )

    cmd_config& optional(){required_ = false; return *this;}

    void done() {
      if (  dimension_.has_value()) { handle -> SetUnitCategory (  dimension_.value()); }
      if (       unit_.has_value()) { handle -> SetUnit         (       unit_.value()); }
      if (      range_.has_value()) { handle -> SetRange        (      range_.value()); }
      if (    options_.has_value()) { handle -> SetCandidates   (    options_.value()); }
      if (defaults_to_.has_value()) { handle -> SetDefaultValue (defaults_to_.value()); }
      if (description_.has_value()) { handle -> SetGuidance     (description_.value()); }
      else {
        std::cerr << "Command " << name
                  << "does not have a description. "
                  << "It is mandatory to specify this property with "
                  << "`.description(...)`."<< std::endl;
        exit(EXIT_FAILURE);
      }
      if (   required_.has_value()) { handle -> SetParameterName(name, required_.value()); }
      else {
        std::cerr << "Command " << name
                  << "has not been declared neither required nor optional. "
                  << "It is mandatory to specify this property with "
                  << "`.optional()` or `.required()`."<< std::endl;
        exit(EXIT_FAILURE);
      }
    }
  };

  struct messenger {
    messenger() = default;

    template<class... ArgTypes>
    messenger(ArgTypes&&... args) {
      msg = std::make_unique<G4GenericMessenger>(std::forward<ArgTypes>(args)...);
    }

    template<class VAR>
    cmd_config add(G4String name, VAR var) {
      auto g4cmd = msg -> DeclareProperty(name, var);
      return cmd_config{&g4cmd, name};
    }

  private:
    std::unique_ptr<G4GenericMessenger> msg = nullptr;
    std::vector<cmd_config> commands;
  };


#undef G4CMD
#undef VAR_AND_SETTER


  /*









  using G4CMD = G4GenericMessenger::G4GenericMessenger::Command;

  struct command {
    G4CMD*   handle;
    G4String name(){ std::cout << "NAME " << handle -> command -> GetCommandName() << std::endl;
      return handle -> command -> GetCommandName(); }

    command(G4CMD* handle) : handle(handle) {}

#define VAR_AND_SETTER(NAME)                                 \
    std::optional<G4String> NAME##_ = std::nullopt;          \
    command& NAME(G4String x) { NAME##_ = x; return *this; }

    VAR_AND_SETTER(  dimension)
    VAR_AND_SETTER(       unit)
    VAR_AND_SETTER(description)
    VAR_AND_SETTER(      range)
    VAR_AND_SETTER(    options)
    VAR_AND_SETTER(defaults_to)

    G4bool required_ = false;
    command& required(){required_ =  true; return *this;}
    command& optional(){required_ = false; return *this;}

    void done() {
      std::cout << "DONE HANDLE " << handle << std::endl;
      if (  dimension_.has_value()) { handle -> SetUnitCategory(  dimension_.value()); }
      if (       unit_.has_value()) { handle -> SetUnit        (       unit_.value()); }
      if (description_.has_value()) { handle -> SetGuidance    (description_.value()); }
      if (      range_.has_value()) { handle -> SetRange       (      range_.value()); }
      if (    options_.has_value()) { handle -> SetCandidates  (    options_.value()); }
      if (defaults_to_.has_value()) { handle -> SetDefaultValue(defaults_to_.value()); }

      std::cout << "DONE HANDLE " << handle << std::endl;
      handle -> SetParameterName(name(), required_);
      std::cout << "DONE HANDLE " << handle << std::endl;
    }
  };


// #define FORWARD(NEW_NAME, TYPE, OLD_NAME) command& NEW_NAME(TYPE v){handle.OLD_NAME(v); return *this;}
//     FORWARD(  dimension, G4String, SetUnitCategory)
//     FORWARD(       unit, G4String, SetUnit)
//     FORWARD(description, G4String, SetGuidance)
//     FORWARD(      range, G4String, SetRange)
//     FORWARD(    options, G4String, SetCandidates)
//     FORWARD( default_to, G4String, SetDefaultValue)

//     #undef FORWARD

//   private:
//     G4CMD& handle;

//     inline G4String get_name() { return handle.command->GetCommandName(); }
//   };


  struct messenger : public G4GenericMessenger {
    using G4GenericMessenger::G4GenericMessenger;

    template<class VAR>
    command add(G4String name, VAR& variable) {
      auto g4cmd = this -> DeclareProperty(name, variable, "");
      std::cout << "G4CMD " << &g4cmd << std::endl;
      return command{&g4cmd};
    }
  };
  */
}

#endif // N4_MESSENGER_HH
