
#include <stdio.h>
#include <stdlib.h>

#include <errno.h>

#include <unistd.h>

#include <string.h>

#include <sys/wait.h>

#include <sys/socket.h>
#include <ctype.h>

#include <resolv.h>

#include <arpa/inet.h>

#include <pthread.h>



/* Definations */

#define DEFAULT_BUFLEN 1024

#define PORT 1888
int tempPort;




void PANIC(char* msg);

#define PANIC(msg)  { perror(msg); exit(-1); }





/--------------------------------------------------------------------/

/--- Child - echo server                                         ---/

/--------------------------------------------------------------------/

void doStuffs(int client, char* command, int* bytes_read){
int i;
    if(strcmp(command,"QUIT\n") == 0 ||strcmp(command,"QUIT") == 0 ){
            send(client,"Goodbye!\n", 9, 0);
            close(client);
            *bytes_read = 0;
            
            return;
        }
    

    else if(strcmp(command,"HELP\n") == 0){
        char*c2 = "Command Server commands are\nECHO text : This is echo text message back\nRECHO text : This is echo text message in reverse order\n";
        char* c = "SUM A B :This is sum A + B and print result\nSUB A B :This is subtract B from A and print result\nMUL A B : This is multiply A and B and print result\nDIV A B :This is divide A and B and print result\n.\n";
            send(client,c2, strlen(c2), 0);
            send(client, c, strlen(c), 0);  
            return;
        }
    else{
        char* arg1st = strtok(command, " \n");
      
        if(strcmp(arg1st, "ECHO")==0){
            char* dataToSend = strtok(NULL, "\n");
            strcat(dataToSend, "\n");
            send(client, dataToSend, strlen(dataToSend), 0);
        }
        else if(strcmp(arg1st, "RECHO")==0){
            char* s = strtok(NULL, "\n");
            size_t l = strlen(s);
            char* dataToSend = (char*)malloc((l + 1) * sizeof(char));
            dataToSend[l] = '\0';
            
            for(i = 0; i < l; i++) {
                dataToSend[i] = s[l - 1 - i];
            }
            strcat(dataToSend, "\n");
            send(client, dataToSend, l+1, 0);
            free(dataToSend);
        }
        else if(strcmp(arg1st, "SUM")==0){
            int firsNUm = atoi(strtok(NULL, " "));
            int secondNum = atoi(strtok(NULL, " \n"));
            int sum = firsNUm+secondNum;
            char dataToSend[20];
            sprintf(dataToSend, "%d", sum);
            int l=0;
            for(i=0;dataToSend[i]!='\0';i++){
                l++;
            }
            strcat(dataToSend, "\n");
            send(client, dataToSend, strlen(dataToSend), 0);
        }
        else if(strcmp(arg1st, "SUB")==0){
            int firsNUm = atoi(strtok(NULL, " "));
            int secondNum = atoi(strtok(NULL, " \n"));
            int diff = firsNUm-secondNum;
            char dataToSend[20];
            sprintf(dataToSend, "%d", diff);
            int l=0;
            for(i=0;dataToSend[i]!='\0';i++){
                l++;
            }
            strcat(dataToSend, "\n");
            send(client, dataToSend, strlen(dataToSend), 0);
        }
        else if(strcmp(arg1st, "MUL")==0){
            int firsNUm = atoi(strtok(NULL, " "));
            int secondNum = atoi(strtok(NULL, " \n"));
            int mul = firsNUm*secondNum;
            char dataToSend[20];
            sprintf(dataToSend, "%d", mul);
            int l=0;
            for(int i=0;dataToSend[i]!='\0';i++){
                l++;
            }
            strcat(dataToSend, "\n");
            send(client, dataToSend, strlen(dataToSend), 0);
        }
        else if(strcmp(arg1st, "DIV")==0){
            int firsNUm = atoi(strtok(NULL, " "));
            int secondNum = atoi(strtok(NULL, " \n"));
            float ans = (float)firsNUm/secondNum;
            char dataToSend[20];
            sprintf(dataToSend, "%f", ans);
            int l=0;
            for(i=0;dataToSend[i]!='\0';i++){
                l++;
            }
            strcat(dataToSend, "\n");
            send(client, dataToSend, strlen(dataToSend), 0);
        }
        else{
            char dataToSend[150]="400 ";
            strcat(dataToSend, arg1st);
            strcat(dataToSend, " Command not implemented");
            strcat(dataToSend, "\n");
            int l =0;
            for(i=0;dataToSend[i]!='\0';i++){
                l++;
            }
            send(client, dataToSend, l, 0);
        }
    }
    
}

void* Child(void* arg)

{   

    char line[DEFAULT_BUFLEN];

    char filename[DEFAULT_BUFLEN];

    char *lp;

    int tmpcnt=0;

    int bytes_read;

    int client = *(int *)arg;


 send(client, "Welcome to Command server\n", 27, 0);
    do
{
  
        bytes_read = recv(client, line, sizeof(line), 0);
        if (bytes_read > 0) {
            char temp;
       //     send(client, line, sizeof(line), 0);
       
           
            doStuffs(client, line, &bytes_read);
                
        } if (bytes_read == 0 ) {
            
                printf("Connection closed by client");
                break;
        } else if(bytes_read < 0){
                printf("Connection has problem\n");
                break;
        }
          
    } while (bytes_read > 0);
    close(client);
    return arg;
}



/--------------------------------------------------------------------/

/--- main - setup server and await connections (no need to clean  ---/

/--- up after terminated children.                                ---/

/--------------------------------------------------------------------/

int main(int argc, char *argv[])

{   int sd,opt,optval;

    struct sockaddr_in addr;

    unsigned short port=0;
    tempPort = atoi(argv[1]);
    


    while ((opt = getopt(argc, argv, "p:")) != -1) {

        switch (opt) {

        case 'p':

                port=atoi(optarg);

                break;

        }

    }





    if ( (sd = socket(PF_INET, SOCK_STREAM, 0)) < 0 )

        PANIC("Socket");

    addr.sin_family = AF_INET;



    if ( port > 0 )

                addr.sin_port = htons(port);

    else

                addr.sin_port = htons(tempPort);



    addr.sin_addr.s_addr = INADDR_ANY;



   // set SO_REUSEADDR on a socket to true (1):

   optval = 1;

   setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);





    if ( bind(sd, (struct sockaddr*)&addr, sizeof(addr)) != 0 )

        PANIC("Bind");

    if ( listen(sd, SOMAXCONN) != 0 )

        PANIC("Listen");



    printf("Command server listening on localhost port %d\n",ntohs(addr.sin_port));



    while (1)

    {

        int client, addr_size = sizeof(addr);

        pthread_t child;



        client = accept(sd, (struct sockaddr*)&addr, &addr_size);

        printf("Connected: %s:%d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));

        if ( pthread_create(&child, NULL, Child, &client) != 0 )

            perror("Thread creation");

        else

            pthread_detach(child);  /* disassociate from parent */

    }

    return 0;

}