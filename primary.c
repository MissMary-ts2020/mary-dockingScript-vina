//
// Created by mary on 2/26/24.
//

#include "primary.h"
#include <stdio.h>

#include <assert.h>
#include <string.h>

int doashell(const char* cmd,char* result,size_t result_size){
    //执行shell命令，联系外部的vina命令
    assert(cmd&&result);
    FILE* result_stream=NULL;
    result_stream=popen(cmd,"r");
    if(!result_stream){
        return -1;//执行失败或者挂载管道失败
    }
    //其实此处命令已经执行完毕了
    size_t size=fread(result,sizeof(char),result_size,result_stream);
    return (int)size;
}

void print_file(char* addr){
    FILE * help_file=NULL;
    help_file=fopen(addr,"r");
    if(!help_file){
        //打开失败
        printf("不存在该文本\n");
        return;
    }
    char help_txt[1000]="";
    int size=(int)fread(help_txt,sizeof(char),sizeof(help_txt)/sizeof(char),help_file);
    printf("文本大小：%d\n%s\n",size,help_txt);
    fclose(help_file);
    help_file=NULL;
    return;
}