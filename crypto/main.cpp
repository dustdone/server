#include<iostream>
#include<string.h>
#include<stdio.h>

//extern "C"
#include "hmac.h"
#include "Base64.h"

int main()
{
  std::string upyunpw = "ccef9815ffd3fddbbab7057c1b85f48b";
  //std::string sign = "POST&/ro-xdcdn&eyJidWNrZXQiOiJyby14ZGNkbiIsImV4cGlyYXRpb24iOjE1MTQ0NDY1NDgsInNhdmUta2V5IjoiL2RlYnVnL2ljb24vMTAwMjQvZ3VpbGQvNi83LmpwZyIsImNvbnRlbnQtbGVudGgtcmFuZ2UiOiIwLDEwMjQwMDAifQ==";
  std::string sign = "POST&/ro-xdcdn&eyJidWNrZXQiOiJyby14ZGNkbiIsImV4cGlyYXRpb24iOjE1MTQ0NDg5MjAsInNhdmUta2V5IjoiL2RlYnVnL2ljb24vMTAwMjQvZ3VpbGQvNi8yNy5qcGciLCJjb250ZW50LWxlbnRoLXJhbmdlIjoiMCwxMDI0MDAwIn0=";
  unsigned char sha1[41];
  bzero(sha1, sizeof(sha1));

  hmac_sha1(upyunpw.c_str(), upyunpw.size(), sign.c_str(), sign.size(), sha1);

  for(int i=0; i<20; i++)
  {
    printf("%d,", sha1[i]);
  }

  std::cout<<std::endl;

  std::cout<< "sha1: " << sha1 << std::endl;

  char sign_out[100];
  base64_encode(sha1, sign_out, 20);

  std::cout<<"sign_out: "<< sign_out <<std::endl;

  return 0;
}
