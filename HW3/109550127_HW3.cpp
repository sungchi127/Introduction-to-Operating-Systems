#include<bits/stdc++.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>
#include <limits.h>
using namespace std;
#define MAX INT_MAX
//read sort merge
void ReadFile(string file);
void bubblesort(int head, int tail);
void mergesort(int head, int mid, int tail);
void *fcthread(void *arg);
void *dispatch(void *arg);
void Swap(int *a, int *b);
void OutputFile(char filename[], int *array);
void pop_front(std::vector<int> &v);

int num, sec, usec;
int *arr;
struct timeval _start, _end;
int *jobfinish;
int *jobdispatch;
vector<int> jqueue;
int rcvnum;
sem_t mux,finishnum;
int jnum;

int main(){
    // Multi thread merge sort
    string filename;
    cout<<"input file name: ";
    cin>>filename;
    for(int i = 1; i <= 8; i++){
        ReadFile(filename);
        //init
        jobfinish=new int[15];
        jobdispatch=new int[15];
        jnum=0;
        for(int j=0;j<15;j++){
            jobfinish[j]=0;
            jobdispatch[j]=0;
        }
        rcvnum=0;
        vector<int> jqueue;
        sem_init(&mux, 0, 0);
        sem_init(&finishnum, 0, 0);
        pthread_t tdispatch;
        pthread_t tsort[i];
        //pthread_create
        pthread_create(&tdispatch, NULL, dispatch, NULL);
        for(int j = 0; j < i; j++){
            pthread_create(&tsort[j], NULL, fcthread, NULL);
        }
        //計算時間
        gettimeofday(&_start, 0);
        pthread_join(tdispatch, NULL);
        for(int j = 0; j < i; j++){
            pthread_join(tsort[j], NULL);
        }
        gettimeofday(&_end, 0);
        sec = _end.tv_sec - _start.tv_sec;
		usec = _end.tv_usec - _start.tv_usec;
		cout<<"worker thread #"<<i<<", elapsed "<<(sec*1000+(usec/1000.0))<<" ms"<<endl;
        //output
        string flname="output_" + to_string(i) + ".txt";
        OutputFile((char*)flname.c_str(), arr);
        delete [] arr;
        delete [] jobfinish;
        sem_destroy(&mux);
        sem_destroy(&finishnum);
    }
    return 0;
}

void ReadFile(string file){
    char *filename=(char*)file.c_str();

    FILE *fin;
    if ((fin=fopen(filename, "r"))==NULL){
        printf("Error open file\n");
        exit(1);
    }
    
    fscanf(fin, "%d", &num);
    
     arr=(int*)malloc(sizeof(int)*num);

    for(int j=0; j < num;j++){
        fscanf(fin, "%d", &arr[j]);
    }
    return;
}


void mergesort(int head,int mid,int tail){
    int tem[tail-head];
    int indleft=0,indright=0,indsum=0;
    int size=tail-head;
    //cout<<"MergeSort "<<start<<" "<<mid<<" "<<end<<endl;
    while((indleft < (mid - head)) && (indright < (tail - mid))){
        if(arr[head+indleft]<arr[mid+indright]){
            tem[indsum]=arr[head+indleft];
            indleft++;
            indsum++;
        }
        else{
            tem[indsum]=arr[mid+indright];
            indright++;
            indsum++;
        }
    }

    while(indleft<(mid-head)){
        tem[indsum]=arr[head+indleft];
        indleft++;
        indsum++;
    }
    while(indright<(tail-mid)){
        tem[indsum]=arr[mid+indright];
        indright++;
        indsum++;
    }
    for(int i=0;i<tail-head;i++){
        arr[head+i]=tem[i];
    }
    return ;
}

void bubblesort(int head, int tail){
    int size=tail-head+1;
    for(int i=size-1; i>0; i--)
        for(int j=head; j<head+i; j++)
            if(arr[j]>arr[j+1])
                Swap(&arr[j], &arr[j+1]);
    return ;
}

void* fcthread(void *arg){
    while(1){
        int n_block=16;
        sem_wait(&mux);
        if(jnum==0){
            if(rcvnum==15){
                sem_post(&mux);
                break;
            }
            else{
                sem_post(&mux);
                continue;
            }
        }
        n_block=jqueue.front();
        pop_front(jqueue);
        rcvnum++;
        jnum--;
        if(n_block==17){
            cout<<"error: task code is "<<n_block<<endl;
            break;
        }
        sem_post(&mux);
        if((n_block<0)&&(n_block>14)){
            return NULL;
        }
        else if(n_block<8){
            bubblesort(int(n_block*num/8),int((n_block+1)*num/8));
            jobfinish[n_block]=1;
            // cout<<int(n_block*num/8)<<" "<<int((n_block+1)*num/8);
            sem_post(&finishnum);
        }
        else if(n_block==8){
            mergesort(0,num/8,num/4);
            jobfinish[8]=1;
            sem_post(&finishnum);
        }
        else if(n_block==9){
            mergesort(num/4,3*num/8,num/2);
            jobfinish[9]=1;
            sem_post(&finishnum);
        }
        else if(n_block==10){
            mergesort(num/2,5*num/8,3*num/4);
            jobfinish[10]=1;
            sem_post(&finishnum);        
        }
        else if(n_block==11){
            mergesort(3*num/4,7*num/8,num);
            jobfinish[11]=1;
            sem_post(&finishnum);
        }
        else if(n_block==12){
            mergesort(0,num/4,num/2);
            jobfinish[12]=1;
            sem_post(&finishnum);
        }
        else if(n_block==13){
            mergesort(num/2,3*num/4,num);
            jobfinish[13]=1;
            sem_post(&finishnum);
        }
        else if(n_block==14){
            mergesort(0,num/2,num);
            jobfinish[14]=1;
            sem_post(&finishnum);
        }
    }
    pthread_exit(NULL);
}

void* dispatch(void *arg){
    for(int i=0;i<8;i++){
        jqueue.push_back(i);
        jobdispatch[i]=1;
        jnum++;
    }
    sem_post(&mux);
    for(int i=0;i<15;i++){
        sem_wait(&finishnum);
        if((jobfinish[0]==1)&&(jobfinish[1]==1)&&(jobdispatch[8]==0)){
            sem_wait(&mux);
            jqueue.push_back(8);
            jobdispatch[8]=1;
            jnum++;
            sem_post(&mux);
        }
        else if((jobfinish[2]==1)&&(jobfinish[3]==1)&&(jobdispatch[9]==0)){
            sem_wait(&mux);
            jqueue.push_back(9);
            jobdispatch[9]=1;
            jnum++;
            sem_post(&mux);
        }
        else if((jobfinish[4]==1)&&(jobfinish[5]==1)&&(jobdispatch[10]==0)){
            sem_wait(&mux);
            jqueue.push_back(10);
            jobdispatch[10]=1;
            jnum++;
            sem_post(&mux);
        }
        else if((jobfinish[6]==1)&&(jobfinish[7]==1)&&(jobdispatch[11]==0)){
            sem_wait(&mux);
            jqueue.push_back(11);
            jobdispatch[11]=1;
            jnum++;
            sem_post(&mux);
        }
        else if((jobfinish[8]==1)&&(jobfinish[9]==1)&&(jobdispatch[12]==0)){
            sem_wait(&mux);
            jqueue.push_back(12);
            jobdispatch[12]=1;
            jnum++;
            sem_post(&mux);
        }
        else if((jobfinish[10]==1)&&(jobfinish[11]==1)&&(jobdispatch[13]==0)){
            sem_wait(&mux);
            jqueue.push_back(13);
            jobdispatch[13]=1;
            jnum++;
            sem_post(&mux);
        }
        else if((jobfinish[12]==1)&&(jobfinish[13]==1)&&(jobdispatch[14]==0)){
            sem_wait(&mux);
            jqueue.push_back(14);
            jobdispatch[14]=1;
            jnum++;
            sem_post(&mux);
        }
    }
    pthread_exit(NULL);
}

void Swap(int *a, int *b){
    int temp=*a;
    *a=*b;
    *b=temp;
    return;
}

void pop_front(std::vector<int> &v){
    if (v.size() > 0) {
        v.erase(v.begin());
    }
}

void OutputFile(char filename[], int *array){
    FILE *fout;
    int i;
    fout=fopen(filename, "w");
    for(i=0;i<num;i++)
        fprintf(fout,"%d ", array[i]);
    fclose(fout);
}
