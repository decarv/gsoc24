# GSoC Proposal - PostgreSQL: pgagroal

## 0. Contributor Information

**Name:** Henrique A. de Carvalho<br>
**Email:** [decarv.henrique@gmail.com](mailto:decarv.henrique@gmail.com)<br>
**GitHub:** [decarv](https://github.com/decarv)<br>
**Languages:** Portuguese, English<br>
**Country:** Brazil (GMT -03:00)<br>
**Who Am I:** I earned a Bachelor's degree in Computer Science from the University of São Paulo (2020 - 2023) and currently work as a Backend Software Engineer at a medium-scale Investment Fund. Throughout my bachelor's program, I did bunch of different stuff, from algorithms and data structures, to systems engineering, to machine learning theory and applications, computer vision, computer graphics and application development and benchmarking. Nevertheless, the things that live in my heart are systems engineering, performance-oriented development and computer graphics, which are things I rarely have the opportunity to do. Therefore, I see contributing to open-source as a great way to stay close to what makes me happy in software engineering and, of course, to be able to provide good software for free to other humans. Specifically, my interest in pgagroal stems from its system program nature and its promise of high-performance — subjects I'm passionate about, especially when it involves optimizing for speed (not that I know that much, it's just that I want to learn about it that much). My main objective with this project is to learn from the community and assist pgagroal in achieving unparalleled performance among connection pools.<br>
**Contributions:** [#408](https://github.com/agroal/pgagroal/pull/408), [#411](https://github.com/agroal/pgagroal/pull/411), [#427](https://github.com/agroal/pgagroal/pull/427), [#431](https://github.com/agroal/pgagroal/pull/431)<br>

## 1. Synopsis

The project consists of replacing the I/O Layer of pgagroal, today highly dependant on [libev library](TODO), for a pgagroal's own implementation of this I/O Layer. The so called I/O Layer is an event loop (ev) abstracted by libev. 

The motivation behind this project is because libev is not being maintained any longer. Therefore pgagroal needs an efficient (i.e. maintainable, reliable, fast, lightweight, secure and scalable) implementation of an ev that can be maintained by the pgagroal community.

Currently, pgagroal depends on libev to (a) watch for incomming read/write requests from its connections in a non-blocking fashion (I/O multiplexing); (b) launch timer events; and (c) watch signals.

In Linux, these functionalities may be optimally achieved by using io\_uring (feature introduced in Linux kernel 5.1). io\_uring is a communication channel between a system's application and the kernel by providing an interface to receive notifications when I/O is possible on file descriptors. io\_uring is accessible to system applications through liburing, which is a library that contains helpers for setup of io\_uring. A successful Linux implementation of an efficient ev for pgagroal necessarilly utilizes io\_uring -- as well as other Linux I/O interfaces (e.g. stdio) -- for efficient I/O. For cases where io\_uring may fall short, Linux has other interfaces that may replace it, such as epoll.

In FreeBSD, these functionalities may be optimally achieved by using kqueue. [COMPLETE]


The objective of this proposal is to provide a plan to achieve such implementation, which shall be, at the end of this program, at least as efficient as libev, but fully maintained and controlled by the pgagroal community.

## 2. Proposal

As mentioned before, my objectives with this proposal is to implement an efficient ev for pgagroal. 

In order to accomplish this, I could benefit of dividing the implementation into two phases: (a) Experimentation (Phase 1); (b) Continuous Implementation and Profiling (Phase 2).

For the *Phase 1*, I propose the implementation of io.h containing a simple abstraction for an ev (with io\_uring, for Linux, and with kqueue, for FreeBSD). 

The result of this first phase would be a maintainable and small footprint ev that suffices for pgagroal specific uses, potentially (but not necessarily) resulting in minimal changes in functions and behavior of the main code -- as io.h would work as an interface for watching file descriptors, timers and signals.

For the *Phase 2*, I propose the definition of tests and profiling (for speed and memory) for pgagroal's new ev, done in different settings, enabling comparison between previous and future versions. 

The idea here is to acurately measure resource utilization for pgagroal in areas we believe are important, so that we can make sure that the new implementation of pgagroal is actually going on the right direction.

The specific benchmark criteria (performance metrics) should be discussed with the community prior to the development of the strategy, but this should include attempts to measure latency, throughput, CPU usage, and memory footprint.

Measuring resource utilization will enable the identification of bottlenecks and places where pgagroal can benefit from optimizations while enabling to measure potential optimizations implementations. 

The measurements made in propose diving deeper into improvements that could be made to the simple ev implementation of the previous phase. Here I intend to investigate the potential necessary changes of structure of the main code, considering other optimizations (e.g. `io_uring_sqe` fields, kernel side polling, cache performance, memory layout, reducing system calls, vectorization).


### 2.1. Phase 1 Details

The Phase 1 implementation depends on me knowing how io\_uring and kqueue work, and me knowing how libev functions are used throughout the code, so I know how they be replaced.

#### 2.1.1. Linux implementation

io\_uring is a communication channel between the application and the kernel. It works by placing submission events and completion events into two queues (ring buffers).
These two queues are the submission queue and a completion queue, and they function as an asynchronous I/O interface.

Much of the complexity of managing these data structures is abstracted by a library called liburing.

Ref.: https://unixism.net/loti/tutorial/sq_poll.html


```c


```

As for signals, `io_uring_enter` receives a set of signals `sigset_t *sigset`, which makes it an easy . [TODO: CONFIRMAR]


#### 2.1.2. FreeBSD Implementation

[todo: complete]


#### 2.1.3. What will io\_uring and kqueue replace

First, pgagroal calls `ev_default_loop` to set the `struct ev_loop* main_loop` with a configuration set by `pgagroal_libev`. This functionn reads the configuration and decides the backend option that will be passed to `ev_default_loop`, which is one of the I/O multiplexing options available to Linux. Depending on configuration engine set in pgagroal's config. However, this step is not necessary as only epoll and/or io\_uring will be used.

This main loop is fed by pgagroal main code with signals watchers, main file descriptors, file descriptor for management, file descriptor for postgres server, file descriptors for metrics, and with timer watchers. 

For each monitored file descriptor, there is a callback defined by pgagroal and called by the event loop.

The callbacks should be only slightly modified at a first glance, and the new implementation should seemlessly allow for keeping the original callback structure.

This means that implementation should be replace libev structs and functions.


```c
struct ev_io;
void ev_io_init();
void ev_loop();
```
These work by abstracting the whole ev layer.

**Linux implementation.**
The implementation is straightforward, with `io_uring_wait_cqe` wrapped on a loop and a call to the `cqe->user_data` that can potentially hold a callback.
Every process should have it's own loop and the loop could be created as soon as the process is forked.



```c
struct ev_periodic;
void ev_periodic_init();
void ev_periodic_start();
```
These struct and functions work by issuing periodic callbacks in regular intervals [todo complete].

**Linux implementation.** 
These could be accomplished with timeout commands, with `io_uring_prep_timeout`, but this approach should be further evaluated. 
The upside here is to keep the simplicity in implementation with io\_uring.
Since everything is going to be wrapped around a while loop and I/O will wait on cqes, timeouts can be abstracted to work as periodic task (isn't this all they are anyways?).

**FreeBSD implementation.**
[todo complete]


```c
void ev_fork_loop();
```
This function is called whenever a fork() call is made, so the ev is duplicated.
With io\_uring there is no need to duplicate the ev, so this call will not exist anymore.
#### 2.1.2. FreeBSD implementation

[TODO]


### 2.2. Phase 2 Details

With testing and profiling I intend to achieve a way to measure how the implementation of the ev is evolving in comparison to previous pgagroal versions and to previous versions.

Tests could be achieved through testing frameworks in C or just by testing the behaviour with simulated postgres client connections as shell scripts.

Profiling could be achieved through the usage of linux tools such as gprof and perf. These tools could be wrapped around Python scripts to enable easy data parsing, storage and analysis.

Further, this profiling should render insights for the following optimizations.

#### 2.2.1. Definition of a testsuite

[TODO]

#### 2.2.2. Profiling the code
The timeout command supp
[TODO]

#### 2.2.c. Further optimizations

[TODO]

## 3. Deliverables

**Phase 1**
Week X: Basic I/O foundation
Week Y: io_uring - Successful calls
Week Z: io_uring - Failed calls
[TODO]

**Phase 2**
[TODO]

## 4. Timeline [TBD]

Below I set a timeline for 22 weeks.

| Week       | Date             | Description |
|------------|------------------|-------------|
| 1 & 2      | May 01 - May 12  | This is a community bonding period, we could set up a call to know each other, to talk about pgagroal (the history behind it and the future of the project) and to talk about this project in specific. This is likely the start of Phase 1. |
| 3 & 4      | May 13 - May 27  | Phase 1: [TODO]  |
| 4 & 5      | May 27 - June 09 | Phase 1: [TODO] |
| 6 & 7 & 8  | June 10 - June 30 | Phase 1: [TODO] |
|            | July 01 - July 14 | The midterm evaluations, from 8 to 12. I intend to visit my family during the mid-year holidays. I will be reachable and responding to emails, but I intend to mostly rest around this time. |
| 9 & 10    | July 15 - July 28 | Phase 2: [TODO] |
| 11 & 12 & 13   | July 29 - August 18 | Phase 2: [TODO] |
|            | August 19 - September 1   | Phase 2: [TODO] |
| 14 & 15   | September 2 - September 15 | Phase 2: [TODO] |
| 16 & 17    | September 16 - September 29 | Phase 2: [TODO] |
| 18 & 19    | September 30 - October 6 | Phase 2: [TODO] |
| 20 & 21 & 22   | October 7 - November 3 | Phase 2: [TODO] |
|            | November 4 | Deadline to submit final work product and final evaluation. |

