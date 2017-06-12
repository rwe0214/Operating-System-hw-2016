#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

int N,M;

typedef struct PageTable{
    int pfn;
    int inUse;
    int present;
}PageTable;

void FIFO_Page(FILE *, FILE *, int *, PageTable *);
void LRU_Page(FILE *, FILE *, int *, PageTable *);
void Random_Page(FILE *, FILE *, int *, PageTable *);


int main(){
    FILE *fp, *fp1;
    char tmp[30], tmp0[10], policy[6];
    int  mode, i;

    if((fp = fopen("input.txt", "r")) == NULL){
        printf("***OPEN_INPUT_FILE_ERROR***\n");
        exit(0);
    }

    if((fp1 = fopen("output.txt", "w")) == NULL){
        printf("***CREAT_OUTPUT_FILE_ERROR***\n");
        exit(0);
    }

    fgets(tmp, 30, fp);
    sscanf(tmp, "%s%s", tmp0, policy);
    printf("Policy : %s\n", policy);

    if(strcasecmp(policy, "FIFO")==0)
        mode = 1;
    else if(strcasecmp(policy, "LRU")==0)
        mode = 2;
    else if(strcasecmp(policy, "Random")==0)
        mode = 3;
    else 
        mode = 0;

    fgets(tmp, 30, fp);
    sscanf(tmp, "%s%s%s%s%d", tmp0, tmp0, tmp0, tmp0, &N);
    fgets(tmp, 30, fp);
    sscanf(tmp, "%s%s%s%s%d", tmp0, tmp0, tmp0, tmp0, &M);
    
    printf("N = %d\n", N);
    printf("M = %d\n", M);

    int disk[N+1];
    PageTable pageTab[N+1];

    for(i=0; i<=N; i++){      
        disk[i] = -1;
        pageTab[i].pfn = 0;
        pageTab[i].inUse = 0;
        pageTab[i].present = 0;
    }            
       
    switch(mode){    
        case 0 :
                    printf("****ERROR_POLICY****\n");
                    break;               
        case 1 ://FIFO
                    
                    FIFO_Page(fp, fp1, disk, pageTab);
                    break; 
        case 2 ://LRU
                    
                    LRU_Page(fp, fp1, disk, pageTab);
                    break; 
        case 3 ://Random
                    
                    Random_Page(fp, fp1, disk, pageTab);
                    break;     
    }  

    fclose(fp);
    fclose(fp1); 
    return 0;
}

void FIFO_Page(FILE *fp, FILE *fp1, int *disk, PageTable *pageTab){
    char read[20], tmp[10], tmp1[10];
    int vpn, index = 0, i, j;
    float times = 0, hitNum = 0;
    
    fgets(read, 30, fp);

    while(fgets(read, 30, fp)!=NULL){
        memset(tmp, 0, sizeof(tmp));
        if(index == M)
            index=0; 
        sscanf(read, "%s%d", tmp, &vpn);
        memset(tmp, 0, sizeof(tmp)); 
        memset(tmp1, 0, sizeof(tmp));
        if((pageTab[vpn].inUse == 1) && (pageTab[vpn].present == 1)){
            fprintf(fp1, "Hit, %d=>%d\n", vpn, pageTab[vpn].pfn);
            hitNum++;
            index--;
        }
        else{
            //Miss condition and present = 0
            for(i=0; i<=N; i++) //check physical frame full
                if((pageTab[i].pfn == index) && (pageTab[i].present == 1))
                    break;
            for(j=0; j<=N; j++)
                if(disk[j] == -1)
                    break;
            if(i > N){
                fprintf(fp1, "Miss, %d, -1>>-1, ", index);
            }
            else{
                pageTab[i].pfn = j;
                pageTab[i].present = 0;
                disk[j] = 1;
                fprintf(fp1, "Miss, %d, %d>>%d, ", index, i, j);
            }


            if(pageTab[vpn].inUse == 0){  //inUse = 0, present = 0
                fprintf(fp1, "%d<<-1\n",vpn);
                pageTab[vpn].pfn = index;
                pageTab[vpn].inUse = 1;
                pageTab[vpn].present = 1;
            }
            else{   //inUse = 1, present = 0 
                fprintf(fp1, "%d<<%d\n", vpn, pageTab[vpn].pfn);
                disk[pageTab[vpn].pfn] = -1;
                pageTab[vpn].pfn = index;
                pageTab[vpn].present = 1;
            }
            
        }
        index++;
        times++;
    }        
    fprintf(fp1,"Page Fault Rate: %.3f\n",(times-hitNum)/times);   

}
void LRU_Page(FILE *fp, FILE *fp1, int *disk, PageTable *pageTab){
    char read[20], tmp[10], tmp1[10], lru[M+2];
    int vpn, index = 0, i, j, lruIndex = 0;
    float times = 0, hitNum = 0;

    for(i=0; i<=M+1; i++)
        lru[i] = -1;
    
    fgets(read, 30, fp);

    while(fgets(read, 30, fp)!=NULL){
        memset(tmp, 0, sizeof(tmp));
        if(index == M)
            index=0; 
        sscanf(read, "%s%d", tmp, &vpn);
        memset(tmp, 0, sizeof(tmp)); 
        memset(tmp1, 0, sizeof(tmp));
        
        /*  for debug
        for(i=0;i<=M;i++)
            printf("%d ",lru[i]);
        printf("\n");
        */

        if((pageTab[vpn].inUse == 1) && (pageTab[vpn].present == 1)){
            fprintf(fp1, "Hit, %d=>%d\n", vpn, pageTab[vpn].pfn);
            hitNum++;
            index--;
        }
        else{
            //Miss condition and present = 0
            for(i=0; i<=N; i++) //check physical frame full
                if((pageTab[i].pfn == index) && (pageTab[i].present == 1))
                    break;
            for(j=0; j<=N; j++)
                if(disk[j] == -1)
                    break;
            if(i > N){
                fprintf(fp1, "Miss, %d, -1>>-1, ", index);
            }
            else{
                index = pageTab[lru[0]].pfn;
                pageTab[lru[0]].pfn = j;
                pageTab[lru[0]].present = 0;
                disk[j] = 1;
                fprintf(fp1, "Miss, %d, %d>>%d, ", index, lru[0], j);
            }

            if(pageTab[vpn].inUse == 0){  //inUse = 0, present = 0
                fprintf(fp1, "%d<<-1\n",vpn);
                pageTab[vpn].pfn = index;
                pageTab[vpn].inUse = 1;
                pageTab[vpn].present = 1;
            }
            else{   //inUse = 1, present = 0 
                fprintf(fp1, "%d<<%d\n", vpn, pageTab[vpn].pfn);
                disk[pageTab[vpn].pfn] = -1;
                pageTab[vpn].pfn = index;
                pageTab[vpn].present = 1;
            }
            
        }
        if(lruIndex == M+1)
            lruIndex--;
        lru[lruIndex] = vpn;
        for(i = 0; i < lruIndex; i++){
            if(lru[i] == vpn){
                lru[lruIndex] = lru[i];
                for(j = i; j <= lruIndex; j++){
                    lru[j] = lru[j+1];
                }
                lruIndex--;
            }
            else
                lru[lruIndex] = vpn;
        }
        if(lru[M] != -1){
            for(i=0; i<M; i++)
                lru[i]=lru[i+1];
            lru[M]=-1;
        }
        index++;
        times++;
        lruIndex++;
    }        
    fprintf(fp1,"Page Fault Rate: %.3f\n",(times-hitNum)/times);   

}   
void Random_Page(FILE *fp,FILE *fp1,int *disk,PageTable *pageTab){
    char read[20], tmp[10], tmp1[10];
    int vpn, index = 0, i, j, random;
    float times = 0, hitNum = 0;
    
    srand(time(NULL));

    fgets(read, 30, fp);

    while(fgets(read, 30, fp)!=NULL){
        memset(tmp, 0, sizeof(tmp));
        if(index == M)
            index=0; 
        sscanf(read, "%s%d", tmp, &vpn);
        memset(tmp, 0, sizeof(tmp)); memset(tmp1, 0, sizeof(tmp));
        if((pageTab[vpn].inUse == 1) && (pageTab[vpn].present == 1)){
            fprintf(fp1, "Hit, %d=>%d\n", vpn, pageTab[vpn].pfn);
            hitNum++;
            index--;
        }
        else{
            //Miss condition and present = 0
            for(i=0; i<=N; i++)
                if((pageTab[i].pfn == index) && (pageTab[i].present == 1))
                    break;
            for(j=0; j<=N; j++)
                if(disk[j] == -1)
                    break;
            if(i > N){
                fprintf(fp1, "Miss, %d, -1>>-1, ", index);
            }
            else{
                index = rand()%M;
                for(i=0; i<=N; i++)
                    if((pageTab[i].pfn == index) && (pageTab[i].present == 1))
                        break;
                pageTab[i].pfn = j;
                pageTab[i].present = 0;
                disk[j] = 1;
                fprintf(fp1, "Miss, %d, %d>>%d, ", index, i, j);
            }


            if(pageTab[vpn].inUse == 0){  //inUse = 0, present = 0
                fprintf(fp1, "%d<<-1\n",vpn);
                pageTab[vpn].pfn = index;
                pageTab[vpn].inUse = 1;
                pageTab[vpn].present = 1;
            }
            else{   //inUse = 1, present = 0 
                fprintf(fp1, "%d<<%d\n", vpn, pageTab[vpn].pfn);
                disk[pageTab[vpn].pfn] = -1;
                pageTab[vpn].pfn = index;
                pageTab[vpn].present = 1;
            }
            
        }
        index++;
        times++;
    }        
    fprintf(fp1,"Page Fault Rate: %.3f\n",(times-hitNum)/times);   
}








