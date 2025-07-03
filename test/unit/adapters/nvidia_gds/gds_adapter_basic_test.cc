#include <assert.h>
#include <cuda_runtime.h>
#include <fcntl.h>
#include <unistd.h>

#include <cstdlib>
#include <cstring>
#include <iostream>

#include "cufile.h"
using namespace std;

int main(int argc, char *argv[]) {
  int fd;
  ssize_t ret = -1;
  void *devPtr = NULL;
  size_t size = 1024;
  CUfileError_t status;
  CUfileDescr_t cf_descr;
  CUfileHandle_t cf_handle;
  off_t offset = 0;

  status = cuFileDriverOpen();

  if (status.err != CU_FILE_SUCCESS) {
    cout << "Driver failed ..." << endl;
  }

  fd = open("myfile.txt", O_CREAT | O_WRONLY | O_RDONLY | O_DIRECT, 0644);
  if (fd < 0) {
    cout << "Failed to open the file" << endl;
  }

  memset((void *)&cf_descr, 0, sizeof(CUfileDescr_t));

  cf_descr.handle.fd = fd;
  cf_descr.type = CU_FILE_HANDLE_TYPE_OPAQUE_FD;
  status = cuFileHandleRegister(&cf_handle, &cf_descr);
  if (status.err != CU_FILE_SUCCESS) {
    cout << "Error file register..." << endl;
  }

  cudaMalloc(&devPtr, size);

  cudaMemset((void *)devPtr, 1, size);
  cudaStreamSynchronize(0);
  status = cuFileBufRegister(devPtr, size, 0);
  if (status.err != CU_FILE_SUCCESS) {
    cout << "buf register failed: " << endl;
  }

  ret = cuFileWrite(cf_handle, devPtr, size, 0, 0);
  if (ret < 0) {
    cout << "Error in cufile write" << endl;
  }

  status = cuFileBufDeregister(devPtr);
  if (status.err != CU_FILE_SUCCESS) {
    cout << "Error in deregister file buffer. ERROR: " << status.err << endl;
  }

  cuFileHandleDeregister(cf_handle);

  cuFileDriverClose();

  cudaFree(devPtr);
}
