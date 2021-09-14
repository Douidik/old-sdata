#ifndef SDATA_EXCEPTION_HPP
#define SDATA_EXCEPTION_HPP

#include <exception>
#include <unordered_map>

#include "format.hpp"
#include "type_to_string.hpp"

namespace sdata {

class Exception : public std::exception {
  constexpr static std::string_view MESSAGE_PATTERN = "[%s with {%s}]: %s";

 public:
  Exception(std::string_view description) : m_description(description) {}

  inline const char *what() const noexcept override {
    if (m_buffer.empty())
      m_buffer = format(MESSAGE_PATTERN, name().data(), header().data(), m_description.data());

    return m_buffer.data();
  }

  inline const std::string_view description() const {
    return m_description;
  }

 protected:
  // Exception header, describe the exception context
  inline virtual std::string header() const {
    return "{}";
  }

  inline virtual std::string_view name() const {
    return "sdata::Exception";
  }

 private:
  mutable std::string m_buffer;
  std::string m_description;
};

}  // namespace sdata

#endif
