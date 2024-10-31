# Content Assimilation Engine

```mermaid
flowchart TD
 A[Data] -->|ETL-lambda|B[(IOWarp Lake)]
 B-->|stage-in| D[Hierarchical Memory]
 D-->|stage-out| B
```
