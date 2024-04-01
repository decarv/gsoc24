/* 
 * Author: Henrique de Carvalho <decarv.henrique@gmail.com>
 */

#include <stdio.h>
#include <liburing.h>
#include <stdbool.h>

int main(void) 
{
    struct io_uring ring;
    struct io_uring_sqe *sqe;
    struct io_uring_cqe *cqe;

    io_uring_spawn();

    if (io_uring_queue_init(8, &ring, 0) < 0) {
        perror("io_uring_queue_init");
        return 1;
    }

    sqe = io_uring_get_sqe(&ring);
    if (!sqe) {
        perror("io_uring_get_sqe");
        return 1;
    }

    while (true) {

    }


    return 0;
}
