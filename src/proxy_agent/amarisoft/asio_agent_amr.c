#include <assert.h>                        // for assert
#include <errno.h>                         // for errno
#include <fcntl.h>                         // for fcntl, F_GETFL, F_SETFL
#include <stdio.h>                         // for NULL, fprintf, stderr
#include <string.h>                        // for strerror
#include <sys/epoll.h>                     // for epoll_event, epoll_ctl
#include <sys/time.h>                      // for CLOCK_MONOTONIC
#include <sys/timerfd.h>                   // for timerfd_create, timerfd_se...
#include <sys/types.h>                     // for time_t
#include <time.h>                          // for timespec,  itimerspec
#include <unistd.h>                        // for close
#include "asio_agent_amr.h"

static
void set_fd_non_blocking(int sfd)
{
  int flags = fcntl (sfd, F_GETFL, 0);  
  flags |= O_NONBLOCK;
  fcntl (sfd, F_SETFL, flags);
}

static
int init_epoll(void)
{
  const int flags = EPOLL_CLOEXEC; 
  const int efd = epoll_create1(flags);  
  assert(efd != -1);
  return efd;
}

void init_asio_agent_amr(asio_agent_amr_t* io)
{
  assert(io != NULL);

  io->efd = init_epoll();
  set_fd_non_blocking(io->efd);
}

void add_fd_asio_agent_amr(asio_agent_amr_t* io, int fd)
{
  assert(io != NULL);
  const int op = EPOLL_CTL_ADD;
  const epoll_data_t e_data = {.fd = fd};
  const int e_events = EPOLLIN; // | EPOLLET; // open for reading and edge triggered instead of level triggered
  struct epoll_event event = {.events = e_events, .data = e_data};
  int rc = epoll_ctl(io->efd, op, fd, &event);
  if(rc != 0)
    printf("Errno %d \n", errno);
  assert(rc != -1);
}

void rm_fd_asio_agent_amr(asio_agent_amr_t* io, int fd)
{
  assert(io != NULL);
  const int op = EPOLL_CTL_DEL;
  const epoll_data_t e_data = {.fd = fd};
  const int e_events = EPOLLIN; // open for reading
  struct epoll_event event = {.events = e_events, .data = e_data};
  int rc = epoll_ctl(io->efd, op, fd, &event);
  assert(rc != -1);
  rc = close(fd);
  assert(rc == 0);
}

int create_timer_ms_asio_agent_amr(asio_agent_amr_t* io, long initial_ms, long interval_ms)
{
  assert(io != NULL);
  assert(initial_ms > -1);
  assert(interval_ms > -1);

  // Create the timer
  const int clockid = CLOCK_MONOTONIC;
  const int flags = TFD_NONBLOCK | TFD_CLOEXEC;
  const int tfd = timerfd_create(clockid, flags);
  assert(tfd != -1);

  const time_t initial_sec = initial_ms / 1000;
  const long initial_nsec = (initial_ms * 1000000) % 1000000000;
  /* Initial expiration */
  const struct timespec it_value = {.tv_sec = initial_sec, .tv_nsec = initial_nsec};

  const time_t interval_sec = interval_ms / 1000;
  const long interval_nsec = (interval_ms * 1000000) % 1000000000;
  /* Interval for periodic timer */
  const struct timespec it_interval = {.tv_sec = interval_sec, .tv_nsec = interval_nsec};

  const int flags_2 = 0;
  struct itimerspec *old_value = NULL; // not interested in how the timer was previously configured
  const struct itimerspec new_value = {.it_interval = it_interval, .it_value = it_value};
  int rc = timerfd_settime(tfd, flags_2, &new_value, old_value);
  assert(rc != -1);

  add_fd_asio_agent_amr(io, tfd);
  return tfd;
}

int event_asio_agent_amr(asio_agent_amr_t const* io)
{
  assert(io != NULL);

  const int maxevents = 1;
  struct epoll_event events[maxevents];
  const int timeout_ms = 1000;

  const int events_ready = epoll_wait(io->efd, events, maxevents, timeout_ms);
  if (events_ready < 0) {
    fprintf(stderr, "epoll_wait() returned -1: errno %d, %s\n", errno, strerror(errno));
    assert(0);
  }
  assert(events_ready == 0 || events_ready == 1);
  if(events_ready == 0) 
    return -1;

  // Max. one event ready
  assert((events[0].events & EPOLLERR) == 0);
  return events[0].data.fd; 
}

