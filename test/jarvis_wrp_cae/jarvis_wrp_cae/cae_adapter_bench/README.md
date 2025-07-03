Hermes is a multi-tiered I/O buffering platform. This module encompasses
a series of microbenchmarks used to test the performance of the Hermes
native API

# Installation

Check the hermes_run jarvis package.

# Usage

```
spack load hermes
jarvis pipeline create hermes_bench
jarvis pipeline env build
jarvis pipeline append hermes --sleep=5
jarvis pipeline append cae_adapter_bench
jarvis pipeline run
```

## PutGet benchmark

```
jarvis pkg configure cae_adapter_bench \
mode=putget \
blob_size=4k \
blobs_per_rank=16 \
nprocs=2
```

## PartialPutGet benchmark

```
jarvis pkg configure cae_adapter_bench \
mode=pputget \
blobs_per_rank=64 \
blob_size=1m \
part_size=4k \
nprocs=64
```

## Create Bucket benchmark

```
jarvis pkg configure cae_adapter_bench \
mode=create_bkt \
bkts_per_rank=256 \
nprocs=16
```

## Get Bucket benchmark

```
jarvis pkg configure cae_adapter_bench \
mode=get_bkt \
bkts_per_rank=1024 \
nprocs=64
```

## Delete Bucket benchmark

```
jarvis pkg configure cae_adapter_bench \
mode=del_bkt \
bkts_per_rank=1024 \
blobs_per_bkt=1024 \
nprocs=64
```

