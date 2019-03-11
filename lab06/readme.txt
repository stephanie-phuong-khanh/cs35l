At first, the code was slightly overwhelming to understand because
there were many helper functions that played specific roles in the
drawing process but were not necessary to consider for multithreading.
The main function's comment "for every pixel" was very helpful, showing
that the for loop would be run for every pixel, a great candidate for
multithreading, so that each thread can handle certain pixels. 

The first thing I did was add the -lpthread flag in the Makefile's LDLIBS 
variable. Since pthread_create must be passed a function, so I moved the 
for loop to its own function, rayTrace. I declared an array of threads, 
size of nthreads, the argument passed to the program specifying the number
of threads to run. In a for loop, each thread is passed rayTrace, which is
passed the thread ID, called thread_index. The rayTrace function executes
the main "drawing" loop for "columns" of the image that are multiples of
nthread plus the thread_index (for example, thread_index=0 and nthreads=8
means the loop runs for px=0,8,16,24,...). Each loop iteration will "draw"
the image by rewriting a float in the 3d array that holds the rgb value for
that pixel. This array of pixels I made global because all threads have
to access and modify it. I also made global the scene, nthreads, and
constants (previously declared in the main function) like camera_pos,
camera_dir, and camera_fov so that all threads can access them. 

Like was mentioned in the TA session, we prevent race conditions of the
main function finishing before the threads finishing their tasks by using
pthread_join, making the main process wait for each thread to finish their
tasks, then print the scene and finish the program.

For a while, I kept getting output from the diff command, meaning my outputted
scene was different from the original. I was passing the pthread function an
integer (the thread_index), which was wrong because I need to pass pthread a
pointer (void* is like an untyped pointer) which would have to be cast into the
correct type within the thread function, here into an int. In the main function,
I cast i (an int iterator in the for loop) into void* and passed it to rayTrace,
which cast i back into an int for usage. This fixed the problem.

Implementing threading improved the performance like so:

time ./srt 1-test.ppm >1-test.ppm.tmp
real	0m46.575s
user	0m46.557s
sys	    0m0.007s

time ./srt 2-test.ppm >2-test.ppm.tmp
real	0m22.952s
user	0m45.532s
sys	    0m0.011s

time ./srt 4-test.ppm >4-test.ppm.tmp
real	0m11.222s
user	0m44.541s
sys	    0m0.006s

time ./srt 8-test.ppm >8-test.ppm.tmp
real	0m5.840s
user	0m44.805s
sys	    0m0.003s

Increasing the thread number speeds up the program greatly, reducing the
runtime from 46.6 seconds with 1 thread to 5.8 seconds with 8 threads. Doubling
the thread number halves the runtime showing a rough linear decrease between
thread count and runtime. User time barely increased, and system time fluctuated
with no recognizable trend, probably because the same task was executed, so
the CPU did the same amount of work. The slight increase in user time can
possibly be attributed to the small overhead to create and clean up threads.