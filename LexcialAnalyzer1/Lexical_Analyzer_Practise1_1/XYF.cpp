/*
 * @Description:
 * @Solution:
 * @Version: 2.0
 * @Author: happytraveller-alone
 * @Date: 2021-04-04 21:25:58
 * @LastEditors: happytraveller-alone
 * @LastEditTime: 2021-04-27 08:26:35
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cctype>
using namespace std;

/*保留字列表*/
static char ReceiveWord[12][10] = {"while", "for",  "continue", "break",
                                   "if",    "else", "float",    "int",
                                   "char",  "void", "return",   "main"};

/*
<while,1>
<for,2>
<continue,3>
<break,4>
<if,5>
<else,6>
<float,7>
<int,8>
<char,9>
<void,10>
<return,11>
<main, 12>
*/

/*数字运算符列表*/
static char Delimiter[21][3] = {"&&", "||", "+",  "-",  "*",  "/",  "%",
                                "=",  ">",  "<",  "==", "<=", ">=", "!=",
                                "++", "--", "+=", "-=", "*=", "/=", "%="};

/*
<&&，31>
<||, 32>
<+, 33>
<_, 34>
<*, 35>
</, 36>
<%, 37>
<=, 38>
<>, 39>
<<, 40>
<==, 41>
<<=, 42>
<>=, 43>
<!=, 44>
<++, 45>
<--, 46>
<+=, 47>
<-=, 48>
<*=, 49>
</=, 50>
<%=, 51>
*/

/*界符列表*/
static char Operator[8][2] = {"(", ")", "{", "}", ";", ",", "[", "]"};

/*
<(, 71>
<), 72>
<{, 73>
<}, 74>
<;, 75>
<,, 76>这里是逗号
<[, 77>
<], 78>
*/

int index = 0;                //数组下标
static char tokens[300][20];  //存储tokens的数组

/*查找保留字*/
int SearchReserve(char ReceiveWord[][10], char s[]) {
    int count;
    //保留字列表遍历
    for (count = 0; count < 12; count++) {
        //轮询比较列表内部每个保留字，相同则返回种别号
        if (strcmp(ReceiveWord[count], s) == 0) {
            return count + 1;
        }
    }
    //未查找到，返回异常
    return -1;
}
/*判定是否为字符*/
int IsLetter(char letter) {
    if ((letter >= 'a' && letter <= 'z') || (letter >= 'A' && letter <= 'Z')) {
        return 1;  //符合情况进行返回
    }
    return 0;  //不符合进行错误返回
}

/*判定是否为数字*/
int IsDigit(char digit) {
    if (digit >= '0' && digit <= '9') {
        return 1;
    }
    return 0;
}

/*过滤程序文件，删除空行*/
void FileRead(char read[], int PreProject) {
    char tempString[10000];  //存储文件字符
    int count = 0;           //遍历下标
    for (int i = 0; i < PreProject; i++) {
        if (read[i] != '\n' && read[i] != '\t' && read[i] != '\r') {
            tempString[count++] = read[i];
        }
    }
    tempString[count] = '\0';
    strcpy(read, tempString);
}

/*字符串进行分析，匹配对应的序号*/
void Scanner(int &syn, char resourceFile[], char temptoken[], char token[],
             int &FilePot) {
    int i, count = 0;  // count用来做token[]的指示器，收集有用字符
    char character;    //作为判断使用
    character = resourceFile[FilePot];
    //过滤空格，防止程序因识别不了空格而结束
    while (character == ' ') {
        FilePot++;
        character = resourceFile[FilePot];
    }
    for (i = 0; i < 20; i++) {  //每次收集前先清零
        token[i] = '\0';
        temptoken[i] = '\0';
    }
    if (IsLetter(resourceFile[FilePot])) {
        token[count] = resourceFile[FilePot];
        temptoken[count++] = toupper(resourceFile[FilePot]);
        FilePot++;
        while (IsLetter(resourceFile[FilePot]) ||
               IsDigit(resourceFile[FilePot])) {
            token[count] = resourceFile[FilePot];
            temptoken[count++] = toupper(resourceFile[FilePot]);
            FilePot++;
        }
        //字符串末尾添加‘\0’
        token[count] = '\0';
        temptoken[count] = '\0';
        //进入保留字表中进行查询
        syn = SearchReserve(ReceiveWord, token);
        //未找到匹配保留字，默认为标识符
        if (syn == (-1)) {
            syn = 100;
        }
        return;
    } else if (IsDigit(resourceFile[FilePot])) {
        while (IsDigit(resourceFile[FilePot])) {
            token[count++] = resourceFile[FilePot];
            FilePot++;
        }
        //字符串末尾添加‘\0’
        token[count] = '\0';
        //默认为数字
        syn = 99;
    } else if (character == '(' || character == ')' || character == '{' ||
               character == '}' || character == ';' || character == ',' ||
               character == '[' || character == ']') {
        token[0] = resourceFile[FilePot];
        token[1] = '\0';

        for (i = 0; i < 8; i++) {  //查运算符界符表
            if (strcmp(token, Operator[i]) == 0) {
                syn = 71 + i;  //获得种别码，使用了一点技巧，使之呈线性映射
                break;  //查到即推出
            }
        }
        FilePot++;  //指针下移，为下一扫描做准备
        return;
    } else if (character == '&') {
        FilePot++;
        if (resourceFile[FilePot] == '&') {
            syn = 31;
        } else {
            FilePot--;
            syn = 100;
        }
        FilePot++;
        return;
    } else if (character == '|') {
        FilePot++;
        if (resourceFile[FilePot] == '|') {
            syn = 32;
        } else {
            FilePot--;
            syn = 100;
        }
        FilePot++;
        return;
    } else if (character == '!') {
        FilePot++;
        if (resourceFile[FilePot] == '=') {
            syn = 44;
        } else {
            FilePot--;
            syn = 100;
        }
        FilePot++;
        return;
    } else if (character == '+') {
        FilePot++;
        if (resourceFile[FilePot] == '+') {
            syn = 45;
        } else if (resourceFile[FilePot] == '=') {
            syn = 47;
        } else {
            FilePot--;
            syn = 33;
        }
        FilePot++;
        return;
    } else if (character == '-') {
        FilePot++;
        if (resourceFile[FilePot] == '-') {
            syn = 46;
        } else if (resourceFile[FilePot] == '=') {
            syn = 48;
        } else {
            FilePot--;
            syn = 34;
        }
        FilePot++;
        return;
    } else if (character == '*') {
        FilePot++;
        if (resourceFile[FilePot] == '=') {
            syn = 49;
        } else {
            FilePot--;
            syn = 35;
        }
        FilePot++;
        return;
    } else if (character == '/') {
        FilePot++;
        if (resourceFile[FilePot] == '=') {
            syn = 50;
        } else {
            FilePot--;
            syn = 36;
        }
        FilePot++;
        return;
    } else if (character == '%') {
        FilePot++;
        if (resourceFile[FilePot] == '=') {
            syn = 51;
        } else {
            FilePot--;
            syn = 37;
        }

        FilePot++;
        return;
    } else if (character == '<') {
        FilePot++;
        if (resourceFile[FilePot] == '=') {
            syn = 42;
        } else {
            FilePot--;
            syn = 40;
        }
        FilePot++;
        return;
    } else if (character == '>') {
        FilePot++;
        if (resourceFile[FilePot] == '=') {
            syn = 43;
        } else {
            FilePot--;
            syn = 39;
        }
        FilePot++;
        return;
    } else if (character == '=') {
        FilePot++;
        if (resourceFile[FilePot] == '=') {
            syn = 41;
        } else {
            FilePot--;
            syn = 38;
        }
        FilePot++;
        return;
    } else if (character == EOF) {
        syn = 0;
    } else {
        printf("Error,the character %c is not exist!", character);
        exit(-2);
    }
}

/*主程序进行文件调用*/
int main(void) {
    char resourceFile[30000];
    char token[20] = {0};  // token值的大小
    char temptoken[20] = {0};
    int store;
    int syn = -1;     // toekn值序列号
    int FilePot = 0;  //文件读取指针
    FILE *fp;         //文件读入流
    if ((fp = fopen("LexcialAnalyzer1\\Lexical_Analyzer_Practise1_1\\test.txt",
                    "r")) == NULL) {
        printf("can't open the file!");
        exit(-1);
    }
    //获取文件首字符，判断文件是否为空
    resourceFile[FilePot] = fgetc(fp);

    //读取文件
    while (resourceFile[FilePot] != EOF) {  //判断是否读到文件末尾
        FilePot++;                          //指针后移
        resourceFile[FilePot] = fgetc(fp);  //字符读入
    }
    resourceFile[++FilePot] = '\0';  //添加文件结束符号
    fclose(fp);                      //关闭文件输入流

    //输出结果演示
    printf("\n源程序为:\n");
    printf(resourceFile);
    FileRead(resourceFile, FilePot);  //处理函数，字符数组
    printf("\n过滤之后的程序:\n");
    printf(resourceFile);
    printf("\n");
    FilePot = 0;  //指针重置
    while (syn != 0) {
        Scanner(syn, resourceFile, temptoken, token, FilePot);
        if (syn == 100) {
            printf("%s <IDN,%s>\n", token, token);
            char temp[] = {"IDN"};
            strcpy(tokens[index++], temp);
        } else if (syn == 99) {
            printf("%s <CONST,%s>\n", token, token);
            char temp[] = {"INT"};
            strcpy(tokens[index++], temp);
        } else if (syn >= 1 && syn <= 30) {
            printf("%s <%s,_>\n", token, temptoken);
            strcpy(tokens[index++], token);
        } else if (syn >= 31 && syn <= 70) {
            printf("%s <OP,_>\n", Delimiter[syn - 31]);
            strcpy(tokens[index++], Delimiter[syn - 31]);
        } else if (syn >= 71 && syn <= 80) {
            printf("%s <SE,_>\n", Operator[syn - 71]);
            strcpy(tokens[index++], Operator[syn - 71]);
        }
    }
    index--;
    printf("[");
    for (int i = 0; i < index; i++) {
        printf("%s, ", tokens[i]);
    }
    printf("%s]", tokens[index]);

    return 0;
}
