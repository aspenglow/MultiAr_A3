# Assignement 3 - Group 48

## part 1

The idea of deoptimizing is that we need to change the loading step in the array, and the step size shound not be a constant number so that the access pattern is not linear and cache line prefetching cannot happen. in this problem we changed the `next_addr` function and made it return `i%mod+64`. `mod` is a variable relevant to the value of `i` with three choices, thus the address of the next access can only be calculated after the first access completes since we need the latest `i` to calculate the return value, and the `mod` variable can make the access parttern more random. The reason why we added 64 to the return value is that there is 64 bytes in a cache line so we can avoid the benefits from data caching.

Through multiple attempts we found that changing the `init_array` function have little influence on both the number of access and the time per access so we did not change the `init_array` function eventually.

There are three NUMA memory allocation policies, 'local', 'remote', 'interleave', respectively. We editted the `execute_numa.sh` to submit the code through these three policies. `numactl --cpunodebind=0 --membind=1 ./numa` correspond to 'remote' policy, `numactl --interleave=all ./numa` correspond to 'interleave' policy, `numactl --localalloc ./numa` corresponds to 'local' policy. The result is:

### modified numa program

| NUMA policy | total time(s) | number of accesses |  Time per access(s) |
|:-----------:|--------------:|-------------------:|--------------------:|
|remote       |0.9693         |25768665            |3.749e-8             |
|interleave   |0.7782         |25768665            |3.014e-8             |
|local        |0.6252         |25768665            |2.426e-8             |

### origin numa program

| NUMA policy | total time(s) | number of accesses |  Time per access(s) |
|:-----------:|--------------:|-------------------:|--------------------:|
|remote       |5.104          |8589934592          |5.942e-10            |
|interleave   |5.095          |8589934592          |5.932e-10            |
|local        |5.072          |8589934592          |5.904e-10            |




