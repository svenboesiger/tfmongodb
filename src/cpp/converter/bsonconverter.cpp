/* Copyright 2018 Sven Boesiger. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
    http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include "bsonconverter.h"

namespace tensorflow {

const std::string DELIMITER = ",";

std::tuple<std::string, std::string>
bson_document_to_csv_row(bsoncxx::document::view document) {

  std::string oid = "";
  std::string output = "";
  for (bsoncxx::document::element element : document) {
    if (element.type() == bsoncxx::type::k_oid)
      oid = element.get_oid().value.to_string();

    if (element.type() == bsoncxx::type::k_int64 || // currently supported types
        element.type() == bsoncxx::type::k_int32 || // @TODO implement binaries (?)
        element.type() == bsoncxx::type::k_double ||
        element.type() == bsoncxx::type::k_bool ||
        element.type() == bsoncxx::type::k_decimal128 ||
        element.type() == bsoncxx::type::k_utf8 ||
        element.type() == bsoncxx::type::k_null ||
        element.type() == bsoncxx::type::k_date ||
        element.type() == bsoncxx::type::k_timestamp) {

      output = output.append(element_to_string(element));
      output = output.append(DELIMITER);
    }
  }
  if (output.size() > 0) output.resize(output.size() - 1);
  return std::make_tuple(oid, output);
}

} // namespace


