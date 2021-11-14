#define CATCH_CONFIG_RUNNER
#include <catch2/catch.hpp>

#include "regex_test.hpp"
#include "scanner_test.hpp"
#include "parser_test.hpp"
#include "emitter_test.hpp"

int main(int argc, char **argv) {
  return Catch::Session().run(argc, argv);
}
