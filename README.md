# AlgoBOWL

AlgoBOWL project for CSCI406

## Documentation

### Structure

- `docs/`: documentation and algorithm description/psuedocode
- `src/`: source code

### Development

1. If not running the chad OS (Linux), install [Docker Desktop](https://docs.docker.com/desktop/) and start a new container:

```bash
# Start the container (Unix)
docker run -d -it --name algobowl -v $(pwd):/root/csci406-algobowl ubuntu:latest

# Start the container (Windows)
docker run -d -it --name algobowl -v "$(PWD):/root/csci406-algobowl" ubuntu:latest

# Attach to the container (You may need to run "docker start algobowl" if the container is not running first)
docker exec -it algobowl /bin/bash

# Install tools
apt update -y && apt install -y build-essential cmake gdb git libboostall-dev nano
```

2. Install [CMake](https://cmake.org) and a C++ compiler (e.g. [GCC](https://gcc.gnu.org))

3. Clone the repository

```bash
git clone https://github.com/wakeful-cloud/csci406-algobowl.git

cd csci406-algobowl
```

4. Build the project

```bash
cmake -S . -B build # Or cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug to include debug symbols
cmake --build build
```

5. (Optional) Run the tests

```bash
./build/tests
```

6. Run the project

```bash
./build/solver
./build/verifier
```
