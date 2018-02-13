#include "tensorflow/core/kernels/dataset.h"
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

    *output = new Dataset(ctx, database, collection);
  }

 private:

  class Dataset : public GraphDatasetBase {
   public:
    Dataset(OpKernelContext *ctx, const string &database, const string &collection)
        : GraphDatasetBase(ctx), database_(database), collection_(collection) {}

    std::unique_ptr<IteratorBase> MakeIterator(
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

    string DebugString() override { return "MongoDBDatasetOp::Dataset"; }

   protected:
    Status AsGraphDefInternal(DatasetGraphDefBuilder *b,
                              Node **output) const override {
      Node *database = nullptr;
      Node *collection = nullptr;
      TF_RETURN_IF_ERROR(b->AddScalar(database_, &database));
      TF_RETURN_IF_ERROR(b->AddScalar(collection_, &collection));
      TF_RETURN_IF_ERROR(b->AddDataset(
          this, {database, collection}, output));

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
                                   dataset()->collection_});
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
      std::unique_ptr<MongoDBConnector> connector_ = nullptr;
    };

    const string database_;
    const string collection_;
  };
  mongocxx::instance inst_{};
};

REGISTER_KERNEL_BUILDER(Name("MongoDataset").Device(DEVICE_CPU),
                        MongoDatasetOp);

}  // namespace tensorflow
