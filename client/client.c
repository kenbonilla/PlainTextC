/*
 */

/* 
 * File:   BonillaClient.c
 * Author: Kenneth Bonilla
 * 
 * TCP client for plaintext files. Will get single file, print contents,
 * save file to directory, then exit
 * Capable of dynamic file size!
 * default port is 8899
 * port to be entered as argument bonillaclient 8888
 *
 * Created on April 22, 2017, 5:00 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>


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

    
    int sockid=socket(AF_INET, SOCK_STREAM, 0); //create socket and return id
    int size, i;
    char *fl_recv;
    if(sockid < 0) { puts("Failed to open socket.\nExiting\n"); return 1;}
        //Fail if socket did not open
    
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
        //zero the structs
    
    //set server_addr struct
    server_addr.sin_family = AF_INET; //always AF_INET
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //localhost
    server_addr.sin_port = htons(port_id); //converts host to network short
    
    //connect to server
    if(connect(sockid, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        puts("Failed to connect.\nExiting\n");
        return 1;
    }
    
    printf("Connected to server on port:, %d\n", port_id);
    puts("Enter file name: ");
    char file_name[256];
    fgets(file_name, 256, stdin);
    file_name[strcspn(file_name,"\n")] = 0;
    
    if( send(sockid, file_name, strlen(file_name) , 0) < 0)
    {
        puts("Send failed.");
    }
    recv(sockid, &size, sizeof(size), 0);
    if(size == -1){
        puts("NO SUCH FILE\nExiting\n");
        return 1;
    }

    fl_recv = (char *) malloc(size * sizeof(char) );

    if(fl_recv == NULL){
            puts("Malloc error.\nExiting\n");
            return 1;
        }
    recv(sockid, fl_recv, size, 0);

    printf("Printing file:\n######\n%s\n######", fl_recv);
 
    FILE* file = fopen(file_name, "w");
    for(i=0;i<size;i++){
            fwrite(&fl_recv[i], sizeof(char), 1, file);
        }
    printf("\n\nFile saved to directory, closing client.\n");
    fclose(file);
    close(sockid);
    free(fl_recv);
    return (EXIT_SUCCESS);
}

