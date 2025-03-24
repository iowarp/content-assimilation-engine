Globus

1. Acess the file `globus::${HOME}/hi.h5`
2. Read the file `globus::${HOME}/hi.h5`
3. VFD or VOL fronted intercepts this and converts to iowarp tasks.
4. Tasks get sent to CTE.
5. CTE sees that the data is not loaded.
6. CTE sends a task to CAE globus backend to read the data.
  1. Globus backend checks local filesystem if data is downloaded
  2. If that fails, it downloads the entire file.
    * Where does the file get stored?
      We allow users to define a staging directory as a conf option.
