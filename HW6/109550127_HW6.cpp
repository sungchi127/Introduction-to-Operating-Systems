#define FUSE_USE_VERSION 30
#include <bits/stdc++.h>
#include <time.h>
#include <fuse.h>
#include <fstream>
#include <cstdlib>
#include <sstream>
#include <unistd.h>
#include <sys/types.h>
// #define TMAGIC "ustar"
using namespace std;

int cnt=0;
//POSIX ustar Archives
typedef struct tar_header_format
{
    char name[100];    
    char mode[8]; 
    char uid[8];      
    char gid[8];     
    char size[12];     
    char mtime[12];    
    char chksum[8];    
    char typeflag;   
    char linkname[100]; 
    char magic[6];     
    char version[2];    
    char uname[32];    
    char gname[32];     
    char devmajor[8];   
    char devminor[8];   
    char prefix[155];   
    char pad[12];     
} tar_header;

class file{
    public:
        string name;
        size_t size, offset;
        int mode;
        unsigned int uid, gid;
        char type;
        time_t m_time;
        file(tar_header *head, size_t pos){
            name = head->name;
            type = head->typeflag;
            offset = pos;
            uid = strtol(head->uid, NULL, 8);
            gid = strtol(head->gid, NULL, 8);
            size = strtol(head->size, NULL, 8);
            mode = strtol(head->mode, NULL, 8);
            m_time = strtol(head->mtime, NULL, 8);
        }
};

static struct fuse_operations op;

class tar{
    private:
        ifstream data;
        size_t size;
        vector<file> files;
        void clear(){
            files.clear();
        }

    public:
        tar(const char *path){
            clear();
            size=0;
            data.open(path);
        }
        void checkUpdate(tar_header *head, size_t pos){
            bool alias = false;
            for(auto &it:files){
                if (it.name == string(head->name)){//找名字相同檔案
                    alias = true;
                    size_t old_time= strtol(head->mtime, NULL, 8);
                    size_t new_time= it.m_time;
                    if (old_time > new_time)//比時間
                        it = file(head, pos);
                }
            }
            if (alias==false)
                files.push_back(file(head, pos));
        }
        bool isTar(){
            if (data.fail())
                return false;
            const int block_size = 512;
            char buffer[block_size] = "\0";
            tar_header *header = (tar_header *)buffer;
            //看檔案總長度
            data.seekg(0, ios::end);
            size = data.tellg();//返回當前指針位置

            size_t pos = 0;
            data.seekg(0, ios::beg);

            while (1){
                data.read(buffer, (int)sizeof(buffer));
                // cout<<"buffer:"<<buffer<<"\n";
                if (data.gcount() != block_size) break;//返回对对象执行的上一次无格式输入操作提取的字符数。
                string header_magic=header->magic;
                header_magic=header_magic.substr(0,5);
                if (header_magic.compare("ustar")!=0) break;//看header的magic來確認

                size_t sz = 0;
                sscanf(header->size, "%lo", &sz); //File size in bytes
                size_t file_block_count = sz + block_size - 1;
                pos += block_size;
                file_block_count /= block_size;

                switch (header->typeflag){
                    case '0':  // intentionally dropping through
                    case '5':  // directory
                    case '\0': // normal file
                        checkUpdate(header, pos);//加分題
                        break;
                    default:
                        break;
                }
                pos += file_block_count * block_size;
                cnt++;
                data.seekg(pos, ios::beg);
            }
            data.seekg(0, ios::beg);
            return true;
        }
        size_t getContent(size_t offset, char *content, size_t size){
            size_t count=0;
            data.seekg(offset, ios::beg);
            data.read(content, size);
            data.seekg(0, ios::beg);
            count = data.gcount();
            return count;
        }
        vector<file> getFiles(){
            return files;
        }
};

tar tar_file("./test.tar");

bool rootOfName(deque<string> path, deque<string> &file){

    while (path.empty()==0){
        if (file.empty()==1)
            return false;
        string temp1 = path.front();
        path.pop_front();
        string temp2 = file.front();
        file.pop_front();
        if (temp1 != temp2)
            return false;
    }
    return true;
}

deque<string> nameToken(stringstream &name){
    deque<string> tokens;
    string token,file_road;
    while (getline(name, token, '/'))
        tokens.push_back(token);
        file_road+=token;
    return tokens;
}

string get_name(string name){
    size_t found = name.rfind("/");
    string filename=name;
    if(found==string::npos){
        filename=name;
        return filename;
    }
    // cout<<name<<":"<<found<<"and"<<(int)filename.length()<<endl;
    if(found+1==(int)filename.length()){
        // filename=filename.substr(0,found);
        filename.pop_back();
        found = filename.rfind("/");
        if(found==string::npos){
            return filename;
        }
        else{
            filename=filename.substr(found+1);
            return filename;
        }
    }
    else{
        filename=filename.substr(found+1);
        return filename;
    }
    return filename;
}

int my_readdir(const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi){
    vector<file> files = tar_file.getFiles();
    stringstream path_name(path+1);
    deque<string> p = nameToken(path_name);
    filler(buffer, ".", NULL, 0);
    filler(buffer, "..", NULL, 0);
    for(auto &it:files){
        stringstream tmp(it.name);
        deque<string> f = nameToken(tmp);

        string name=it.name;
        name=get_name(name);
        if (rootOfName(p, f) && (int)f.size() == 1 ){
            filler(buffer, name.c_str(), NULL, 0);
            cout<<"path name:"<<name<<"\n";
        }
    }
    return 0;
}

int my_getattr(const char *path, struct stat *st){
    memset(st, 0, sizeof(struct stat));
    if (!strcmp(path, "/")){
        st->st_mode = S_IFDIR | 0444;
        return 0;
    }
    vector<file> files = tar_file.getFiles();
    stringstream path_name(path + 1);
    deque<string> p = nameToken(path_name);
    for (auto &it:files){
        stringstream tmp(it.name);
        deque<string> f = nameToken(tmp);
        string name=it.name;
        name=get_name(name);
        if (rootOfName(p, f) && f.empty()==1){
            if(it.type=='5'){
                st->st_mode=S_IFDIR | it.mode;
            }
            else{
                st->st_mode=S_IFREG | it.mode;
            }
            st->st_mtime = it.m_time;
            st->st_uid = it.uid;
            st->st_gid = it.gid;
            st->st_size = it.size;
            return 0;
        }
    }
    return -ENOENT;
}

int my_read(const char *path, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi){
    vector<file> files = tar_file.getFiles();
    stringstream path_name(path + 1);
    deque<string> p = nameToken(path_name);
    for (auto &it:files){
        stringstream tmp(it.name);
        deque<string> f = nameToken(tmp);

        string name=it.name;
        name=get_name(name);
        off_t reoffset;
        size_t retsize;
        if (rootOfName(p, f) && f.empty()==1){
            reoffset=it.offset + offset;
            if(size>it.size){
                retsize=it.size;
            }
            else{
                retsize=size;
            }
            return tar_file.getContent(reoffset, buffer, retsize);
        }
    
    }
    return 0;
}

int main(int argc,char *argv[]){
    if (!tar_file.isTar())
    memset(&op, 0, sizeof(op));
    op.getattr = my_getattr;
    op.readdir = my_readdir;
    op.read = my_read;
    return fuse_main(argc, argv, &op, NULL);
}
