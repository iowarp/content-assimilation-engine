# Content Assimilation Engine

```mermaid
flowchart TD
 A[Data] -->|ETL-lambda|B[(IOWarp Lake)]
 B-->|stage-in| D[Hierarchical Memory]
 D-->|stage-out| B
```

* [s.cpp](s.cpp): Transfer a sample CSV file from Globus to local file system.
* [ydata.ipynb](ydata.ipynb): Measure profiling [performance](https://github.com/iowarp/content-assimilation-engine/wiki/Performance).
