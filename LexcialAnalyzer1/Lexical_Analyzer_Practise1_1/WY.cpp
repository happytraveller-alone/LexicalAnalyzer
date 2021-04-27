/*
 * @Description:
 * @Solution:
 * @Version: 2.0
 * @Author: happytraveller-alone
 * @Date: 2021-04-26 20:54:11
 * @LastEditors: happytraveller-alone
 * @LastEditTime: 2021-04-26 20:54:29
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*保留字列表*/
static char ReceiveWord[12][10] = {"while", "for",  "continue", "break",
                                   "if",    "else", "float",    "int",
                                   "char",  "void", "return",   "main"};

/*
<while,0>
<for,1>
<continue,2>
<break,3>
<if,4>
<else,5>
<float,6>
<int,7>
<char,8>
<void,9>
<return,10>
*/

/*数字运算符列表*/
static char Delimiter[21][3] = {"&&", "||", "+",  "-",  "*",  "/",  "%",
                                "=",  ">",  "<",  "==", "<=", ">=", "!=",
                                "++", "--", "+=", "-=", "*=", "/=", "%="};

/*
<&&，11>
<||, 12>
<+, 13>
<_, 14>
<*, 15>
</, 16>
<%, 17>
<=, 18>
<>, 19>
<<, 20>
<==, 21>
<<=, 22>
<>=, 23>
<!=, 24>
<++, 25>
<--, 26>
<+=, 27>
<-=, 28>
<*=, 29>
</=, 30>
<%=, 31>
*/

/*界符列表*/
static char Operator[8][2] = {"(", ")", "{", "}", ";", ",", "[", "]"};

/*
<(, 32>
<), 33>
<{, 34>
<}, 35>
<;, 36>
<,  37>这里是逗号
<[, 38>
<], 39>
*/

/*查找保留字*/
int SearchReserve(char ReceiveWord[][10], char s[]) {
    int count;
    //保留字列表遍历
    for (count = 0; count < 11; count++) {
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
        return 1;
    }
    return 0;
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
    char tempString[10000];
    int count = 0;
    for (int i = 0; i < PreProject; i++) {
        if (read[i] != '\n' && read[i] != '\t' && read[i] != '\r') {
            tempString[count++] = read[i];
        }
    }
    tempString[count] = '\0';
    strcpy(read, tempString);
}

/*字符串进行分析，匹配对应的序号*/
void Scanner(int &syn, char resourceFile[], char token[], int &FilePot) {
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
    }
    if (IsLetter(resourceFile[FilePot])) {
        token[count++] = resourceFile[FilePot];
        FilePot++;
        while (IsLetter(resourceFile[FilePot]) ||
               IsDigit(resourceFile[FilePot])) {
            token[count++] = resourceFile[FilePot];
            FilePot++;
        }
        //字符串末尾添加‘\0’
        token[count] = '\0';
        //进入保留字表中进行查询
        syn = SearchReserve(ReceiveWord, token);
        //未找到匹配保留字，默认为标识符
        if (syn == -1) {
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
                syn = 32 + i;  //获得种别码，使用了一点技巧，使之呈线性映射
                break;  //查到即推出
            }
        }
        FilePot++;  //指针下移，为下一扫描做准备
        return;
    } else if (character == '&') {
        FilePot++;
        if (resourceFile[FilePot] == '&')
            syn = 11;
        else {
            FilePot--;
            syn = 100;
        }
        FilePot++;
        return;
    } else if (character == '|') {
        FilePot++;
        if (resourceFile[FilePot] == '|')
            syn = 12;
        else {
            FilePot--;
            syn = 100;
        }
        FilePot++;
        return;
    } else if (character == '!') {
        FilePot++;
        if (resourceFile[FilePot] == '=')
            syn = 24;
        else {
            FilePot--;
            syn = 100;
        }
        FilePot++;
        return;
    } else if (character == '+') {
        FilePot++;
        if (resourceFile[FilePot] == '+')
            syn = 25;
        else if (resourceFile[FilePot] == '=') {
            syn = 27;
        } else {
            FilePot--;
            syn = 13;
        }
        FilePot++;
        return;
    } else if (character == '-') {
        FilePot++;
        if (resourceFile[FilePot] == '-')
            syn = 26;
        else if (resourceFile[FilePot] == '=') {
            syn = 28;
        } else {
            FilePot--;
            syn = 14;
        }
        FilePot++;
        return;
    } else if (character == '*') {
        FilePot++;
        if (resourceFile[FilePot] == '=') {
            syn = 29;
        } else {
            FilePot--;
            syn = 15;
        }
        FilePot++;
        return;
    } else if (character == '/') {
        FilePot++;
        if (resourceFile[FilePot] == '=') {
            syn = 30;
        } else {
            FilePot--;
            syn = 16;
        }
        FilePot++;
        return;
    } else if (character == '%') {
        FilePot++;
        if (resourceFile[FilePot] == '=') {
            syn = 31;
        } else {
            FilePot--;
            syn = 17;
        }
        FilePot++;
        return;
    } else if (character == '<') {
        FilePot++;
        if (resourceFile[FilePot] == '=') {
            syn = 22;
        } else {
            FilePot--;
            syn = 20;
        }
        FilePot++;
        return;
    } else if (character == '>') {
        FilePot++;
        if (resourceFile[FilePot] == '=') {
            syn = 23;
        } else {
            FilePot--;
            syn = 19;
        }
        FilePot++;
        return;
    } else if (character == '=') {
        FilePot++;
        if (resourceFile[FilePot] == '=') {
            syn = 21;
        } else {
            FilePot--;
            syn = 18;
        }
        FilePot++;
        return;
    } else if (character == '$') {
        syn = 50;
    } else {
        printf("Error,the character %c is not exist!", character);
        exit(-2);
    }
}
/*主程序进行文件调用*/
int main(void) {
    char resourceFile[30000];
    char token[20] = {0};
    int syn = -1;
    int FilePot = 0;
    FILE *fp;
    if ((fp = fopen("LexcialAnalyzer1\\Lexical_Analyzer_Practise1_1\\test.txt",
                    "r")) == NULL) {
        printf("can't open the file!");
        exit(-1);
    }
    //获取文件首字符，判断文件是否为空
    resourceFile[FilePot] = fgetc(fp);

    //读取文件
    while (resourceFile[FilePot] != EOF) {
        FilePot++;                          //指针后移
        resourceFile[FilePot] = fgetc(fp);  //字符读入
    }
    resourceFile[++FilePot] = '$';
    fclose(fp);

    //输出结果演示
    printf("\n源程序为:\n");
    printf(resourceFile);
    FileRead(resourceFile, FilePot);  //处理函数，字符数组
    printf("\n过滤之后的程序:\n");
    printf(resourceFile);
    printf("\n");
    FilePot = 0;  //指针重置
    while (syn != 50) {
        Scanner(syn, resourceFile, token, FilePot);
        if (syn == 100) {
            printf("(标识符  ,%s)\n", token);
        } else if (syn == 99) {
            printf("(数字  ,%s)\n", token);
        } else if (syn >= 0 && syn <= 10) {
            printf("(保留字   ,  %s)\n", token);
        } else if (syn >= 11 && syn <= 31) {
            printf("(%s   ,   --)\n", Delimiter[syn - 11]);
        } else if (syn >= 32 && syn <= 39) {
            printf("(%s   ,   --)\n", Operator[syn - 32]);
        }
    }
    //我正在进行注释编辑
    return 0;
}
