Let's change the FormatClient API. Below is an example for binary_file_omni.h. Let's make this header-only. No need for binary_file_omni.cc. We will also be removing the system() calling mpi to call binary_file_omni executable. Let's take the code that is currently in binary_file_omni.cc and move to the Import function. Make the namespace cae and inherit from FormatClient. 

``` 
#ifndef HERMES_TASKS_DATA_STAGER_SRC_BINARY_STAGER_H_
#define HERMES_TASKS_DATA_STAGER_SRC_BINARY_STAGER_H_

#include "abstract_stager.h"
#include "hermes_adapters/mapper/abstract_mapper.h"

namespace hermes {

class BinaryFileStager : public AbstractStager {
public:
  size_t page_size_;
  std::string path_;
  bitfield32_t flags_;

public:
  /** Default constructor */
  BinaryFileStager() = default;

  /** Destructor */
  ~BinaryFileStager() {}

  /** Build context for staging */
  static Context BuildContext(size_t page_size, u32 flags = 0,
                              size_t elmt_size = 1) {
    Context ctx;
    ctx.flags_.SetBits(HERMES_SHOULD_STAGE);
    ctx.bkt_params_ = BuildFileParams(page_size, flags, elmt_size);
    return ctx;
  }

  /** Build serialized file parameter pack */
  static std::string BuildFileParams(size_t page_size, u32 flags = 0,
                                     size_t elmt_size = 1) {
    chi::string params(32);
    page_size = (page_size / elmt_size) * elmt_size;
    hipc::LocalSerialize srl(params);
    srl << std::string("file");
    srl << flags;
    srl << page_size;
    return params.str();
  }

  /** Assimilate a file  */
  void Import(const FormatContext &ctx) override {
    // Essentially what is currently in binary_file_omni.cc
  }

  /** Create the data stager payload */
  void RegisterStager(const hipc::MemContext &mctx, const std::string &tag_name,
                      const std::string &params) override {
    std::string protocol;
    hipc::LocalDeserialize srl(params);
    srl >> protocol;
    srl >> flags_.bits_;
    srl >> page_size_;
    path_ = tag_name;
  }

  /** Stage data in from remote source */
  void ImportBlob(const hipc::MemContext &mctx, hermes::Client &client,
               const TagId &tag_id, const std::string &blob_name,
               float score) override {
    if (flags_.Any(HERMES_STAGE_NO_READ)) {
      return;
    }
    // Get the position of the file to stage in
    adapter::BlobPlacement plcmnt;
    plcmnt.DecodeBlobName(blob_name, page_size_);
    HILOG(kDebug,
          "Attempting to stage {} bytes from the backend file {} at offset {}",
          page_size_, path_, plcmnt.bucket_off_);
    // Stage in the data from the file
    FullPtr<char> blob = CHI_CLIENT->AllocateBuffer(mctx, page_size_);
    int fd = HERMES_POSIX_API->open(path_.c_str(), O_CREAT | O_RDWR, 0666);
    if (fd < 0) {
      HELOG(kError, "Failed to open file {}", path_);
      CHI_CLIENT->FreeBuffer(HSHM_MCTX, blob);
      return;
    }
    ssize_t real_size = HERMES_POSIX_API->pread(fd, blob.ptr_, page_size_,
                                                (off_t)plcmnt.bucket_off_);
    HERMES_POSIX_API->close(fd);
    // Verify the data was staged in
    if (real_size < 0) {
      CHI_CLIENT->FreeBuffer(HSHM_MCTX, blob);
      return;
    } else if (real_size == 0) {
      CHI_CLIENT->FreeBuffer(HSHM_MCTX, blob);
      return;
    }
    // Put the new blob into hermes
    HILOG(kDebug, "Staged {} bytes from the backend file {}", real_size, path_);
    client.PutBlob(mctx, chi::DomainQuery::GetDynamic(), tag_id,
                   chi::string(blob_name), hermes::BlobId::GetNull(), 0,
                   real_size, blob.shm_, score, TASK_DATA_OWNER, 0);
  }

  /** Stage data out to remote source */
  void ExportBlob(const hipc::MemContext &mctx, hermes::Client &client,
                const TagId &tag_id, const std::string &blob_name,
                hipc::Pointer &data_p, size_t data_size) override {
    if (flags_.Any(HERMES_STAGE_NO_WRITE)) {
      return;
    }
    // Get the position of the file to stage out
    adapter::BlobPlacement plcmnt;
    plcmnt.DecodeBlobName(blob_name, page_size_);
    HILOG(kDebug,
          "Attempting to stage {} bytes to the backend file {} at offset {}",
          page_size_, path_, plcmnt.bucket_off_);
    // Stage out the data to the file
    FullPtr data(data_p);
    int fd = HERMES_POSIX_API->open(path_.c_str(), O_CREAT | O_RDWR, 0666);
    if (fd < 0) {
      HELOG(kError, "Failed to open file {}", path_);
      return;
    }
    ssize_t real_size = HERMES_POSIX_API->pwrite(fd, data.ptr_, data_size,
                                                 (off_t)plcmnt.bucket_off_);
    HERMES_POSIX_API->close(fd);
    // Verify the data was staged out
    if (real_size < 0) {
      HELOG(kError, "Failed to stage out {} bytes from {}", data_size, path_);
    }
    HILOG(kDebug, "Staged out {} bytes to the backend file {}", real_size,
          path_);
  }

  void UpdateSize(const hipc::MemContext &mctx, hermes::Client &client,
                  const TagId &tag_id, const std::string &blob_name,
                  size_t blob_off, size_t data_size) override {
    adapter::BlobPlacement p;
    p.DecodeBlobName(blob_name, page_size_);
    client.AsyncTagUpdateSize(mctx, chi::DomainQuery::GetDynamic(), tag_id,
                              p.bucket_off_ + blob_off + data_size,
                              UpdateSizeMode::kCap);
  }
};

} // namespace hermes

#endif // HERMES_TASKS_DATA_STAGER_SRC_BINARY_STAGER_H_
```