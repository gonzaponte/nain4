#include "CLHEP/Units/SystemOfUnits.h"
#include "G4PrimaryParticle.hh"
#include "G4PrimaryVertex.hh"
#include <n4-all.hh>

#include <G4VUserPhysicsList.hh>
#include <G4Geantino.hh>
#include <G4Event.hh>
#include <G4GenericMessenger.hh>

namespace config {
  enum class axis {x, y, z};

  unsigned nshoot{10000};

  double min_theta{0};
  double max_theta{180};
  double min_cos_theta{-1};
  double max_cos_theta{ 1};
  double min_phi{0};
  double max_phi{360};
  bool   bidirectional{false};
  bool   exclude{false};
  double rotation_x{0};
  double rotation_y{0};
  double rotation_z{0};
  double unit{deg};
  std::vector<axis> order{};

#define ALREADY_SET(value) (std::find(order.begin(), order.end(), axis::x) != order.end())

  void rotate_x(double x) { rotation_x = x; if (! ALREADY_SET(axis::x)) { order.push_back(axis::x); } }
  void rotate_y(double y) { rotation_y = y; if (! ALREADY_SET(axis::y)) { order.push_back(axis::y); } }
  void rotate_z(double z) { rotation_z = z; if (! ALREADY_SET(axis::z)) { order.push_back(axis::z); } }

  void units(std::string s) {
    if (s == "deg") { unit = deg; } else
    if (s == "rad") { unit = rad; } else
                    { /* UNREACHABLE */ }
  }

  struct A {
    void units(std::string s) {
      if (s == "deg") { unit = deg; } else
      if (s == "rad") { unit = rad; } else
                      { /* UNREACHABLE */ }
    }

    void rotate_x(double x) { rotation_x = x; if (! ALREADY_SET(axis::x)) { order.push_back(axis::x); } }
    void rotate_y(double y) { rotation_y = y; if (! ALREADY_SET(axis::y)) { order.push_back(axis::y); } }
    void rotate_z(double z) { rotation_z = z; if (! ALREADY_SET(axis::z)) { order.push_back(axis::z); } }
};
}

struct no_physics : G4VUserPhysicsList {
  using G4VUserPhysicsList::G4VUserPhysicsList;
  ~no_physics() override {}

  void ConstructParticle() override { G4Geantino::Definition(); }
  void ConstructProcess () override {      AddTransportation(); }
};

auto sphere() {
  return n4::sphere("sphere").r(1).place(n4::material("G4_AIR")).now();
}

void random_directions(G4Event* event) {
  auto geantino = G4Geantino::Definition();
  auto vertex   = new G4PrimaryVertex{};

  auto gen = n4::random::direction()
    .min_cos_theta(config::min_cos_theta)
    .max_cos_theta(config::max_cos_theta)
    .min_theta    (config::min_theta     * config::unit)
    .max_theta    (config::max_theta     * config::unit)
    .min_phi      (config::min_phi       * config::unit)
    .max_phi      (config::max_phi       * config::unit)
    // .rotate  (m) // this one would be a pain via CLI
    ;

  if (config::bidirectional) { gen = gen.bidirectional(); }
  if (config::exclude      ) { gen = gen.exclude();       }

  for (auto& order : config::order) {
    switch (order) {
      case config::axis::x: gen = gen.rotate_x(config::rotation_x * config::unit); break;
      case config::axis::y: gen = gen.rotate_y(config::rotation_y * config::unit); break;
      case config::axis::z: gen = gen.rotate_z(config::rotation_z * config::unit); break;
    }
  }

  for (auto k=0; k<config::nshoot; k++) {
    auto dir = gen.get();
    vertex -> SetPrimary(new G4PrimaryParticle{geantino, dir.x(), dir.y(), dir.z()});
  }
  event -> AddPrimaryVertex(vertex);
}


// TODO give client ability to pass C++ code snippets to visualize. The obvious
// choice is for the user to point at a file containing the code, but maybe we
// could accept C++ code directly as a CLI argument. In any case, it would need
// to be compile automatically and linked with the random direction utilities.
int main(int argc, char** argv) {
  auto msg = new G4GenericMessenger(nullptr, "/dir/", "Setters for random directions");

  msg->DeclareProperty("nshoot"        ,  config::nshoot       ).SetParameterName("nshoot"       , true).SetRange("nshoot > 0");
  msg->DeclareProperty("min_theta"     ,  config::min_theta    ).SetParameterName("min_theta"    , true).SetRange("(min_theta >= 0) & (min_theta <= 180.)");
  msg->DeclareProperty("max_theta"     ,  config::max_theta    ).SetParameterName("max_theta"    , true).SetRange("(max_theta >= 0) & (max_theta <= 180.)");
  msg->DeclareProperty("min_cos_theta" ,  config::min_cos_theta).SetParameterName("min_cos_theta", true).SetRange("(min_cos_theta >= -1) & (min_cos_theta <= 1)");
  msg->DeclareProperty("max_cos_theta" ,  config::max_cos_theta).SetParameterName("max_cos_theta", true).SetRange("(max_cos_theta >= -1) & (max_cos_theta <= 1)");
  msg->DeclareProperty("min_phi"       ,  config::min_phi      ).SetParameterName("min_phi"      , true).SetRange("(min_phi >= 0) & (min_phi <= 360.)");
  msg->DeclareProperty("max_phi"       ,  config::max_phi      ).SetParameterName("max_phi"      , true).SetRange("(max_phi >= 0) & (max_phi <= 360.)");
  msg->DeclareProperty("bidirectional" ,  config::bidirectional).SetParameterName("bidirectional", true).SetCandidates("true false");
  msg->DeclareProperty("exclude"       ,  config::exclude      ).SetParameterName("exclude"      , true).SetCandidates("true false");
  msg->DeclareMethod  ("units"         , &config::A::units     ).SetParameterName("units"        , true).SetCandidates("deg rad");
  msg->DeclareMethod  ("rotate_x"      , &config::A::rotate_x  ).SetParameterName("rotate_x"     , true).SetRange("(rotate_x >= -360) & (rotate_x <= 360.)");
  msg->DeclareMethod  ("rotate_y"      , &config::A::rotate_y  ).SetParameterName("rotate_y"     , true).SetRange("(rotate_y >= -360) & (rotate_y <= 360.)");
  msg->DeclareMethod  ("rotate_z"      , &config::A::rotate_z  ).SetParameterName("rotate_z"     , true).SetRange("(rotate_z >= -360) & (rotate_z <= 360.)");

  n4::run_manager::create()
    .ui("random-directions-visualizer", argc, argv)
    .apply_cli_early()
    .physics<no_physics>()
    .geometry(sphere)
    .actions(new n4::actions{random_directions})
    .run();
}
