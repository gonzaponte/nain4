#ifndef N4_MESSENSER_HH
#define N4_MESSENSER_HH

#include <G4Types.hh>
#include <G4GenericMessenger.hh>

#include <optional>
#include <memory>


namespace nain4 {

#define shared_msg std::shared_ptr<G4GenericMessenger>

#define VAR_AND_SETTER(NAME, TYPE)                              \
  std::optional<TYPE> NAME##_ = std::nullopt;                   \
  cmd_config& NAME(TYPE x) { NAME##_ = x; return *this; }

  template<class T>
  struct cmd_config {
  private:
    friend struct messenger;

    shared_msg msg;
    G4String name;
    T& var;

    cmd_config(shared_msg msg,  G4String& name, T& var) : msg(msg), name(name), var(var) {}


  public:
    VAR_AND_SETTER(  dimension, G4String)
    VAR_AND_SETTER(       unit, G4String)
    VAR_AND_SETTER(description, G4String)
    VAR_AND_SETTER(      range, G4String)
    VAR_AND_SETTER(    options, G4String)
    VAR_AND_SETTER(defaults_to, G4String)

    std::optional<G4bool> required_;
    cmd_config& required(){required_ =  true; return *this;}
    cmd_config& optional(){required_ = false; return *this;}

    void done() {
      auto handle = msg -> DeclareProperty(name, var);
      if (  dimension_.has_value()) { handle.SetUnitCategory (  dimension_.value()); }
      if (       unit_.has_value()) { handle.SetUnit         (       unit_.value()); }
      if (      range_.has_value()) { handle.SetRange        (      range_.value()); }
      if (    options_.has_value()) { handle.SetCandidates   (    options_.value()); }
      if (defaults_to_.has_value()) { handle.SetDefaultValue (defaults_to_.value()); }
      if (description_.has_value()) { handle.SetGuidance     (description_.value()); }
      else {
        std::cerr << "Command " << name
                  << "does not have a description. "
                  << "It is mandatory to specify this property with "
                  << "`.description(...)`."<< std::endl;
        exit(EXIT_FAILURE);
      }
      if (   required_.has_value()) { handle.SetParameterName(name, required_.value()); }
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
      msg = std::make_shared<G4GenericMessenger>(std::forward<ArgTypes>(args)...);
    }

    template<class VAR>
    cmd_config<VAR> add(G4String name, VAR& var) {
      return {msg, name, var};
    }

  private:
    shared_msg msg = nullptr;
  };

}

#undef shared_msg
#undef VAR_AND_SETTER

#endif // N4_MESSENGER_HH
