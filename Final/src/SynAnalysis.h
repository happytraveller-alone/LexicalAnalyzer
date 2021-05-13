/*
 * @Description:
 * @Solution:
 * @Version: 2.0
 * @Author: happytraveller-alone
 * @Date: 2021-04-29 23:32:45
 * @LastEditors: happytraveller-alone
 * @LastEditTime: 2021-04-30 17:56:55
 */

#ifndef _SYNANALYSIS_H
#define _SYNANALYSIS_H

#define GRAMMAR_ARROW 2000    //->
#define GRAMMAR_NULL 2002     //��ֵ
#define GRAMMAR_SPECIAL 2003  //�������#
#define GRAMMAR_BASE 2010     //��̬���ɵĻ�ֵ

#define Stack_Size 5000

typedef struct {
    int elem[Stack_Size];
    int top;
} SeqStack;

void initGrammer();
int seekCodeNum(char* word);
void First();
void Follow();
void Select();
void MTable();
void Analysis();
#endif
