# Skiplist Project

Advanced Multiprocessor Programming - TU Wien SS2017

Requirements:
* CMake
* Boost
* GTest (only when building with tests)

Build Project incl. Benchmarks:
* `mkdir build; cd build; cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_BENCHMARKS=ON ..; make; cd ..`

Run Benchmarks:
* `./build/benchmarks/SkipListBenchmark`
