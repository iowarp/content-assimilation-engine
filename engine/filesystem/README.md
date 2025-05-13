Local File Access

1. LD_PRELOAD the POSIX frontend.
2. Read a file `/path/to/local/file.txt`.
3. POSIX frontend intercepts this and converts to iowarp tasks.
4. Tasks are sent to CTE.
5. CTE sees that the data is not loaded.
6. CTE sends a task to CAE local filesystem backend to read the data.

