#ifndef SDATA_EMITTER_TEST_HPP
#define SDATA_EMITTER_TEST_HPP

#include <catch2/catch.hpp>
#include <sdata.hpp>

using namespace sdata;

TEST_CASE("Emitter<char>") {
  auto source = read_source_file<char>("examples/game.sd");
  auto root = Parser<char>{source}.parse();
  auto emitted = to_source<char>(root);

  CHECK(emitted == source);
  CHECK(*from_source<char>(emitted) == *root);
}

TEST_CASE("Emitter<char16_t>") {
  auto source = read_source_file<char16_t>("examples/dialog.sd");
  auto root = Parser<char16_t>{source}.parse();
  auto emitted = to_source<char16_t>(root);

  CHECK(emitted == source);
  CHECK(*from_source<char16_t>(emitted) == *root);
}

#endif
