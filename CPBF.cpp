/*
 * BrainFuck.cpp
 * CPBF: BrainFuck������
 * V1.0
 * by:�⺤�uuo ���ǣ�������һ���ˣ�
 */

#include <cstdio>
#include <iostream>
#include <fstream>
#include <map>
#include <iterator>
#include <string>
#include <windows.h>
#include <conio.h>

using namespace std;

typedef unsigned int uint;

char c[32767] = "";     //BF��������
uint p = 0;             //����ִ�д�
string source = "";     //Դ���ַ���
int t = 1;

void init(int _argc,char* _argv[],bool& _exitOrNo,ifstream& _f_source,string& _sFN); //_argc:������������  _argv:������������
                                                                                     //_exitOrNo:�Ƿ��˳�  _f_source:����Դ����ļ���
                                                                                     //_sFN:Դ���ļ�������д��
int checkError(string _source);    //��������_source:�洢����ı���
int execute(string _source);      //ִ�д��룬_source:�洢����ı���

int main(int argc,char* argv[])
{
    bool exitOrNo;                      //�Ƿ��˳�
    ifstream f_source;                  //����Դ����ļ���
    string sourceFileName = "";         //Դ���ļ���
    init(argc,argv,exitOrNo,f_source,sourceFileName);  //���ó�ʼ������
    if(exitOrNo == true) exit(0);       //����˳�����Ϊtrue�����˳�
    if(f_source.fail())                 //���������ʧ��
    {
        if(argc == 1)                   //���û�в�������ֻ��һ������������BrainFuck.exe��
        {
            cout<<sourceFileName<<": ";perror("");  //���init()�������ص��û�������ļ�������ʹ��perror()�����������ԭ��
            exit(1);                                //�˳�
        }
        else if(argc >= 2)                          //����û������˲���
        {
            cout<<argv[1]<<": ";perror("");         //����û����ӵĲ��������������ԭ��
            exit(1);                                //�˳�
        }
    }
    istream_iterator<char> in(f_source),eos;        //������������������ȷ���ļ��Ŀ�ͷ�ͽ�β�����Կո���س�
    copy(in,eos,back_inserter(source));             //ʹ�ø��Ʋ�������Դ�뵽source����
    //for(int i = 0;i <= 50;++i)
    source.push_back('\0');
    int srcError = checkError(source);              //Դ������
    if(srcError == 0) {}                            //���������Ϣ
    else if(srcError == 1)
    {
        cout<<"�﷨���󣺡�[�������롮]����������ȡ�"<<endl;
        exit(-1);
    }
    else if(srcError == 2)
    {
        cout<<"�﷨����δ֪���ַ���"<<endl;
        exit(-1);
    }
    else if(srcError == 3)
    {
        cout<<"�﷨���󣺡�[�������롮]����������ȡ�"<<endl
            <<"�﷨����δ֪���ַ���"<<endl;
        exit(-1);
    }
    int runningError = execute(source); //ִ��Դ����
    return runningError;            //����
}

void init(int _argc,char* _argv[],bool& _exitOrNo,ifstream& _f_source,string& _sFN)
{
    string _sourceFileName = "";    //Դ���ļ������Թ�֮������
    if(_argc >= 2)                  //��������������ϵĲ�����һ��Ϊ����������һ��Ϊ������
    {
        _f_source.open(_argv[1]);   //���ļ������������Ĳ�����
        _exitOrNo = false;          //���˳�
        return;                     //����
    }
    else if(_argc == 1)             //���ֻ��һ������
    {
        cout<<"�������ļ���(��\'Ctrl+Z\'�˳�)��";   //��ʾ�����ļ���
        cin>>_sourceFileName;       //����
        if(not cin)                 //���Ϊ�գ��ַ��������ܳ���
        {
            _exitOrNo = true;       //�˳�
            return;                 //����
        }
        _sFN = _sourceFileName;     //���û�����ĺ�������ֵ������
        _f_source.open(_sourceFileName);    //������ļ�
        _exitOrNo = false;          //���˳�
        return;                     //����
    }
}

int checkError(string _source)
{
    bool sqBrOK;                    //bool�ͱ������洢�������Ƿ�ƥ��
    bool charOK = true;             //bool�ͱ������洢�Ƿ�û�в��������ַ�
    map<char,int> sqBr;             //һ��ӳ�䣬���洢�����ź��ҷ����ŵ�����
    for(uint i = 0;i <= _source.size();++i)     //����Դ������
    {
        if(_source[i] == '[') ++sqBr['['];      //�������һ�������ţ��������Ӧ��ֵ��һ
        else if(_source[i] == ']') ++sqBr[']']; //�������һ���ҷ����ţ��������Ӧ��ֵ��һ
    }
    if(sqBr['['] == sqBr[']']) sqBrOK = true;   //������������ŵ�������ȣ�ƥ�䣩��sqBrOK��ֵΪtrue
    else sqBrOK = false;                       //�������ȣ�sqBrOK��ֵΪfalse
    for(uint i = 0;i <= _source.size();++i)
    {                                           //����Ƿ��в��������ַ�
        if(_source[i] != '>' and _source[i] != '<' and _source[i] != '+' and _source[i] != '-'
           and _source[i] != '.' and _source[i] != ',' and _source[i] != '[' and _source[i] != ']' and _source[i] != '\0')
        {
            charOK = false;                     //����еĻ���charOK��ֵΪfalse,���ڱ���ʼ��Ϊtrue���������û�У���Ϊtrue
        }
    }
    if(sqBrOK == true and charOK == true) return 0;         //����޴�����0
    else if(sqBrOK == false and charOK == true) return 1;   //���ֻ�з����Ų�ƥ�䣬����1
    else if(sqBrOK == true and charOK == false) return 2;   //���ֻ�в��������ַ�������2
    else if(sqBrOK == false and charOK == false) return 3;  //��������Ų�ƥ�����в��������ַ�������3
}

int execute(string _source)
{
    for(uint pv_p = 0;_source[pv_p] != '\0';++pv_p)     //�����������
    {
        Begin:
        switch(_source[pv_p])
        {
        case '<':   //ָ���1�������
            --p;
            if(p < 0) p = 32767;
            break;
        case '>':   //ָ���1�������
            ++p;
            if(p > 32767) p = 0;
            break;
        case '+':   //ָ��ָ���ֵ��1
            ++c[p];
            break;
        case '-':   //ָ��ָ���ֵ��1
            --c[p];
            break;
        case ',':   //��ȡ����
            c[p] = _getch();
            break;
        case '.':   //���
            cout<<c[p];
        case '[':
            if(c[p] != 0) break;    //�����ǰָ��ָ���ֵ��Ϊ0��������,����
            t = 1;                  //tΪ1
            while(not t == 0)       //ѭ��ֱ��tΪ0
            {
                ++pv_p;             //����������һ������
                if(_source[pv_p] == '\0')   //�����������β�����������Ϣ������
                {
                    cout<<endl<<"����ʱ���󣺷����Ų�ƥ�䡣"<<endl;
                    return 1;
                }
                if(_source[pv_p] == '[') ++t;   //����ֳ�����һ��'['��t��1��
                else if(_source[pv_p] == ']') --t;  //���������һ��']'��t��1.
            }
            ++pv_p;     //��']'����һ��λ������
            goto Begin; //���ڲ���Ҫ��ʹpv_p����1������ֱ������Begin
        case ']':
            if(c[p] == 0) break;    //�����ǰָ��ָ���ֵΪ0��������������
            t = 1;                  //tΪ1
            while(not t == 0)       //ѭ��ֱ��tΪ0
            {
                --pv_p;
                if(_source[pv_p] == '\0')   //�����������β�����������Ϣ������
                {
                    cout<<endl<<"����ʱ���󣺷����Ų�ƥ�䡣"<<endl;
                    return 1;
                }
                if(_source[pv_p] == '[') --t;   //���������һ��'['��t��1��
                else if(_source[pv_p] == ']') ++t;  //����ֳ�����һ��']'��t��1.
            }
            ++pv_p;     //��'['����һ��λ������
            goto Begin; //���ڲ���Ҫ��ʹpv_p����1������ֱ������Begin
        case '\0':
            return 0;
        }
    }
}
