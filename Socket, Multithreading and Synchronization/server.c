#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>
#include <stdbool.h>
#include "status.h"

#define MYPORT "12345"
#define BACKLOG 10
#define BUF_SIZE 256

void *runner(void *fd);
struct domain_IP_table{
    char domain[50];
    char ip[50];
}table[20];
int tab_num=0;
pthread_mutex_t mutex;
void send_respond(int,int,char *);

int main()
{
    int status , sockfd , new_fd;
    struct sockaddr_storage their_addr;
    socklen_t addr_size;
    struct addrinfo hints;
    struct addrinfo *servinfo , *ptr;
    int yes = 1;

    memset(&hints,0,sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if((status = getaddrinfo(NULL,MYPORT,&hints,&servinfo))!=0)
    {
        fprintf(stderr,"getaddrinfo error : %s\n",gai_strerror(status));
        return 1;
    }

    for(ptr = servinfo ; ptr!=NULL ; ptr = ptr -> ai_next)
    {
        if((sockfd = socket(ptr->ai_family,ptr->ai_socktype,ptr->ai_protocol))==-1)
        {
            perror("server:socket");
            continue;
        }
            if(setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1)
            {
                perror("setsockopt");
                exit(1);
            }
            if(bind(sockfd,ptr->ai_addr,ptr->ai_addrlen)==-1)
            {
                close(sockfd);
                perror("server:bind");
                continue;
            }
            break;
    }

    if(ptr == NULL)
    {
        fprintf(stderr,"serever : fail to bind\n");
        return 2;
    }

    freeaddrinfo(servinfo);

    
    if(listen(sockfd,BACKLOG)==-1)
    {
        perror("listen");
        exit(1);
    }

    pthread_t workers[10];
    int num=0;

    pthread_mutex_init(&mutex, NULL);

    while(1)
    {
        //if(num>=2)
         //       break;
        addr_size = sizeof(their_addr);
        new_fd = accept(sockfd,(struct sockaddr *)&their_addr,&addr_size);
        if(new_fd == -1)
        {
            perror("accept");
            continue;
        }
        else
        {
            //break;
            pthread_create(workers+num,NULL,runner,(void *)new_fd);
            //pthread_join(workers[num],NULL);
            num++;
            
        }
    }    
    pthread_mutex_destroy(&mutex);
    printf("Numbers of pthread : %d\n",num);
    return 0;
}

void *runner(void *fd)
{
    size_t size , size_msg;
    char buf[BUF_SIZE]="" , request[4]="" , argum0[20]="" , argum1[20]="" , msg[25]="" , *delim = "." , *pch;
    int len , bytes_recv , bytes_send , i;
    bool check=true,check2=true;    

    printf("***IN_THREAD***\n");

    while(1){
        
        recv((int)fd,&size,sizeof(size_t),0);
        bytes_recv = recv((int)fd,buf,BUF_SIZE,0);
        
        
        
        if(bytes_recv>0){            
            pthread_mutex_lock(&mutex);
            printf("recv()'d : %s\n",buf);  
            
            sscanf(buf,"%s%s%s",request,argum0,argum1);
            memset(buf,0,sizeof(buf));
            printf("split : \"%s\" , %s , %s\n",request,argum0,argum1);  
         
            if((strcmp(request,"SET"))==0)
            {
                printf("****IN_SET****\n");
                int ip[4];
                char test[BUF_SIZE]="";

                sscanf(argum1,"%d.%d.%d.%d%s",ip+0,ip+1,ip+2,ip+3,test);
                printf("IP : %d.%d.%d.%d %s\n",ip[0],ip[1],ip[2],ip[3],test);
                check=true;check2=true;
                for(i=0;i<4;i++)
                {
                    if(ip[i]>255 || ip[i]<0)
                        check=false;
                }
                if(strcmp(test,"")!=0)
                    check=false;
                pch = strtok(argum0,delim);
                while(pch != NULL)
                {
                    check2=true;
                    printf("%s\n",pch);
                    if((strcasecmp(pch,"com")!=0)&&(strcasecmp(pch,"org")!=0)&&(strcasecmp(pch,"edu")!=0)&&(strcasecmp(pch,"net")!=0)&&(strcasecmp(pch,"int")!=0)&&(strcasecmp(pch,"gov")!=0))
                        check2=false;
                    pch = strtok(NULL,delim);
                }
                if(check&&check2)
                {
                    if(tab_num==0)
                    {
                        strcpy(table[tab_num].domain,argum0);
                        strcpy(table[tab_num].ip,argum1);
                        send_respond((int)fd,OK,"");
                        tab_num++;
                        printf("buf : %s\n",buf);
                    }
                    else
                    {
                        bool reset=false;
                        for(i=0;i<tab_num;i++)
                        {
                            if((strcasecmp(argum0,table[i].domain))==0)
                            {
                                reset=true;        
                                break;
                            }
                        }
                        if(reset)
                        {
                            strcpy(table[i].domain,argum0);
                            strcpy(table[i].ip,argum1);
                            send_respond((int)fd,OK,"");
                        }
                        else
                        {
                            strcpy(table[tab_num].domain,argum0);
                            strcpy(table[tab_num].ip,argum1);
                            send_respond((int)fd,OK,"");
                            tab_num++;
                        }
                    }
                    
                }
                else
                {
                    send_respond((int)fd,BAD_REQUEST,"");
                    
                }printf("****EXIT_SET****\n");
            }
            else if((strcmp(request,"GET"))==0)
            {printf("****IN_GET****\n");
            
                check=true;            

                if(strcmp(argum0,"")==0)
                {
                    check=false;
                    printf("error in argum0\n");
                }
                if(strcmp(argum1,"")!=0)
                {
                    check=false;
                    printf("error in argum1\n");
                }
                
                if(check)
                {
                    if(tab_num==0)
                    {
                        send_respond((int)fd,NOT_FOUND,"");
                    
                    }
                    else
                    {    
                        for(i=0;i<tab_num;i++)
                        {
                            if((strcasecmp(argum0,table[i].domain))==0)
                                break;
                        }
                        if(i==tab_num)
                            send_respond((int)fd,NOT_FOUND,"");
                        else
                        {
                            send_respond((int)fd,OK,table[i].ip);
                            printf("%d %s\n",i,table[i].ip);
                        }
                    }             
                }
                else
                {
                    send_respond((int)fd,BAD_REQUEST,"");
                }printf("****EXIT_GET****\n");
            }   
            else if((strcmp(request,"INFO"))==0)
            {
                sprintf(msg,"%d",tab_num);
                send_respond((int)fd,OK,msg);
            }
            else
            {    printf("****IN_METHOD_NOT_ALLOWED****\n");                                                                          
                send_respond((int)fd,METHOD_NOT_ALLOWED,"");
            }
            pthread_mutex_unlock(&mutex);
            strcpy(argum0,"");
            strcpy(argum1,"");
        }
        
    }
    printf("***EXIT_THREAD***\n");
    pthread_exit(0);
}

void send_respond(int fd , int status ,char *respond){
    int bytes_send;
    char msg[100];
    sprintf(msg,"%d \"",status_code[status]);
    strcat(msg,status_str[status]);
    strcat(msg,"\" ");
    strcat(msg,respond);

    size_t size_msg = sizeof(msg);

    send((int)fd,&size_msg,sizeof(size_t),0);          
    bytes_send=send((int)fd,msg,sizeof(msg),0);

    if(bytes_send==-1)
    {
        perror("send");                    
    }
    printf("send()'d : %s\n",msg);

}

