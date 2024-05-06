#ifndef ASIO_AGENT_AMR_MIR_H
#define ASIO_AGENT_AMR_MIR_H

typedef struct{
  // epoll based fd
  int efd; 
  // Aperiodic events
  // pipe fd, for communication with epoll
  //fd_pair_t pipe; 
} asio_agent_amr_t;

void init_asio_agent_amr(asio_agent_amr_t* io);

void add_fd_asio_agent_amr(asio_agent_amr_t* io, int fd);

void rm_fd_asio_agent_amr(asio_agent_amr_t* io, int fd);

int create_timer_ms_asio_agent_amr(asio_agent_amr_t* io, long initial_ms, long interval_ms);

int event_asio_agent_amr(asio_agent_amr_t const* io);

#endif
