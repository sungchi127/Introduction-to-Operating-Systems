/*
Student No.:109550127
Student Name:宋哲頤
Email:sung891220@gmail.com
SE tag:xnxcxtxuxoxsx
Statement: I am fully aware that this program is not
supposed to be posted to a public server, such as a
public GitHub repository or a public web page.
*/
#include<bits/stdc++.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
using namespace std;

const vector<string> split(const string &str, const char &delimiter);
void f(char**str,int size);
// void genarg(char *argp[],string *arg){
//     for(int i=0;i<(int)sizeof(arg);i++){
//         arg[i] = strdup((arg[i]).c_str());
//     }
//     arg[sizeof(arg)]=NULL;
// }

int main(){
    char *com[2][10];
    string str; 
    int len[2],fd,fl,andd,wnum;
    vector<string> s;
    while(1){
        wnum=0,andd=0,fl=0;
        cout<<">";
        for(int i=0;i<2 ;i++){
            len[i]=0;
            for(int j=0;j<10;j++){
                com[i][j]=(char*)malloc(sizeof(char)*30);
            }
        }
        getline(cin, str);
        s = split(str, ' ');

        int pipefd[2];
        for(int i=0, j=0; i < (int)s.size(); i++, len[wnum]++,j++){
            // cout<<wnum<<":";
            if(s[i] =="|"){
                pipe(pipefd);
                wnum++;
                len[wnum]--;
                j=-1;
                continue;
                // cout<<"1"<<"\n";
            }
            if(s[i]==">"){
                len[wnum]--;
                const char *c=s[i+1].c_str();
                fd=open(c,O_WRONLY|O_CREAT|O_TRUNC,0664);//不同
                fl=1;
                i+=1;
                // cout<<"2"<<"\n";
            }
            else if(s[i]!="&"){
                // com[wnum][j]=const_cast<char*>(s[i].c_str());
                strcpy(com[wnum][j],s[i].c_str());
                // cout<<"3"<<"\n";
           }
           else{
                len[wnum]--;
                andd=1;
                // cout<<"4"<<"\n";
           }
        }
        // for(int i=0;i<2;i++){
		// 	for(int j=0;j<(int)sizeof(*com[i]);j++){
		// 		cout<<*com[i][j]<<" ";
		// 	}
		// 	cout<<"\n";
		// }
        // cout<<"x:"<<(*com[0])<<" "<<(*com[0]+2);
        if(andd){
            // cout<<"asd";
			if(fork()==0){
				if(fork()==0){
					if(fl){
						if(wnum){
							if(fork()==0){
								if(fl){
									dup2(fd,1);
									close(fd);
								}
								f(com[0]+len[0],10-len[0]);
								if(wnum){
									f(com[1],10);
									close(pipefd[0]);
									dup2(pipefd[1],1);
									close(pipefd[1]);
								}
                                // char *arg[256]={};
                                // cout<<"8"<<arg<<" ";
                                // // genarg(arg,*com[0]);
                                // for(int i=0;i<(int)sizeof(*com[0]);i++){
                                //     arg[i] = strdup((*com[0][i]).c_str());
                                // }
                                // arg[(int)sizeof(*com[0])]=NULL;
                                // execvp((*com[0][0]).c_str(),arg);
								execvp(com[0][0],com[0]);
							}
							else{
								wait(NULL);
								f(com[0],10);
								if(wnum)
								{
									if(fork()==0){
										f(com[1]+len[1],10-len[1]);
										close(pipefd[1]);
										dup2(pipefd[0],0);
										close(pipefd[0]);
                                        char *arg[256]={};
                                        // genarg(arg,*com[0]);
                                        // cout<<"9"<<arg<<" ";
                                        // for(int i=0;i<(int)sizeof(*com[1]);i++){
                                        //     arg[i] = strdup((*com[1][i]).c_str());
                                        // }
                                        // arg[(int)sizeof(*com[1])]=NULL;
                                        // execvp((*com[1][0]).c_str(),arg);
										execvp(com[1][0],com[1]);
									}
									else{
										f(com[1],10);
										close(pipefd[1]);
										close(pipefd[0]);
										wait(NULL);
									}
								}
							}
						}
						else{
							dup2(fd,1);
							close(fd);
						}
					}
					f(com[0]+len[0],10-len[0]);
                    // char *arg[256]={};
                    // // genarg(arg,*com[0]);
                    // cout<<"10"<<arg<<" ";
                    // for(int i=0;i<(int)sizeof(*com[0]);i++){
                    //     arg[i] = strdup((*com[0][i]).c_str());
                    // }
                    // arg[(int)sizeof(*com[0])]=NULL;
                    // execvp((*com[0][0]).c_str(),arg);
					execvp(com[0][0],com[0]);
				}
				else{
					f(com[0],10);
					exit(0);
				}
			}
			else{
				f(com[0],10);
				wait(NULL);
			}
        }

        else{
            if(fork()==0){
                if(fl){
                    dup2(fd,1);
                    close(fd);
                }
                f(com[0]+len[0],10-len[0]);
                if(wnum){
                    f(com[1],10);
                    close(pipefd[0]);
                    dup2(pipefd[1],1);
                    close(pipefd[1]);
                }
                // char *arg[256]={};
                // // genarg(arg,*com[0]);
                // cout<<"6"<<arg<<" ";
                // for(int i=0;i<(int)sizeof(*com[0]);i++){
                //     arg[i] = strdup((*com[0][i]).c_str());
                // }
                // arg[(int)sizeof(*com[0])]=NULL;
                // execvp((*com[0][0]).c_str(),arg);
                execvp(com[0][0],com[0]);
            }
            else{
                wait(NULL);
                f(com[0],10);
                if(wnum){
					if(fork()==0){
                        // cout<<"rty";
						f(com[1]+len[1],10-len[0]);
						close(pipefd[1]);
						dup2(pipefd[0],0);
						close(pipefd[0]);
                        // char *arg[256]={};
                        // // genarg(arg,*com[0]);
                        // cout<<"7"<<arg<<" ";
                        // for(int i=0;i<(int)sizeof(*com[1]);i++){
                        //     arg[i] = strdup((*com[1][i]).c_str());
                        // }
                        // arg[(int)sizeof(*com[1])]=NULL;
                        // execvp((*com[1][0]).c_str(),arg);
                        execvp(com[1][0],com[1]);
                    }
					else{
                        // cout<<"asd";
						f(com[1],10);
						close(pipefd[1]);
						close(pipefd[0]);
						wait(NULL);
					}
				}
            }
        }
    }
    return 0;
}

const vector<string> split(const string &str, const char &delimiter) {
    vector<string> result;
    stringstream ss(str);
    string tok;

    while (getline(ss, tok, delimiter)) {
        result.push_back(tok);
    }
    return result;
}

void f(char **str,int l){
	for(int i=0;i<l;i++)
	{
		free(str[i]);
		str[i]=NULL;
	}
}


   