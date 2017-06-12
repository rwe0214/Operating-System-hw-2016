#include "mailbox.h"

mailbox_t mailbox_open(int id){
    char num[32];
    sprintf(num,"%d",id);
    char name[64] ="__mailbox_";
    strcat(name,num);

    int fd = shm_open(name,O_CREAT | O_RDWR,0666);
    mailbox_t mailbox = (void *)fd;
    ftruncate(fd,SIZE);
    
    return mailbox;
}

int mailbox_close(mailbox_t box){
    return close((int)box);   
}

int mailbox_unlink(int id){
    char num[32];
    sprintf(num,"%d",id);
    char name[64] ="__mailbox_";
    strcat(name,num); 

    return shm_unlink(name);;
}

void mailbox_sft(mailbox_t box){
    //lseek((int)box,584,SEEK_SET);
    int i,j;    
    mail_t tmp[10];
    for(i=0;i<10;i++)
    {
        
        read((int)box,tmp+i,sizeof(mail_t));
        /*printf("sft\n");
        printf("from : %d\n",(tmp+i)->from);
        printf("type : %d\n",(tmp+i)->type);
        printf("sstr : %s\n",(tmp+i)->sstr);
        printf("lstr : %s\n",(tmp+i)->lstr);
        printf("ENDsft\n");*/
        if((int )(tmp+i)->type != 1 && (int )(tmp+i)->type != 2 && (int )(tmp+i)->type != 3)
            break;
    }
    //printf("%d\n",i);
    lseek((int)box,0,SEEK_SET);
    for(j=0;j<i;j++)
    {
        write((int)box,tmp+j,sizeof(mail_t));
        
        /*printf("mov\n");
        printf("from : %d\n",(tmp+j)->from);
        printf("type : %d\n",(tmp+j)->type);
        printf("sstr : %s\n",(tmp+j)->sstr);
        printf("lstr : %s\n",(tmp+j)->lstr);
        printf("ENDmov\n");*/
        //printf("ptr : %ld\n",lseek((int)box,0,SEEK_CUR));
    }
    (tmp+j+1)->from = 0;
    (tmp+j+1)->type = 0;
    strcpy((tmp+j+1)->sstr,"");
    strcpy((tmp+j+1)->lstr,"");
    write((int)box,tmp+j+1,sizeof(mail_t));
    lseek((int)box,-584,SEEK_CUR);
    //printf("ptr : %ld\n",lseek((int)box,-584,SEEK_CUR));

}

int mailbox_send(mailbox_t box,mail_t *mail){
    int ret = write((int)box,mail,sizeof(mail_t));//printf("(apiw)%d,%d\n",mail->type,ret);
   
    if(ret==-1)
        return -1;
    else
        return 0;
    
}

int mailbox_recv(mailbox_t box,mail_t *mail){
    lseek((int)box,0,SEEK_SET);
    int ret = read((int)box,mail,sizeof(mail_t));//printf("(apir)%d\n",mail->type);
    if(ret!=0 && ((int )mail->type == 1 || (int )mail->type == 2 || (int )mail->type == 3 || (int )mail->type == 4 || (int )mail->type == 5))
    {    
        //printf("(api)from : %d\n",mail->from);
        //printf("(api)type : %d\n",mail->type);
        //printf("(api)sstr : %s\n",mail->sstr);
        //printf("(api)lstr : %s\n",mail->lstr);
        //printf("ret : %d\n",ret);
        mailbox_sft(box);
    }
    
    if(ret==-1)
        return -1;
    else
        return 0;
}

int mailbox_check_full(mailbox_t box){
    /*off_t cur_pos = lseek((int)box,0,SEEK_CUR);
    //printf("cur : %ld\n",cur_pos);
    off_t end_pos = lseek((int)box,-584,SEEK_END);
    //printf("end : %ld\n",end_pos);
    if(cur_pos == end_pos) 
    {
        lseek((int)box,cur_pos,0);
        //printf("cur' : %ld\n",lseek((int)box,cur_pos,0));
        return 1;
    }
    else
    {
        lseek((int)box,cur_pos,0);
        //printf("cur' : %ld\n",lseek((int)box,cur_pos,0));
        return 0;
    }*/
    lseek((int)box,0,SEEK_SET);    
    off_t cur_pos;
    off_t end_pos;
    mail_t mail;
    do{
        read((int)box,&mail,sizeof(mail_t));
        //printf("type : %d\n",mail.type);
    }while(mail.type == 1 || mail.type == 2 || mail.type == 3 || mail.type == 4|| mail.type == 5);
    //printf("Now : %ld\n",lseek((int)box,0,SEEK_CUR));    
    cur_pos = lseek((int)box,-584,SEEK_CUR);//printf("cur : %ld\n",cur_pos);
    end_pos = lseek((int)box,-584,SEEK_END);//printf("end : %ld\n",end_pos);
    if(cur_pos == end_pos)
    {
        lseek((int)box,cur_pos,0);
        //printf("return 1\n");
        return 1;
    }
    else
    {
        lseek((int)box,cur_pos,0);
        //printf("return 0\n");
        return 0;
    }
    
}

int mailbox_check_empty(mailbox_t box){
    mail_t mail;
    //off_t cur_pos = lseek((int)box,0,SEEK_CUR);
    //off_t ori_pos = lseek((int)box,0,SEEK_SET);
    
    lseek((int)box,0,SEEK_SET);    
    read((int)box,&mail,sizeof(mail_t));
      
    if(mail.type == 1 || mail.type == 2 || mail.type == 3 || mail.type == 4|| mail.type == 5)
    {
        return 0;
    }
    else
    {
        lseek((int)box,0,SEEK_SET);
        return 1;   
    }    
/*    
    if(cur_pos == ori_pos) 
    {
        lseek((int)box,cur_pos,0);
        return 1;
    }
    else
    {
        lseek((int)box,cur_pos,0);
        return 0;
    }
*/
}

void tabble_sft(tabble *map,int id){
    int i;   
    int tmp_ID;
    char tmp_name[SIZE_OF_SHORT_STRING]; 
    
    map[id].ID = 0;
    strcpy(map[id].name,"");

    for(i=id;i<9;i++)
    {
        tmp_ID = map[i+1].ID;
        if(tmp_ID == 0)
            break;
        strcpy(tmp_name,map[i+1].name);
        map[i].ID = tmp_ID;
        strcpy(map[i].name,tmp_name);        
    }
}
