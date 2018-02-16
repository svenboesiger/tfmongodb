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

#include "mongodb_connector.h"
#include "../converter/bsonconverter.h"

namespace tensorflow {

MongoDBConnector::MongoDBConnector(std::string database_name,
                                   std::string collection_name,
                                   std::string uri) {
  database_name_ = database_name;
  collection_name_ = collection_name;
  uri_ = uri;
};

tensorflow::Status MongoDBConnector::connect() {
  tensorflow::Status connection_status = this->connect_to_server();
  if (connection_status != tensorflow::Status::OK())
    return connection_status;

  if (this->isStringInDocumentsName(conn_.list_databases(), database_name_))
    db_ = conn_.database(database_name_);
  else
    return tensorflow::errors::NotFound("Couldn't find the database.");

  if (this->isStringInDocumentsName(db_.list_collections(), collection_name_))
    coll_ = db_.collection(collection_name_);
  else
    return tensorflow::errors::NotFound("Couldn't find the collection.");

  return tensorflow::Status::OK();
}

tensorflow::Status MongoDBConnector::connect_to_server() {
  mongocxx::uri uri{};
  if (uri_.size() == 0)
    uri = mongocxx::uri{"mongodb://localhost:27017"};
  else
    uri = mongocxx::uri{uri_};

  conn_ = mongocxx::client{mongocxx::uri{}};
  return tensorflow::Status::OK();
}

bool MongoDBConnector::isStringInDocumentsName(mongocxx::cursor cursor, std::string searchString) {
  for (const bsoncxx::document::view &doc :cursor) {
    bsoncxx::document::element ele = doc["name"];
    std::string name = ele.get_utf8().value.to_string();
    if (name.compare(searchString) == 0)
      return true;
  }
  return false;
}

tensorflow::Status MongoDBConnector::query_database() {
  this->cursor_ = new mongocxx::cursor(coll_.find(document{} << finalize));
  return tensorflow::Status::OK();
}

tensorflow::Status MongoDBConnector::retrieve_row(std::tuple<std::string, std::string> &mongodb_row) {
  mongocxx::cursor::iterator iter = cursor_->begin();

  if (iter == cursor_->end())
    return tensorflow::errors::OutOfRange("No more mongodb rows to read for current query.");

  mongodb_row = bson_document_to_csv_row(*iter);
  iter++;
  return tensorflow::Status::OK();
};

MongoDBConnector::~MongoDBConnector() {
  delete this->cursor_;
}

} // namespace
