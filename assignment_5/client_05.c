#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>

#define PORT 8081
#define BUFFER_SIZE 1024

int main(){
    int sock;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE];
    char input[BUFFER_SIZE];

    // create socket

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port=htons(PORT);

    inet_pton(AF_INET , "127.0.0.1",&serv_addr.sin_addr);
    connect(sock,(struct sockaddr *)&serv_addr,sizeof(serv_addr));
   
   if(connect(sock ,(struct sockaddr *)&serv_addr ,sizeof(serv_addr))<0){
    printf("connection fail "); return -1;

   }

   printf("connect to server: ");

while(1){
    printf("enter the request : ");
    fgets(input,BUFFER_SIZE,stdin);
    input[sock,input,strlen(input),0];

    send(sock,input,strlen(input),0);
    if(strcmp(input ,"bye")==0) break;

    memset(buffer,0,BUFFER_SIZE);
    read(sock,buffer,BUFFER_SIZE);
    printf("server response: %s\n",buffer);

}

    close(sock);
    return 0;
}
