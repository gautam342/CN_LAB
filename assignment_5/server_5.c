#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<ctype.h>

#define PORT 8081
#define BUFFER_SIZE 1024

int count_words(char *str){
    int count =0, in_word =0;

    while (*str){
if(*str==' ' || *str =='\n' || "\t"){
in_word=0;
}
else if(in_word==0){
    in_word=1;
    count++;
}
str++;
    }
    return count;
}

int count_vowels(char *str){
    int count =0;
while(*str){
    char c= tolower(*str);
    if(c=='a' || c=='e' || c=='i' || c=='o' || c=='u'){
     count ++;
    }
    str++;
}

    return count;
}

int main(){

    int server_fd , new_socket;
    struct sockaddr_in address;
    char buffer[BUFFER_SIZE];

//create socket

  server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)
    {
        perror("Socket failed");
        exit(1);
    }



// intialize the value 

 
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);




// write bind syntax
 if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("Bind failed");
        exit(1);
    }


// write listen 

if (listen(server_fd, 3) < 0)
    {
        perror("Listen failed");
        exit(1);
    }

    printf("Waiting for client...\n");

// write accept function call

new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
    if (new_socket < 0)
    {
        perror("Accept failed");
        exit(1);
    }



while(1){
    memset(buffer , 0 , BUFFER_SIZE);
    int valread = read(new_socket,buffer,BUFFER_SIZE);

    if(valread <=0) break;

    buffer[strcspn(buffer , "\n")]=0;
    if(strcmp(buffer , "bye")==0) break;

    char command[20], text[BUFFER_SIZE];
    scanf(buffer , "%[-1]%[\n]",command,text);

    if(strcmp(command ,"ANALYZE")==0){
        int chars =strlen(text);
        int words =count_words(text);
        int vowels = count_vowels(text);

        char response[BUFFER_SIZE];
        sprintf(response , "Chars=%d,words=%d,vowels=%d",chars,words,vowels);

        send(new_socket,response,strlen(response),0);

    }
    else{
        char *msg="Invalid Command";
        send(new_socket,msg,strlen(msg),0);
    }
}

close(new_socket);
close(server_fd);
return 0;

}
