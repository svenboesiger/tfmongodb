/* Copyright 2018 Sven Boesiger.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
    limitations under the License.
*/
#include "element_to_string.h"

namespace tensorflow {

using namespace bsoncxx;

std::string element_to_string(const bsoncxx::document::element &element) {
  std::string output = "";

  switch (element.type()) {
    case type::k_utf8:output = escape(element.get_utf8().value.to_string());
      break;
    case type::k_bool:output = element.get_bool().value ? "1" : "0";
      break;
    case type::k_double:output = std::to_string(element.get_double().value);
      break;
    case type::k_int32:output = std::to_string(element.get_int32().value);
      break;
    case type::k_int64:output = std::to_string(element.get_int64().value);
      break;
    case type::k_decimal128:output = element.get_decimal128().value.to_string();
      break;
    case type::k_date:output = std::to_string(element.get_date().to_int64());
    //case type::k_timestamp:output = std::to_string(element.get_timestamp().timestamp);
    default:
      // k_zero falls through which is an expected behaviour
      break;
  }
  return
      output;
}

std::string escape(const std::string &input_string) {
  std::size_t n = input_string.length();
  std::string escaped_sequence = "\"";
  escaped_sequence.reserve(n * 2);

  for (std::size_t i = 0;
       i < n;
       ++i) {
    if (input_string[i] == '\\' || input_string[i] == '\"')
      escaped_sequence += '\\';
    escaped_sequence += input_string[i];
  }
  return escaped_sequence.append("\"");
}

} // namespace

