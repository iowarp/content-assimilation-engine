/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Distributed under BSD 3-Clause license.                                   *
 * Copyright by The HDF Group.                                               *
 * Copyright by the Illinois Institute of Technology.                        *
 * All rights reserved.                                                      *
 *                                                                           *
 * This file is part of Hermes. The full Hermes copyright notice, including  *
 * terms governing use, modification, and redistribution, is contained in    *
 * the COPYING file, which can be found at the top directory. If you do not  *
 * have access to the file, you may request a copy from help@hdfgroup.org.   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

bool mpiio_intercepted = true;

#define CAE_DECL(name) name

#include "mpiio_api.h"

#include <hermes/bucket.h>
#include <hermes/hermes.h>

#include "hermes_shm/util/singleton.h"
#include "mpiio_fs_api.h"

// #define CAE_DISABLE_MPIIO

/**
 * Namespace declarations
 */
using cae ::AdapterStat;
using cae ::File;
using cae ::FsIoOptions;
using cae ::IoStatus;
using cae ::MetadataManager;
using cae ::MpiioApi;
using cae ::MpiioFs;
using cae ::MpiioSeekModeConv;
using cae ::SeekMode;

extern "C" {

/**
 * MPI
 */
int MPI_Init(int *argc, char ***argv) {
  HILOG(kDebug, "MPI Init intercepted.");
  cae::IOWARP_CAE_INIT();
  auto real_api = CAE_MPIIO_API;
  return real_api->MPI_Init(argc, argv);
}

int CAE_DECL(MPI_Finalize)(void) {
  HILOG(kDebug, "MPI Finalize intercepted.");
  auto real_api = CAE_MPIIO_API;
  return real_api->MPI_Finalize();
}

int CAE_DECL(MPI_Wait)(MPI_Request *req, MPI_Status *status) {
  HILOG(kDebug, "In MPI_Wait.");
  auto fs_api = CAE_MPIIO_FS;
  return fs_api->Wait(req, status);
}

int CAE_DECL(MPI_Waitall)(int count, MPI_Request *req, MPI_Status *status) {
  HILOG(kDebug, "In MPI_Waitall.");
  auto fs_api = CAE_MPIIO_FS;
  return fs_api->WaitAll(count, req, status);
}

/**
 * Metadata functions
 */
int CAE_DECL(MPI_File_open)(MPI_Comm comm, const char *filename, int amode,
                            MPI_Info info, MPI_File *fh) {
  auto real_api = CAE_MPIIO_API;
  auto fs_api = CAE_MPIIO_FS;
#ifndef CAE_DISABLE_MPIIO
  if (fs_api->IsPathTracked(filename)) {
    HILOG(kDebug, "Intercept MPI_File_open ({}) for filename: {} and mode {}",
          (void *)MPI_File_open, filename, amode);
    AdapterStat stat;
    stat.comm_ = comm;
    stat.amode_ = amode;
    stat.info_ = info;
    File f = fs_api->Open(stat, filename);
    (*fh) = stat.mpi_fh_;
    return f.mpi_status_;
  }
#endif
  HILOG(kDebug, "NOT intercept MPI_File_open ({}) for filename: {} and mode {}",
        (void *)MPI_File_open, filename, amode);
  return real_api->MPI_File_open(comm, filename, amode, info, fh);
}

int CAE_DECL(MPI_File_close)(MPI_File *fh) {
  bool stat_exists;
  auto real_api = CAE_MPIIO_API;
  auto fs_api = CAE_MPIIO_FS;
#ifndef CAE_DISABLE_MPIIO
  if (fs_api->IsMpiFpTracked(fh)) {
    HILOG(kDebug, "Intercept MPI_File_close");
    File f;
    f.hermes_mpi_fh_ = *fh;
    return fs_api->Close(f, stat_exists);
  }
#endif
  return real_api->MPI_File_close(fh);
}

int CAE_DECL(MPI_File_seek)(MPI_File fh, MPI_Offset offset, int whence) {
  bool stat_exists;
  auto real_api = CAE_MPIIO_API;
  auto fs_api = CAE_MPIIO_FS;
#ifndef CAE_DISABLE_MPIIO
  if (fs_api->IsMpiFpTracked(&fh)) {
    HILOG(kDebug, "Intercept MPI_File_seek");
    File f;
    f.hermes_mpi_fh_ = fh;
    return fs_api->Seek(f, stat_exists, offset, whence);
  }
#endif
  return real_api->MPI_File_seek(fh, offset, whence);
}

int CAE_DECL(MPI_File_seek_shared)(MPI_File fh, MPI_Offset offset, int whence) {
  bool stat_exists;
  auto real_api = CAE_MPIIO_API;
  auto fs_api = CAE_MPIIO_FS;
#ifndef CAE_DISABLE_MPIIO
  if (fs_api->IsMpiFpTracked(&fh)) {
    HILOG(kDebug, "Intercept MPI_File_seek_shared offset: {} whence: {}",
          offset, whence);
    File f;
    f.hermes_mpi_fh_ = fh;
    return fs_api->SeekShared(f, stat_exists, offset, whence);
  }
#endif
  return real_api->MPI_File_seek_shared(fh, offset, whence);
}

int CAE_DECL(MPI_File_get_position)(MPI_File fh, MPI_Offset *offset) {
  bool stat_exists;
  auto real_api = CAE_MPIIO_API;
  auto fs_api = CAE_MPIIO_FS;
#ifndef CAE_DISABLE_MPIIO
  if (fs_api->IsMpiFpTracked(&fh)) {
    HILOG(kDebug, "Intercept MPI_File_get_position");
    File f;
    f.hermes_mpi_fh_ = fh;
    (*offset) = static_cast<MPI_Offset>(fs_api->Tell(f, stat_exists));
    return MPI_SUCCESS;
  }
#endif
  return real_api->MPI_File_get_position(fh, offset);
}

int CAE_DECL(MPI_File_read_all)(MPI_File fh, void *buf, int count,
                                MPI_Datatype datatype, MPI_Status *status) {
  bool stat_exists;
  auto real_api = CAE_MPIIO_API;
  auto fs_api = CAE_MPIIO_FS;
#ifndef CAE_DISABLE_MPIIO
  if (fs_api->IsMpiFpTracked(&fh)) {
    HILOG(kDebug, "Intercept MPI_File_read_all");
    File f;
    f.hermes_mpi_fh_ = fh;
    return fs_api->ReadAll(f, stat_exists, buf, count, datatype, status);
  }
#endif
  return real_api->MPI_File_read_all(fh, buf, count, datatype, status);
}
int CAE_DECL(MPI_File_read_at_all)(MPI_File fh, MPI_Offset offset, void *buf,
                                   int count, MPI_Datatype datatype,
                                   MPI_Status *status) {
  bool stat_exists;
  auto real_api = CAE_MPIIO_API;
  auto fs_api = CAE_MPIIO_FS;
#ifndef CAE_DISABLE_MPIIO
  if (fs_api->IsMpiFpTracked(&fh)) {
    HILOG(kDebug, "Intercept MPI_File_read_at_all");
    File f;
    f.hermes_mpi_fh_ = fh;
    return fs_api->ReadAll(f, stat_exists, buf, offset, count, datatype,
                           status);
  }
#endif
  return real_api->MPI_File_read_at_all(fh, offset, buf, count, datatype,
                                        status);
}
int CAE_DECL(MPI_File_read_at)(MPI_File fh, MPI_Offset offset, void *buf,
                               int count, MPI_Datatype datatype,
                               MPI_Status *status) {
  bool stat_exists;
  auto real_api = CAE_MPIIO_API;
  auto fs_api = CAE_MPIIO_FS;
#ifndef CAE_DISABLE_MPIIO
  if (fs_api->IsMpiFpTracked(&fh)) {
    HILOG(kDebug, "Intercept MPI_File_read_at");
    File f;
    f.hermes_mpi_fh_ = fh;
    return fs_api->Read(f, stat_exists, buf, offset, count, datatype, status);
  }
#endif
  return real_api->MPI_File_read_at(fh, offset, buf, count, datatype, status);
}
int CAE_DECL(MPI_File_read)(MPI_File fh, void *buf, int count,
                            MPI_Datatype datatype, MPI_Status *status) {
  bool stat_exists;
  auto real_api = CAE_MPIIO_API;
  auto fs_api = CAE_MPIIO_FS;
#ifndef CAE_DISABLE_MPIIO
  if (fs_api->IsMpiFpTracked(&fh)) {
    HILOG(kDebug, "Intercept MPI_File_read");
    File f;
    f.hermes_mpi_fh_ = fh;
    int ret = fs_api->Read(f, stat_exists, buf, count, datatype, status);
    if (stat_exists)
      return ret;
  }
#endif
  return real_api->MPI_File_read(fh, buf, count, datatype, status);
}
int CAE_DECL(MPI_File_read_ordered)(MPI_File fh, void *buf, int count,
                                    MPI_Datatype datatype, MPI_Status *status) {
  bool stat_exists;
  auto real_api = CAE_MPIIO_API;
  auto fs_api = CAE_MPIIO_FS;
#ifndef CAE_DISABLE_MPIIO
  if (fs_api->IsMpiFpTracked(&fh)) {
    HILOG(kDebug, "Intercept MPI_File_read_ordered");
    File f;
    f.hermes_mpi_fh_ = fh;
    return fs_api->ReadOrdered(f, stat_exists, buf, count, datatype, status);
  }
#endif
  return real_api->MPI_File_read_ordered(fh, buf, count, datatype, status);
}
int CAE_DECL(MPI_File_read_shared)(MPI_File fh, void *buf, int count,
                                   MPI_Datatype datatype, MPI_Status *status) {
  bool stat_exists;
  auto real_api = CAE_MPIIO_API;
  auto fs_api = CAE_MPIIO_FS;
#ifndef CAE_DISABLE_MPIIO
  if (fs_api->IsMpiFpTracked(&fh)) {
    HILOG(kDebug, "Intercept MPI_File_read_shared");
    File f;
    f.hermes_mpi_fh_ = fh;
    return fs_api->Read(f, stat_exists, buf, count, datatype, status);
  }
#endif
  return real_api->MPI_File_read_shared(fh, buf, count, datatype, status);
}
int CAE_DECL(MPI_File_write_all)(MPI_File fh, const void *buf, int count,
                                 MPI_Datatype datatype, MPI_Status *status) {
  bool stat_exists;
  auto real_api = CAE_MPIIO_API;
  auto fs_api = CAE_MPIIO_FS;
#ifndef CAE_DISABLE_MPIIO
  if (fs_api->IsMpiFpTracked(&fh)) {
    HILOG(kDebug, "Intercept MPI_File_write_all");
    File f;
    f.hermes_mpi_fh_ = fh;
    return fs_api->WriteAll(f, stat_exists, buf, count, datatype, status);
  }
#endif
  return real_api->MPI_File_write_all(fh, buf, count, datatype, status);
}
int CAE_DECL(MPI_File_write_at_all)(MPI_File fh, MPI_Offset offset,
                                    const void *buf, int count,
                                    MPI_Datatype datatype, MPI_Status *status) {
  bool stat_exists;
  auto real_api = CAE_MPIIO_API;
  auto fs_api = CAE_MPIIO_FS;
#ifndef CAE_DISABLE_MPIIO
  if (fs_api->IsMpiFpTracked(&fh)) {
    HILOG(kDebug, "Intercept MPI_File_write_at_all");
    File f;
    f.hermes_mpi_fh_ = fh;
    return fs_api->WriteAll(f, stat_exists, buf, offset, count, datatype,
                            status);
  }
#endif
  return real_api->MPI_File_write_at_all(fh, offset, buf, count, datatype,
                                         status);
}
int CAE_DECL(MPI_File_write_at)(MPI_File fh, MPI_Offset offset, const void *buf,
                                int count, MPI_Datatype datatype,
                                MPI_Status *status) {
  bool stat_exists;
  auto real_api = CAE_MPIIO_API;
  auto fs_api = CAE_MPIIO_FS;
#ifndef CAE_DISABLE_MPIIO
  if (fs_api->IsMpiFpTracked(&fh)) {
    HILOG(kDebug, "Intercept MPI_File_write_at");
    File f;
    f.hermes_mpi_fh_ = fh;
    return fs_api->Write(f, stat_exists, buf, offset, count, datatype, status);
  }
#endif
  return real_api->MPI_File_write_at(fh, offset, buf, count, datatype, status);
}
int CAE_DECL(MPI_File_write)(MPI_File fh, const void *buf, int count,
                             MPI_Datatype datatype, MPI_Status *status) {
  bool stat_exists;
  auto real_api = CAE_MPIIO_API;
  auto fs_api = CAE_MPIIO_FS;
#ifndef CAE_DISABLE_MPIIO
  if (fs_api->IsMpiFpTracked(&fh)) {
    HILOG(kDebug, "Intercept MPI_File_write");
    File f;
    f.hermes_mpi_fh_ = fh;
    return fs_api->Write(f, stat_exists, buf, count, datatype, status);
  }
#endif
  return real_api->MPI_File_write(fh, buf, count, datatype, status);
}
int CAE_DECL(MPI_File_write_ordered)(MPI_File fh, const void *buf, int count,
                                     MPI_Datatype datatype,
                                     MPI_Status *status) {
  bool stat_exists;
  auto real_api = CAE_MPIIO_API;
  auto fs_api = CAE_MPIIO_FS;
#ifndef CAE_DISABLE_MPIIO
  if (fs_api->IsMpiFpTracked(&fh)) {
    HILOG(kDebug, "Intercept MPI_File_write_ordered");
    File f;
    f.hermes_mpi_fh_ = fh;
    return fs_api->WriteOrdered(f, stat_exists, buf, count, datatype, status);
  }
#endif
  return real_api->MPI_File_write_ordered(fh, buf, count, datatype, status);
}
int CAE_DECL(MPI_File_write_shared)(MPI_File fh, const void *buf, int count,
                                    MPI_Datatype datatype, MPI_Status *status) {
  bool stat_exists;
  auto real_api = CAE_MPIIO_API;
  auto fs_api = CAE_MPIIO_FS;
#ifndef CAE_DISABLE_MPIIO
  if (fs_api->IsMpiFpTracked(&fh)) {
    // NOTE(llogan): originally WriteOrdered
    HILOG(kDebug, "Intercept MPI_File_write_shared");
    File f;
    f.hermes_mpi_fh_ = fh;
    return fs_api->WriteOrdered(f, stat_exists, buf, count, datatype, status);
  }
#endif
  return real_api->MPI_File_write_shared(fh, buf, count, datatype, status);
}

/**
 * Async Read/Write
 */
int CAE_DECL(MPI_File_iread_at)(MPI_File fh, MPI_Offset offset, void *buf,
                                int count, MPI_Datatype datatype,
                                MPI_Request *request) {
  bool stat_exists;
  auto real_api = CAE_MPIIO_API;
  auto fs_api = CAE_MPIIO_FS;
#ifndef CAE_DISABLE_MPIIO
  if (fs_api->IsMpiFpTracked(&fh)) {
    HILOG(kDebug, "Intercept MPI_File_iread_at");
    File f;
    f.hermes_mpi_fh_ = fh;
    fs_api->ARead(f, stat_exists, buf, offset, count, datatype, request);
    return MPI_SUCCESS;
  }
#endif
  return real_api->MPI_File_iread_at(fh, offset, buf, count, datatype, request);
}
int CAE_DECL(MPI_File_iread)(MPI_File fh, void *buf, int count,
                             MPI_Datatype datatype, MPI_Request *request) {
  bool stat_exists;
  auto real_api = CAE_MPIIO_API;
  auto fs_api = CAE_MPIIO_FS;
#ifndef CAE_DISABLE_MPIIO
  if (fs_api->IsMpiFpTracked(&fh)) {
    HILOG(kDebug, "Intercept MPI_File_iread");
    File f;
    f.hermes_mpi_fh_ = fh;
    fs_api->ARead(f, stat_exists, buf, count, datatype, request);
  }
#endif
  return real_api->MPI_File_iread(fh, buf, count, datatype, request);
}
int CAE_DECL(MPI_File_iread_shared)(MPI_File fh, void *buf, int count,
                                    MPI_Datatype datatype,
                                    MPI_Request *request) {
  bool stat_exists;
  auto real_api = CAE_MPIIO_API;
  auto fs_api = CAE_MPIIO_FS;
#ifndef CAE_DISABLE_MPIIO
  if (fs_api->IsMpiFpTracked(&fh)) {
    HILOG(kDebug, "Intercept MPI_File_iread_shared");
    File f;
    f.hermes_mpi_fh_ = fh;
    fs_api->ARead(f, stat_exists, buf, count, datatype, request);
    return MPI_SUCCESS;
  }
#endif
  return real_api->MPI_File_iread_shared(fh, buf, count, datatype, request);
}
int CAE_DECL(MPI_File_iwrite_at)(MPI_File fh, MPI_Offset offset,
                                 const void *buf, int count,
                                 MPI_Datatype datatype, MPI_Request *request) {
  bool stat_exists;
  auto real_api = CAE_MPIIO_API;
  auto fs_api = CAE_MPIIO_FS;
#ifndef CAE_DISABLE_MPIIO
  if (fs_api->IsMpiFpTracked(&fh)) {
    HILOG(kDebug, "Intercept MPI_File_iwrite_at");
    File f;
    f.hermes_mpi_fh_ = fh;
    fs_api->AWrite(f, stat_exists, buf, offset, count, datatype, request);
    return MPI_SUCCESS;
  }
#endif
  return real_api->MPI_File_iwrite_at(fh, offset, buf, count, datatype,
                                      request);
}

int CAE_DECL(MPI_File_iwrite)(MPI_File fh, const void *buf, int count,
                              MPI_Datatype datatype, MPI_Request *request) {
  bool stat_exists;
  auto real_api = CAE_MPIIO_API;
  auto fs_api = CAE_MPIIO_FS;
#ifndef CAE_DISABLE_MPIIO
  if (fs_api->IsMpiFpTracked(&fh)) {
    HILOG(kDebug, "Intercept MPI_File_iwrite");
    File f;
    f.hermes_mpi_fh_ = fh;
    fs_api->AWrite(f, stat_exists, buf, count, datatype, request);
    return MPI_SUCCESS;
  }
#endif
  return real_api->MPI_File_iwrite(fh, buf, count, datatype, request);
}
int CAE_DECL(MPI_File_iwrite_shared)(MPI_File fh, const void *buf, int count,
                                     MPI_Datatype datatype,
                                     MPI_Request *request) {
  bool stat_exists;
  auto real_api = CAE_MPIIO_API;
  auto fs_api = CAE_MPIIO_FS;
#ifndef CAE_DISABLE_MPIIO
  if (fs_api->IsMpiFpTracked(&fh)) {
    HILOG(kDebug, "Intercept MPI_File_iwrite_shared");
    File f;
    f.hermes_mpi_fh_ = fh;
    fs_api->AWriteOrdered(f, stat_exists, buf, count, datatype, request);
    return MPI_SUCCESS;
  }
#endif
  return real_api->MPI_File_iwrite_shared(fh, buf, count, datatype, request);
}

/**
 * Other functions
 */
int CAE_DECL(MPI_File_sync)(MPI_File fh) {
  bool stat_exists;
  auto real_api = CAE_MPIIO_API;
  auto fs_api = CAE_MPIIO_FS;
#ifndef CAE_DISABLE_MPIIO
  if (fs_api->IsMpiFpTracked(&fh)) {
    HILOG(kDebug, "Intercept MPI_File_sync");
    File f;
    f.hermes_mpi_fh_ = fh;
    fs_api->Sync(f, stat_exists);
    return 0;
  }
#endif
  return real_api->MPI_File_sync(fh);
}

} // extern C
