#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <memory.h>
#include <unistd.h>

using namespace std;

#define BUFFER_SIZE 1024

int main(int argc, char* argv[])
{
  cout<< "hello client!!" << endl;

  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if(0 >= sock)
  {
    cout<< "create socket error!! code: "<< sock <<endl;
    return -1;
  }

  struct sockaddr_in server_addr;
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(9999);
  server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

  int ret = connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
  if(0 > ret)
  {
    cout<< "connect error!! code: "<< ret << endl;
    return -1;
  }
  cout<< "connect success!!" <<endl;

  char recv_buffer[BUFFER_SIZE];
  char send_buffer[BUFFER_SIZE];

  //send(sock, "hello", 6, 0);

  while(fgets(send_buffer, sizeof(send_buffer), stdin) != NULL)
  {
    send(sock, send_buffer, sizeof(send_buffer), 0);
    if(0 == strcmp("exit\n", send_buffer))
      break;
    recv(sock, recv_buffer, sizeof(recv_buffer), 0);
    fputs(recv_buffer, stdout);

    memset(recv_buffer, 0, sizeof(recv_buffer));
    memset(send_buffer, 0, sizeof(send_buffer));
  }

  close(sock);

  return 0;
}
