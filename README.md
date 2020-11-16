# Assignement 3 - Group 48

## part 1

The idea of deoptimizing is that we need to change the loading step in the array, and the step size should not be a constant number so that the access pattern is not linear and cache line prefetching cannot happen. In this problem we changed the `next_addr` function and made it return `i%mod+64`. `mod` is a variable relevant to the value of `i` with three choices, thus the address of the next access can only be calculated after the first access completes since we need the latest `i` to calculate the return value, and the `mod` variable can make the access parttern more random. The reason why we added 64 to the return value is that there is 64 bytes in a cache line so we can avoid the benefits from data caching if the step is always no less than 64.

Through multiple attempts we found that changing the `init_array` function have little influence on both the number of access and the time per access so we did not change the `init_array` function eventually.

There are three NUMA memory allocation policies, 'local', 'remote', 'interleaved', respectively. We editted the `execute_numa.sh` to submit the code through these three policies. `numactl --cpunodebind=0 --membind=1 ./numa` correspond to 'remote' policy, `numactl --interleave=all ./numa` correspond to 'interleaved' policy, `numactl --localalloc ./numa` corresponds to 'local' policy. The result is:

### modified numa program

| NUMA policy | total time(s) | number of accesses |  Time per access(s) |
|:-----------:|--------------:|-------------------:|--------------------:|
|remote       |0.9693         |25768665            |3.749e-08            |
|interleaved  |0.7782         |25768665            |3.014e-08            |
|local        |0.6252         |25768665            |2.426e-08            |

### origin numa program

| NUMA policy | total time(s) | number of accesses |  Time per access(s) |
|:-----------:|--------------:|-------------------:|--------------------:|
|remote       |5.104          |8589934592          |5.942e-10            |
|interleaved  |5.095          |8589934592          |5.932e-10            |
|local        |5.072          |8589934592          |5.904e-10            |

We can observe that:

1. The number of accesses only depend on the `next_addr` function and is irrelevant to allocation policy.
2. Both the total traversing time and the time per access with three policies: remote > interleaved > local
3. After deoptimizing the program, time per access multiplied nearly 50 and total traversing time multiplied nearly 5. Optimization can shorten both the time per access and the total traversing time significantly.

We de-optimized both the data caching and cache line prefetching. To know how much each of them effect the (time taken/memory access) metric, we need to make a experiment with only one of the deoptimization executed. We changed the `next_addr` function and made it returns 64 so that the code only de-optimized data caching, and make the cache line prefetching exist in the code. and the result is:

### only de-optimize data caching
| NUMA policy | total time(s) | number of accesses |  Time per access(s) |
|:-----------:|--------------:|-------------------:|--------------------:|
|remote       |1.08           |134217728           |8.047e-09            |
|interleaved  |0.8913         |134217728           |6.641e-09            |
|local        |0.7195         |134217728           |5.361e-09            |

We can find that with only de-optimizing data caching, compared with the first table, both the number of accesses and the time per access has increased. So data caching optimization can make the memory access increase and time per access decrease. The second table is the result with both of the two optimization, so we can conclude that cache line optimization can also increase the memory access and decrease the time per access.

