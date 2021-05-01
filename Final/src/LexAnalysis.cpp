/*
 * @Description: �ʷ�������
 * @Solution:
 * @Version: 4.0
 * @Author: ��Խ лԶ�� ��һ��
 * @Date: 2021-04-22
 * @LastEditors: happytraveller-alone
 * @LastEditTime: 2021-04-30 22:45:46
 */
#include "LexAnalysis.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>

using namespace std;

int leftSmall = 0;           //
int rightSmall = 0;          //
int leftMiddle = 0;          //
int rightMiddle = 0;         //
int leftBig = 0;             //
int rightBig = 0;            //
int lineBra[6][1000] = {0};  //
int static_iden_number = 0;  //ģ���־���ĵ�ַ
fstream filecifa;            //��������Ĵʷ��ļ�

NormalNode *normalHead;  //����ʶ��TOKEN����׽��

//���屨���TOKEN�ṹ��
struct ErrorNode {
    char content[30];   //����δʶ��TOKEN����
    char describe[30];  //����δʶ��TOKEN����
    int type;           //����δʶ��TOKEN����
    int line;           //����δʶ��TOKEN������
    ErrorNode *next;    //����ָ����һ��δʶ��TOKEN��ָ��
};
ErrorNode *errorHead;  //�����ͷ

//������֤��TOIKEN��Ϣ
struct IdentiferNode {
    char content[30];     //����
    char describe[30];    //����
    int type;             //�ֱ���
    int addr;             //��ڵ�ַ
    int line;             //������
    IdentiferNode *next;  //��һ���ڵ�
};
IdentiferNode *idenHead;  //�׽ڵ�

vector<pair<const char *, int> > keyMap;    //����ؼ��ֱ�
vector<pair<const char *, int> > operMap;   //������ѧ������ű�
vector<pair<const char *, int> > limitMap;  //�����޽���ű�
vector<pair<const char *, int> > AddMap;    //���������ӵı�

/*
 *�������ƣ�initKeyMapping
 *��������
 *ʵ�ֹ��ܣ���ʼ���ؼ���Map
 *�������ʱ�䣺4/30
 *�������ߣ�лԶ��
 *��������DONE
 */
void initKeyMapping() {
    keyMap.clear();
    keyMap.push_back(make_pair("break", BREAK));
    keyMap.push_back(make_pair("char", CHAR));
    keyMap.push_back(make_pair("continue", CONTINUE));
    keyMap.push_back(make_pair("else", ELSE));
    keyMap.push_back(make_pair("float", FLOAT));
    keyMap.push_back(make_pair("for", FOR));
    keyMap.push_back(make_pair("if", IF));
    keyMap.push_back(make_pair("int", INT));
    keyMap.push_back(make_pair("return", RETURN));
    keyMap.push_back(make_pair("void", VOID));
    keyMap.push_back(make_pair("while", WHILE));
    keyMap.push_back(make_pair("string", STR));
}

/*
 *�������ƣ�initOperMapping
 *��������
 *ʵ�ֹ��ܣ���ʼ����ѧ�������Map
 *�������ʱ�䣺4/30
 *�������ߣ�лԶ��
 *��������DONE
 */
void initOperMapping() {
    operMap.clear();
    operMap.push_back(make_pair("*", MUL));
    operMap.push_back(make_pair("/", DIV));
    operMap.push_back(make_pair("%", MOD));
    operMap.push_back(make_pair("+", ADD));
    operMap.push_back(make_pair("-", SUB));
    operMap.push_back(make_pair("<", LES_THAN));
    operMap.push_back(make_pair(">", GRT_THAN));
    operMap.push_back(make_pair("=", ASG));
    operMap.push_back(make_pair("++", SELF_ADD));
    operMap.push_back(make_pair("--", SELF_SUB));
    operMap.push_back(make_pair("<=", LES_EQUAL));
    operMap.push_back(make_pair(">=", GRT_EQUAL));
    operMap.push_back(make_pair("==", EQUAL));
    operMap.push_back(make_pair("!=", NOT_EQUAL));
    operMap.push_back(make_pair("&&", AND));
    operMap.push_back(make_pair("||", OR));
    operMap.push_back(make_pair("+=", COMPLETE_ADD));
    operMap.push_back(make_pair("-=", COMPLETE_SUB));
    operMap.push_back(make_pair("*=", COMPLETE_MUL));
    operMap.push_back(make_pair("/=", COMPLETE_DIV));
    operMap.push_back(make_pair("%=", COMPLETE_MOD));
}

/*
 *�������ƣ�initLimitMapping
 *��������
 *ʵ�ֹ��ܣ���ʼ�������Map
 *�������ʱ�䣺4/30
 *�������ߣ�лԶ��
 *��������DONE
 */
void initLimitMapping() {
    limitMap.clear();
    limitMap.push_back(make_pair("(", LEFT_BRA));
    limitMap.push_back(make_pair(")", RIGHT_BRA));
    limitMap.push_back(make_pair("[", LEFT_INDEX));
    limitMap.push_back(make_pair("]", RIGHT_INDEX));
    limitMap.push_back(make_pair("{", L_BOUNDER));
    limitMap.push_back(make_pair("}", R_BOUNDER));
    limitMap.push_back(make_pair(".", POINTER));
    limitMap.push_back(make_pair(",", COMMA));
    limitMap.push_back(make_pair(";", SEMI));
}

/*
 *�������ƣ�initAddMap
 *��������
 *ʵ�ֹ��ܣ���ʼ���������ͷ���Map
 *�������ʱ�䣺4/30
 *�������ߣ�лԶ��
 *��������DONE
 */
void initAddMap() {
    AddMap.clear();
    AddMap.push_back(make_pair("STR", str_val));
    AddMap.push_back(make_pair("CHAR", char_val));
    AddMap.push_back(make_pair("FLOAT", float_val));
    AddMap.push_back(make_pair("INT", int_val));
    AddMap.push_back(make_pair("init", INIT));
    AddMap.push_back(make_pair("IDN", IDN));
}

/*
 *�������ƣ�initNode
 *��������
 *ʵ�ֹ��ܣ���ʼ���������ͷ���
 *�������ʱ�䣺4/30
 *�������ߣ�лԶ��
 *��������DONE
 */
void initNode() {
    normalHead = new NormalNode();
    strcpy(normalHead->content, "");
    strcpy(normalHead->describe, "");
    normalHead->type = -1;
    normalHead->addr = -1;
    normalHead->line = -1;
    normalHead->next = NULL;

    errorHead = new ErrorNode();
    strcpy(errorHead->content, "");
    strcpy(errorHead->describe, "");
    errorHead->line = -1;
    errorHead->next = NULL;

    idenHead = new IdentiferNode();
    strcpy(idenHead->content, "");
    strcpy(idenHead->describe, "");
    idenHead->type = -1;
    idenHead->addr = -1;
    idenHead->line = -1;
    idenHead->next = NULL;
}

/*
 *�������ƣ�createNewNode
 *������token���ݣ�token������token���ͣ�token�����е�ַ��token��������
 *ʵ�ֹ��ܣ����ݴ����������TOKEN���
 *�������ʱ�䣺4/30
 *�������ߣ�лԶ��
 *��������DONE
 */
void createNewNode(const char *content, const char *descirbe, int type,
                   int addr, int line) {
    NormalNode *p = normalHead;
    NormalNode *temp = new NormalNode();

    while (p->next != NULL) {
        p = p->next;
    }

    strcpy(temp->content, content);
    strcpy(temp->describe, descirbe);
    temp->type = type;
    temp->addr = addr;
    temp->line = line;
    temp->next = NULL;

    p->next = temp;
}

/*
 *�������ƣ�createNewError
 *������token���ݣ�token������token���ͣ�token�����е�ַ��token��������
 *ʵ�ֹ��ܣ����ݴ��������������TOKEN���
 *�������ʱ�䣺4/30
 *�������ߣ�лԶ��
 *��������DONE
 */
void createNewError(const char *content, const char *descirbe, int type,
                    int line) {
    ErrorNode *p = errorHead;
    ErrorNode *temp = new ErrorNode();

    strcpy(temp->content, content);
    strcpy(temp->describe, descirbe);
    temp->type = type;
    temp->line = line;
    temp->next = NULL;
    while (p->next != NULL) {
        p = p->next;
    }
    p->next = temp;
}

/*
 *�������ƣ�createNewError
 *������token���ݣ�token������token���ͣ�token�����е�ַ��token��������
 *ʵ�ֹ��ܣ������µı�ʶ�����
 *�������ʱ�䣺4/30
 *�������ߣ�лԶ��
 *��������DONE
 */
int createNewIden(const char *content, const char *descirbe, int type, int addr,
                  int line) {
    IdentiferNode *p = idenHead;
    IdentiferNode *temp = new IdentiferNode();
    int flag = 0;
    int addr_temp = -2;
    while (p->next != NULL) {
        if (strcmp(content, p->next->content) == 0) {
            flag = 1;
            addr_temp = p->next->addr;
        }
        p = p->next;
    }
    if (flag == 0) {
        addr_temp = ++static_iden_number;  //��������ģ����ڵ�ַ
    }
    strcpy(temp->content, content);
    strcpy(temp->describe, descirbe);
    temp->type = type;
    temp->addr = addr_temp;
    temp->line = line;
    temp->next = NULL;
    p->next = temp;
    return addr_temp;
}

/*
 *�������ƣ�printNode1
 *��������
 *ʵ�ֹ��ܣ������ӡʶ�𵽵�TOKEN
 *�������ʱ�䣺4/30
 *�������ߣ�лԶ��
 *��������DONE
 */
void printNode1() {
    filecifa.open("OutputFile\\Filecifa.txt", ios::out);
    filecifa << "TOKEN:" << endl << "[ ";
    NormalNode *p = normalHead;
    p = p->next;
    while (p != NULL) {
        switch (p->type) {
            case int_val:
                filecifa << "INT, ";
                break;
            case float_val:
                filecifa << "FLOAT, ";
                break;
            case char_val:
                filecifa << "CHAR, ";
                break;
            case str_val:
                filecifa << "STRING, ";
                break;
            case IDN:
                if (!strcmp(p->content, "main"))
                    filecifa << "main, ";
                else
                    filecifa << "IDN, ";
                break;
            default:
                filecifa << p->content << ", ";
                break;
        }
        p = p->next;
    }
    filecifa << ']' << endl;
    filecifa.close();
}

/*
 *�������ƣ�printNode2
 *��������
 *ʵ�ֹ��ܣ������ӡʶ�𵽵�TOKEN
 *�������ʱ�䣺4/30
 *�������ߣ�лԶ��
 *��������DONE
 */
void printNode2() {
    NormalNode *p = normalHead;
    p = p->next;

    filecifa.open("OutputFile\\Filecifa.txt", ios::app);
    filecifa << endl << endl;
    while (p != NULL) {
        if (p->type == IDN) {
            filecifa << setw(10) << p->content << setw(10) << "<"
                     << "IDN," << p->content << ">" << endl;
        } else if (p->type < 20 && p->type >= 0) {
            filecifa << setw(10) << p->content << setw(10) << "<"
                     << (strupr(p->content)) << ",_>" << endl;
        } else if (p->type > 16 && p->type < 30) {
            filecifa << setw(10) << p->content << setw(10) << "<"
                     << "CONST," << p->content << ">" << endl;
        } else if (p->type >= 70 && p->type < 100) {
            filecifa << setw(10) << p->content << setw(10) << "<"
                     << "OP," << p->content << ">" << endl;
        } else if (p->type >= 100) {
            filecifa << setw(10) << p->content << setw(10) << "<"
                     << "SE,_>" << endl;
        }
        p = p->next;
    }
    filecifa << endl;
    filecifa.close();
}

/*
 *�������ƣ�printNodeLink
 *��������
 *ʵ�ֹ��ܣ������ӡʶ�𵽵�TOKEN����ϸ��Ϣ
 *�������ʱ�䣺4/30
 *�������ߣ�лԶ��
 *��������DONE
 */
void printNodeLink() {
    fstream Nodelink;
    Nodelink.open("OutputFile\\TokenAnalysis.txt", ios::out);
    NormalNode *p = normalHead;
    p = p->next;
    Nodelink << "***********************�ʷ�������************************"
             << endl;
    Nodelink << setw(8) << "����" << setw(10) << "����" << setw(11) << "�ֱ���"
             << setw(10) << "��ַ" << setw(8) << "�к�" << endl;
    while (p != NULL) {
        if (p->type == IDN) {
            Nodelink << setw(8) << p->content << setw(10) << p->describe
                     << setw(10) << p->type << setw(8) << p->addr << setw(7)
                     << p->line << endl;
        } else {
            Nodelink << setw(8) << p->content << setw(10) << p->describe
                     << setw(10) << p->type << setw(15) << p->line << endl;
        }
        p = p->next;
    }
    Nodelink << endl;
    Nodelink.close();
}

/*
 *�������ƣ�printErrorLink
 *��������
 *ʵ�ֹ��ܣ������ӡδʶ�𵽵�TOKEN����ϸ��Ϣ
 *�������ʱ�䣺4/30
 *�������ߣ�лԶ��
 *��������DONE
 */
void printErrorLink() {
    fstream Errorlink;
    Errorlink.open("OutputFile\\ErrorAnalysis.txt", ios::out);
    ErrorNode *p = errorHead;
    p = p->next;
    Errorlink << "***********************�����************************"
              << endl;
    Errorlink << setw(8) << "����" << setw(10) << "����" << setw(8) << "����"
              << setw(8) << "�к�" << endl;
    while (p != NULL) {
        Errorlink << setw(8) << p->content << setw(10) << p->describe << setw(8)
                  << p->type << setw(7) << p->line << endl;
        p = p->next;
    }
    cout << endl;
    Errorlink.close();
}

void printIdentLink() {
    fstream IdentLink;
    IdentLink.open("OutputFile\\IdenList.txt", ios::out);
    IdentiferNode *p = idenHead;
    p = p->next;
    IdentLink << "***********************���ű�************************"
              << endl;
    IdentLink << setw(8) << "����" << setw(10) << "����" << setw(11) << "�ֱ���"
              << setw(10) << "��ַ" << setw(8) << "�к�" << endl;
    while (p != NULL) {
        IdentLink << setw(8) << p->content << setw(10) << p->describe
                  << setw(10) << p->type << setw(15) << p->line << endl;
        p = p->next;
    }
    IdentLink << endl;
}
/*
 *�������ƣ�mystrlen
 *������char* word �ַ�ָ��
 *ʵ�ֹ��ܣ���ȡһ��������TOKEN
 *�������ʱ�䣺4/30
 *�������ߣ�лԶ��
 *��������DONE
 */
int mystrlen(char *word) {
    if (*word == '\0') {
        return 0;
    } else {
        return 1 + mystrlen(word + 1);
    }
}

/*
 *�������ƣ�close
 *��������
 *ʵ�ֹ��ܣ�ɾ�����еı�ͷ
 *�������ʱ�䣺4/30
 *�������ߣ�лԶ��
 *��������DONE
 */
void close() {
    // delete idenHead;
    // delete errorHead;
    // delete normalHead;
}

/*
 *�������ƣ�seekKey
 *������TOKEN����ʼ�ַ�ָ��
 *ʵ�ֹ��ܣ�����ؼ��ֱ���в�ѯ�������ֱ�ţ�Ĭ�ϱ�ʶ��
 *�������ʱ�䣺4/30
 *�������ߣ�лԶ��
 *��������DONE
 */
int seekKey(char *word) {
    for (int i = 0; i < int(keyMap.size()); i++) {
        if (strcmp(word, keyMap[i].first) == 0) {
            return keyMap[i].second;
        }
    }
    return IDN;
}

/*
 *�������ƣ�scanner
 *��������
 *ʵ�ֹ��ܣ��򿪲����ļ������ж�ȡ�ַ���ȡTOKEN�������ݼ�ֵ����ң�����Ϊ�ս����ͷ��ս�������
 *�������ʱ�䣺4/30
 *�������ߣ�лԶ��
 *��������DONE
 */
void scanner() {
    char ch;
    char array[30];  //���ʳ�������??30
    char *word;
    int i;
    int line = 1;  //����

    FILE *infile;
    infile = fopen("testtxt\\test.txt", "r");
    while (!infile) {
        printf("���ļ�ʧ�ܣ�\n");
        return;
    }
    ch = fgetc(infile);
    while (ch != EOF) {
        i = 0;
        //����ĸ�����»��߿�,����ؼ��ֻ��߱�ʶ��
        if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') || ch == '_') {
            while ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') ||
                   (ch >= '0' && ch <= '9') || ch == '_') {
                array[i++] = ch;
                ch = fgetc(infile);
            }
            word = new char[i + 1];
            memcpy(word, array, i);
            word[i] = '\0';
            int seekTemp = seekKey(word);
            if (seekTemp != IDN) {  //���Ȳ鿴�Ƿ�Ϊ�ؼ���
                createNewNode(word, KEY_DESC, seekTemp, -1, line);
            } else {
                int addr_tmp =
                    createNewIden(word, IDENTIFER_DESC, seekTemp, -1, line);
                createNewNode(word, IDENTIFER_DESC, seekTemp, addr_tmp, line);
            }
            fseek(infile, -1L, SEEK_CUR);  //������һ??
        }
        //�����ֿ�ͷ����������
        else if (ch >= '0' && ch <= '9') {
            int flag = 0;
            int flag2 = 0;
            //��������
            while (ch >= '0' && ch <= '9') {
                array[i++] = ch;
                ch = fgetc(infile);
            }
            //����float
            if (ch == '.') {
                flag2 = 1;
                array[i++] = ch;
                ch = fgetc(infile);
                if (ch >= '0' && ch <= '9') {
                    while (ch >= '0' && ch <= '9') {
                        array[i++] = ch;
                        ch = fgetc(infile);
                    }
                } else {
                    flag = 1;
                }
            }
            word = new char[i + 1];
            memcpy(word, array, i);
            word[i] = '\0';
            if (flag == 1) {  //�����float����
                createNewError(word, FLOAT_ERROR, FLOAT_ERROR_NUM, line);
            } else {
                if (flag2 == 0) {
                    createNewNode(word, CONSTANT_DESC, int_val, -1, line);
                } else {
                    createNewNode(word, CONSTANT_DESC, float_val, -1, line);
                }
            }
            fseek(infile, -1L, SEEK_CUR);  //������һ??
        }
        //??"/"��??
        else if (ch == '/') {
            ch = fgetc(infile);
            //��������??"/="
            if (ch == '=') {
                createNewNode("/=", OPE_DESC, COMPLETE_DIV, -1, line);
            }  //�������
            else {
                createNewNode("/", OPE_DESC, DIV, -1, line);
            }
        }
        //�������ַ�??
        else if (ch == '"') {
            ch = fgetc(infile);
            i = 0;
            while (ch != '"') {
                array[i++] = ch;
                if (ch == '\n') {
                    line++;
                }
                ch = fgetc(infile);
                if (ch == EOF) {
                    createNewError(_NULL, STRING_ERROR, STRING_ERROR_NUM, line);
                    return;
                }
            }
            word = new char[i + 1];
            memcpy(word, array, i);
            word[i] = '\0';
            createNewNode(word, CONSTANT_DESC, str_val, -1, line);
        }
        //�������ַ�
        else if (ch == '\'') {
            ch = fgetc(infile);
            i = 0;
            while (ch != '\'') {
                array[i++] = ch;
                if (ch == '\n') {
                    line++;
                }
                ch = fgetc(infile);
                if (ch == EOF) {
                    createNewError(_NULL, CHARCONST_ERROR, CHARCONST_ERROR_NUM,
                                   line);
                    return;
                }
            }
            word = new char[i + 1];
            memcpy(word, array, i);
            word[i] = '\0';
            createNewNode(word, CONSTANT_DESC, char_val, -1, line);
        } else if (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n') {
            if (ch == '\n') {
                line++;
            }
        } else {
            if (ch == EOF) {
                return;
            }
            //����-��ͷ������??
            else if (ch == '-') {
                array[i++] = ch;
                ch = fgetc(infile);
                if (ch >= '0' && ch <= '9') {
                    int flag = 0;
                    int flag2 = 0;
                    //��������
                    while (ch >= '0' && ch <= '9') {
                        array[i++] = ch;
                        ch = fgetc(infile);
                    }
                    //����float
                    if (ch == '.') {
                        flag2 = 1;
                        array[i++] = ch;
                        ch = fgetc(infile);
                        if (ch >= '0' && ch <= '9') {
                            while (ch >= '0' && ch <= '9') {
                                array[i++] = ch;
                                ch = fgetc(infile);
                            }
                        } else {
                            flag = 1;
                        }
                    }
                    word = new char[i + 1];
                    memcpy(word, array, i);
                    word[i] = '\0';
                    if (flag == 1) {
                        createNewError(word, FLOAT_ERROR, FLOAT_ERROR_NUM,
                                       line);
                    } else {
                        if (flag2 == 0) {
                            createNewNode(word, CONSTANT_DESC, INT, -1, line);
                        } else {
                            createNewNode(word, CONSTANT_DESC, FLOAT, -1, line);
                        }
                    }
                    fseek(infile, -1L, SEEK_CUR);  //������һ??
                } else if (ch == '-') {
                    createNewNode("--", OPE_DESC, SELF_SUB, -1, line);
                } else if (ch == '=') {
                    createNewNode("-=", OPE_DESC, SELF_SUB, -1, line);
                } else {
                    createNewNode("-", OPE_DESC, SUB, -1, line);
                    fseek(infile, -1L, SEEK_CUR);
                }
            }
            //����+��ͷ������
            else if (ch == '+') {
                ch = fgetc(infile);
                if (ch == '+') {
                    createNewNode("++", OPE_DESC, SELF_ADD, -1, line);
                } else if (ch == '=') {
                    createNewNode("+=", OPE_DESC, COMPLETE_ADD, -1, line);
                } else {
                    createNewNode("+", OPE_DESC, ADD, -1, line);
                    fseek(infile, -1L, SEEK_CUR);
                }
            }
            //����*��ͷ������
            else if (ch == '*') {
                ch = fgetc(infile);
                if (ch == '=') {
                    createNewNode("*=", OPE_DESC, COMPLETE_MUL, -1, line);
                } else {
                    createNewNode("*", OPE_DESC, MUL, -1, line);
                    fseek(infile, -1L, SEEK_CUR);
                }
            }
            //����%��ͷ������??
            else if (ch == '%') {
                ch = fgetc(infile);
                if (ch == '=') {
                    createNewNode("%=", OPE_DESC, COMPLETE_MOD, -1, line);
                } else {
                    createNewNode("%", OPE_DESC, MOD, -1, line);
                    fseek(infile, -1L, SEEK_CUR);
                }
            }
            //����&��ͷ������
            else if (ch == '&') {
                ch = fgetc(infile);
                if (ch == '&') {
                    createNewNode("&&", OPE_DESC, AND, -1, line);
                }
            }
            //����<��ͷ������
            else if (ch == '<') {
                ch = fgetc(infile);
                if (ch == '=') {
                    createNewNode("<=", OPE_DESC, LES_EQUAL, -1, line);
                } else {
                    createNewNode("<", OPE_DESC, LES_THAN, -1, line);
                    fseek(infile, -1L, SEEK_CUR);
                }
            }
            //����>��ͷ������??
            else if (ch == '>') {
                ch = fgetc(infile);
                if (ch == '=') {
                    createNewNode(">=", OPE_DESC, GRT_EQUAL, -1, line);
                } else {
                    createNewNode(">", OPE_DESC, GRT_THAN, -1, line);
                    fseek(infile, -1L, SEEK_CUR);
                }
            }
            //����|��ͷ������??
            else if (ch == '|') {
                ch = fgetc(infile);
                if (ch == '|') {
                    createNewNode("||", OPE_DESC, OR, -1, line);
                }
            } else if (ch == '=') {
                ch = fgetc(infile);
                if (ch == '=') {
                    createNewNode("==", OPE_DESC, EQUAL, -1, line);
                } else {
                    createNewNode("=", OPE_DESC, ASG, -1, line);
                    fseek(infile, -1L, SEEK_CUR);
                }
            } else if (ch == '(') {
                leftSmall++;
                lineBra[0][leftSmall] = line;
                createNewNode("(", CLE_OPE_DESC, LEFT_BRA, -1, line);
            } else if (ch == ')') {
                rightSmall++;
                lineBra[1][rightSmall] = line;
                createNewNode(")", CLE_OPE_DESC, RIGHT_BRA, -1, line);
            } else if (ch == '[') {
                leftMiddle++;
                lineBra[2][leftMiddle] = line;
                createNewNode("[", CLE_OPE_DESC, LEFT_INDEX, -1, line);
            } else if (ch == ']') {
                rightMiddle++;
                lineBra[3][rightMiddle] = line;
                createNewNode("]", CLE_OPE_DESC, RIGHT_INDEX, -1, line);
            } else if (ch == '{') {
                leftBig++;
                lineBra[4][leftBig] = line;
                createNewNode("{", CLE_OPE_DESC, L_BOUNDER, -1, line);
            } else if (ch == '}') {
                rightBig++;
                lineBra[5][rightBig] = line;
                createNewNode("}", CLE_OPE_DESC, R_BOUNDER, -1, line);
            } else if (ch == '.') {
                createNewNode(".", CLE_OPE_DESC, POINTER, -1, line);
            } else if (ch == ',') {
                createNewNode(",", CLE_OPE_DESC, COMMA, -1, line);
            } else if (ch == ';') {
                createNewNode(";", CLE_OPE_DESC, SEMI, -1, line);
            } else {
                char temp[2];
                temp[0] = ch;
                temp[1] = '\0';
                createNewError(temp, CHAR_ERROR, CHAR_ERROR_NUM, line);
            }
        }
        ch = fgetc(infile);
    }
}

/*
 *�������ƣ�BraMappingError
 *��������
 *ʵ�ֹ��ܣ��ж����ŵ�ƥ�������С���ţ������ţ������ţ�
 *�������ʱ�䣺4/30
 *�������ߣ�лԶ��
 *��������DONE
 */
void BraMappingError() {
    if (leftSmall != rightSmall) {
        int i = (leftSmall > rightSmall) ? (leftSmall - rightSmall)
                                         : (rightSmall - leftSmall);
        bool flag = (leftSmall > rightSmall) ? true : false;
        if (flag) {
            while (i--) {
                createNewError(_NULL, LEFT_BRA_ERROR, LEFT_BRA_ERROR_NUM,
                               lineBra[0][i + 1]);
            }
        } else {
            while (i--) {
                createNewError(_NULL, RIGHT_BRA_ERROR, RIGHT_BRA_ERROR_NUM,
                               lineBra[1][i + 1]);
            }
        }
    }
    if (leftMiddle != rightMiddle) {
        int i = (leftMiddle > rightMiddle) ? (leftMiddle - rightMiddle)
                                           : (rightMiddle - leftMiddle);
        bool flag = (leftMiddle > rightMiddle) ? true : false;
        if (flag) {
            while (i--) {
                createNewError(_NULL, LEFT_INDEX_ERROR, LEFT_INDEX_ERROR_NUM,
                               lineBra[2][i + 1]);
            }
        } else {
            while (i--) {
                createNewError(_NULL, RIGHT_INDEX_ERROR, RIGHT_INDEX_ERROR_NUM,
                               lineBra[3][i + 1]);
            }
        }
    }
    if (leftBig != rightBig) {
        int i = (leftBig > rightBig) ? (leftBig - rightBig)
                                     : (rightBig - leftSmall);
        bool flag = (leftBig > rightBig) ? true : false;
        if (flag) {
            while (i--) {
                createNewError(_NULL, L_BOUNDER_ERROR, L_BOUNDER_ERROR_NUM,
                               lineBra[4][i + 1]);
            }
        } else {
            while (i--) {
                createNewError(_NULL, R_BOUNDER_ERROR, R_BOUNDER_ERROR_NUM,
                               lineBra[5][i + 1]);
            }
        }
    }
}
