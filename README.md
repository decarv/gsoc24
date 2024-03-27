# GSoC Proposal - PostgreSQL: pgagroal

## 0. Contributor Information

**Name:** Henrique A. de Carvalho<br>
**Email:** [decarv.henrique@gmail.com](mailto:decarv.henrique@gmail.com)<br>
**GitHub:** [decarv](https://github.com/decarv)<br>
**Languages:** Portuguese, English<br>
**Country:** Brazil (GMT -03:00)<br>
**Who Am I:** I earned a Bachelor's degree in Computer Science from the University of São Paulo (2020 - 2023) and currently work as a Backend Software Engineer at a medium-scale Investment Fund. Throughout my bachelor's program, I did bunch of different stuff, from algorithms and data structures, to systems engineering, to machine learning theory and applications, computer vision, computer graphics and application development and benchmarking. Nevertheless, the things that live in my heart are systems engineering, performance-oriented development and computer graphics, which are things I rarely have the opportunity to do. Therefore, I see contributing to open-source as a great way to stay close to what makes me happy in software engineering and, of course, to be able to provide good software for free to other humans. Specifically, my interest in pgagroal stems from its system program nature and its promise of high-performance — subjects I'm passionate about, especially when it involves optimizing for speed (not that I know that much, it's just that I want to learn about it that much). My main objective with this project is to learn from the community and assist pgagroal in achieving unparalleled performance among connection pools.<br>
**Contributions:** [#408](https://github.com/agroal/pgagroal/pull/408), [#411](https://github.com/agroal/pgagroal/pull/411), [#427](https://github.com/agroal/pgagroal/pull/427)<br>

## 1. Synopsis

This project consists of replacing the I/O Layer of pgagroal, today highly dependant on [libev library](TODO), for a pgagroal's own implementation of this I/O Layer. The so called I/O Layer consists of an event loop (ev) abstracted by libev. 

The motivation behind this project is because libev is not being maintained any longer. Therefore we need an efficient (i.e. maintainable, reliable, fast, lightweight, secure and scalable) implementation of an ev that can be maintained by the pgagroal community.

Currently, pgagroal depends on libev to (a) watch for incomming read/write requests from its connections in a non-blocking fashion (I/O multiplexing); (b) launch timer events; and (c) watch signals.

I/O multiplexing in Linux is done with select, poll and, most recently (kernel version 2.6) with epoll, which is used by libev on the background. epoll works by decoupling the monitor registration from the actual monitoring, and does this with an intuitive API, using three simple system calls, one to create an epoll instance, one to add or remove file descriptors to monitor (along with the events to monitor in each file descriptor), and one to actually monitor the file descriptors.

A successful implementation of an efficient ev for pgagroal necessarilly utilizes epoll as well as other linux I/O features for efficient I/O.

The objective of this proposal is to provide a plan to achieve such implementation, which shall be, at the end of this program, at least as efficient as libev, but fully maintained and controlled by the pgagroal community.

## 2. Proposal

As mentioned before, my objectives with this proposal is to implement an efficient ev for pgagroal. 

In order to accomplish this, I could benefit of dividing the implementation into two phases: (a) Experimentation (Phase 1); (b) Continuous Implementation and Profiling Loop (Phase 2).

For the **Phase 1**, I propose the implementation of ev.h and ev.c containing a simple abstraction for an ev (with `epoll`) that closely follows (in a first moment) the interface used by pgagroal with libev. 

This would allow the main code to be changed only slightly, where it makes sense, avoiding unnecessaryly redesigning the main code and understanding where I would have to modify it in order to connect it to an ev interface. 

The result of this first phase would be a small footprint ev that suffices for pgagroal specific uses, resulting in minimal changes in function signatures and behavior of the main code. 

For the **Phase 2**, I first propose the definition of tests and profiling (for speed and memory) for pgagroal's new ev, done in different settings, enabling comparison between previous and future versions. 

The idea here is to acurately measure resource utilisation for pgagroal in areas we believe are important for a connection pool, so that we can make sure that pgagroal is going on the direction we believe is correct. 

This will enable the identification of bottlenecks and places where pgagroal can benefit from optimizations while being able to measure potential optimizations implementations. 

The specific benchmark criteria (performance metrics) should be discussed with the community prior to the development of the strategy.

Second, I propose diving deeper into improvements that could be made to the simple ev implementation of the previous phase, here I intend to investigate the potential necessary changes of structure of the main code, considering (a) the usage of io\_uring, (b) the different configurations for epoll (e.g. edge vs. level trigger), (c) cache performance, (d) optimizations with memory layout, (e) reducing system calls, (f) vectorization of read and writes.

The tests and the profiling should lead the development after each commit.


## . EV Implementation Details

pgagroal uses a default main loop configuration for libev, which uses epoll and io\_uring where needed. 

This main loop is fed by pgagroal main code with main file descriptors, file descriptor for management and file descriptor for postgres server. 

For each monitored file descriptor, there is a callback defined by pgagroal and called by the event loop.

None of the callbacks should be modified at a first glance, and the new implementation should seemlessly allow for keeping the original structure.

Therefore, the **Phase 1** implementation depends on me knowing where libev functions are used throughout the code and replacing them with our own implementation.

For example, when a connection is accepted through pgagroal the `accept_main_cb` callback is called to register a client. At this point, the 


## . Testing and Profiling Implementation Details

With testing and profiling I intend to achieve a way to measure how the implementation of the ev is evolving in comparison to previous pgagroal versions and to previous versions.

Tests could be achieved through testing frameworks in C or just by testing the behaviour with simulated postgres client connections as shell scripts.

Profiling could be achieved through the usage of linux tools such as gprof and perf. These tools could be wrapped around Python scripts to enable easy data parsing, storage and analysis.

## . Deliverables

**Phase 1**
- Fully functional event loop 

**Phase 2**
- Testing and Profiling Strategy and Setup
- Investigation of Optimizations


## . Timeline

Below I set a timeline for 22 weeks.

| Week       | Date             | Description |
|------------|------------------|-------------|
| 1 & 2      | May 01 - May 12  | This is a community bonding period, we could set up a call to know each other, to talk about pgagroal (the history behind it and the future of the project) and to talk about this project in specific. This is likely the start of Phase 1. |
| 3 & 4      | May 13 - May 27  | Work on the first version of the ev. |
| 4 & 5      | May 27 - June 09 | Deliver the first version of an ev, fully working. |
| 6 & 7 & 8  | June 10 - June 30 | Work on testing and profiling strategy and setup. |
|            | July 01 - July 14 | The midterm evaluations, from 8 to 12. I intend to visit my family during the mid-year holidays. I will be reachable and responding to emails, but I intend to mostly rest around this time. |
| 9 & 10    | July 15 - July 28 | Investigation of improvements to the ev. |
| 11 & 12 & 13   | July 29 - August 18 | |
|            | August 19 - September 1   | |
| 14 & 15   | September 2 - September 15 | |
| 16 & 17    | September 16 - September 29 | |
| 18 & 19    | September 30 - October 6 | |
| 20 & 21 & 22   | October 7 - November 3 | |
|            | November 4 | Deadline to submit final work product and final evaluation. |

