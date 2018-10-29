/* Copyright 2017 The TensorFlow Authors. All Rights Reserved.
   Copyright 2018 Sven Boesiger. All Rights Reserved.

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

#include "tensorflow/core/framework/dataset.h"
#include "tensorflow/core/framework/partial_tensor_shape.h"
#include "tensorflow/core/framework/tensor.h"
#include "../connector/mongodb_connector.h"

namespace tensorflow {

class MongoDatasetOp : public DatasetOpKernel {
 public:
  using DatasetOpKernel::DatasetOpKernel;

  void MakeDataset(OpKernelContext *ctx, DatasetBase **output) override {

    string database;
    OP_REQUIRES_OK(ctx, ParseScalarArgument<string>(ctx, "database",
                                                    &database));
    string collection;
    OP_REQUIRES_OK(ctx, ParseScalarArgument<string>(ctx, "collection",
                                                    &collection));
    string uri;
    OP_REQUIRES_OK(ctx, ParseScalarArgument<string>(ctx, "uri",
                                                    &uri));
    *output = new Dataset(ctx, database, collection, uri);
  }

 private:

  class Dataset : public DatasetBase {
   public:
    Dataset(OpKernelContext *ctx, const string &database, const string &collection,
            const string &uri)
        : DatasetBase(DatasetContext(ctx)), database_(database), collection_(collection), 
            uri_(uri) {}

    std::unique_ptr<IteratorBase> MakeIteratorInternal (
        const string &prefix) const override {
      return std::unique_ptr<IteratorBase>(
          new Iterator({this, strings::StrCat(prefix, "::TextLine")}));
    }

    const DataTypeVector &output_dtypes() const override {
      static DataTypeVector *dtypes = new DataTypeVector({DT_STRING});
      return *dtypes;
    }

    const std::vector<PartialTensorShape> &output_shapes() const override {
      static std::vector<PartialTensorShape> *shapes =
          new std::vector<PartialTensorShape>({{}});
      return *shapes;
    }

    string DebugString() const override { return "MongoDBDatasetOp::Dataset"; }

  protected:
    Status AsGraphDefInternal(SerializationContext *ctx,
                              DatasetGraphDefBuilder *b,
                              Node **output) const override {
      Node *database = nullptr;
      Node *collection = nullptr;
      Node *uri = nullptr;
      TF_RETURN_IF_ERROR(b->AddScalar(database_, &database));
      TF_RETURN_IF_ERROR(b->AddScalar(collection_, &collection));
      TF_RETURN_IF_ERROR(b->AddScalar(uri_, &uri));
      TF_RETURN_IF_ERROR(b->AddDataset(
          this, {database, collection, uri}, output));

      return Status::OK();
    }

   private:
    class Iterator : public DatasetIterator<Dataset> {
     public:
      explicit Iterator(const Params &params)
          : DatasetIterator<Dataset>(params) {}

      Status GetNextInternal(IteratorContext *ctx,
                             std::vector<Tensor> *out_tensors,
                             bool *end_of_sequence) override {
        mutex_lock l(mu_);

        if (!this->connector_) {
          this->connector_ = std::unique_ptr<MongoDBConnector>(
              new MongoDBConnector{dataset()->database_,
                                   dataset()->collection_, 
                                   dataset()->uri_});
          TF_RETURN_IF_ERROR(this->connector_->connect());
          TF_RETURN_IF_ERROR(this->connector_->query_database());
        }

        do {
          std::tuple<std::string, std::string> keyValue;
          Status status = this->connector_->retrieve_row(keyValue);

          if (status.ok()) {
            Tensor line_tensor(cpu_allocator(), DT_STRING, {});
            line_tensor.scalar<string>()() = std::get<1>(keyValue);
            out_tensors->emplace_back(std::move(line_tensor));
            *end_of_sequence = false;
            return Status::OK();
          } else if (!errors::IsOutOfRange(status)) {
            // Report non-EOF errors to the caller.
            return status;
          }

          *end_of_sequence = true;
          return Status::OK();
        } while (true);

      }

     protected:
      Status SaveInternal(IteratorStateWriter *writer) override {
        mutex_lock l(mu_);

        return Status::OK();
      }

      Status RestoreInternal(OpKernelContext *ctx,
                             IteratorStateReader *reader) {

        //connector_->connect();
        return Status::OK();
      }

     private:

      mutex mu_;
      std::string database_name_ = "";
      std::string collection_name_ = "";
      std::string uri_name_ = "";
      std::unique_ptr<MongoDBConnector> connector_ = nullptr;
    };

    const string database_;
    const string collection_;
    const string uri_;
  };
  mongocxx::instance inst_{};
};

REGISTER_KERNEL_BUILDER(Name("MongoDataset").Device(DEVICE_CPU),
                        MongoDatasetOp);

}  // namespace tensorflow
