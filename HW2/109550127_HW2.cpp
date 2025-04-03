/*
Student No.: 109550127  
Student Name: 宋哲頤
Email: sung891220@gmail.com
SE tag: xnxcxtxuxoxsx
Statement: I am fully aware that this program is not
supposed to be posted to a public server, such as a
public GitHub repository or a public web page.
*/
#include<bits/stdc++.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <unistd.h>
using namespace std;

void mul(unsigned int *a,unsigned int *c,int beg,int n,int num)
{
	unsigned int sum;
	for(int i=beg;i<n;i++){
		for(int j=0;j<num;j++){
			sum=0;
			for(int k=0;k<num;k++)
				sum+=a[i*num+k]*a[k*num+j];
			c[i*num+j]=sum;
		}
    }
	return;
}

int main(){
    int Ablk=shmget(0,3000000,IPC_CREAT|0666), Cblk=shmget(0,3000000,IPC_CREAT|0666);
    int num;
    cin>>num;
    struct timeval begin,end;
    unsigned int *a=(unsigned int *)shmat(Ablk,NULL,0), *c=(unsigned int *)shmat(Cblk,NULL,0);;
    for(int i=0;i<num;i++){
        for(int j=0;j<num;j++){
            a[i*num+j]=i*num+j;
        }
    }
    int tmp=0;
    for(int p=1;p<=16;p++){
        for(int i=0;i<num;i++){
            for(int j=0;j<num;j++){
                c[i*num+j]=0;
            }
        }
        gettimeofday(&begin,0);
        int l=num/p;
        for(tmp=0;tmp<p-1;tmp++){
            if(fork()==0)
                break;
        }
        if(tmp==p-1){
            mul(a,c,tmp*l,num,num);
        }
        else{
            mul(a,c,tmp*l,(tmp+1)*l,num);
            exit(0);
        }
        signal(SIGCHLD, SIG_IGN);
        wait(NULL);
        gettimeofday(&end,0);
        double sec=end.tv_sec-begin.tv_sec;
        double usec=end.tv_usec-begin.tv_usec;
        cout<<p<<".";
        cout<<"elapsed time:"<<sec*1000+(usec/1000.0)<<"ms  ";
        
        unsigned int chksum=0;
        for(int i=0;i<num;i++){
            for(int j=0;j<num;j++)
                chksum+=c[i*num+j];
        }
        cout<<"checksum:"<<chksum<<"\n";
    
    }
    shmctl(Ablk,IPC_RMID,NULL),shmctl(Cblk,IPC_RMID,NULL);
    shmdt(a),shmdt(c);
    return 0;
}