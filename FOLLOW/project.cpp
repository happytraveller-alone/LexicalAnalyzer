/*
 * @Description:
 * @Solution:
 * @Version: 2.0
 * @Author: happytraveller-alone
 * @Date: 2021-04-29 09:37:04
 * @LastEditors: happytraveller-alone
 * @LastEditTime: 2021-04-29 17:15:48
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
    // initNode();
    // scanner();
    // BraMappingError();
    // printNodeLink();
    // printErrorLink();
    // printIdentLink();
    // printNode1();
    // printNode2();

    // LL(1)�﷨��������
    initGrammer();
    First();
    return 0;
}
