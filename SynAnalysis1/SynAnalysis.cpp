/*
 * @Description:
 * @Solution:
 * @Version: 2.0
 * @Author: happytraveller-alone
 * @Date: 2021-04-27 23:03:17
 * @LastEditors: happytraveller-alone
 * @LastEditTime: 2021-04-28 21:07:45
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>

using namespace std;

vector<pair<const char *, int> >
    specialMap;  //文法中的特殊符号映射表，包括-> | $(空)
//读取文件进行处理，实现非终结符，终结符的分类

int main() {
    FILE *infile;
    char ch;
    char array[30];
    char *word;
    int i;
    int codeNum;
    int line = 1;
    int count = 0;
    int orNum = 0;
    infile = fopen("SynAnalysis1\\wenfa.txt", "r");
    if (!infile) {
        printf("文法打开失败！\n");
        return -1;
    }
    //实现
    return 0;
}
