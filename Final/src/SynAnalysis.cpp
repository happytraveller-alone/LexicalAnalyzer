#include "SynAnalysis.h"

#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>

#include "LexAnalysis.h"

using namespace std;

#define Max_Proc 500
#define Max_Length 500  //指拆分后产生式的最大长度

#define Max_NonTer 60
#define Max_Ter 60
#define Max_Length2 100

int procNum = 0;
//产生式的数组，里边存储了终结符或者非终结符对应的syn，proc的维数都是从1开始的
int proc[Max_Proc][Max_Length];  //行号：产生式

// FISRT集合
int first[Max_Proc][Max_Length];  // FIRST 位置：非终结符编号
int firstVisit[Max_Proc];  //记录某非终结符的First集是否已经求过
int emptyRecu[Max_Proc];  //在求可推出空的非终结符的编号集时使用的防治递归的集合
int EmptyStore[Max_Proc];  //可推出空的非终结符的编号

// FOLLOW集合
int follow[Max_Proc][Max_Length];
int connectFirst[Max_Length];  //将某些First集结合起来的集合
int followVisit[Max_Proc];  //记录某非终结符的Follow集是否已经求过
int followRecu[Max_Proc];   //在求Follow集时使用的防治递归的集合

// 构造预测分析表
int select[Max_Proc][Max_Length];
int M[Max_NonTer][Max_Ter][Max_Length2];

extern NormalNode *normalHead;  //首结点

fstream resultfile;

// extern的部分代表可能出现的终结符和其编号,extern表示变量可能出现在别的文件中
extern vector<pair<const char *, int> > keyMap;
extern vector<pair<const char *, int> > operMap;
extern vector<pair<const char *, int> > limitMap;
extern vector<pair<const char *, int> > AddMap;

vector<pair<const char *, int> > nonTerMap;  //非终结符映射表,不可重复的
vector<pair<const char *, int> > terMap;  //终结符映射表,不可重复的
vector<pair<const char *, int> >
    specialMap;  //文法中的特殊符号映射表，包括-> | $(空)

/*
 *函数名称：initSpecialMapping
 *参数：无
 *实现功能：对文法中的约定符号进行处理->, 空($)、#
 *最近更改时间：4/30
 *更改作者：谢远峰
 *完成情况：DONE
 */
void initSpecialMapping() {
    specialMap.clear();
    specialMap.push_back(make_pair("->", GRAMMAR_ARROW));
    specialMap.push_back(make_pair("$", GRAMMAR_NULL));
    specialMap.push_back(make_pair("#", GRAMMAR_SPECIAL));
}

/*
 *函数名称：dynamicNonTer
 *参数：TOKEN的起始字符
 *实现功能：动态生成非终结符放入map，在基点的基础上，确保不和终结符冲突，返回种别号
 *最近更改时间：4/30
 *更改作者：谢远峰
 *完成情况：DONE
 */
int dynamicNonTer(char *word) {
    int i = 0;
    int dynamicNum;
    //先判断在不在非终结符map中
    for (i = 0; i < int(nonTerMap.size()); i++) {
        if (strcmp(word, nonTerMap[i].first) == 0) {
            return nonTerMap[i].second;
        }
    }
    //如果此非终结符不在Map中
    if (i == int(nonTerMap.size())) {
        if (i == 0) {  //相当于表为空，初始化，添加首结点
            dynamicNum = GRAMMAR_BASE;  //基地址
            nonTerMap.push_back(make_pair(word, dynamicNum));
        } else {
            dynamicNum = nonTerMap[int(nonTerMap.size()) - 1].second +
                         1;  //后面地址依次增加
            nonTerMap.push_back(make_pair(word, dynamicNum));
        }
    }
    return dynamicNum;
}

/*
 *函数名称：seekCodeNum
 *参数：TOKEN的起始字符
 *实现功能：判断文法中提取的符号是不是终结符，符合存入map返回种别号，否则调用dynamicNoter处理非终结符
 *最近更改时间：4/30
 *更改作者：谢远峰
 *完成情况：DONE
 */
int seekCodeNum(char *word) {
    //处理文法中的特殊符号
    for (int i = 0; i < int(specialMap.size()); i++) {
        if (strcmp(word, specialMap[i].first) == 0) {
            return specialMap[i].second;
        }
    }
    //先搜索终结符映射表中有没有此终结符key，op，limt都是终结符
    for (int i = 0; i < int(terMap.size()); i++) {
        if (strcmp(word, terMap[i].first) == 0) {
            return terMap[i].second;
        }
    }
    for (int i = 0; i < int(keyMap.size()); i++) {
        if (strcmp(word, keyMap[i].first) == 0) {
            terMap.push_back(make_pair(word, keyMap[i].second));
            return keyMap[i].second;
        }
    }

    for (int i = 0; i < int(operMap.size()); i++) {
        if (strcmp(word, operMap[i].first) == 0) {
            terMap.push_back(make_pair(word, operMap[i].second));
            return operMap[i].second;
        }
    }

    for (int i = 0; i < int(limitMap.size()); i++) {
        if (strcmp(word, limitMap[i].first) == 0) {
            terMap.push_back(make_pair(word, limitMap[i].second));
            return limitMap[i].second;
        }
    }

    for (int i = 0; i < int(AddMap.size()); i++) {
        if (strcmp(word, AddMap[i].first) == 0) {
            terMap.push_back(make_pair(word, AddMap[i].second));
            return AddMap[i].second;
        }
    }

    return dynamicNonTer(word);
}

/*
 *函数名称：searchMapping
 *参数：种别号
 *实现功能：通过编号匹配内容
 *最近更改时间：4/30
 *更改作者：谢远峰
 *完成情况：DONE
 */
const char *searchMapping(int num) {
    //标志符
    if (num == IDN) {
        return "IDN";
    }
    //处理文法中的特殊符号
    for (int i = 0; i < int(specialMap.size()); i++) {
        if (specialMap[i].second == num) {
            return specialMap[i].first;
        }
    }
    //处理非终结符
    for (int i = 0; i < int(nonTerMap.size()); i++) {
        if (nonTerMap[i].second == num) {
            return nonTerMap[i].first;
        }
    }
    //处理终结符
    for (int i = 0; i < int(terMap.size()); i++) {
        if (terMap[i].second == num) {
            return terMap[i].first;
        }
    }
    return "wrong";
}

/*
 *函数名称：initGrammer
 *参数：无
 *实现功能：读取处理文法，将产生式拆分以后存入到了proc中输出，并且填入了终结符和非终结符MAP输出
 *最近更改时间：4/30
 *更改作者：谢远峰
 *完成情况：DONE
 */
void initGrammer() {
    FILE *infile;
    char ch;
    char array[30];
    char *word;
    int i;
    int codeNum;
    int line = 1;
    int count = 0;  //是文法读入过程中当前行读到了第几个文法单元
    infile = fopen("testtxt\\wenfa.txt", "r");
    if (!infile) {
        printf("文法打开失败！\n");
        return;
    }
    initSpecialMapping();
    nonTerMap.clear();
    terMap.clear();

    memset(proc, -1, sizeof(proc));

    memset(first, -1, sizeof(first));
    memset(firstVisit, 0, sizeof(firstVisit));  //非终结符的first集还未求过
    memset(EmptyStore, -1, sizeof(EmptyStore));
    memset(emptyRecu, -1, sizeof(emptyRecu));

    memset(follow, -1, sizeof(follow));
    memset(connectFirst, -1, sizeof(connectFirst));
    memset(followVisit, 0, sizeof(followVisit));  //非终结符的follow集还未求过
    memset(followRecu, -1, sizeof(followRecu));

    memset(select, -1, sizeof(select));
    memset(M, -1, sizeof(M));

    ch = fgetc(infile);
    i = 0;
    while (ch != EOF) {
        i = 0;
        // 读取一个产生式中的单元
        while (ch == ' ' || ch == '\t') {
            ch = fgetc(infile);
        }
        while (ch != ' ' && ch != '\n' && ch != EOF) {
            array[i++] = ch;  // array一个连续的字符串（不被空格换行等分割）
            ch = fgetc(infile);
        }
        while (ch == ' ' || ch == '\t') {
            ch = fgetc(infile);
        }
        // 读取一个产生式中的单元
        word = new char[i + 1];
        memcpy(word, array, i);
        word[i] = '\0';
        codeNum = 0;
        codeNum = seekCodeNum(word);  //对此单元进行是否为终结符的分类和存入MAP
        if (codeNum != 0) {
            count++;
            proc[line][count] =
                codeNum;  //将此字符存在产生式数组中，此时的line是txt中文法的line
        }
        //原本需要回退一个字符，由于是冗余字符，不回退
        if (ch == '\n') {  //一行读完以后才需要进行拆分工作
            count = 0;
            // orNum = 0;
            line++;
            ch = fgetc(infile);
        }
    }
    procNum = line;

    fstream outfile0;
    outfile0.open("OutputFile\\grammar.txt", ios::out);
    outfile0 << "***************Grammar Table******************" << endl;
    for (int i = 1; i <= line; i++) {
        for (int j = 1; j < Max_Length; j++) {
            if (proc[i][j] != -1) {
                outfile0 << searchMapping(proc[i][j]);
            } else {
                break;
            }
        }
        outfile0 << endl;
    }
    outfile0.close();

    // 输出终结符到文件
    fstream outfile1;
    outfile1.open("OutputFile\\terminal.txt", ios::out);
    outfile1 << "terminal list:" << endl;
    for (int i = 0; i < int(terMap.size()); i++) {
        outfile1 << terMap[i].first << endl;
    }
    outfile1.close();

    // 输出非终结符到文件
    fstream outfile2;
    outfile2.open("OutputFile\\nonterminal.txt", ios::out);
    outfile2 << "nonterminal list:" << endl;
    for (int i = 0; i < int(nonTerMap.size()); i++) {
        outfile2 << nonTerMap[i].first << endl;
    }
    outfile2.close();
}

/*
 *函数名称：inTer
 *参数：种别号
 *实现功能：判断某个标号是不是终结符的标号，1代表是，0代表否
 *最近更改时间：4/30
 *更改作者：谢远峰
 *完成情况：DONE
 */
int inTer(int n) {
    for (int i = 0; i < int(terMap.size()); i++) {
        if (terMap[i].second == n) {
            return 1;
        }
    }
    return 0;
}

/*
 *函数名称：inNonTer
 *参数：TOKEN的起始字符
 *实现功能：判断某个标号是不是非终结符的标号,1代表是，0代表否
 *最近更改时间：4/30
 *更改作者：谢远峰
 *完成情况：DONE
 */
int inNonTer(int n) {
    for (int i = 0; i < int(nonTerMap.size()); i++) {
        if (nonTerMap[i].second == n) {
            return 1;
        }
    }
    return 0;
}

/*
 *函数名称：inEmpty
 *参数：种别号
 *实现功能：判断某个标号在不在此时的empty集中，1代表是，0代表否
 *最近更改时间：4/30
 *更改作者：谢远峰
 *完成情况：DONE
 */
int inEmpty(int n) {
    //当前Empty集的长度
    int emptyLength = 0;
    for (emptyLength = 0;; emptyLength++) {
        if (EmptyStore[emptyLength] == -1) {
            break;
        }
    }
    for (int i = 0; i < emptyLength; i++) {
        if (EmptyStore[i] == n) {
            return 1;
        }
    }
    return 0;
}

/*
 *函数名称：inEmptyRecu
 *参数：种别号
 *实现功能：判断某个标号在不在此时的emptyRecu集中，1代表是，0代表否
 *最近更改时间：4/30
 *更改作者：谢远峰
 *完成情况：DONE
 */
int inEmptyRecu(int n) {
    //当前Empty集的长度
    int emptyLength = 0;
    for (emptyLength = 0;; emptyLength++) {
        if (emptyRecu[emptyLength] == -1) {
            break;
        }
    }
    for (int i = 0; i < emptyLength; i++) {
        if (emptyRecu[i] == n) {
            return 1;
        }
    }
    return 0;
}

/*
 *函数名称：merge
 *参数：Fisrt[i]集合，First[j]集合，传入类型
 *实现功能：将s集合合并至d集合中，type = 1代表包括空（$）,type = 2代表不包括空
 *最近更改时间：4/30
 *更改作者：谢远峰
 *完成情况：DONE
 */
void merge(int *d, int *s, int type) {
    int flag = 0;
    for (int i = 0;; i++) {
        flag = 0;
        if (s[i] == -1) {
            break;
        }
        int j = 0;
        for (j = 0;; j++) {
            if (d[j] == -1) {
                break;
            }
            if (d[j] == s[i]) {
                flag = 1;
                break;
            }
        }
        if (flag == 1) {
            continue;
        }
        if (type == 1) {
            d[j] = s[i];
        } else {
            if (s[i] != GRAMMAR_NULL) {
                d[j] = s[i];
            }
        }
        d[j + 1] = -1;
    }
}

/*
 *函数名称：nullSet
 *参数：种别号
 *实现功能：导入能直接推出空的非终结符
 *局限性：A->BC B->空 C->空 无法得到A 能推出空 产生式右端只有一个单元
 *最近更改时间：4/30
 *更改作者：谢远峰
 *完成情况：DONE
 */
void nullSet(int currentNum) {
    int temp[2];
    for (int j = 1; j <= procNum; j++) {
        //如果右边的第一个是该字符，并且长度只有1
        if (proc[j][3] == currentNum && proc[j][4] == -1) {
            temp[0] = proc[j][1];
            temp[1] = -1;
            merge(EmptyStore, temp, 1);
            //递归判定 A->B、B->空
            nullSet(proc[j][1]);
        }
    }
}

/*
 *函数名称：nullSet
 *参数：种别号
 *实现功能：判断该非终结符是否能推出空
 *局限性：A->BC B->空 C->空 无法得到A 能推出空 产生式右端只有一个单元
 *最近更改时间：4/30
 *更改作者：谢远峰
 *完成情况：DONE
 */
int reduNull(int currentNon) {
    int temp[2];
    int result = 1;
    int mark = 0;
    temp[0] = currentNon;
    temp[1] = -1;
    merge(emptyRecu, temp, 1);  //先将此符号并入防递归集合中
    if (inEmpty(currentNon) == 1) {
        return 1;
    }

    for (int j = 1; j <= procNum; j++) {
        if (proc[j][1] == currentNon) {
            int rightLength = 0;
            //先求出右部的长度
            for (rightLength = 3;; rightLength++) {
                if (proc[j][rightLength] == -1) {
                    break;
                }
            }
            rightLength--;
            //如果长度为1，并且已经求过
            if (rightLength - 2 == 1 && inEmpty(proc[j][rightLength])) {
                return 1;
            }
            //如果长度为1，并且是终结符
            else if (rightLength - 2 == 1 && inTer(proc[j][rightLength])) {
                return 0;
            }
            //如果长度超过了2
            else {
                for (int k = 3; k <= rightLength; k++) {
                    if (inEmptyRecu(proc[j][k])) {
                        mark = 1;
                    }
                }
                if (mark == 1) {
                    continue;
                } else {
                    for (int k = 3; k <= rightLength; k++) {
                        result *= reduNull(proc[j][k]);
                        temp[0] = proc[j][k];
                        temp[1] = -1;
                        merge(emptyRecu, temp, 1);  //先将此符号并入防递归集合中
                    }
                }
            }
            if (result == 0) {
                continue;
            } else if (result == 1) {
                return 1;
            }
        }
    }
    return 0;
}

/*
 *函数名称：firstSet
 *参数：
 *实现功能：求first集，传入的参数是在非终结符集合中的序号
 *最近更改时间：4/30
 *更改作者：张一鸣
 *完成情况：DONE
 */
void firstSet(int i) {
    int k = 0;
    int currentNon = nonTerMap[i].second;  //当前的非终结符标号
    //依次遍历全部产生式
    // cout<<"         "<<nonTerMap[i].first<<endl;
    for (int j = 1; j <= procNum; j++)  // j代表第几个产生式
    {
        //找到该非终结符的产生式
        if (currentNon == proc[j][1])  //注意从1开始
        {
            // if (i == 12) cout << nonTerMap[i].first << endl;
            //当右边的第一个是终结符或者空的时候
            if (inTer(proc[j][3]) == 1 || proc[j][3] == GRAMMAR_NULL) {
                //并入当前非终结符的first集中
                // if (i == 12) cout << "              " << proc[j][3] << endl;
                int temp[2];
                temp[0] = proc[j][3];
                temp[1] = -1;  //其实是模拟字符串操作的手段
                merge(first[i], temp, 1);
            }
            //当右边的第一个是非终结符的时候
            else if (inNonTer(proc[j][3]) == 1) {
                //如果遇到左递归形式的，这相当于A->A...跳过了简单的左递归问题
                if (proc[j][3] == currentNon) {
                    continue;
                }
                //记录下右边第一个非终结符的位置
                for (k = 0;; k++) {
                    if (nonTerMap[k].second == proc[j][3]) {
                        break;
                    }
                }
                //当右边第一个非终结符还未访问过的时候
                if (firstVisit[k] == 0) {
                    firstSet(k);
                    firstVisit[k] = 1;
                }
                merge(
                    first[i], first[k],
                    2);  //如果first[k]此时有空值的话，暂时不把空值并入first[i]中
                int rightLength = 0;
                //先求出右部的长度
                for (rightLength = 3;; rightLength++) {
                    if (proc[j][rightLength] == -1) {
                        break;
                    }
                }
                //到目前为止，只求出了右边的第一个(还不包括空的部分)，For循环处理之后的
                for (k = 3; k < rightLength; k++) {
                    emptyRecu[0] = -1;  //相当于初始化这个防递归集合？？？？
                    //如果右部的当前字符能推出空并且还不是最后一个字符，就将之后的一个字符并入First集中
                    if (reduNull(proc[j][k]) == 1 && k < rightLength - 1) {
                        int u = 0;
                        int tornot = 0;
                        for (u = 0;; u++) {
                            //注意是记录下一个符号的位置
                            if (nonTerMap[u].second == proc[j][k + 1]) {
                                tornot = 2;
                                break;
                            }
                            if (terMap[u].second == proc[j][k + 1]) {
                                tornot = 1;
                                break;
                            }
                        }
                        if (tornot == 2) {
                            if (firstVisit[u] == 0) {
                                firstSet(u);
                                firstVisit[u] = 1;
                            }
                            merge(first[i], first[u], 2);
                        } else if (tornot == 1) {
                            int temp[2];
                            temp[0] = proc[j][k + 1];
                            temp[1] = -1;  //其实是模拟字符串操作的手段
                            merge(first[i], temp, 1);
                        }
                    }
                    // A->BCd
                    // k=B虽然刚才B已经求过frist并加入，但是C要加入的需要判断
                    // B的是不是能推出空，所以k还是从B开始，但是实际上考察的是C
                    //到达最后一个字符，并且产生式右部都能推出空,将$并入First集中
                    else if (reduNull(proc[j][k]) == 1 &&
                             k == rightLength - 1) {
                        int temp[2];
                        temp[0] = GRAMMAR_NULL;
                        temp[1] = -1;  //其实是模拟字符串操作的手段
                        merge(first[i], temp, 1);
                    } else {
                        break;
                    }
                }
            }
        }
    }
    firstVisit[i] = 1;
}

/*
 *函数名称：First
 *参数：
 *实现功能： first构造
 *最近更改时间：4/30
 *更改作者：张一鸣
 *完成情况：DONE
 */
void First() {
    //先求出能直接推出空的非终结符集合
    nullSet(GRAMMAR_NULL);  // OK
    for (int i = 0; i < int(nonTerMap.size()); i++) {
        firstSet(i);
    }

    fstream outfile3;
    outfile3.open("OutputFile\\first.txt", ios::out);
    outfile3 << "first list:" << endl;
    for (int i = 0; i < int(nonTerMap.size()); i++) {
        outfile3 << "First[" << nonTerMap[i].first << "] = ";
        for (int j = 0;; j++) {
            if (first[i][j] == -1) {
                break;
            }
            outfile3 << searchMapping(first[i][j]) << " ";
        }
        outfile3 << endl;
    }
    outfile3.close();
}

/*
 *函数名称：inFollowRecu
 *参数：行号
 *实现功能：判断某个标号在不在此时的followRecu集中，1代表是，0代表否
 *最近更改时间：4/30
 *更改作者：张一鸣
 *完成情况：DONE
 */
int inFollowRecu(int n) {
    int followLength = 0;
    for (followLength = 0;; followLength++) {
        if (followRecu[followLength] == -1) {
            break;
        }
    }
    for (int i = 0; i < followLength; i++) {
        if (followRecu[i] == n) {
            return 1;
        }
    }
    return 0;
}

/*
 *函数名称：inProcRight
 *参数：表达式指针和行号
 *实现功能：判断某个标号是不是在产生式的右边
 *最近更改时间：4/30
 *更改作者：张一鸣
 *完成情况：DONE
 */
int inProcRight(int n, int *p) {
    //注意这里默认是从3开始
    for (int i = 3;; i++) {
        if (p[i] == -1) {
            break;
        }
        if (p[i] == n) {
            return 1;
        }
    }
    return 0;
}

/*
 *函数名称：connectFirstSet
 *参数：
 *实现功能：将First结合起来的函数
 *最近更改时间：4/30
 *更改作者：张一鸣
 *完成情况：DONE
 */
void connectFirstSet(int *p) {
    int i = 0;
    int flag = 0;
    int temp[2];
    //如果P的长度为1
    if (p[1] == -1) {
        if (p[0] == GRAMMAR_NULL) {
            connectFirst[0] = GRAMMAR_NULL;
            connectFirst[1] = -1;
        } else {
            for (i = 0; i < int(nonTerMap.size()); i++) {
                if (nonTerMap[i].second == p[0]) {
                    flag = 1;
                    merge(connectFirst, first[i], 1);
                    break;
                }
            }
            //也可能是终结符
            if (flag == 0) {
                for (i = 0; i < int(terMap.size()); i++) {
                    if (terMap[i].second == p[0]) {
                        temp[0] = terMap[i].second;
                        temp[1] = -1;
                        merge(connectFirst, temp,
                              2);  //终结符的First集就是其本身
                        break;
                    }
                }
            }
        }
    }
    //如果p的长度大于1
    else {
        for (i = 0; i < int(nonTerMap.size()); i++) {
            if (nonTerMap[i].second == p[0]) {
                flag = 1;
                merge(connectFirst, first[i], 2);
                break;
            }
        }
        //也可能是终结符
        if (flag == 0) {
            for (i = 0; i < int(terMap.size()); i++) {
                if (terMap[i].second == p[0]) {
                    temp[0] = terMap[i].second;
                    temp[1] = -1;
                    merge(connectFirst, temp, 2);  //终结符的First集就是其本身
                    break;
                }
            }
        }
        flag = 0;
        int length = 0;
        for (length = 0;; length++) {
            if (p[length] == -1) {
                break;
            }
        }
        for (int k = 0; k < length; k++) {
            emptyRecu[0] = -1;  //相当于初始化这个防递归集合

            //如果右部的当前字符能推出空并且还不是最后一个字符，就将之后的一个字符并入First集中
            if (reduNull(p[k]) == 1 && k < length - 1) {
                int u = 0;
                for (u = 0; u < int(nonTerMap.size()); u++) {
                    //注意是记录下一个符号的位置
                    if (nonTerMap[u].second == p[k + 1]) {
                        flag = 1;
                        merge(connectFirst, first[u], 2);
                        break;
                    }
                }
                //也可能是终结符
                if (flag == 0) {
                    for (u = 0; u < int(terMap.size()); u++) {
                        //注意是记录下一个符号的位置
                        if (terMap[u].second == p[k + 1]) {
                            temp[0] = terMap[i].second;
                            temp[1] = -1;
                            merge(connectFirst, temp, 2);
                            break;
                        }
                    }
                }
                flag = 0;
            }
            //到达最后一个字符，并且产生式右部都能推出空,将$并入First集中
            else if (reduNull(p[k]) == 1 && k == length - 1) {
                temp[0] = GRAMMAR_NULL;
                temp[1] = -1;  //其实是模拟字符串操作的手段
                merge(connectFirst, temp, 1);
            } else {
                break;
            }
        }
    }
}

/*
 *函数名称：followSet
 *参数：
 *实现功能：Follow集处理
 *最近更改时间：4/30
 *更改作者：张一鸣
 *完成情况：DONE
 */
void followSet(int i) {
    int currentNon = nonTerMap[i].second;  //当前的非终结符标号
    int temp[2];
    int result = 1;
    temp[0] = currentNon;
    temp[1] = -1;
    merge(followRecu, temp, 1);  //将当前标号加入防递归集合中

    //如果当前符号就是开始符号,把特殊符号加入其Follow集中
    if (proc[1][1] == currentNon) {
        temp[0] = GRAMMAR_SPECIAL;  //这个也是要处理的
        temp[1] = -1;
        merge(follow[i], temp, 1);
    }
    for (int j = 1; j <= procNum; j++)  // j代表第几个产生式
    {
        //如果该非终结符在某个产生式的右部存在
        if (inProcRight(currentNon, proc[j]) == 1) {
            int rightLength = 1;
            int k = 0;  // k为该非终结符在产生式右部的序号
            // int flag = 0;
            int leftNum = proc[j][1];  //产生式的左边
            int h = 0;
            int kArray[Max_Length2];
            memset(kArray, -1, sizeof(kArray));
            // 得到产生式左边的在非终结符表中的下标
            for (h = 0; h < int(nonTerMap.size()); h++) {
                if (nonTerMap[h].second == leftNum) {
                    break;
                }
            }
            for (rightLength = 1;; rightLength++) {
                if (currentNon == proc[j][rightLength + 2]) {
                    kArray[k++] = rightLength;
                }
                if (proc[j][rightLength + 2] == -1) {
                    break;
                }
            }
            rightLength--;
            for (int y = 0;; y++) {
                if (kArray[y] == -1) {
                    break;
                }
                //如果该非终结符在右部产生式的最后
                if (kArray[y] == rightLength) {
                    if (inFollowRecu(leftNum) == 1) {
                        merge(follow[i], follow[h], 1);
                        continue;
                    }
                    if (followVisit[h] == 0) {
                        followSet(h);
                        followVisit[h] = 1;
                    }
                    merge(follow[i], follow[h], 1);
                }
                //如果不在最后
                else {
                    int n = 0;
                    result = 1;  //这是关键的，曾在这里失误过
                    for (n = kArray[y] + 1; n <= rightLength; n++) {
                        emptyRecu[0] = -1;
                        result *= reduNull(proc[j][n + 2]);
                    }
                    if (result == 1) {
                        if (inFollowRecu(leftNum) == 1) {
                            merge(follow[i], follow[h], 1);
                            continue;
                        }
                        if (followVisit[h] == 0) {
                            followSet(h);
                            followVisit[h] = 1;
                        }
                        merge(follow[i], follow[h], 1);
                    }
                    int temp2[Max_Length];
                    memset(temp2, -1, sizeof(temp2));
                    for (n = kArray[y] + 1; n <= rightLength; n++) {
                        temp2[n - kArray[y] - 1] = proc[j][n + 2];
                    }
                    temp2[rightLength - kArray[y]] = -1;
                    connectFirst[0] = -1;  //应该重新初始化一下
                    connectFirstSet(temp2);
                    merge(follow[i], connectFirst, 2);
                }
            }
        }
    }
    followVisit[i] = 1;
}

/*
 *函数名称：Follow
 *参数：
 *实现功能：求所有非终结符的Follow集
 *最近更改时间：4/30
 *更改作者：张一鸣
 *完成情况：DONE
 */
void Follow() {
    for (int i = 0; i < int(nonTerMap.size()); i++) {
        followRecu[0] = -1;
        followSet(i);
    }

    fstream outfile4;
    outfile4.open("OutputFile\\follow.txt", ios::out);
    for (int i = 0; i < int(nonTerMap.size()); i++) {
        outfile4 << "Follow[" << nonTerMap[i].first << "] = ";
        for (int j = 0;; j++) {
            if (follow[i][j] == -1) {
                break;
            }
            outfile4 << searchMapping(follow[i][j]) << " ";
        }
        outfile4 << endl;
    }
    outfile4.close();
}

/*
 *函数名称：Select
 *参数：
 *实现功能：求已经分解的产生式对应的Select集,注意Select集中不能含有空($),因而Type=2
 *最近更改时间：4/30
 *更改作者：张一鸣
 *完成情况：DONE
 */
void Select() {
    for (int i = 1; i <= procNum; i++)  // j代表第几个产生式
    {
        int leftNum = proc[i][1];  //产生式的左边
        int h = 0;
        int result = 1;
        for (h = 0; h < int(nonTerMap.size()); h++) {
            if (nonTerMap[h].second == leftNum) {
                break;
            }
        }

        int rightLength = 1;
        for (rightLength = 1;; rightLength++) {
            if (proc[i][rightLength + 2] == -1) {
                break;
            }
        }
        rightLength--;
        //如果右部推出式的长度为1并且是空,select[i-1] = follow[左边]
        if (rightLength == 1 && proc[i][rightLength + 2] == GRAMMAR_NULL) {
            merge(select[i - 1], follow[h], 2);
        }
        //如果右部不是空的时候,select[i-1] = first[右部全部]
        //如果右部能够推出空，select[i-1] = first[右部全部] ^ follow[左边]
        else {
            int temp2[Max_Length];
            int n = 0;
            memset(temp2, -1, sizeof(temp2));
            for (n = 1; n <= rightLength; n++) {
                temp2[n - 1] = proc[i][n + 2];
            }
            temp2[rightLength] = -1;
            connectFirst[0] = -1;  //应该重新初始化一下
            connectFirstSet(temp2);
            merge(select[i - 1], connectFirst, 2);
            for (n = 1; n <= rightLength; n++) {
                emptyRecu[0] = -1;
                result *= reduNull(proc[i][n + 2]);
            }
            //如果右部能推出空，将follow[左边]并入select[i-1]中
            if (result == 1) {
                merge(select[i - 1], follow[h], 2);
            }
        }
    }
}

/*
 *函数名称：MTable
 *参数：
 *实现功能：输出预测分析表
 *最近更改时间：4/30
 *更改作者：张一鸣
 *完成情况：DONE
 */
void MTable() {
    fstream outfile;
    outfile.open("OutputFile\\preciateTable.csv", ios::out);

    for (int i = 0; i < procNum; i++) {
        int m = 0;  //非终结符的序号
        for (int t = 0; t < int(nonTerMap.size()); t++) {
            if (nonTerMap[t].second == proc[i + 1][1]) {
                m = t;
                break;
            }
        }

        for (int j = 0;; j++) {
            if (select[i][j] == -1) {
                break;
            }
            for (int k = 0; k < int(terMap.size()); k++) {
                if (terMap[k].second == select[i][j]) {
                    int n = 0;
                    for (n = 1; n <= Max_Length2; n++) {
                        M[m][k][n - 1] = proc[i + 1][n];
                        if (proc[i + 1][n] == -1) {
                            break;
                        }
                    }
                    break;
                }
            }
        }
    }
    outfile << "预测分析表"
            << ",";
    for (int j = 0; j < int(terMap.size()); j++) {
        if (strcmp(terMap[j].first, ",") == 0)
            outfile << "，"
                    << ",";

        else
            outfile << terMap[j].first << ",";
    }
    outfile << endl;
    for (int i = 0; i < int(nonTerMap.size()); i++) {
        outfile << nonTerMap[i].first << ",";
        for (int j = 0; j < int(terMap.size()); j++) {
            for (int k = 0;; k++) {
                if (M[i][j][k] == -1) {
                    break;
                }
                outfile << searchMapping(M[i][j][k]);
                // printf("%s ",searchMapping(M[i][j][k]));
            }
            outfile << ",";
            // printf("\n");
        }
        outfile << endl;
        // printf("\n\n");
    }
    outfile.close();
}

/*
 *函数名称：InitStack
 *参数：*S 栈指针
 *实现功能：初始化顺序栈
 *最近更改时间：4/30
 *更改作者：张一鸣
 *完成情况：DONE
 */
void InitStack(SeqStack *S) { S->top = -1; }

/*
 *函数名称：Push
 *参数：*S 栈指针 *x 字符指针
 *实现功能：入栈
 *最近更改时间：4/30
 *更改作者：张一鸣
 *完成情况：DONE
 */
int Push(SeqStack *S, int x) {
    if (S->top == Stack_Size - 1) return 0;
    S->top++;
    S->elem[S->top] = x;
    return 1;
}

/*
 *函数名称：Pop
 *参数：*S 栈指针
 *实现功能：出栈
 *最近更改时间：4/30
 *更改作者：张一鸣
 *完成情况：DONE
 */
int Pop(SeqStack *S) {
    if (S->top == -1)
        return 0;
    else {
        S->top--;
        return 1;
    }
}

/*
 *函数名称：GetTop
 *参数：*S 栈指针，*x 字符指针
 *实现功能：取栈顶元素
 *最近更改时间：4/30
 *更改作者：张一鸣
 *完成情况：DONE
 */
int GetTop(SeqStack *S, int *x) {
    if (S->top == -1)
        return 0;
    else {
        *x = S->elem[S->top];
        return 1;
    }
}

/*
 *函数名称：ShowStack1
 *参数：*S 栈指针
 *实现功能：显示栈的字符，先输出栈底元素
 *最近更改时间：4/30
 *更改作者：张一鸣
 *完成情况：DONE
 */
void ShowStack1(SeqStack *S) {
    int i;
    for (i = S->top; i >= 0; i--) {
        // printf("%s ",searchMapping(S->elem[i]));
        resultfile << searchMapping(S->elem[i]) << " ";
    }
}

/*
 *函数名称：ShowStack2
 *参数：*S 栈指针
 *实现功能：显示栈的字符，先输出栈顶元素
 *最近更改时间：4/30
 *更改作者：张一鸣
 *完成情况：DONE
 */
void ShowStack2(SeqStack *S) {
    int i;
    for (i = S->top; i >= 0; i--) {
        // printf("%s ",searchMapping(S->elem[i]));
        resultfile << searchMapping(S->elem[i]) << " ";
    }
}

/*
 *函数名称：Analysis
 *参数：
 *实现功能：分析源程序，对结果输出进行分析
 *最近更改时间：4/30
 *更改作者：张一鸣
 *完成情况：DONE
 */
void Analysis() {
    //分析结果输出
    resultfile.open("OutputFile\\preciateResult.csv", ios::out);

    SeqStack s1, s2;  // 符号栈中间形式 和 输入串
    int c1, c2;
    int i = 0;
    int reserve[Stack_Size];  //符号栈反向入栈
    NormalNode *p = normalHead;
    int s1Length = 0;
    memset(reserve, -1, sizeof(reserve));

    InitStack(&s1); /*初始化符号栈和输入串*/
    InitStack(&s2);
    Push(&s1, GRAMMAR_SPECIAL);
    Push(&s1, proc[1][1]);
    Push(&s2, GRAMMAR_SPECIAL);

    p = p->next;
    while (p != NULL) {
        reserve[i++] = p->type;
        p = p->next;
    }
    //求左边栈的长度
    for (s1Length = 0;; s1Length++) {
        if (reserve[s1Length] == -1) {
            break;
        }
    }
    //反向入栈
    for (i = s1Length; i > 0; i--) {
        Push(&s2, reserve[i - 1]);
    }
    resultfile << "步骤"
               << ","
               << "符号栈"
               << ","
               << "输入栈"
               << ","
               << "判断" << endl;
    for (i = 0;; i++) /*分析*/
    {
        // getch();
        int flag = 0;
        int h1;
        int h2;

        // printf("第%d步：\n",i+1);  /*输出该步的相应信息*/
        resultfile << i + 1 << ",";
        // printf("符号栈:");
        // resultfile << "符号栈:";
        // resultfile << setw(30);
        ShowStack1(&s1);
        // printf("\n");
        resultfile << ",";
        // printf("输入栈:");
        // resultfile << "输入栈:";
        // resultfile << setw(30);
        ShowStack2(&s2);
        resultfile << ",";
        // printf("\n");
        // resultfile << endl;

        GetTop(&s1, &c1); /*取栈顶元素，记为c1，c2*/
        GetTop(&s2, &c2);
        if (c1 == GRAMMAR_SPECIAL &&
            c2 == GRAMMAR_SPECIAL) /*当符号栈和输入栈都剩余#时，分析成功*/
        {
            // printf("成功!\n");
            resultfile << "成功!" << endl;
            break;
        }
        if (c1 == GRAMMAR_SPECIAL &&
            c2 !=
                GRAMMAR_SPECIAL) /*当符号栈剩余#，而输入串未结束时，分析失败 */
        {
            // printf("失败!\n");
            resultfile << "失败!" << endl;
            break;
        }
        if (c1 == c2) /*符号栈的栈顶元素和输入串的栈顶元素相同时，同时弹出*/
        {
            Pop(&s1);
            Pop(&s2);
            flag = 1;
        }

        else /*查预测分析表*/
        {
            //记录下非终结符的位置
            for (h1 = 0; h1 < int(nonTerMap.size()); h1++) {
                if (nonTerMap[h1].second == c1) {
                    break;
                }
            }
            //记录下终结符的位置
            for (h2 = 0; h2 < int(terMap.size()); h2++) {
                if (terMap[h2].second == c2) {
                    break;
                }
            }
            if (M[h1][h2][0] == -1) {
                // printf("Error\n");
                resultfile << "Error" << endl;
                break;  //如果错误的话，直接终止分析
            } else {
                int length = 0;
                //记录下推导式的长度
                for (length = 0;; length++) {
                    if (M[h1][h2][length] == -1) {
                        break;
                    }
                }
                Pop(&s1);
                //如果不是空的话，反向入栈
                if (M[h1][h2][2] != GRAMMAR_NULL) {
                    for (int k = length - 1; k >= 2; k--) {
                        Push(&s1, M[h1][h2][k]);
                    }
                }
            }
        }
        if (flag == 1) {
            // printf("匹配！\n");
            resultfile << "匹配!" << endl;
        } else {
            resultfile << "所用推出式：";
            // printf("所用推出式：");
            int w = 0;
            //记录下推导式的长度
            for (w = 0;; w++) {
                if (M[h1][h2][w] == -1) {
                    break;
                }
                // printf("%s ",searchMapping(M[h1][h2][w]));
                resultfile << searchMapping(M[h1][h2][w]);
            }
            // printf("\n\n");
            resultfile << endl;
        }
    }
    resultfile.close();
}
