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

// extern�Ĳ��ִ�����ܳ��ֵ��ս��������,extern��ʾ�������ܳ����ڱ���ļ���
extern vector<pair<const char *, int> > keyMap;
extern vector<pair<const char *, int> > operMap;
extern vector<pair<const char *, int> > limitMap;
extern vector<pair<const char *, int> > AddMap;

vector<pair<const char *, int> > nonTerMap;  //���ս��ӳ���,�����ظ���
vector<pair<const char *, int> > terMap;  //�ս��ӳ���,�����ظ���
vector<pair<const char *, int> >
    specialMap;  //�ķ��е��������ӳ�������-> | $(��)

// ���ķ��е�Լ�����Ž��д���->, ��($)��#
void initSpecialMapping() {
    specialMap.clear();
    specialMap.push_back(make_pair("->", GRAMMAR_ARROW));
    specialMap.push_back(make_pair("$", GRAMMAR_NULL));
    specialMap.push_back(make_pair("#", GRAMMAR_SPECIAL));
}

// ok ��̬���ɷ��ս������map���ڻ���Ļ����ϣ�ȷ�������ս����ͻ������syn
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

// ok
// �ж��ķ�����ȡ�ķ����ǲ����ս�����ǵĻ�����map����syn���������dynamicNoter������ս��
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

// ͨ�����ƥ������
const char *searchMapping(int num) {
    //��־��
    if (num == IDN) {
        return "id";
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

// ok
// ��ȡ�����ķ���������ʽ����Ժ���뵽��proc������������������ս���ͷ��ս��MAP���
void initGrammer() {
    FILE *infile;
    char ch;
    char array[30];
    char *word;
    int i;
    int codeNum;
    int line = 1;
    int count = 0;  //���ķ���������е�ǰ�ж����˵ڼ����ķ���Ԫ
    // int orNum = 0;
    infile = fopen("wenfa.txt", "r");
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

    // ����ս�����ļ�
    fstream outfile1;
    outfile1.open("terminal.txt", ios::out);
    outfile1 << "terminal list:" << endl;
    for (int i = 0; i < int(terMap.size()); i++) {
        outfile1 << terMap[i].first << endl;
    }
    outfile1.close();

    // ������ս�����ļ�
    fstream outfile2;
    outfile2.open("nonterminal.txt", ios::out);
    outfile2 << "nonterminal list:" << endl;
    for (int i = 0; i < int(nonTerMap.size()); i++) {
        outfile2 << nonTerMap[i].first << endl;
    }
    outfile2.close();
}

//�ж�ĳ������ǲ����ս���ı�ţ�1�����ǣ�0�����
int inTer(int n) {
    for (int i = 0; i < int(terMap.size()); i++) {
        if (terMap[i].second == n) {
            return 1;
        }
    }
    return 0;
}
//�ж�ĳ������ǲ��Ƿ��ս���ı��,1�����ǣ�0�����
int inNonTer(int n) {
    for (int i = 0; i < int(nonTerMap.size()); i++) {
        if (nonTerMap[i].second == n) {
            return 1;
        }
    }
    return 0;
}

//�ж�ĳ������ڲ��ڴ�ʱ��empty���У�1�����ǣ�0�����
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

//????�ж�ĳ������ڲ��ڴ�ʱ��emptyRecu���У�1�����ǣ�0�����
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

//������������������
//��s���Ϻϲ���d�����У�type = 1��������գ�$��,type = 2����������
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

//�������ֱ���Ƴ��յķ��ս������
//�����ԣ�A->BC B->�� C->�� �޷��õ�A ���Ƴ��� ����ʽ�Ҷ�ֻ��һ����Ԫ
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

//�жϸ÷��ս���Ƿ����Ƴ��գ����ս��Ҳ���ܴ��룬��û��ϵ
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

//��first��������Ĳ������ڷ��ս�������е����
void firstSet(int i) {
    int k = 0;
    int currentNon = nonTerMap[i].second;  //��ǰ�ķ��ս�����
    //���α���ȫ������ʽ
    // cout<<nonTerMap[i].first<<endl;
    for (int j = 1; j <= procNum; j++)  // j����ڼ�������ʽ
    {
        //�ҵ��÷��ս���Ĳ���ʽ
        if (currentNon == proc[j][1])  //ע���1��ʼ
        {
            //���ұߵĵ�һ�����ս�����߿յ�ʱ��
            if (inTer(proc[j][3]) == 1 || proc[j][3] == GRAMMAR_NULL) {
                //���뵱ǰ���ս����first����
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

void First() {
    //�������ֱ���Ƴ��յķ��ս������
    nullSet(GRAMMAR_NULL);  // OK
    printf("\n");
    for (int i = 0; i < int(nonTerMap.size()); i++) {
        firstSet(i);
    }

    fstream outfile3;
    outfile3.open("first.txt", ios::out);
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
