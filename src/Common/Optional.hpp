// Copyright (c) 2018-2023 Conceal Network & Conceal Devs
//
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

class bad_optional_access : public std::exception {
 public:
  const char* what() const noexcept override {
    return "bad_optional_access: optional has no value";
  }
};

template <typename T>
class optional {
 public:
  optional() : has_value_(false) {}
  optional(const T& value) : has_value_(true), value_(value) {}

  bool has_value() const { return has_value_; }

  const T& value() const {
    if (!has_value_) {
      throw bad_optional_access();
    }
    return value_;
  }

 private:
  bool has_value_;
  T value_;
};