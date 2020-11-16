# Assignement 3 - Group 48

## part 1

The idea of deoptimizing is that we need to change the loading step in the array, and the step size shound not be a constant number so that the access pattern is not linear and cache line prefetching cannot happen. in this problem we changed the `next_addr` function and made it return `i%mod+64`. `mod` is a variable relevant to the value of `i` with three choices, thus the address of the next access can only be calculated after the first access completes since we need the latest `i` to calculate the return value, and the `mod` variable can make the access parttern more random. The reason why we added 64 to the return value is that there is 64 bytes in a cache line so we can avoid the benefits from data caching.

Through multiple attempts we found that changing the `init_array` function have little influence on both the number of access and the time per access so we did not change the `init_array` function eventually.

There are three NUMA memory allocation policies, 'local', 'remote', 'interleave', respectively. We editted the `execute_numa.sh` to submit the code through these three policies. `numactl --cpunodebind=0 --membind=1 ./numa` correspond to 'remote' policy, `numactl --interleave=all ./numa` correspond to 'interleave' policy, `numactl --localalloc ./numa` corresponds to 'local' policy. The result is:

| # NUMA policy| Execution Time(s)| Speedup |
|:------------:|-----------------:|--------:|
|1             |31.64             |-        |
|2             |15.89             |1.991    |
|4             |8.057             |3.927    |
|8             |6.368             |4.969    |
|16            |4.766             |6.639    |


The problem with the basic algorithm is that to compute one value of output we need to access at least 4 cache lines (one for the output, one for the row `i-1`, `i` and `i+1`) in a fairly unpredictable way because there are `length` values between two cache lines. An optimization to this problem is separate the inner loop into three separate loops. Each of these loops accumulate the result of its calculation to the row. Now to compute one value of output the minimum of cache lines accessed is 2 and since we read only values in a sequential way this reduces the number of cache misses.

Since each iteration is approximatively of the same length we should be runing a static scheduler. However have found serious load imbalances while runing the program but could not find any solution to it. For example at 16 threads 5 of our threads where doing approximatively 47746256348164 rows of calculation while others did only 60636. We have tried various parameters for chunk size and tried the guided and dynamic scheduler with various results. Dynamic was sometimes faster, sometimes much worse than the static scheduler.

We could have put another optimization but chose not to since it is not related to memory. The basic algorithm performs too many if calls. We could optimize this by checking if `i == length/2 - 1 || i == length/2` at the begining of each iteration of the for loop with the `i` variable. If it is false, we can avoid all subsequent checks for `(j == length/2-1) || (j == length/2)` in this iteration of `i`. If it is false we need only to check for `(j == length/2-1) || (j == length/2)`.

## No Optimization

| # of threads | Execution Time(s)| Speedup |
|:------------:|-----------------:|--------:|
|1             |31.64             |-        |
|2             |15.89             |1.991    |
|4             |8.057             |3.927    |
|8             |6.368             |4.969    |
|16            |4.766             |6.639    |

We can see that we get an almost optimal speed up up to 4 threads, but that after that there is a significant decrease in performance per thread. This is because of a growing number of cache misses due to the larger amount of threads.

We can also see this in the graph:

![No_opt](theory.png)

## Optimization of cache

| # of threads | Execution Time(s)| Speedup |
|:------------:|-----------------:|--------:|
|1             |49.9              |-        |
|2             |26.12             |1.910    |
|4             |12.87             |3.877    |
|8             |6.614             |7.545    |
|16            |4.457             |11.196   |

We can see here that with a small amount of threads we actually have worse performance than with no optimization but as the number of thread scales, so does our speedup. At 16 threads we have almost double the speedup as without optimization.

The graph shows us that with this optimization we are clearly nearer the expected speedup scaling :

![cache](cache.png)

## Comparison Graph

![cache_vs_no_opt](Cache_vs_No_opt.png)
