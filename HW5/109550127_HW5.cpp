/*
Student No.: 109550127
Student Name: 宋哲頤
Email: sung891220@gmail.com
SE tag:
xnxcxtxuxoxsx
Statement: I am fully aware that this program is not
supposed to be posted to a public server, such as a
public GitHub repository or a public web page.
*/

#include<bits/stdc++.h>
#include<ctime>
#include<sys/time.h>
using namespace std;

class Node{
	public:
		int task;
		int ref_count;
        int head;
        int tail;
		int seq;
		bool operator<(const Node& b) const{
			return ref_count < b.ref_count || (ref_count == b.ref_count) && seq < b.seq ;
		}
};

char* filename[100];
FILE *file;

void LFU();
void LRU();

int main(int argc, char *argv[]){
    *filename=argv[1];
    //LFU
    cout<<"LFU policy:"<<"\n"<<"Frame\tHit\t\tMiss\t\tPage fault ratio"<<"\n";
    LFU();
    //LRU
    cout<<"LRU policy:"<<"\n"<<"Frame\tHit\t\tMiss\t\tPage fault ratio"<<"\n";
    LRU();

    return 0;
}

void LFU(){
    int frame_size=128, hit=0, miss=0, now_size=0 , seq=0 ,pop=0;
    int task;
    double fault_ratio=0;
    struct  timeval start;
    struct  timeval end;
    double diff;
    set<Node> ListLFU;
    map<int, set<Node>::iterator>::iterator iter_map;//iter_map->first是一個MapHash的key(int) iter_map->second為MapHash的value(set<Node>::iterator)
    map<int, set<Node>::iterator> MapHash;
    pair<set<Node>::iterator, bool> ret;
    gettimeofday(&start,NULL);
    while(frame_size<=1024){
        hit=0, miss=0, fault_ratio=0, now_size=0 , seq=0;
        if ((file=fopen(*filename, "r"))==NULL){
            printf("Error open file\n");
            exit(1);
        }
        MapHash.clear();
		ListLFU.clear();
        while(fscanf(file, "%d", &task)!=EOF){
            iter_map=MapHash.find(task);
            Node tmp;
            seq++;
            if(iter_map==MapHash.end()){
                tmp.ref_count=0;
                tmp.task=task;
                tmp.seq=seq;
                miss++;
                if(now_size < frame_size){
                    now_size++;
                    ret=ListLFU.insert(tmp);
                    MapHash[task]=ret.first;//ret.first為一個set<Node>::iterator
                }
                else{
                    auto del=ListLFU.begin();
                    pop=ListLFU.begin()->task;
                    ListLFU.erase(del);
                    MapHash.erase(pop);
                    ret=ListLFU.insert(tmp);
                    if(ret.second==false){
                        printf("error\n");
                    }
                    else{
                        MapHash[task]=ret.first;
                    }
                }
            }
            else{
	    		Node tmp;
				tmp.task = task;
				tmp.ref_count = iter_map->second->ref_count;
                tmp.seq = iter_map->second->seq;
				tmp.ref_count++;
                hit++;
				ListLFU.erase(iter_map->second);
	    		ret=ListLFU.insert(tmp);
                if(ret.second==false){
                    printf("error\n");
                }
                else{
                    MapHash[task]=ret.first;
                }
            }
        }
        fault_ratio=double(miss)/double(miss+hit);
		printf("%d\t%d\t\t%d\t\t%.10lf\n", frame_size, hit, miss, fault_ratio);
        frame_size*=2;
        fclose(file);
    }

    gettimeofday(&end,NULL);
    diff =  1000000 * (end.tv_sec-start.tv_sec)+ (end.tv_usec-start.tv_usec);
    printf("Total elapsed time: %lf\n",diff/1000000);
}

void LRU(){
    int frame_size=128, hit=0, miss=0, now_size=0 , seq=0 ,pop=0;
    int task;
    double fault_ratio=0;
    struct  timeval start;
    struct  timeval end;
    double diff;
    list<int> ListLRU;
    map<int, list<int>::iterator>::iterator iter;//iter_map->first是一個MapHash的key(int) iter_map->second為MapHash的value(set<Node>::iterator)
    map<int, list<int>::iterator> MapHash;
    gettimeofday(&start,NULL);
    while(frame_size<=1024){
        hit=0, miss=0, fault_ratio=0, now_size=0 , seq=0;
        if ((file=fopen(*filename, "r"))==NULL){
            printf("Error open file\n");
            exit(1);
        }
        MapHash.clear();
		ListLRU.clear();
        while(fscanf(file,"%d",&task)!=EOF){
            iter=MapHash.find(task);
            if(iter==MapHash.end()){
                if(now_size < frame_size){
                    now_size++;
                    ListLRU.push_front(task);
                    MapHash[task]=ListLRU.begin();
                }
                else{
                    ListLRU.push_front(task);
                    MapHash[task]=ListLRU.begin();
                    MapHash.erase(ListLRU.back());
                    ListLRU.pop_back();
                }
                miss++;
            }
            else{
                ListLRU.erase(iter->second);
                ListLRU.push_front(task);
                MapHash[task]=ListLRU.begin();
                hit++;
            }
        }
        fault_ratio=double(miss)/double(miss+hit);
		printf("%d\t%d\t\t%d\t\t%.10lf\n", frame_size, hit, miss, fault_ratio);
        frame_size*=2;
        fclose(file);
    }
    gettimeofday(&end,NULL);
    diff = 1000000 * (end.tv_sec-start.tv_sec)+ end.tv_usec-start.tv_usec;
    printf("Total elapsed time: %lf\n",diff/1000000);
}