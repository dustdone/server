#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>

#include<sys/epoll.h>
#include<fcntl.h>

#define MYPORT 9999
#define BUFFSIZE 1024

void set_nonblock(int fd)
{
  int fl = fcntl(fd, F_GETFL);
  fcntl(fd, F_SETFL, fl|O_NONBLOCK);
}

int startListen()
{
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if(0 > sock)
  {
    perror("sock");
    return -1;
  }

  int opt = 1;
  setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  struct sockaddr_in addr;
  addr.sin_port = htons(MYPORT);
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;

  if(-1 == bind(sock, (struct sockaddr*)&addr, sizeof(addr)))
  {
    perror("bind");
    return -1;
  }

  if(-1 == listen(sock, 10))
  {
    perror("listen");
    return -1;
  }

  return sock;
}

int main()
{
  int sockListen = startListen();
  if(-1 == sockListen)
  {
    return -1;
  }

  printf("start listen ...\n");

  int epfd = epoll_create(256);
  if(0 > epfd)
  {
    perror("epoll_create");
    exit(1);
  }

  struct epoll_event _ev;
  _ev.events = EPOLLIN;
  _ev.data.fd = sockListen;

  epoll_ctl(epfd, EPOLL_CTL_ADD, sockListen, &_ev);

  struct epoll_event revs[64];

  int timeout = -1;
  int num = 0;
  int done = 0;

  while(!done)
  {
    num = epoll_wait(epfd, revs, 64, timeout);
    if(-1 == num)
    {
      perror("epoll_wait");
      break;
    }
    else if(0 == num)
    {
      printf("timeout\n");
      break;
    }

    struct sockaddr_in peer;
    socklen_t len = sizeof(peer);

    for(int i=0; i<num; i++)
    {
      int sock = revs[i].data.fd;
      if(sock == sockListen && (revs[i].events) && EPOLLIN)
      {
        int new_fd = accept(sockListen, (struct sockaddr*)&peer, &len);
        if(0 > new_fd)
        {
          perror("new_fd");
          continue;
        }

        printf("new client connect %s:%d\n", inet_ntoa(peer.sin_addr), ntohs(peer.sin_port));

        set_nonblock(new_fd);
        _ev.events = EPOLLIN|EPOLLET;
        _ev.data.fd = new_fd;
        epoll_ctl(epfd, EPOLL_CTL_ADD, new_fd, &_ev);
      }
      else
      {
        if(revs[i].events & EPOLLIN)
        {
          char buf[1024];
          int _s = read(sock, buf, sizeof(buf)-1);
          if(0>_s)
          {
            perror("read");
            continue;
          }
          else if(0 == _s)
          {
            printf("client:%d close\n", sock);
            epoll_ctl(epfd, EPOLL_CTL_DEL, sock, NULL);
            close(sock);
          }
          else
          {
            printf("client:%s\n", buf);

            _ev.events = EPOLLOUT|EPOLLET;
            _ev.data.fd = sock;
            epoll_ctl(epfd, EPOLL_CTL_MOD, sock, &_ev);
          }
        }
        else if(revs[i].events&EPOLLOUT)
        {
          const char* msg = "Good, very nice\n";

          write(sock, msg, strlen(msg) + 1);
          epoll_ctl(epfd, EPOLL_CTL_MOD, sock, &_ev);
        }
      }
    }

  }


}

