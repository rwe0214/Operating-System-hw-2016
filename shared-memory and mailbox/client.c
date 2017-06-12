#include "mailbox.h"


int main(){
    int id,i,j,k,m,check=0;
    char name[SIZE_OF_SHORT_STRING],tmp[SIZE_OF_SHORT_STRING],esc[64];
    printf("ID(1~10) : ");
    scanf("%d",&id);
    printf("Name : ");
    scanf("%s",name);
    
    mailbox_t Server = mailbox_open(0);
    mailbox_t Client = mailbox_open(id);

    mail_t creat,rcv;
    //int i=0;


    creat.from=id;
    creat.type=1;
    strcpy(creat.sstr,name);
    strcpy(creat.lstr,"");
 
    while(1)
    {
        if(mailbox_check_full(Server)==0)
        {            
            mailbox_send(Server,&creat);
            printf("****Welcome to the ChatRoom , %s****\n",name);
            printf("****Note : Leave word is gg****\n");
            printf("****Note : use \"/r NAME MSG\" to send private msg****\n");
            break;
        }    
    }
    for(i=0;i<64;i++)
    {
        if(esc[i]=='\0')
        break;
    }
    int lenth = i;
    //char quit[SIZE_OF_LONG_STRING]="LEAVE";
    
//nonblocking   
    
    while(1)
    {
        if(mailbox_check_empty(Client)==0)
        {
            mailbox_recv(Client,&rcv);
            if(rcv.type==1)
            {
                printf("--%8s join the ChatRoom--\n",rcv.sstr);
            }
            else if(rcv.type==2)
            {
                printf("%s : %s\n",rcv.sstr,rcv.lstr);
            }
            else if(rcv.type==3)
            {
                printf("##%8s leave the ChatRoom##\n",rcv.sstr);
            }
            else if(rcv.type==4)
            {
                printf("[PRIVATE_FROM_%s] : %s\n",rcv.sstr,rcv.lstr);
            }
            else if(rcv.type==5)
            {
                printf("<Online user : %s>\n",rcv.lstr);
            }
        }
        
        fcntl(0,F_SETFL,fcntl(0,F_GETFL)|O_NONBLOCK);
        char chat[SIZE_OF_LONG_STRING]="";
        int numRead = read(0,chat,sizeof(chat));
                
        if(numRead>0)
        {
            mail_t msg;
            if(chat[0]=='g' && chat[1]=='g')
            {
                msg.from=id;
                msg.type=3;
                strcpy(msg.sstr,"");
                strcpy(msg.lstr,"");
                while(1)
                {
                    if(mailbox_check_full(Server)==0)
                    {            
                        mailbox_send(Server,&msg);                    
                        printf("You left the ChatRoom\n");
                        return 0;
                    }                    
                }
            }
            else if(chat[0]=='L' && chat[1]=='I' && chat[2]=='S' && chat[3]=='T')
            {
                msg.from=id;
                msg.type=5;
                strcpy(msg.sstr,"");
                strcpy(msg.lstr,"");
                while(1)
                {
                    if(mailbox_check_full(Server)==0)
                    {            
                        mailbox_send(Server,&msg);                    
                        break;
                    }                    
                }
            }
            else if(chat[0]=='/' && chat[1]=='r')
            {
                msg.from = id;
                msg.type = 4;
                
                for(i=3;i<SIZE_OF_SHORT_STRING;i++)
                {
                    if(chat[i]==' ')
                    {
                        char tmp[SIZE_OF_LONG_STRING]="";
                        for(j=3,k=0;j<i;j++,k++)
                        {
                            tmp[k]=chat[j];
                        }
                        
                        strcpy(msg.sstr,"");
                        strcpy(msg.sstr,tmp);
                        strcpy(tmp,"");
                        break;
                    }
                }
                for(j=i+1;j<SIZE_OF_LONG_STRING;j++)
                {
                    if(chat[j]=='\n')
                    {                        
                        char tmp2[SIZE_OF_LONG_STRING]="";
                        for(k=i+1,m=0;k<j;k++,m++)
                        {
                            tmp2[m]=chat[k];
                        }
                        strcpy(msg.lstr,"");
                        strcpy(msg.lstr,tmp2);
                        strcpy(tmp2,"");
                        break;
                    }
                }
                while(1)
                {   
                    if(mailbox_check_full(Server)==0)
                    {            
                        mailbox_send(Server,&msg);                    
                        printf("**private_to_%s** : %s\n",msg.sstr,msg.lstr);
                        break;
                    }                    
                }
               
            }
            else
            {
                msg.from=id;
                msg.type=2;
                strcpy(msg.sstr,"");
                //strcpy(msg.lstr,chat);
                for(i=0;i<SIZE_OF_LONG_STRING;i++)
                {
                    if(chat[i]=='\n')
                    {
                        char tmp3[SIZE_OF_LONG_STRING]="";
                        for(k=0,m=0;k<i;k++,m++)
                        {
                            tmp3[m]=chat[k];
                        }
                        strcpy(msg.lstr,tmp3);
                        break;
                    }
                }
                while(1)
                {   
                    if(mailbox_check_full(Server)==0)
                    {            
                        mailbox_send(Server,&msg) ;                  
                        printf("%s (me) : %s\n",name,msg.lstr);
                        break;
                    }                    
                }
            }
        }
        
    }

    
   
    return 0;

}
