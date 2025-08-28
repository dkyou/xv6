#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void 
searchFile(char* FileName,char* DirPath)
{
    char TargetDirPath[512], *Ptr;
    int Fd;
    struct dirent DirEntry;
    struct stat State;
    
    // 打开指定的路径
    if((Fd = open(DirPath, 0)) < 0)
    {
        fprintf(2, "find: cannot open %s\n", DirPath);
        exit(1);
    }
	
	// 读取对应路径的目录信息
    if(fstat(Fd, &State) < 0)
    {
        fprintf(2, "find: cannot stat %s\n", DirPath);
        close(Fd);
        exit(1);
    }

    switch(State.type)
    {
    	// 如果对应的文件类型是文件，那么结束此次搜索
        case T_FILE:
            fprintf(2, "find: you are supposed to search for a file in a directory rather than a file \n");
            break;
		
		// 如果输入的路径确实指向一个目录，开始搜索
        case T_DIR:
        	// 首先看看目录路径是否过长，这可能导致路径数组溢出
            if(strlen(DirPath) + 1 + DIRSIZ + 1 > sizeof TargetDirPath)
            {
                printf("find: directory's path too long\n");
                break;
            }
            // 将目录路径拷贝过去
            strcpy(TargetDirPath, DirPath);
            Ptr = TargetDirPath + strlen(TargetDirPath);
            *Ptr++ = '/';      
			// 读取这个目录下的文件项                                                                         
            while(read(Fd, &DirEntry, sizeof(DirEntry)) == sizeof(DirEntry))
            {
            	// 如果当前的目录项不包含信息，则跳过
                if(DirEntry.inum == 0)                                                  
                    continue;
                // 否则将新的目录加入到已有的路径中，准备检索
                memmove(Ptr, DirEntry.name, DIRSIZ);
                Ptr[DIRSIZ] = 0;                                                        			
                if(stat(TargetDirPath, &State) < 0)
                {
                    fprintf(2, "find: cannot state %s\n", TargetDirPath);
                    exit(1);
                }
				
				// 找到目标名的文件，注意使用strcmp函数来完成这个任务
                if(strcmp(FileName, DirEntry.name) == 0 && State.type == T_FILE)         
                {
                    fprintf(1, TargetDirPath);
                    fprintf(1, "\n");
                }
                
                // 如果目录之下还是目录，则开始递归搜索
                // 注意要跳过.和..这两个特殊目录
                else if (                                                               
                            State.type == T_DIR &&
                            strcmp(DirEntry.name, ".") != 0 &&
                            strcmp(DirEntry.name, "..") != 0 
                        )
                    searchFile(FileName,TargetDirPath);                               
            }
            break;
    }
    close(Fd);
    return;
}

int 
main(int argc, char* argv[])
{
    if(argc < 3)
    {
        fprintf(2, "find: command needs at least 2 params\n");
        exit(1);
    }
    
    searchFile(argv[1], argv[2]);       // 在主函数中直接调用之即可
    exit(0);
}
