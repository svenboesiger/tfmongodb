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


#ifndef MONGODB_READER_MONGODB_CONNECTOR_H
#define MONGODB_READER_MONGODB_CONNECTOR_H

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/exception/query_exception.hpp>

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>

#include "tensorflow/core/lib/core/errors.h"
#include "../converter/bsonconverter.h"

using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::open_document;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::finalize;

namespace tensorflow {

class MongoDBConnector {
 public:
  MongoDBConnector(std::string database_name, std::string collection_name, std::string uri = "");
  tensorflow::Status connect();
  tensorflow::Status query_database();
  tensorflow::Status retrieve_row(std::tuple<std::string, std::string> &mongodb_row);
  virtual ~MongoDBConnector();

 private:
  std::string uri_, database_name_, collection_name_;
  mongocxx::client conn_{};
  mongocxx::database db_{};
  mongocxx::collection coll_{};
  mongocxx::cursor* cursor_ = nullptr;
  tensorflow::Status connect_to_server();
  bool isStringInDocumentsName(mongocxx::cursor cursor, std::string searchString);
};

} // namespace

#endif //MONGODB_READER_MONGODB_CONNECTOR_H
