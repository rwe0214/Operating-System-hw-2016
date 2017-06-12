#ifndef MAILBOX_H
#define MAILBOX_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>

#define SIZE_OF_SHORT_STRING 64
#define SIZE_OF_LONG_STRING 512
#define SIZE 1752//2336//1752//1168//584 //3
typedef void *mailbox_t;
typedef struct __MAIL{
    int from;
    int type; 
    char sstr[SIZE_OF_SHORT_STRING];
    char lstr[SIZE_OF_LONG_STRING];
} mail_t;
typedef struct clientTab{
        int ID;
        char name[SIZE_OF_SHORT_STRING];
} tabble;

mailbox_t mailbox_open(int id);
void mailbox_sft(mailbox_t box);
int mailbox_unlink(int id);
int mailbox_close(mailbox_t box);
int mailbox_send(mailbox_t box,mail_t *mail);
int mailbox_recv(mailbox_t box,mail_t *mail);
int mailbox_check_empty(mailbox_t box);
int mailbox_check_full(mailbox_t box);

void tabble_sft(tabble *map,int id);

#endif
