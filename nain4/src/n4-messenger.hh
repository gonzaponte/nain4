#ifndef N4_MESSENSER_HH
#define N4_MESSENSER_HH

#include <G4Types.hh>
#include <G4GenericMessenger.hh>

#include <optional>
#include <memory>
#include <type_traits>


namespace nain4 {

#define shared_msg std::shared_ptr<G4GenericMessenger>

#define VAR_AND_SETTER(NAME, TYPE)                              \
  std::optional<TYPE> NAME##_ = std::nullopt;                   \
  cmd_config& NAME(TYPE x) { NAME##_ = x; return *this; }

#define IS_METHOD                                       \
  std::is_function_v               <VAR_OR_FN> ||       \
  std::is_member_function_pointer_v<VAR_OR_FN>


  template<class VAR_OR_FN>
  struct cmd_config {
  private:
    friend struct messenger;

    shared_msg msg;
    G4String name;
    VAR_OR_FN& var;

    cmd_config(shared_msg msg,  G4String& name, VAR_OR_FN& var) : msg(msg), name(name), var(var) {}

  public:
    VAR_AND_SETTER(  dimension, G4String)
    VAR_AND_SETTER(       unit, G4String)
    VAR_AND_SETTER(description, G4String)
    VAR_AND_SETTER(      range, G4String)
    VAR_AND_SETTER(    options, G4String)

    // From the book for app developers:
    // If [the second parameter of SetParameterName] is true, you
    // should define the default value using [SetDefaultValue].
    // not working at the moment
    // VAR_AND_SETTER(defaults_to, G4String)
    std::optional<G4bool> required_;
    // cmd_config& required(){required_ =  true; return *this;}
    // cmd_config& optional(){required_ = false; return *this;}

    void done() {
      G4GenericMessenger::Command* handle_ptr;
      if constexpr (IS_METHOD) {
        handle_ptr = unit_.has_value() ?
                     &msg -> DeclareMethodWithUnit(name, unit_.value(), var, description_.value_or("")) :
                     &msg -> DeclareMethod        (name,                var, description_.value_or(""));
      }
      else {
        handle_ptr = unit_.has_value() ?
                     &msg -> DeclarePropertyWithUnit(name, unit_.value(), var, description_.value_or("")) :
                     &msg -> DeclareProperty        (name,                var, description_.value_or("")) ;
      }

      auto& handle = *handle_ptr;

      handle.SetParameterName(name, required_.value_or(false));
      if (  dimension_.has_value()) { handle.SetUnitCategory (  dimension_.value()); }
      if (      range_.has_value()) { handle.SetRange        (      range_.value()); }
      if (    options_.has_value()) { handle.SetCandidates   (    options_.value()); }
      // if (defaults_to_.has_value()) { handle.SetDefaultValue (defaults_to_.value()); }
    }
  };

  struct messenger {
    messenger() = default;

    template<class... ArgTypes>
    messenger(ArgTypes&&... args) {
      msg = std::make_shared<G4GenericMessenger>(std::forward<ArgTypes>(args)...);
    }

    template<class VAR_OR_FN>
    cmd_config<VAR_OR_FN> add(G4String name, VAR_OR_FN& var) {
      return {msg, name, var};
    }

  private:
    shared_msg msg = nullptr;
  };

} // namespace nain4

#undef IS_METHOD
#undef shared_msg
#undef VAR_AND_SETTER

#endif // N4_MESSENGER_HH
