#ifndef _LEXANALYSIS_H
#define _LEXANALYSIS_H

//�ؼ���
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
#define WHILE 12
#define KEY_DESC "�ؼ���"

//����
#define INIT 15

// ����
#define int_val 20
#define float_val 21
#define char_val 22
#define str_val 23
#define CONSTANT_DESC "����"

// ������
#define IDN 30
#define IDENTIFER_DESC "��־��"

//�����
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
#define OPE_DESC "�����"

//�޽��
#define LEFT_BRA 100     // (
#define RIGHT_BRA 101    // )
#define LEFT_INDEX 102   // [
#define RIGHT_INDEX 103  // ]
#define L_BOUNDER 104    //  {
#define R_BOUNDER 105    // }
#define POINTER 106      // .
#define COMMA 107        // ,
#define SEMI 108         // ;
#define CLE_OPE_DESC "�޽��"

//��������
#define FLOAT_ERROR "float��ʾ����"
#define FLOAT_ERROR_NUM 1
#define STRING_ERROR "�ַ�������û�н�����"
#define STRING_ERROR_NUM 2
#define CHARCONST_ERROR "�ַ�����û�н�����"
#define CHARCONST_ERROR_NUM 3
#define CHAR_ERROR "�Ƿ��ַ�"
#define CHAR_ERROR_NUM 4
#define LEFT_BRA_ERROR "'('û�ж�Ӧ��"
#define LEFT_BRA_ERROR_NUM 5
#define RIGHT_BRA_ERROR "')'û�ж�Ӧ��"
#define RIGHT_BRA_ERROR_NUM 6
#define LEFT_INDEX_ERROR "'['û�ж�Ӧ��"
#define LEFT_INDEX_ERROR_NUM 7
#define RIGHT_INDEX_ERROR "']'û�ж�Ӧ��"
#define RIGHT_INDEX_ERROR_NUM 8
#define L_BOUNDER_ERROR "'{'û�ж�Ӧ��"
#define L_BOUNDER_ERROR_NUM 9
#define R_BOUNDER_ERROR "'}'û�ж�Ӧ��"
#define R_BOUNDER_ERROR_NUM 10

#define _NULL "��"

#define DESCRIBE 4000
#define TYPE 4001
#define STRING 4002
#define DIGIT 4003

struct NormalNode {
    char content[30];   //����
    char describe[30];  //�ʷ���Ԫ��
    int type;           //�ֱ���
    int line;           //��������
    NormalNode *next;   //��һ���ڵ�
};

void initKeyMapping();
void initOperMapping();
void initLimitMapping();
void initAddMap();

void initNode();  //��ʼ�����
void createNewNode(const char *content, const char *descirbe, int type,
                   int addr, int line);  //�����½��
void createNewError(const char *content, const char *descirbe, int type,
                    int line);  //����������
void printErrorLink();
// void printIdentLink();
int mystrlen(char *word);
void preProcess(char *word, int line);
void close();
int seekKey(char *word);
void scanner();
void BraMappingError();
void printNode1();
void printNode2();

#endif
