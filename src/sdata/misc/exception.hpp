#ifndef SDATA_EXCEPTION_HPP
#define SDATA_EXCEPTION_HPP

#include <unordered_map>
#include "format.hpp"
#include "type_to_string.hpp"

namespace sdata {

class Exception {
  constexpr static std::string_view MESSAGE_PATTERN = "[%s with {%s}] %s";

 public:
  Exception(std::string_view description) : m_description(description) {}

  std::string what() const {
    return format(MESSAGE_PATTERN, name().data(), header().data(), m_description.data());
  }

 protected:
  // Exception header, describe exception parameters
  virtual std::string header() const {
     return "{}";
  }
  
  virtual std::string_view name() const {
    return "sdata::Exception";
  }

  static std::string format_parameters(
      std::unordered_map<std::string_view, std::string> parameters) {
    constexpr std::string_view PARAMETER_PATTERN = " '%s': %s; ";
    std::string parameters_fmt;

    for (const auto& [name, value] : parameters) {
      parameters_fmt += format(PARAMETER_PATTERN, name.data(), value.data());
    }

    return parameters_fmt;
  }

 private:
  std::string m_description;
};

}  // namespace sdata

#endif
