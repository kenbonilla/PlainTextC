/*
 * 
 */

/* 
 * File:   server.c
 * Author: Kenneth Bonilla
 *
 * Created on April 22, 2017, 3:29 PM
 * 
 * Multi threaded TCP server, sends plaintext files in same directory.
 * Quit program by typing 'q' enter, WARNING: do not exit with active connections
 * 'c' enter to see how many clients are on the server
 * Port must be entered as argument ./server 8888
 * default port is 8899
 * Capable of handling multiple active connections.
 * Capable of dynamic file size
 * file name limited to 256 char
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define CONS 5

//////////////////////////////

typedef struct {
    int flag;
}arg_run;


int cl_count = 0;
int sockid;


//////////////////////////////

//reads the message to the client
void* file_runner(void* arg){
    int client_sock = *(int*)arg;
    int fl_nm_sz;
    int size;
    int i;
    char file_name[256];
    char *cl_message;
    
    
    
    //Receive file request
    while( (fl_nm_sz = recv(client_sock, file_name, 256, 0)) > 0)
    {
        printf("Search for %s\n", file_name);
        FILE* file =fopen(file_name, "r");
        if(file == NULL){
            puts("NOT FOUND");
            size = -1; //flag to terminate client file not found
            write(client_sock, &size, sizeof(size)); //tell client file not found
            
            
            break; //if file not found, escape
        }
        fseek(file, 0, SEEK_END);
        size = ftell(file);
        rewind(file);
        

        cl_message = (char*) malloc(size * sizeof(char));
        
        //READ FILE
        for(i=0;i<size;i++){
            fread(&cl_message[i], sizeof(char), 1, file);
        }

        
        if(cl_message == NULL){
            puts("Malloc error.");
            fclose(file);

            break; //if unable to cast malloc, close file and end loop
        }

        
        //Sends client size of file so client can malloc appropriate space
        write(client_sock, &size, sizeof(size));
        //Send client file
        write(client_sock, cl_message, size);
        puts("File sent");
        free(cl_message);
    }
    close(client_sock);
    cl_count--;
    pthread_exit(0);
}

//Handles stdin functions for server
void* exit_runner(void *arg){
    arg_run *args = (arg_run*) arg;
    char buf[32] = {0};

    
    puts("Type 'q' enter to exit program.");
    puts("Type 'c' enter to show number of clients.");
    while(buf[0] != 'q'){
        fgets(buf, sizeof(buf),stdin);
        if(buf[0] == 'c'){
            printf("Clients connected: %d\n", cl_count);
        }

    }
    close(sockid);
    exit(EXIT_SUCCESS);
    pthread_exit(0);
}

/*
 * 
 */
int main(int argc, char** argv) {
    int port_id;
    if(argc < 2) { 
        printf("Using default port\n"); 
        port_id = 8899;
    }else{
        port_id = atoi(argv[1]);} //get port number from arguments
    sockid=socket(AF_INET, SOCK_STREAM, 0); //create socket and return id
    
    
    if(sockid < 0) { puts("Failed to open socket.\nExiting\n"); return 1;}
        //Fail if socket did not open
    int client_sock, fl_nm_sz, size, i;
    
    

    
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;
    client_len = sizeof(client_addr);
    memset(&server_addr, 0, sizeof(server_addr));
        //zero the struct
    
    //set server_addr struct
    server_addr.sin_family = AF_INET; //always AF_INET
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //localhost
    //server_addr.sin_addr.s_addr = htons(INADDR_ANY); //all/any interfaces
    server_addr.sin_port = htons(port_id); //converts host to network short
    //puts("struct built");
    
    //bind returns 0 if success, -1 if fail
    if((bind(sockid,(struct sockaddr *)&server_addr , sizeof(server_addr))) < 0)
        { puts("Failed to bind.\nExiting\n"); return 1; }
    //puts("socket bind");
    
    listen(sockid, CONS);
    printf("Listening on port: %d\t...\twaiting for connections...\n", port_id);
    

    
    pthread_t tids;
    pthread_t liner;
    arg_run argg;
    argg.flag=0;
    pthread_create(&liner, NULL, exit_runner, &argg);
    pthread_detach(liner);
    while(1){
        
        client_sock = accept(sockid, (struct sockaddr *)&client_addr, &client_len);
        if(client_sock<0) 
        {
            puts("Accept Failed.\nExiting\n"); 
            //return 1;
            continue;
        }
        cl_count++;
        printf("Client connected: %d\n", cl_count);
        

        pthread_attr_t attr;
        pthread_attr_init(&attr);
        //puts("starting thread");
        pthread_create(&tids, &attr, file_runner, &client_sock);
        //puts("detaching thread");
        pthread_detach(tids);
        

    }
    close(client_sock);
    close(sockid);
    
    return (EXIT_SUCCESS);
}

