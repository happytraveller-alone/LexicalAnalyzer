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
