#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fstream>
#include <iomanip>
#include <iostream>

#include "LexAnalysis.h"
#include "SynAnalysis.h" 
 
int main() { 
    // 词法分析的部分 
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

    // LL(1)语法分析过程
    initGrammer();
    First();
    return 0;
}
