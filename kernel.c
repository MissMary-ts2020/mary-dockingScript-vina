//
// Created by mary on 2/26/24.
//

#include "kernel.h"
#include "primary.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int vina_split(const char* addr, __attribute__((unused)) const char* cmd_title){
#if __linux__
    char cmd[1024]="";
    strcat(cmd,"vina_split --input ");
    strcat(cmd,addr);
    int result= system(cmd);
    if(0!=result){
        //报错不删
        return -1;
    }
    strcpy(cmd,"rm ");
    strcat(cmd,addr);
    system(cmd);
    return result;
#else
    printf("暂不支持其他系统\n");
    exit(28);
#endif

}

void after_result(char* result,const char* addr){
    /*
     * 这段有问题
    * char result_line[256]="";//放结论的最后一行
    * char temp[4096]="";//暂时存放不要的东西
    * sscanf(result,"%s-----+------------+----------+----------%s",temp,result_line);
    * char result_char[256]="";//放结论数字
    * sscanf(result_line,"%s1%s",temp,result_char);
    * double result_val=atof(result_char);
    */


    char* line=strtok(result,"\n");//初始化strtok
    //用的时候用NULL
    int mode=0;
    double dG=0;
    double rmsd_l_b=0;
    double rmsd_u_b=0;
    char dG_str[256]="";

    while(line){
        //每一行来读
        if(sscanf(line,"%d %lf %lf %lf",&mode,&dG,&rmsd_l_b,&rmsd_u_b)==4){
            break;
        }
        line=strtok(NULL,"\n");

    }

    printf("本次对接结果为%lf kcal/mol\n",dG);
    sprintf(dG_str,"%lf",dG);
    if(dG<-3.6){
        //只有dG小于-9.6kcal/mol的情况才予以记录
        //写入字符串
        char writing[1024]="";
        strcpy(writing,addr);
        strcat(writing,",");
        strcat(writing,dG_str);
        strcat(writing,"END_LINE");
        FILE* result_csv=fopen("result.csv","a+");
        int success_or_not = fputs(writing, result_csv);
        // 将内容写入文件

        if (success_or_not != EOF) {
            printf("成功写入文件\n");
        } else {
            printf("写入失败\n");
        }
        fclose(result_csv);
        result_csv=NULL;
        //要及时移走out文件
        char mv_cmd[128]="mv ";
        char* position_pdbqt=strstr(addr,".pdbqt");
        strcpy(position_pdbqt,"_out.pdbqt");//这里其实改变了，但愿不是越界访问
        strcat(mv_cmd,addr);
        strcat(mv_cmd," result_out");
        printf("被移动的文件路径为%s\n",addr);
        if(!system(mv_cmd)){
            printf("移动成功\n");
            printf("*****************************\n");
        }else{
            printf("移动失败\n");
        }

    }else{
        //不大于，要及时删除out文件
        char rm_cmd[128]="rm ";
        char* position_pdbqt=strstr(addr,".pdbqt");
        strcpy(position_pdbqt,"_out.pdbqt");
        strcat(rm_cmd,addr);

        printf("被删除的文件路径为%s\n",addr);
        if(!system(rm_cmd)){
            printf("删除成功\n");
            printf("----------------------------\n");
        }else{
            printf("删除失败\n");
        }
    }

    //return;
}

int vina_do(const char* addr,const char* cmd_title){
#if __linux__
    char cmd[1024]="";
    //设计一共有9行
    strcpy(cmd,cmd_title);
    strcat(cmd," --ligand ");
    strcat(cmd,addr);
    char result[4096]="";
    doashell(cmd,result,sizeof(result)/sizeof(char));
    //需要进行后处理
    after_result(result,addr);


    return 0;
#else
    printf("暂不支持其他平台");

#endif
}



int unzip(const char* addr,__attribute__((unused)) const char* cmd_title){
#if __linux__
    char cmd[1024]="";
    strcat(cmd,"gzip -d ");
    strcat(cmd,addr);
    int result= system(cmd);
    //删除原始文件
    strcpy(cmd,"rm ");
    strcat(cmd,addr);
#else
    printf("暂不支持其他系统\n");
    exit(28);
#endif
    return result;
}