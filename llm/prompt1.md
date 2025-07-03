We are building a content assimilation engine. It can download data from various repositories such as filesystems, globus, and Amazon S3.

It can interact with different data formats, such as binary files and HDF5.

We want to use a YML file format to specify how to download data from these various resources. We call this format omni format.

# Omni Parser Structure
We should have the following structure: 
1. C++ source file for the OMNI parser that parses the YAML file (wrp.cc) that compiles into a binary (wrp)
2. C++ header file that provides abstract classes for importing data from different file formats (e.g., binary file) and data repositories (e.g., filesystem) (OMNI.h)
3. C++ header file that provides a factory of file formats and data repositores (OMNI_factory.h)
4. A C++ header file that imports data from binary POSIX files (binary_file_omni.h). It inherits from the factory in (2).
5. C++ source file (binary_file_omni.cc) that compiles into a binary (binary_file_omni)
6. A c++ header file that implements a filesystem repo client, particularly implementing RecommendScale

These files should be placed in the omni subdirectory.

# OMNI.h
The factory should provide the following methods at a minimum. You should expand as-needed:
```cpp
struct FormatContext {
  std::string description_;
  std::string filename_;
  size_t offset_;
  size_t size_;
  size_t hash_;
};

class FormatClient {
public:
  virtual void Import(const FormatContext &ctx) = 0;
};

struct RepoContext {
public:
  std::string username_;
  std::string passwd_;
  std::string key_;
};

class RepoClient {
public:
  virtual void RecommendScale(int max_scale, int &nprocs, int &nthreads);
  virtual void Download(const RepoContext &ctx);
};
```

# Binary Posix File

## omni.cc

Below is a sample OMNI file for assimilating data from a binary posix file.
```yaml
# Sample OMNI format in YAML
name: cae posix job
max_scale: 100  # Max num processes
data:
- path: /path/to/file.txt  # Path to the file (required)
  range: [0, N]  # range of file we want to assimilate (optional)
  offset: 0  # Offset we want to begin assimilating (optional)
  size: 256  # Total size of data we want to assimilat starting from offset (optional)
  description:  # Description of the data (optional)
    - text
    - unstructured
  hash: sha256 of file maybe # Integrity information (optional)
```

wpr.cc should parse this yaml file using yaml-cpp and then parameterize the binary_file_omni binary, which is described in more detail below. Use the system() method from C++ to call the omni binary. range can be derived from offset, size.

Use MPI to increase the scale of the download. Use the RepoFactory

## filesystem_repo_omni.h
This will create a class named FilesystemRepoClient that inherits from RepoClient. It implements RecommendScale. It is based solely on the file . The objective is to do at least 64MB per process. If the file is larger than 64MB, then add new processes until a certain maximum scale is reached. The max_scale is defined in the omni file.

## binary_file_omni.h
This file should inherit from FormatClient. Import should simply use stdio to read data from the file as specified by the format context. Read the file in chunks of no larger than 16MB within the specified range (offset, size) sequentially.

## binary_file_omni.cc

```cpp
int main() {
    MPI_Init(...);
    // Take as input the following args:
    // path, offset, size, description, hash
    FormatContext fctx;
    fctx.path = ...
    fctx.offset = ...

    // Let's divide the file into smaller chunks as evenly as possible based on MPI rank
    int rank, nprocs;
    MPI_Comm_size();
    MPI_Comm_rank(); 
    fctx.offset = // some function of rank and offset
    fctx.size = // fctx.size / nprocs, but accounting for boundary conditions
    
    // Get the format client
    std::unique_ptr<FormatClient> client = FormatFactory::Get(Format::kPosix);
    client->Import(fctx);

    MPI_Finalize(MPI_COMM_WORLD);
}
```
