/*
 * @Description:
 * @Solution:
 * @Version: 2.0
 * @Author: happytraveller-alone
 * @Date: 2021-04-27 10:34:42
 * @LastEditors: happytraveller-alone
 * @LastEditTime: 2021-04-27 11:31:18
 */
#ifndef _SYNANALYSIS_H
#define _SYNANALYSIS_H

#define GRAMMAR_ARROW 2000    //->
#define GRAMMAR_OR 2001       // |
#define GRAMMAR_NULL 2002     //空值
#define GRAMMAR_SPECIAL 2003  //特殊符号#
#define GRAMMAR_BASE 2010     //动态生成的基值

#define Stack_Size 5000

typedef struct {
    int elem[Stack_Size];
    int top;
} SeqStack;

void initGrammer();
int seekCodeNum(char* word);
void ceshi();
void First();
void Follow();
void Select();
void MTable();
void Analysis();
#endif
