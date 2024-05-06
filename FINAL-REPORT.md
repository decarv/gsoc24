# GSoC 2024 Final Report: Implementing a Custom I/O Layer for pgagroal

**Author**: Henrique de Carvalho <decarv.henrique@gmail.com>
**Organization**: PostgreSQL
**Mentors**:
My-Wine-Cellar
Luca Ferrari

## Short Description

This project focused on replacing pgagroal's dependency on the unmaintained libev library with a custom, efficient I/O implementation.
For Linux, this meant implementing an event loop with io_uring and epoll.
For FreeBSD and Darwin, this meant implementing an event loop with kqueue.

This project lasted a total of 6 months, from May 1st to November 4th.

The planning and timeline can be found [here](https://github.com/agroal/pgagroal/discussions/442).

The pull request can be found [here](https://github.com/agroal/pgagroal/pull/456).

## What I did

Below are the main tasks I completed:

- Event Loop Implementation: Initial implementation of custom event loop using (a) io_uring, incorporating advanced features like multishot accept/receive and ring buffers; and (b) epoll. Implemented support for signals and conducted initial tests to verify functionality. Initial development branch can be found [here]().
- Custom libev Replacement: Created a custom version of libev, replicating its abstractions (ev_loop, ev_io, ev_periodic, ev_signal, etc.) to facilitate smoother integration with pgagroal's codebase. This involved significant refactoring to separate loop abstractions from watcher abstractions.
- Integration with pgagroal: Integrated the custom event loops into pgagroal's codebase, making necessary modifications to function signatures in the pipelines. This was a challenging process that required careful adjustments to ensure compatibility and maintain functionality.
- Debugging and Testing: Spent considerable time debugging issues related to accept/receive operations and client-server communication. Utilized packet capturing and testing with pgbench to identify and resolve issues. 
- FreeBSD and Darwin Support: Implemented the I/O foundation for FreeBSD and Darwin using kqueue. Adapted the event loop to FreeBSD's system specifics.
- Configuration Enhancements: Implemented configuration options for backend selection in pgagroal.conf, allowing the event backend to be dynamically set on startup to improve flexibility and ease of use for end-users. This required a lot of refactoring as before the event loop was selected during compilation. A change required me to assign a bunch of function pointers during startup.
- Continuous Integration (CI) Updates: Modified the CI configurations to accommodate the new implementations, ensuring that builds and tests run successfully on both Linux and Darwin platforms.
- Performance Benchmarking: Conducted performance evaluations using pgbench to compare the new I/O layers against the baseline. Has not been able to do this extensively yet, as the main focus is correctness.
- Improvements and Bug Fixing: Addressed feedback and issues reported by maintainers.
- Documentation: Documented the code, updated user guides, configuration references, and developer documentation.

## The current state

Here's where the project stands right now:

- Functionality: The custom I/O layer using io_uring, epoll, and kqueue is fully implemented but NOT YET integrated into pgagroal. I am working to fix all the bugs and make sure the system remains resilient.
- Performance: Initial performance benchmarks using pgbench show that the new I/O layers perform on par with or better than the previous implementation.
- Stability: The system is unstable and still has bugs present. The system has not yet been tested under heavy loads yet.

## What's left to do

There are still some important tasks to complete:

- Fix Bugs: I am expected to fix the bugs before this can be merged or the project can go on.
- Finalize Documentation: The architecture documentation is present, but could use some refinement. 
- Performance Optimization: Continue performance evaluations to identify any remaining bottlenecks. Optimize code based on findings to improve efficiency and reduce latency.
- Merge Pending Work: Merge the work into the main codebase.

## Challenges and important things I learned

Throughout this project, I faced several challenges and learned valuable lessons:

- Complex Integration: Integrating io_uring into pgagroal required significant changes to existing code structures, particularly the function signatures in the pipelines. This taught me the importance of designing flexible code architectures.
- Debugging Difficult Issues: Debugging issues with accept/receive operations and client-server communication was challenging. I learned advanced debugging techniques, including packet capturing and in-depth analysis of asynchronous I/O operations.
- Cross-Platform Development: Implementing kqueue for FreeBSD and Darwin introduced challenges due to differences in system APIs. This improved my understanding of cross-platform development and the need for testing on each target platform.
- Performance Considerations: Working with advanced an event loop highlighted the complexities of high-performance I/O operations.
- Importance of Collaboration: Engaging with the community, responding to feedback, and addressing reported issues emphasized the importance of collaboration in open-source development.
- Importance of Communication: Effective communication showed to be crucial throughout the project, especially when collaborating with the community and mentors. I need to improve on this.
- Time Management and Adaptability: Balancing the scope of the project with time constraints required careful planning and prioritization. It was very hard to keep that going.
- Importance of Early Testing: The challenges faced during integration highlighted the importance of early and continuous testing, which I did not do.

