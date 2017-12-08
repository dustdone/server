#include<sys/time.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<string.h>
#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>

#define MYPORT 9999
#define BACKLOG 10
#define BUF_SIZE 128

int fd_A[BACKLOG];
int conn_index;

void showClient()
{
  int i;
  printf("client index: %d \n", conn_index);
  for(int i=0; i<BACKLOG; i++)
    printf("[%d]:%d \n", i, fd_A[i]);

  printf("\n\n");
}



int main()
{
  int sock_fd, new_fd;
  struct sockaddr_in server_addr;
  struct sockaddr_in client_addr;
  socklen_t sin_size;
  char buf[BUF_SIZE];
  int yes = 1;
  int ret;
  int i;

  if((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
  {
    perror("socket");
    exit(1);
  }

  // setsockopt
  if(setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
  {
    perror("setsockopt");
    exit(1);
  }

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(MYPORT);
  server_addr.sin_addr.s_addr = INADDR_ANY;
  memset(server_addr.sin_zero, '\0', sizeof(server_addr.sin_zero));

  if(bind(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
  {
    perror("bind");
    exit(1);
  }

  if(listen(sock_fd, BACKLOG) == -1)
  {
    perror("listen");
    exit(1);
  }

  printf("listen port:%d\n", MYPORT);

  fd_set fdsr;
  int maxsock;
  struct timeval tv;

  conn_index = 0;
  sin_size = sizeof(client_addr);
  maxsock = sock_fd;

  while(true)
  {
    FD_ZERO(&fdsr);
    FD_SET(sock_fd, &fdsr);

    tv.tv_sec = 20;
    tv.tv_usec = 0;

    for(i=0; i<BACKLOG; i++)
    {
      if(fd_A[i] != 0)
        FD_SET(fd_A[i], &fdsr);
    }

    ret = select(maxsock + 1, &fdsr, NULL, NULL, NULL);
    if(0>ret)
    {
      perror("select");
      break;
    }
    else if(0 == ret)
    {
      printf("timeout\n");
      continue;
    }

    // check every fd in the set
    for(i=0; i<conn_index; i++)
    {
      if(FD_ISSET(fd_A[i], &fdsr))
      {
        if((ret = recv(fd_A[i], buf, sizeof(buf), 0)) <= 0)
        {
          printf("client[%d] close\n", i);
          close(fd_A[i]);
          FD_CLR(fd_A[i], &fdsr);
          fd_A[i] = 0;
        }
        else
        {
          printf("client[%d]:%s\n", i, buf);
          char str[] = "Good, very nice";
          send(fd_A[i], str, sizeof(str) + 1, 0);
          memset(buf, 0, sizeof(buf));
        }
      }
    }

    // check new connection comes
    if(FD_ISSET(sock_fd, &fdsr))
    {
      new_fd = accept(sock_fd, (struct sockaddr*)&client_addr, &sin_size);
      if(0 >= new_fd)
      {
        perror("accept");
        continue;
      }

      // add to fd queue
      if(BACKLOG > conn_index)
      {
        fd_A[conn_index++] = new_fd;
        printf("new connection client[%d] %s:%d\n", conn_index, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        if(new_fd > maxsock)
          maxsock = new_fd;
      }
      else
      {
        printf("max connections arrive, exit\n");
        send(new_fd, "bye", 4, 0);
        close(new_fd);
        break;
      }
    }
    //showClient();
  }

  // close all connections
  for(i=0; i<BACKLOG; i++)
  {
    if(fd_A[i] !=0) close(fd_A[i]);
  }

  return 0;
}

