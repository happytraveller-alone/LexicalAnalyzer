#include "SynAnalysis.h"

#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fstream>
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

// extern的部分代表可能出现的终结符和其编号,extern表示变量可能出现在别的文件中
extern vector<pair<const char *, int> > keyMap;
extern vector<pair<const char *, int> > operMap;
extern vector<pair<const char *, int> > limitMap;
extern vector<pair<const char *, int> > AddMap;

vector<pair<const char *, int> > nonTerMap;  //非终结符映射表,不可重复的
vector<pair<const char *, int> > terMap;  //终结符映射表,不可重复的
vector<pair<const char *, int> >
    specialMap;  //文法中的特殊符号映射表，包括-> | $(空)

// 对文法中的约定符号进行处理->, 空($)、#
void initSpecialMapping() {
    specialMap.clear();
    specialMap.push_back(make_pair("->", GRAMMAR_ARROW));
    specialMap.push_back(make_pair("$", GRAMMAR_NULL));
    specialMap.push_back(make_pair("#", GRAMMAR_SPECIAL));
}

// ok 动态生成非终结符放入map，在基点的基础上，确保不和终结符冲突，返回syn
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

// ok
// 判断文法中提取的符号是不是终结符，是的话存入map返回syn，否则调用dynamicNoter处理非终结符
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

    if (strcmp(word, "IDN") == 0) {
        //处理标志符
        terMap.push_back(make_pair(word, IDN));
        return IDN;
    } else {
        //处理关键字、运算符、限界符表，即非终结符
        return dynamicNonTer(word);
    }
}

// ok
// 读取处理文法，将产生式拆分以后存入到了proc中输出，并且填入了终结符和非终结符MAP输出
void initGrammer() {
    FILE *infile;
    char ch;
    char array[30];
    char *word;
    int i;
    int codeNum;
    int line = 1;
    int count = 0;  //是文法读入过程中当前行读到了第几个文法单元
    // int orNum = 0;
    infile = fopen("wenfa.txt", "r");
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
                codeNum;  //将存字符存在产生式数组中，此时的line是txt中文法的line
        }
        //原本需要回退一个字符，由于是冗余字符，不回退
        if (ch == '\n') {  //一行读完以后才需要进行拆分工作
            count = 0;
            // orNum = 0;
            line++;
            ch = fgetc(infile);
        }
    }
    procNum = line - 1;
    // 输出终结符到文件
    fstream outfile1;
    outfile1.open("terminal.txt", ios::out);
    outfile1 << endl << "terminal list:" << endl;
    for (int i = 0; i < int(terMap.size()); i++) {
        outfile1 << terMap[i].first << endl;
    }
    outfile1.close();

    // 输出非终结符到文件
    fstream outfile2;
    outfile2.open("nonterminal.txt", ios::out);
    outfile2 << endl << "nonterminal list:" << endl;
    for (int i = 0; i < int(nonTerMap.size()); i++) {
        outfile2 << nonTerMap[i].first << endl;
    }
    outfile2.close();
}

// 通过编号匹配内容
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

//判断某个标号是不是终结符的标号，1代表是，0代表否
int inTer(int n) {
    for (int i = 0; i < int(terMap.size()); i++) {
        if (terMap[i].second == n) {
            return 1;
        }
    }
    return 0;
}
//判断某个标号是不是非终结符的标号,1代表是，0代表否
int inNonTer(int n) {
    for (int i = 0; i < int(nonTerMap.size()); i++) {
        if (nonTerMap[i].second == n) {
            return 1;
        }
    }
    return 0;
}

//判断某个标号在不在此时的empty集中，1代表是，0代表否
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

//判断某个标号在不在此时的emptyRecu集中，1代表是，0代表否
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

//？？？？？？？？？
//将s集合合并至d集合中，type = 1代表包括空（$）,type = 2代表不包括空
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

//先求出能直接推出空的非终结符集合
//局限性：A->BC B->空 C->空 无法得到A 能推出空 产生式右端只有一个单元
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

//判断该非终结符是否能推出空，但终结符也可能传入，但没关系
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

//求first集，传入的参数是在非终结符集合中的序号
void firstSet(int i) {
    int k = 0;
    int currentNon = nonTerMap[i].second;  //当前的非终结符标号
    //依次遍历全部产生式
    for (int j = 1; j <= procNum; j++)  // j代表第几个产生式
    {
        //找到该非终结符的产生式
        if (currentNon == proc[j][1])  //注意从1开始
        {
            //当右边的第一个是终结符或者空的时候
            if (inTer(proc[j][3]) == 1 || proc[j][3] == GRAMMAR_NULL) {
                //并入当前非终结符的first集中
                int temp[2];
                temp[0] = proc[j][3];
                temp[1] = -1;  //其实是模拟字符串操作的手段
                printf("1\n");
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
                printf("2\n");
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
                        for (u = 0;; u++) {
                            //注意是记录下一个符号的位置
                            if (nonTerMap[u].second == proc[j][k + 1]) {
                                break;
                            }
                        }
                        if (firstVisit[u] == 0) {
                            firstSet(u);
                            firstVisit[u] = 1;
                        }
                        printf("3\n");
                        merge(first[i], first[u], 2);
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
                        printf("4\n");
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

void First() {
    //先求出能直接推出空的非终结符集合
    nullSet(GRAMMAR_NULL);  // OK
    printf("\n");
    for (int i = 0; i < int(nonTerMap.size()); i++) {
        firstSet(i);
    }

    fstream outfile3;
    outfile3.open("first.txt", ios::out);
    outfile3 << endl << "first list:" << endl;
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
