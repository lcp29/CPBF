/*
 * BrainFuck.cpp
 * CPBF: BrainFuck解释器
 * V1.0
 * by:氢氦锂uuo 果壳（他们是一个人）
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

char c[32767] = "";     //BF程序数组
uint p = 0;             //代码执行处
string source = "";     //源码字符串
int t = 1;

void init(int _argc,char* _argv[],bool& _exitOrNo,ifstream& _f_source,string& _sFN); //_argc:从主函数传递  _argv:从主函数传递
                                                                                     //_exitOrNo:是否退出  _f_source:程序源码的文件流
                                                                                     //_sFN:源码文件名（缩写）
int checkError(string _source);    //给代码查错，_source:存储代码的变量
int execute(string _source);      //执行代码，_source:存储代码的变量

int main(int argc,char* argv[])
{
    bool exitOrNo;                      //是否退出
    ifstream f_source;                  //程序源码的文件流
    string sourceFileName = "";         //源码文件名
    init(argc,argv,exitOrNo,f_source,sourceFileName);  //调用初始化函数
    if(exitOrNo == true) exit(0);       //如果退出变量为true，则退出
    if(f_source.fail())                 //否则，如果打开失败
    {
        if(argc == 1)                   //如果没有参数（即只有一个“参数”：BrainFuck.exe）
        {
            cout<<sourceFileName<<": ";perror("");  //输出init()函数返回的用户输入的文件名，再使用perror()函数输出错误原因
            exit(1);                                //退出
        }
        else if(argc >= 2)                          //如果用户输入了参数
        {
            cout<<argv[1]<<": ";perror("");         //输出用户附加的参数，再输出错误原因
            exit(1);                                //退出
        }
    }
    istream_iterator<char> in(f_source),eos;        //输入流迭代器，用来确定文件的开头和结尾，忽略空格与回车
    copy(in,eos,back_inserter(source));             //使用复制操作输入源码到source变量
    //for(int i = 0;i <= 50;++i)
    source.push_back('\0');
    int srcError = checkError(source);              //源代码查错
    if(srcError == 0) {}                            //输出错误信息
    else if(srcError == 1)
    {
        cout<<"语法错误：‘[’数量与‘]’数量不相等。"<<endl;
        exit(-1);
    }
    else if(srcError == 2)
    {
        cout<<"语法错误：未知的字符。"<<endl;
        exit(-1);
    }
    else if(srcError == 3)
    {
        cout<<"语法错误：‘[’数量与‘]’数量不相等。"<<endl
            <<"语法错误：未知的字符。"<<endl;
        exit(-1);
    }
    int runningError = execute(source); //执行源代码
    return runningError;            //返回
}

void init(int _argc,char* _argv[],bool& _exitOrNo,ifstream& _f_source,string& _sFN)
{
    string _sourceFileName = "";    //源码文件名，以供之后输入
    if(_argc >= 2)                  //如果有两个及以上的参数（一个为程序名，另一个为参数）
    {
        _f_source.open(_argv[1]);   //打开文件（忽略其他的参数）
        _exitOrNo = false;          //不退出
        return;                     //返回
    }
    else if(_argc == 1)             //如果只有一个参数
    {
        cout<<"请输入文件名(按\'Ctrl+Z\'退出)：";   //提示输入文件名
        cin>>_sourceFileName;       //输入
        if(not cin)                 //如果为空（字符串不可能出错）
        {
            _exitOrNo = true;       //退出
            return;                 //返回
        }
        _sFN = _sourceFileName;     //将用户输入的函数名赋值给引用
        _f_source.open(_sourceFileName);    //否则打开文件
        _exitOrNo = false;          //不退出
        return;                     //返回
    }
}

int checkError(string _source)
{
    bool sqBrOK;                    //bool型变量，存储方括号是否匹配
    bool charOK = true;             //bool型变量，存储是否没有不正常的字符
    map<char,int> sqBr;             //一个映射，来存储左方括号和右方括号的数量
    for(uint i = 0;i <= _source.size();++i)     //遍历源码数组
    {
        if(_source[i] == '[') ++sqBr['['];      //如果出现一个左方括号，这个键对应的值加一
        else if(_source[i] == ']') ++sqBr[']']; //如果出现一个右方括号，这个键对应的值加一
    }
    if(sqBr['['] == sqBr[']']) sqBrOK = true;   //如果两个方括号的数量相等（匹配），sqBrOK赋值为true
    else sqBrOK = false;                       //如果不相等，sqBrOK赋值为false
    for(uint i = 0;i <= _source.size();++i)
    {                                           //检测是否有不正常的字符
        if(_source[i] != '>' and _source[i] != '<' and _source[i] != '+' and _source[i] != '-'
           and _source[i] != '.' and _source[i] != ',' and _source[i] != '[' and _source[i] != ']' and _source[i] != '\0')
        {
            charOK = false;                     //如果有的话，charOK赋值为false,由于被初始化为true，所以如果没有，就为true
        }
    }
    if(sqBrOK == true and charOK == true) return 0;         //如果无错，返回0
    else if(sqBrOK == false and charOK == true) return 1;   //如果只有方括号不匹配，返回1
    else if(sqBrOK == true and charOK == false) return 2;   //如果只有不正常的字符，返回2
    else if(sqBrOK == false and charOK == false) return 3;  //如果方括号不匹配且有不正常的字符，返回3
}

int execute(string _source)
{
    for(uint pv_p = 0;_source[pv_p] != '\0';++pv_p)     //遍历程序代码
    {
        Begin:
        switch(_source[pv_p])
        {
        case '<':   //指针减1，带溢出
            --p;
            if(p < 0) p = 32767;
            break;
        case '>':   //指针加1，带溢出
            ++p;
            if(p > 32767) p = 0;
            break;
        case '+':   //指针指向的值加1
            ++c[p];
            break;
        case '-':   //指针指向的值减1
            --c[p];
            break;
        case ',':   //读取输入
            c[p] = _getch();
            break;
        case '.':   //输出
            cout<<c[p];
        case '[':
            if(c[p] != 0) break;    //如果当前指针指向的值不为0，则跳出,否则
            t = 1;                  //t为1
            while(not t == 0)       //循环直到t为0
            {
                ++pv_p;             //程序代码的下一个符号
                if(_source[pv_p] == '\0')   //如果到达程序结尾，输出错误信息，返回
                {
                    cout<<endl<<"运行时错误：方括号不匹配。"<<endl;
                    return 1;
                }
                if(_source[pv_p] == '[') ++t;   //如果又出现了一个'['，t加1。
                else if(_source[pv_p] == ']') --t;  //如果出现了一个']'，t减1.
            }
            ++pv_p;     //从']'的下一个位置运行
            goto Begin; //由于不需要再使pv_p自增1，所以直接跳到Begin
        case ']':
            if(c[p] == 0) break;    //如果当前指针指向的值为0，则跳出，否则
            t = 1;                  //t为1
            while(not t == 0)       //循环直到t为0
            {
                --pv_p;
                if(_source[pv_p] == '\0')   //如果到达程序结尾，输出错误信息，返回
                {
                    cout<<endl<<"运行时错误：方括号不匹配。"<<endl;
                    return 1;
                }
                if(_source[pv_p] == '[') --t;   //如果出现了一个'['，t减1。
                else if(_source[pv_p] == ']') ++t;  //如果又出现了一个']'，t加1.
            }
            ++pv_p;     //从'['的下一个位置运行
            goto Begin; //由于不需要再使pv_p自增1，所以直接跳到Begin
        case '\0':
            return 0;
        }
    }
}
