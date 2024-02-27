#include <stdio.h>


#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include "primary.h"
#include "kernel.h"

char docking_cmd_title[1024]="vina ";

void writing_docking_cmd_title(){

    FILE* config=fopen("config.conf","r");
    if(!config){
        printf("配置文件不存在,请检查配置文件\n");
        exit(60);
    }
    char item[256]="";
    char val[256]="";
    while(fscanf(config,"%s = %s",item,val)){
        if(!strcmp(item,"END_CONFIG")){
            break;
        }
        strcat(docking_cmd_title,"--");
        strcat(docking_cmd_title,item);
        strcat(docking_cmd_title," ");
        strcat(docking_cmd_title,val);
        strcat(docking_cmd_title," ");
    }

    printf("执行对接命令的头部为%s\n",docking_cmd_title);
    fclose(config);
    config=NULL;
}

int main(int argc,char** argv) {
    int doing_num=0;
    //开局处理参数

    if(1==argc||0==strcmp(*(argv+1),"-h")){
        //打印帮助文本
        print_file("help.txt");
        return 0;
    }
    else if(0==strcmp(*(argv+1),"-v")||0== strcmp(*(argv+1),"--version")){
        //打印版本信息
        print_file("version.txt");
        return 0;
    }else if(0== strcmp(*(argv+1),"-s")||0== strcmp(*(argv+1),"--split")){
        doing_num=2;
    } else if(0== strcmp(*(argv+1),"-u")||0== strcmp(*(argv+1),"--unzip")){
        doing_num=1;
    }else if(0== strcmp(*(argv+1),"-d")||0== strcmp(*(argv+1),"--doing")){
        doing_num=3;
        writing_docking_cmd_title();
    }else{
        //没有命中任何正确参数
        printf("参数错误\n");
        print_file("help.txt");
        exit(44);
    }
    //接下来是真正的正式部分
    printf("选择的操作数是%d\n",doing_num);
    //思路
    //第一步 迭代，找到待操作文件的完整路径
    //第二步 到达最深入的部分以后，根据文件路径和操作数，合成命令字符串
    //第三步 根据操作数，进行后处理
#if __linux__
    //函数指针数组
    int(*funcs[3])(const char*,const char* cmd_title)={0};
    *(funcs)=unzip;
    *(funcs+1)=vina_split;//操作数为2
    *(funcs+2)=vina_do;

    int count_files=0;//文件计数器

    DIR* zinc=NULL;//定义zinc文件夹
    zinc=opendir("zinc");
    if(!zinc){
        printf("zinc文件夹不存在\n请进行检查\n");
        exit(45);
    }
    struct dirent* first_level=NULL;
    while(first_level= readdir(zinc)) {
        if (!strcmp(first_level->d_name, ".") || !strcmp(first_level->d_name, "..")) {
            continue;//自身和上一级菜单当然要跳过
        }
        if (first_level->d_type != DT_DIR) {
            printf("找到文件zinc/%s,是一个文件不是目录\n", first_level->d_name);
            continue;//一级目录只处理文件夹
        }
        printf("找到目录zinc/%s\n", first_level->d_name);
        DIR *XX = NULL;
        char XX_addr[256] = "";
        strcat(XX_addr, "zinc/");
        strcat(XX_addr, first_level->d_name);
        XX = opendir(XX_addr);
        struct dirent *second_level = NULL;
        while (second_level = readdir(XX)) {
            if (!strcmp(second_level->d_name, ".") || !strcmp(second_level->d_name, "..")) {
                continue;//自身和上一级菜单当然要跳过
            }

            if (second_level->d_type != DT_DIR) {
                printf("找到文件zinc/%s/%s,是一个文件不是目录\n", first_level->d_name, second_level->d_name);
                continue;//二级目录只处理文件夹
            }
            DIR *YYYY = NULL;
            char YYYY_addr[256] = "";
            strcpy(YYYY_addr, XX_addr);
            strcat(YYYY_addr, "/");
            strcat(YYYY_addr, second_level->d_name);
            YYYY = opendir(YYYY_addr);
            struct dirent *third_level = NULL;
            while (third_level = readdir(YYYY)) {
                //这里是真正的文件了
                if (!strcmp(third_level->d_name, ".") || !strcmp(third_level->d_name, "..")) {
                    continue;//自身和上一级菜单当然要跳过
                }
                char address[1024] = "";
                //合成地址
                strcpy(address, YYYY_addr);
                strcat(address, "/");
                strcat(address, third_level->d_name);
                printf("找到路径%s的文件\n", address);
                //必须排除.out.pdbqt文件
                if(strstr(address,"_out.pdbqt")){
                    printf("此为输出文件,跳过");
                    continue;
                }
                int success_or_not=1;//成功0,错误1
                success_or_not=funcs[doing_num-1](address,docking_cmd_title);
                if(!success_or_not){
                    printf("文件%s处理成功\n",address);
                    count_files++;
                    printf("这是第%d个文件\n",count_files);
                }else{
                    printf("文件处理失败\n");
                }

            }
            closedir(YYYY);
            YYYY = NULL;
        }
        closedir(XX);
        XX = NULL;
    }
    closedir(zinc);
    zinc=NULL;

#else
    printf("暂时不支持非linux客户端");
#endif

    printf("\n感谢使用\n");
    print_file("version.txt");
    exit(0);
}
