/*
 * @Description:
 * @Solution:
 * @Version: 2.0
 * @Author: happytraveller-alone
 * @Date: 2021-04-29 23:32:45
 * @LastEditors: happytraveller-alone
 * @LastEditTime: 2021-04-30 14:47:41
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fstream>
#include <iomanip>
#include <iostream>

#include "LexAnalysis.h"
#include "SynAnalysis.h"

int main() {
    // �ʷ������Ĳ���
    initKeyMapping();
    initOperMapping();
    initLimitMapping();
    initAddMap();
    initNode();
    scanner();
    BraMappingError();
    printNodeLink();
    printErrorLink();
    // printIdentLink();
    printNode1();
    printNode2();

    // LL(1)�﷨��������
    initGrammer();
    First();
    Follow();
    Select();
    MTable();
    Analysis();
    return 0;
}
