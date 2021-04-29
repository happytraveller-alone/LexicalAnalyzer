#ifndef _LEXANALYSIS_H
#define _LEXANALYSIS_H

//关键字
#define WHILE 0
#define FOR 1
#define CONTINUE 2
#define BREAK 3
#define IF 4
#define ELSE 5
#define FLOAT 6
#define INT 7
#define CHAR 8
#define STR 9
#define VOID 10
#define RETURN 11
//其他
#define MAIN 12
#define INIT 13
#define IDNEN 15
#define INT1 16
#define FLOAT1 17
#define CHAR1 18
#define STR1 19
#define KEY_DESC "关键字"

//标志符
#define IDN 40
#define IDENTIFER_DESC "标志符"

#define CONSTANT_DESC "常量"

//运算符
#define AND 71           // &&
#define OR 72            // ||
#define ADD 73           // +
#define SUB 74           // -
#define MUL 75           // *
#define DIV 76           // /
#define MOD 77           // %
#define ASG 78           // =
#define GRT_THAN 79      // >
#define LES_THAN 80      // <
#define EQUAL 81         // ==
#define LES_EQUAL 82     // <=
#define GRT_EQUAL 83     // >=
#define NOT_EQUAL 84     // !=
#define SELF_ADD 85      // ++
#define SELF_SUB 86      // --
#define COMPLETE_ADD 87  // +=
#define COMPLETE_SUB 88  // -=
#define COMPLETE_MUL 89  // *=
#define COMPLETE_DIV 90  // /=
#define COMPLETE_MOD 91  //%=
#define OPE_DESC "运算符"

//限界符
#define LEFT_BRA 100     // (
#define RIGHT_BRA 101    // )
#define LEFT_INDEX 102   // [
#define RIGHT_INDEX 103  // ]
#define L_BOUNDER 104    //  {
#define R_BOUNDER 105    // }
#define POINTER 106      // .
#define COMMA 107        // ,
#define SEMI 108         // ;
#define CLE_OPE_DESC "限界符"

//错误类型
#define FLOAT_ERROR "float表示错误"
#define FLOAT_ERROR_NUM 1
#define STRING_ERROR "字符串常量没有结束符"
#define STRING_ERROR_NUM 2
#define CHARCONST_ERROR "字符常量没有结束符"
#define CHARCONST_ERROR_NUM 3
#define CHAR_ERROR "非法字符"
#define CHAR_ERROR_NUM 4
#define LEFT_BRA_ERROR "'('没有对应项"
#define LEFT_BRA_ERROR_NUM 5
#define RIGHT_BRA_ERROR "')'没有对应项"
#define RIGHT_BRA_ERROR_NUM 6
#define LEFT_INDEX_ERROR "'['没有对应项"
#define LEFT_INDEX_ERROR_NUM 7
#define RIGHT_INDEX_ERROR "']'没有对应项"
#define RIGHT_INDEX_ERROR_NUM 8
#define L_BOUNDER_ERROR "'{'没有对应项"
#define L_BOUNDER_ERROR_NUM 9
#define R_BOUNDER_ERROR "'}'没有对应项"
#define R_BOUNDER_ERROR_NUM 10

#define _NULL "无"

#define DESCRIBE 4000
#define TYPE 4001
#define STRING 4002
#define DIGIT 4003

struct NormalNode {
    char content[30];   //内容
    char describe[30];  //词法单元名
    int type;           //种别码
    int addr;           //入口地址
    int line;           //所在行数
    NormalNode *next;   //下一个节点
};

void initKeyMapping();
void initOperMapping();
void initLimitMapping();
void initAddMap();

void initNode();  //初始化结点
void createNewNode(const char *content, const char *descirbe, int type,
                   int addr, int line);  //建立新结点
void createNewError(const char *content, const char *descirbe, int type,
                    int line);  //建立报
int createNewIden(const char *content, const char *descirbe, int type, int addr,
                  int line);
void printNodeLink();
void printErrorLink();
void printIdentLink();
int mystrlen(char *word);
void preProcess(char *word, int line);
void close();
int seekKey(char *word);
void scanner();
void BraMappingError();
void printNode1();
void printNode2();

#endif
