#include "n4_ui.hh"
#include "n4_run_manager.hh"

#include <G4VisManager.hh>
#include <G4VisExecutive.hh>
#include <G4UImanager.hh>
#include <G4UIExecutive.hh>

#include <argparse/argparse.hpp>
#include <cstdlib>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>


unsigned parse_beam_on(const std::string&  arg) {
  auto parsed = std::stoi(arg.c_str());
  if (parsed < 0) { throw std::runtime_error{std::string{"--beam-on requires an unsigned integer, you gave '"} + arg + "'"}; }
  return static_cast<unsigned>(parsed);

}

argparse::ArgumentParser define_args(const std::string& program_name, int argc, char** argv) {
  argparse::ArgumentParser args{program_name};
  args.add_argument("--beam-on"    , "-n", "-b").metavar("N-EVENTS").help("run simulation with given number of events");
  args.add_argument("--early-macro", "-e"      ).metavar("FILENAME").help("execute before run manager instantiation");
  args.add_argument( "--late-macro", "-l"      ).metavar("FILENAME").help("execute after  run manager instantiation");
  args.add_argument(  "--vis-macro", "-g"      ).metavar("FILENAME").help("switch from batch mode to GUI, executing this macro");
  args.add_argument( "--macro-path", "-p"      ).metavar("PATH"    ).help("Extra search paths for macros").nargs(argparse::nargs_pattern::any);

  try {
    args.parse_args(argc, argv);
  } catch(const std::runtime_error& err) {
    std::cerr << err.what() << std::endl;
    std::cerr << args;
    std::exit(EXIT_FAILURE);
  }

  return args;
}


namespace nain4 {

// TODO forward to private constructor that accepts parser as parameter, for direct initialization
ui::ui(const std::string& program_name, int argc, char** argv, bool warn_empty_run)
:
  args{define_args(program_name, argc, argv)},
  n_events{},
  early_macro{},
  late_macro{},
  vis_macro{},
  argc{argc},
  argv{argv},
  g4_ui{*G4UImanager::GetUIpointer()}
{

  if (auto n = args.present("--beam-on")) { n_events = parse_beam_on(n.value()); }
  early_macro = args.present("--early-macro");
  late_macro  = args.present( "--late-macro");
  vis_macro   = args.present(  "--vis-macro");

  // c++ sucks and G4 sucks even more. Here we use std::string because
  // G4String does not work. Don't know why, but it's probably due to
  // an absurd reason. So much time wasted on this...
  auto macro_paths = args.present<std::vector<std::string>>("--macro-path");
  if (macro_paths.has_value()) {
    for (auto& path : macro_paths.value()) {
      prepend_path(path);
    }
  }
  g4_ui.ParseMacroSearchPath();

  if (warn_empty_run && ! (n_events.has_value() ^ vis_macro.has_value())) {
    std::cerr << "'" + program_name + "' is not going to do anything interesting without some command-line arguments.\n\n";
    std::cerr << args << std::endl;
  }

}

void ui::run() {
  if (n_events.has_value() && !vis_macro.has_value()) {
    beam_on(n_events.value());
  }

  if (vis_macro.has_value()) {
    G4UIExecutive ui_executive{argc, argv};
    G4VisExecutive vis_manager;
    vis_manager.Initialize();
    run_vis_macro();
    if (n_events.has_value()) { beam_on(n_events.value()); }
    ui_executive.SessionStart();
  }

}

} // namespace nain4
