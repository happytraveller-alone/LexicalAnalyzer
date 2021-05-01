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
#define Max_Length 500  //ָ��ֺ����ʽ����󳤶�

#define Max_NonTer 60
#define Max_Ter 60
#define Max_Length2 100

int procNum = 0;
//����ʽ�����飬��ߴ洢���ս�����߷��ս����Ӧ��syn��proc��ά�����Ǵ�1��ʼ��
int proc[Max_Proc][Max_Length];  //�кţ�����ʽ

// FISRT����
int first[Max_Proc][Max_Length];  // FIRST λ�ã����ս�����
int firstVisit[Max_Proc];  //��¼ĳ���ս����First���Ƿ��Ѿ����
int emptyRecu[Max_Proc];  //������Ƴ��յķ��ս���ı�ż�ʱʹ�õķ��εݹ�ļ���
int EmptyStore[Max_Proc];  //���Ƴ��յķ��ս���ı��

// FOLLOW����
int follow[Max_Proc][Max_Length];
int connectFirst[Max_Length];  //��ĳЩFirst����������ļ���
int followVisit[Max_Proc];  //��¼ĳ���ս����Follow���Ƿ��Ѿ����
int followRecu[Max_Proc];   //����Follow��ʱʹ�õķ��εݹ�ļ���

// ����Ԥ�������
int select[Max_Proc][Max_Length];
int M[Max_NonTer][Max_Ter][Max_Length2];

extern NormalNode *normalHead;  //�׽��

fstream resultfile;

// extern�Ĳ��ִ�����ܳ��ֵ��ս��������,extern��ʾ�������ܳ����ڱ���ļ���
extern vector<pair<const char *, int> > keyMap;
extern vector<pair<const char *, int> > operMap;
extern vector<pair<const char *, int> > limitMap;
extern vector<pair<const char *, int> > AddMap;

vector<pair<const char *, int> > nonTerMap;  //���ս��ӳ���,�����ظ���
vector<pair<const char *, int> > terMap;  //�ս��ӳ���,�����ظ���
vector<pair<const char *, int> >
    specialMap;  //�ķ��е��������ӳ�������-> | $(��)

/*
 *�������ƣ�initSpecialMapping
 *��������
 *ʵ�ֹ��ܣ����ķ��е�Լ�����Ž��д���->, ��($)��#
 *�������ʱ�䣺4/30
 *�������ߣ�лԶ��
 *��������DONE
 */
void initSpecialMapping() {
    specialMap.clear();
    specialMap.push_back(make_pair("->", GRAMMAR_ARROW));
    specialMap.push_back(make_pair("$", GRAMMAR_NULL));
    specialMap.push_back(make_pair("#", GRAMMAR_SPECIAL));
}

/*
 *�������ƣ�dynamicNonTer
 *������TOKEN����ʼ�ַ�
 *ʵ�ֹ��ܣ���̬���ɷ��ս������map���ڻ���Ļ����ϣ�ȷ�������ս����ͻ�������ֱ��
 *�������ʱ�䣺4/30
 *�������ߣ�лԶ��
 *��������DONE
 */
int dynamicNonTer(char *word) {
    int i = 0;
    int dynamicNum;
    //���ж��ڲ��ڷ��ս��map��
    for (i = 0; i < int(nonTerMap.size()); i++) {
        if (strcmp(word, nonTerMap[i].first) == 0) {
            return nonTerMap[i].second;
        }
    }
    //����˷��ս������Map��
    if (i == int(nonTerMap.size())) {
        if (i == 0) {  //�൱�ڱ�Ϊ�գ���ʼ��������׽��
            dynamicNum = GRAMMAR_BASE;  //����ַ
            nonTerMap.push_back(make_pair(word, dynamicNum));
        } else {
            dynamicNum = nonTerMap[int(nonTerMap.size()) - 1].second +
                         1;  //�����ַ��������
            nonTerMap.push_back(make_pair(word, dynamicNum));
        }
    }
    return dynamicNum;
}

/*
 *�������ƣ�seekCodeNum
 *������TOKEN����ʼ�ַ�
 *ʵ�ֹ��ܣ��ж��ķ�����ȡ�ķ����ǲ����ս�������ϴ���map�����ֱ�ţ��������dynamicNoter������ս��
 *�������ʱ�䣺4/30
 *�������ߣ�лԶ��
 *��������DONE
 */
int seekCodeNum(char *word) {
    //�����ķ��е��������
    for (int i = 0; i < int(specialMap.size()); i++) {
        if (strcmp(word, specialMap[i].first) == 0) {
            return specialMap[i].second;
        }
    }
    //�������ս��ӳ�������û�д��ս��key��op��limt�����ս��
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
 *�������ƣ�searchMapping
 *�������ֱ��
 *ʵ�ֹ��ܣ�ͨ�����ƥ������
 *�������ʱ�䣺4/30
 *�������ߣ�лԶ��
 *��������DONE
 */
const char *searchMapping(int num) {
    //��־��
    if (num == IDN) {
        return "IDN";
    }
    //�����ķ��е��������
    for (int i = 0; i < int(specialMap.size()); i++) {
        if (specialMap[i].second == num) {
            return specialMap[i].first;
        }
    }
    //������ս��
    for (int i = 0; i < int(nonTerMap.size()); i++) {
        if (nonTerMap[i].second == num) {
            return nonTerMap[i].first;
        }
    }
    //�����ս��
    for (int i = 0; i < int(terMap.size()); i++) {
        if (terMap[i].second == num) {
            return terMap[i].first;
        }
    }
    return "wrong";
}

/*
 *�������ƣ�initGrammer
 *��������
 *ʵ�ֹ��ܣ���ȡ�����ķ���������ʽ����Ժ���뵽��proc������������������ս���ͷ��ս��MAP���
 *�������ʱ�䣺4/30
 *�������ߣ�лԶ��
 *��������DONE
 */
void initGrammer() {
    FILE *infile;
    char ch;
    char array[30];
    char *word;
    int i;
    int codeNum;
    int line = 1;
    int count = 0;  //���ķ���������е�ǰ�ж����˵ڼ����ķ���Ԫ
    infile = fopen("testtxt\\wenfa.txt", "r");
    if (!infile) {
        printf("�ķ���ʧ�ܣ�\n");
        return;
    }
    initSpecialMapping();
    nonTerMap.clear();
    terMap.clear();

    memset(proc, -1, sizeof(proc));

    memset(first, -1, sizeof(first));
    memset(firstVisit, 0, sizeof(firstVisit));  //���ս����first����δ���
    memset(EmptyStore, -1, sizeof(EmptyStore));
    memset(emptyRecu, -1, sizeof(emptyRecu));

    memset(follow, -1, sizeof(follow));
    memset(connectFirst, -1, sizeof(connectFirst));
    memset(followVisit, 0, sizeof(followVisit));  //���ս����follow����δ���
    memset(followRecu, -1, sizeof(followRecu));

    memset(select, -1, sizeof(select));
    memset(M, -1, sizeof(M));

    ch = fgetc(infile);
    i = 0;
    while (ch != EOF) {
        i = 0;
        // ��ȡһ������ʽ�еĵ�Ԫ
        while (ch == ' ' || ch == '\t') {
            ch = fgetc(infile);
        }
        while (ch != ' ' && ch != '\n' && ch != EOF) {
            array[i++] = ch;  // arrayһ���������ַ����������ո��еȷָ
            ch = fgetc(infile);
        }
        while (ch == ' ' || ch == '\t') {
            ch = fgetc(infile);
        }
        // ��ȡһ������ʽ�еĵ�Ԫ
        word = new char[i + 1];
        memcpy(word, array, i);
        word[i] = '\0';
        codeNum = 0;
        codeNum = seekCodeNum(word);  //�Դ˵�Ԫ�����Ƿ�Ϊ�ս���ķ���ʹ���MAP
        if (codeNum != 0) {
            count++;
            proc[line][count] =
                codeNum;  //�����ַ����ڲ���ʽ�����У���ʱ��line��txt���ķ���line
        }
        //ԭ����Ҫ����һ���ַ��������������ַ���������
        if (ch == '\n') {  //һ�ж����Ժ����Ҫ���в�ֹ���
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

    // ����ս�����ļ�
    fstream outfile1;
    outfile1.open("OutputFile\\terminal.txt", ios::out);
    outfile1 << "terminal list:" << endl;
    for (int i = 0; i < int(terMap.size()); i++) {
        outfile1 << terMap[i].first << endl;
    }
    outfile1.close();

    // ������ս�����ļ�
    fstream outfile2;
    outfile2.open("OutputFile\\nonterminal.txt", ios::out);
    outfile2 << "nonterminal list:" << endl;
    for (int i = 0; i < int(nonTerMap.size()); i++) {
        outfile2 << nonTerMap[i].first << endl;
    }
    outfile2.close();
}

/*
 *�������ƣ�inTer
 *�������ֱ��
 *ʵ�ֹ��ܣ��ж�ĳ������ǲ����ս���ı�ţ�1�����ǣ�0�����
 *�������ʱ�䣺4/30
 *�������ߣ�лԶ��
 *��������DONE
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
 *�������ƣ�inNonTer
 *������TOKEN����ʼ�ַ�
 *ʵ�ֹ��ܣ��ж�ĳ������ǲ��Ƿ��ս���ı��,1�����ǣ�0�����
 *�������ʱ�䣺4/30
 *�������ߣ�лԶ��
 *��������DONE
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
 *�������ƣ�inEmpty
 *�������ֱ��
 *ʵ�ֹ��ܣ��ж�ĳ������ڲ��ڴ�ʱ��empty���У�1�����ǣ�0�����
 *�������ʱ�䣺4/30
 *�������ߣ�лԶ��
 *��������DONE
 */
int inEmpty(int n) {
    //��ǰEmpty���ĳ���
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
 *�������ƣ�inEmptyRecu
 *�������ֱ��
 *ʵ�ֹ��ܣ��ж�ĳ������ڲ��ڴ�ʱ��emptyRecu���У�1�����ǣ�0�����
 *�������ʱ�䣺4/30
 *�������ߣ�лԶ��
 *��������DONE
 */
int inEmptyRecu(int n) {
    //��ǰEmpty���ĳ���
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
 *�������ƣ�merge
 *������Fisrt[i]���ϣ�First[j]���ϣ���������
 *ʵ�ֹ��ܣ���s���Ϻϲ���d�����У�type = 1��������գ�$��,type = 2����������
 *�������ʱ�䣺4/30
 *�������ߣ�лԶ��
 *��������DONE
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
 *�������ƣ�nullSet
 *�������ֱ��
 *ʵ�ֹ��ܣ�������ֱ���Ƴ��յķ��ս��
 *�����ԣ�A->BC B->�� C->�� �޷��õ�A ���Ƴ��� ����ʽ�Ҷ�ֻ��һ����Ԫ
 *�������ʱ�䣺4/30
 *�������ߣ�лԶ��
 *��������DONE
 */
void nullSet(int currentNum) {
    int temp[2];
    for (int j = 1; j <= procNum; j++) {
        //����ұߵĵ�һ���Ǹ��ַ������ҳ���ֻ��1
        if (proc[j][3] == currentNum && proc[j][4] == -1) {
            temp[0] = proc[j][1];
            temp[1] = -1;
            merge(EmptyStore, temp, 1);
            //�ݹ��ж� A->B��B->��
            nullSet(proc[j][1]);
        }
    }
}

/*
 *�������ƣ�nullSet
 *�������ֱ��
 *ʵ�ֹ��ܣ��жϸ÷��ս���Ƿ����Ƴ���
 *�����ԣ�A->BC B->�� C->�� �޷��õ�A ���Ƴ��� ����ʽ�Ҷ�ֻ��һ����Ԫ
 *�������ʱ�䣺4/30
 *�������ߣ�лԶ��
 *��������DONE
 */
int reduNull(int currentNon) {
    int temp[2];
    int result = 1;
    int mark = 0;
    temp[0] = currentNon;
    temp[1] = -1;
    merge(emptyRecu, temp, 1);  //�Ƚ��˷��Ų�����ݹ鼯����
    if (inEmpty(currentNon) == 1) {
        return 1;
    }

    for (int j = 1; j <= procNum; j++) {
        if (proc[j][1] == currentNon) {
            int rightLength = 0;
            //������Ҳ��ĳ���
            for (rightLength = 3;; rightLength++) {
                if (proc[j][rightLength] == -1) {
                    break;
                }
            }
            rightLength--;
            //�������Ϊ1�������Ѿ����
            if (rightLength - 2 == 1 && inEmpty(proc[j][rightLength])) {
                return 1;
            }
            //�������Ϊ1���������ս��
            else if (rightLength - 2 == 1 && inTer(proc[j][rightLength])) {
                return 0;
            }
            //������ȳ�����2
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
                        merge(emptyRecu, temp, 1);  //�Ƚ��˷��Ų�����ݹ鼯����
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
 *�������ƣ�firstSet
 *������
 *ʵ�ֹ��ܣ���first��������Ĳ������ڷ��ս�������е����
 *�������ʱ�䣺4/30
 *�������ߣ���һ��
 *��������DONE
 */
void firstSet(int i) {
    int k = 0;
    int currentNon = nonTerMap[i].second;  //��ǰ�ķ��ս�����
    //���α���ȫ������ʽ
    // cout<<"         "<<nonTerMap[i].first<<endl;
    for (int j = 1; j <= procNum; j++)  // j����ڼ�������ʽ
    {
        //�ҵ��÷��ս���Ĳ���ʽ
        if (currentNon == proc[j][1])  //ע���1��ʼ
        {
            // if (i == 12) cout << nonTerMap[i].first << endl;
            //���ұߵĵ�һ�����ս�����߿յ�ʱ��
            if (inTer(proc[j][3]) == 1 || proc[j][3] == GRAMMAR_NULL) {
                //���뵱ǰ���ս����first����
                // if (i == 12) cout << "              " << proc[j][3] << endl;
                int temp[2];
                temp[0] = proc[j][3];
                temp[1] = -1;  //��ʵ��ģ���ַ����������ֶ�
                merge(first[i], temp, 1);
            }
            //���ұߵĵ�һ���Ƿ��ս����ʱ��
            else if (inNonTer(proc[j][3]) == 1) {
                //���������ݹ���ʽ�ģ����൱��A->A...�����˼򵥵���ݹ�����
                if (proc[j][3] == currentNon) {
                    continue;
                }
                //��¼���ұߵ�һ�����ս����λ��
                for (k = 0;; k++) {
                    if (nonTerMap[k].second == proc[j][3]) {
                        break;
                    }
                }
                //���ұߵ�һ�����ս����δ���ʹ���ʱ��
                if (firstVisit[k] == 0) {
                    firstSet(k);
                    firstVisit[k] = 1;
                }
                merge(
                    first[i], first[k],
                    2);  //���first[k]��ʱ�п�ֵ�Ļ�����ʱ���ѿ�ֵ����first[i]��
                int rightLength = 0;
                //������Ҳ��ĳ���
                for (rightLength = 3;; rightLength++) {
                    if (proc[j][rightLength] == -1) {
                        break;
                    }
                }
                //��ĿǰΪֹ��ֻ������ұߵĵ�һ��(���������յĲ���)��Forѭ������֮���
                for (k = 3; k < rightLength; k++) {
                    emptyRecu[0] = -1;  //�൱�ڳ�ʼ��������ݹ鼯�ϣ�������
                    //����Ҳ��ĵ�ǰ�ַ����Ƴ��ղ��һ��������һ���ַ����ͽ�֮���һ���ַ�����First����
                    if (reduNull(proc[j][k]) == 1 && k < rightLength - 1) {
                        int u = 0;
                        int tornot = 0;
                        for (u = 0;; u++) {
                            //ע���Ǽ�¼��һ�����ŵ�λ��
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
                            temp[1] = -1;  //��ʵ��ģ���ַ����������ֶ�
                            merge(first[i], temp, 1);
                        }
                    }
                    // A->BCd
                    // k=B��Ȼ�ղ�B�Ѿ����frist�����룬����CҪ�������Ҫ�ж�
                    // B���ǲ������Ƴ��գ�����k���Ǵ�B��ʼ������ʵ���Ͽ������C
                    //�������һ���ַ������Ҳ���ʽ�Ҳ������Ƴ���,��$����First����
                    else if (reduNull(proc[j][k]) == 1 &&
                             k == rightLength - 1) {
                        int temp[2];
                        temp[0] = GRAMMAR_NULL;
                        temp[1] = -1;  //��ʵ��ģ���ַ����������ֶ�
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
 *�������ƣ�First
 *������
 *ʵ�ֹ��ܣ� first����
 *�������ʱ�䣺4/30
 *�������ߣ���һ��
 *��������DONE
 */
void First() {
    //�������ֱ���Ƴ��յķ��ս������
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
 *�������ƣ�inFollowRecu
 *�������к�
 *ʵ�ֹ��ܣ��ж�ĳ������ڲ��ڴ�ʱ��followRecu���У�1�����ǣ�0�����
 *�������ʱ�䣺4/30
 *�������ߣ���һ��
 *��������DONE
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
 *�������ƣ�inProcRight
 *���������ʽָ����к�
 *ʵ�ֹ��ܣ��ж�ĳ������ǲ����ڲ���ʽ���ұ�
 *�������ʱ�䣺4/30
 *�������ߣ���һ��
 *��������DONE
 */
int inProcRight(int n, int *p) {
    //ע������Ĭ���Ǵ�3��ʼ
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
 *�������ƣ�connectFirstSet
 *������
 *ʵ�ֹ��ܣ���First��������ĺ���
 *�������ʱ�䣺4/30
 *�������ߣ���һ��
 *��������DONE
 */
void connectFirstSet(int *p) {
    int i = 0;
    int flag = 0;
    int temp[2];
    //���P�ĳ���Ϊ1
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
            //Ҳ�������ս��
            if (flag == 0) {
                for (i = 0; i < int(terMap.size()); i++) {
                    if (terMap[i].second == p[0]) {
                        temp[0] = terMap[i].second;
                        temp[1] = -1;
                        merge(connectFirst, temp,
                              2);  //�ս����First�������䱾��
                        break;
                    }
                }
            }
        }
    }
    //���p�ĳ��ȴ���1
    else {
        for (i = 0; i < int(nonTerMap.size()); i++) {
            if (nonTerMap[i].second == p[0]) {
                flag = 1;
                merge(connectFirst, first[i], 2);
                break;
            }
        }
        //Ҳ�������ս��
        if (flag == 0) {
            for (i = 0; i < int(terMap.size()); i++) {
                if (terMap[i].second == p[0]) {
                    temp[0] = terMap[i].second;
                    temp[1] = -1;
                    merge(connectFirst, temp, 2);  //�ս����First�������䱾��
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
            emptyRecu[0] = -1;  //�൱�ڳ�ʼ��������ݹ鼯��

            //����Ҳ��ĵ�ǰ�ַ����Ƴ��ղ��һ��������һ���ַ����ͽ�֮���һ���ַ�����First����
            if (reduNull(p[k]) == 1 && k < length - 1) {
                int u = 0;
                for (u = 0; u < int(nonTerMap.size()); u++) {
                    //ע���Ǽ�¼��һ�����ŵ�λ��
                    if (nonTerMap[u].second == p[k + 1]) {
                        flag = 1;
                        merge(connectFirst, first[u], 2);
                        break;
                    }
                }
                //Ҳ�������ս��
                if (flag == 0) {
                    for (u = 0; u < int(terMap.size()); u++) {
                        //ע���Ǽ�¼��һ�����ŵ�λ��
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
            //�������һ���ַ������Ҳ���ʽ�Ҳ������Ƴ���,��$����First����
            else if (reduNull(p[k]) == 1 && k == length - 1) {
                temp[0] = GRAMMAR_NULL;
                temp[1] = -1;  //��ʵ��ģ���ַ����������ֶ�
                merge(connectFirst, temp, 1);
            } else {
                break;
            }
        }
    }
}

/*
 *�������ƣ�followSet
 *������
 *ʵ�ֹ��ܣ�Follow������
 *�������ʱ�䣺4/30
 *�������ߣ���һ��
 *��������DONE
 */
void followSet(int i) {
    int currentNon = nonTerMap[i].second;  //��ǰ�ķ��ս�����
    int temp[2];
    int result = 1;
    temp[0] = currentNon;
    temp[1] = -1;
    merge(followRecu, temp, 1);  //����ǰ��ż�����ݹ鼯����

    //�����ǰ���ž��ǿ�ʼ����,��������ż�����Follow����
    if (proc[1][1] == currentNon) {
        temp[0] = GRAMMAR_SPECIAL;  //���Ҳ��Ҫ�����
        temp[1] = -1;
        merge(follow[i], temp, 1);
    }
    for (int j = 1; j <= procNum; j++)  // j����ڼ�������ʽ
    {
        //����÷��ս����ĳ������ʽ���Ҳ�����
        if (inProcRight(currentNon, proc[j]) == 1) {
            int rightLength = 1;
            int k = 0;  // kΪ�÷��ս���ڲ���ʽ�Ҳ������
            // int flag = 0;
            int leftNum = proc[j][1];  //����ʽ�����
            int h = 0;
            int kArray[Max_Length2];
            memset(kArray, -1, sizeof(kArray));
            // �õ�����ʽ��ߵ��ڷ��ս�����е��±�
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
                //����÷��ս�����Ҳ�����ʽ�����
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
                //����������
                else {
                    int n = 0;
                    result = 1;  //���ǹؼ��ģ���������ʧ���
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
                    connectFirst[0] = -1;  //Ӧ�����³�ʼ��һ��
                    connectFirstSet(temp2);
                    merge(follow[i], connectFirst, 2);
                }
            }
        }
    }
    followVisit[i] = 1;
}

/*
 *�������ƣ�Follow
 *������
 *ʵ�ֹ��ܣ������з��ս����Follow��
 *�������ʱ�䣺4/30
 *�������ߣ���һ��
 *��������DONE
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
 *�������ƣ�Select
 *������
 *ʵ�ֹ��ܣ����Ѿ��ֽ�Ĳ���ʽ��Ӧ��Select��,ע��Select���в��ܺ��п�($),���Type=2
 *�������ʱ�䣺4/30
 *�������ߣ���һ��
 *��������DONE
 */
void Select() {
    for (int i = 1; i <= procNum; i++)  // j����ڼ�������ʽ
    {
        int leftNum = proc[i][1];  //����ʽ�����
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
        //����Ҳ��Ƴ�ʽ�ĳ���Ϊ1�����ǿ�,select[i-1] = follow[���]
        if (rightLength == 1 && proc[i][rightLength + 2] == GRAMMAR_NULL) {
            merge(select[i - 1], follow[h], 2);
        }
        //����Ҳ����ǿյ�ʱ��,select[i-1] = first[�Ҳ�ȫ��]
        //����Ҳ��ܹ��Ƴ��գ�select[i-1] = first[�Ҳ�ȫ��] ^ follow[���]
        else {
            int temp2[Max_Length];
            int n = 0;
            memset(temp2, -1, sizeof(temp2));
            for (n = 1; n <= rightLength; n++) {
                temp2[n - 1] = proc[i][n + 2];
            }
            temp2[rightLength] = -1;
            connectFirst[0] = -1;  //Ӧ�����³�ʼ��һ��
            connectFirstSet(temp2);
            merge(select[i - 1], connectFirst, 2);
            for (n = 1; n <= rightLength; n++) {
                emptyRecu[0] = -1;
                result *= reduNull(proc[i][n + 2]);
            }
            //����Ҳ����Ƴ��գ���follow[���]����select[i-1]��
            if (result == 1) {
                merge(select[i - 1], follow[h], 2);
            }
        }
    }
}

/*
 *�������ƣ�MTable
 *������
 *ʵ�ֹ��ܣ����Ԥ�������
 *�������ʱ�䣺4/30
 *�������ߣ���һ��
 *��������DONE
 */
void MTable() {
    fstream outfile;
    outfile.open("OutputFile\\preciateTable.txt", ios::out);

    for (int i = 0; i < procNum; i++) {
        int m = 0;  //���ս�������
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

    outfile
        << "********************Predictive Analysis Table********************"
        << endl;
    for (int i = 0; i < int(nonTerMap.size()); i++) {
        for (int j = 0; j < int(terMap.size()); j++) {
            outfile << "M[" << nonTerMap[i].first << "][" << terMap[j].first
                    << "] = ";
            // printf("M[%s][%s] = ",nonTerMap[i].first,terMap[j].first);
            for (int k = 0;; k++) {
                if (M[i][j][k] == -1) {
                    break;
                }
                outfile << searchMapping(M[i][j][k]);
                // printf("%s ",searchMapping(M[i][j][k]));
            }
            outfile << endl;
            // printf("\n");
        }
        outfile << endl << endl;
        // printf("\n\n");
    }
    outfile.close();
}

/*
 *�������ƣ�InitStack
 *������*S ջָ��
 *ʵ�ֹ��ܣ���ʼ��˳��ջ
 *�������ʱ�䣺4/30
 *�������ߣ���һ��
 *��������DONE
 */
void InitStack(SeqStack *S) { S->top = -1; }

/*
 *�������ƣ�Push
 *������*S ջָ�� *x �ַ�ָ��
 *ʵ�ֹ��ܣ���ջ
 *�������ʱ�䣺4/30
 *�������ߣ���һ��
 *��������DONE
 */
int Push(SeqStack *S, int x) {
    if (S->top == Stack_Size - 1) return 0;
    S->top++;
    S->elem[S->top] = x;
    return 1;
}

/*
 *�������ƣ�Pop
 *������*S ջָ��
 *ʵ�ֹ��ܣ���ջ
 *�������ʱ�䣺4/30
 *�������ߣ���һ��
 *��������DONE
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
 *�������ƣ�GetTop
 *������*S ջָ�룬*x �ַ�ָ��
 *ʵ�ֹ��ܣ�ȡջ��Ԫ��
 *�������ʱ�䣺4/30
 *�������ߣ���һ��
 *��������DONE
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
 *�������ƣ�ShowStack1
 *������*S ջָ��
 *ʵ�ֹ��ܣ���ʾջ���ַ��������ջ��Ԫ��
 *�������ʱ�䣺4/30
 *�������ߣ���һ��
 *��������DONE
 */
void ShowStack1(SeqStack *S) {
    int i;
    for (i = S->top; i >= 0; i--) {
        // printf("%s ",searchMapping(S->elem[i]));
        resultfile << searchMapping(S->elem[i]) << " ";
    }
}

/*
 *�������ƣ�ShowStack2
 *������*S ջָ��
 *ʵ�ֹ��ܣ���ʾջ���ַ��������ջ��Ԫ��
 *�������ʱ�䣺4/30
 *�������ߣ���һ��
 *��������DONE
 */
void ShowStack2(SeqStack *S) {
    int i;
    for (i = S->top; i >= 0; i--) {
        // printf("%s ",searchMapping(S->elem[i]));
        resultfile << searchMapping(S->elem[i]) << " ";
    }
}

/*
 *�������ƣ�Analysis
 *������
 *ʵ�ֹ��ܣ�����Դ���򣬶Խ��������з���
 *�������ʱ�䣺4/30
 *�������ߣ���һ��
 *��������DONE
 */
void Analysis() {
    //����������
    resultfile.open("OutputFile\\preciateResult.txt", ios::out);

    SeqStack s1, s2;  // ����ջ�м���ʽ �� ���봮
    int c1, c2;
    int i = 0;
    int reserve[Stack_Size];  //����ջ������ջ
    NormalNode *p = normalHead;
    int s1Length = 0;
    memset(reserve, -1, sizeof(reserve));

    InitStack(&s1); /*��ʼ������ջ�����봮*/
    InitStack(&s2);
    Push(&s1, GRAMMAR_SPECIAL);
    Push(&s1, proc[1][1]);
    Push(&s2, GRAMMAR_SPECIAL);

    p = p->next;
    while (p != NULL) {
        reserve[i++] = p->type;
        p = p->next;
    }
    //�����ջ�ĳ���
    for (s1Length = 0;; s1Length++) {
        if (reserve[s1Length] == -1) {
            break;
        }
    }
    //������ջ
    for (i = s1Length; i > 0; i--) {
        Push(&s2, reserve[i - 1]);
    }

    for (i = 0;; i++) /*����*/
    {
        // getch();
        int flag = 0;
        int h1;
        int h2;
        // printf("��%d����\n",i+1);  /*����ò�����Ӧ��Ϣ*/
        resultfile << "��" << i + 1 << "��" << endl;
        // printf("����ջ:");
        resultfile << "����ջ:";
        ShowStack1(&s1);
        // printf("\n");
        resultfile << endl;
        // printf("����ջ:");
        resultfile << "����ջ:";
        ShowStack2(&s2);
        // printf("\n");
        resultfile << endl;

        GetTop(&s1, &c1); /*ȡջ��Ԫ�أ���Ϊc1��c2*/
        GetTop(&s2, &c2);
        if (c1 == GRAMMAR_SPECIAL &&
            c2 == GRAMMAR_SPECIAL) /*������ջ������ջ��ʣ��#ʱ�������ɹ�*/
        {
            // printf("�ɹ�!\n");
            resultfile << "�ɹ�!" << endl;
            break;
        }
        if (c1 == GRAMMAR_SPECIAL &&
            c2 !=
                GRAMMAR_SPECIAL) /*������ջʣ��#�������봮δ����ʱ������ʧ�� */
        {
            // printf("ʧ��!\n");
            resultfile << "ʧ��!" << endl;
            break;
        }
        if (c1 == c2) /*����ջ��ջ��Ԫ�غ����봮��ջ��Ԫ����ͬʱ��ͬʱ����*/
        {
            Pop(&s1);
            Pop(&s2);
            flag = 1;
        }

        else /*��Ԥ�������*/
        {
            //��¼�·��ս����λ��
            for (h1 = 0; h1 < int(nonTerMap.size()); h1++) {
                if (nonTerMap[h1].second == c1) {
                    break;
                }
            }
            //��¼���ս����λ��
            for (h2 = 0; h2 < int(terMap.size()); h2++) {
                if (terMap[h2].second == c2) {
                    break;
                }
            }
            if (M[h1][h2][0] == -1) {
                // printf("Error\n");
                resultfile << "Error" << endl;
                break;  //�������Ļ���ֱ����ֹ����
            } else {
                int length = 0;
                //��¼���Ƶ�ʽ�ĳ���
                for (length = 0;; length++) {
                    if (M[h1][h2][length] == -1) {
                        break;
                    }
                }
                Pop(&s1);
                //������ǿյĻ���������ջ
                if (M[h1][h2][2] != GRAMMAR_NULL) {
                    for (int k = length - 1; k >= 2; k--) {
                        Push(&s1, M[h1][h2][k]);
                    }
                }
            }
        }
        if (flag == 1) {
            // printf("ƥ�䣡\n");
            resultfile << "ƥ��!" << endl;
        } else {
            resultfile << "�����Ƴ�ʽ��";
            // printf("�����Ƴ�ʽ��");
            int w = 0;
            //��¼���Ƶ�ʽ�ĳ���
            for (w = 0;; w++) {
                if (M[h1][h2][w] == -1) {
                    break;
                }
                // printf("%s ",searchMapping(M[h1][h2][w]));
                resultfile << searchMapping(M[h1][h2][w]);
            }
            // printf("\n\n");
            resultfile << endl << endl;
        }
    }
    resultfile.close();
}
