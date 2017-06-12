#include "mailbox.h"


int main(){
    printf("start\n");
    mailbox_t Server = mailbox_open(0);
    mailbox_t Client[10];
    mail_t msg,out;
    
    
    tabble map[10]; 
    int i=0,k=0;
    int check;   

    while(1)
    {
        
        if(mailbox_check_empty(Server)==0)
        {
            mailbox_recv(Server,&msg);
            printf("from : %d\n",msg.from);
            printf("type : %d\n",msg.type);
            printf("sstr : %s\n",msg.sstr);
            printf("lstr : %s\n",msg.lstr);
            if(msg.type==1)//JOIN
            {
                map[i].ID = msg.from;
                strcpy(map[i].name,msg.sstr);
                printf("map[%d].ID = %d\nmap[%d].name = %s\n",i,map[i].ID,i,map[i].name);
                Client[msg.from] = mailbox_open(msg.from);
                for(k=0;k<i;k++)
                {
                    out.from = 0;
                    out.type = 1;
                    strcpy(out.sstr,msg.sstr);
                    strcpy(out.lstr,msg.lstr);
                    while(1)
                    {
                        if(mailbox_check_full(Client[map[k].ID])==0)
                        {
                            mailbox_send(Client[map[k].ID],&out);
                            break;
                        }
                    }
                }
                i++;
            }
            else if(msg.type==2)//broadcast
            {
                out.from = 0;
                out.type = 2;
                for(k=0;k<i;k++)
                {
                    if(map[k].ID == msg.from)break;
                }
                
                strcpy(out.sstr,map[k].name);
                strcpy(out.lstr,msg.lstr);
                for(k=0;k<i;k++)
                {                    
                    while(mailbox_check_full(Client[map[k].ID])==0 && map[k].ID!=msg.from)
                    {
                        mailbox_send(Client[map[k].ID],&out);
                        break;
                    }                   
                }
            }
            else if(msg.type==3)//leave
            {
                
                out.from = 0;
                out.type = 3;
                for(k=0;k<i;k++)
                {
                    if(map[k].ID == msg.from)break;
                }
                strcpy(out.sstr,map[k].name);
                printf("leave ID : %d\nleave name : %s\n",map[k].ID,out.sstr);
                strcpy(out.lstr,"");
                mailbox_close(Client[map[k].ID]);
                mailbox_unlink(map[k].ID);
                
                tabble_sft(map,k);
                i--;
                for(k=0;k<i;k++)
                {
                    //check=false;
                    while(1)
                    {
                        if(mailbox_check_full(Client[map[k].ID])==0) //&& map[k].ID!=msg.from)
                        {
                               mailbox_send(Client[map[k].ID],&out);
                               break;
                        }
                    }
                }
                
            }
            else if(msg.type==4)//private
            {
                out.from = 0;
                out.type = 4;
                for(k=0;k<10;k++)
                {
                    if(map[k].ID==msg.from)break;
                }
                strcpy(out.sstr,map[k].name);
                strcpy(out.lstr,msg.lstr);
                for(k=0;k<10;k++)
                {
                    if(strcmp(map[k].name,msg.sstr)==0)break;
                }
                while(1)
                    {
                        if(mailbox_check_full(Client[map[k].ID])==0) //&& map[k].ID!=msg.from)
                        {
                               mailbox_send(Client[map[k].ID],&out);
                               break;
                        }
                    }
            }
            else if(msg.type==5)
            {
                out.from = 0;
                out.type = 5;
                strcpy(out.sstr,"");
                strcpy(out.lstr,"");
                for(k=0;k<i;k++)
                {
                    strcat(out.lstr,map[k].name);
                    if(k!=(i-1))
                    strcat(out.lstr," , ");
                }
                while(1)
                {
                    if(mailbox_check_full(Client[msg.from])==0)
                    {                        
                        mailbox_send(Client[msg.from],&out);
                        break;
                    }
                }
            }
        }     
       else    
            ;
        
    }    
    mailbox_close(Server);
    mailbox_unlink(0);
    
    return 0;

}
