/*
 * BrainFuck.cpp
 * CPBF: BrainFuck解释器
 * V2.2
 * by:氢氦锂uuo 果壳
 */

#include <cstdio>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <ios>
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
bool debugMem = false; //调试模式，默认为关
bool debugSS = false;   //单步调试，默认为关
int mem_m = 0,mem_x = 99;//存储内存监视器的监视范围
char stepKey = ' ';     //单步调试执行按钮
bool hasinputOrN = false;
bool complie = false;
bool ranOrN = false;
int t = 1;
bool putFrame = false;  //是否已经输出了窗口框架
ifstream conf("CPBFconf.ini");

void init(int _argc,char* _argv[],bool& _exitOrNo,ifstream& _f_source,string& _sFN); //_argc:从主函数传递  _argv:从主函数传递
                                                                                     //_exitOrNo:是否退出  _f_source:程序源码的文件流
                                                                                     //_sFN:源码文件名（缩写为sFN）
int checkError(string _source);    //给代码查错，_source:存储代码的变量
int execute(string _source);       //执行代码，_source:存储代码的变量
int executeSS(string _source);     //以单步执行的方式执行代码
void debugMemFunc();               //debug“内存”监视器绘制
void debugSSFunc(const string& _source,uint _pv_p);//debug”单步调试“窗口绘制
void gotoxy(int x,int y);          //将光标跳转到指定位置
void setcolor(int fg,int bg);      //改变窗口输出字符的前景色和背景色
void getxy(int& x,int& y);         //获取光标位置
void complier(const string& sourceFileName, const string& sourceCode);//编译

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
    f_source.close();
    source.push_back('\0');
    for(string::iterator iter = source.begin();iter != source.end();++iter)
    {
        if(*iter != '.' && *iter != ',' && *iter != '<' && *iter != '>' && *iter != '[' && *iter != ']' && *iter != '+' && *iter != '-' && *iter != '\0')
        {
            source.erase(iter);
            --iter;
        }
    }
    int srcError = checkError(source);              //源代码查错
    if(srcError == 1)
    {
        cout<<"语法错误：‘[’数量与‘]’数量不相等。"<<endl;
        exit(-1);
    }
    if(complie == true)
    {
        complier(sourceFileName,source);
        return 0;
    }
    if(debugMem == true)
    {
        bool confFileErr = false;
        if(not conf)
        {
            if(debugMem == true and debugSS == true)
                cout<<"\n\n\n\n\n\n\n\n";
            else if(debugMem == true)
                cout<<"\n\n\n\n\n\n";
            else if(debugSS == true)
                cout<<"\n\n\n";
            else if(debugMem != true and debugSS != true);
            cout<<"打开配置文件错误，一切将按默认处理。"<<endl;
            confFileErr = true;
        }
        int mn = 0,mx = 99;
        string temp;
        char key = ' ';
        conf>>temp;
        if(temp != "[debugMemRange]")
        {
            if(debugMem == true and debugSS == true)
                cout<<"\n\n\n\n\n\n\n\n";
            else if(debugMem == true)
                cout<<"\n\n\n\n\n\n";
            else if(debugSS == true)
                cout<<"\n\n\n";
            else if(debugMem != true and debugSS != true);
            cout<<"配置文件的读取出错，请检查配置文件未损坏或格式正确。"<<endl;
            confFileErr = true;
            goto jmpDebug;
        }
        else
        {
            conf>>mn;
            if(not conf)
            {
                if(debugMem == true and debugSS == true)
                    cout<<"\n\n\n\n\n\n\n\n";
                else if(debugMem == true)
                    cout<<"\n\n\n\n\n\n";
                else if(debugSS == true)
                    cout<<"\n\n\n";
                else if(debugMem != true and debugSS != true);
                cout<<"读取配置文件错误。"<<endl;
                confFileErr = true;
                goto jmpDebug;
            }
            conf>>mx;
            if(not conf)
            {
                if(debugMem == true and debugSS == true)
                    cout<<"\n\n\n\n\n\n\n\n";
                else if(debugMem == true)
                    cout<<"\n\n\n\n\n\n";
                else if(debugSS == true)
                    cout<<"\n\n\n";
                else if(debugMem != true and debugSS != true);
                cout<<"读取配置文件错误。"<<endl;
                confFileErr = true;
                goto jmpDebug;
            }
            conf>>temp;
            conf>>key;
            if((mx - mn != 99 and mn != 32700 and mn != 32767) or mn < 0 or mx > 32767)
            {
                if(debugMem == true and debugSS == true)
                    cout<<"\n\n\n\n\n\n\n";
                else if(debugMem == true)
                    cout<<"\n\n\n\n\n\n";
                else if(debugSS == true)
                    cout<<"\n\n\n";
                else if(debugMem != true and debugSS != true);
                cout<<"配置文件的数字不合法，详见帮助文档。"<<endl;
                confFileErr = true;
                goto jmpDebug;
            }

            mem_m = mn;
            mem_x = mx;
            stepKey = key;
        }
        jmpDebug:
        conf.close();
        if(confFileErr == false)
        {
            if(debugMem == true and debugSS == true)
                cout<<"\n\n\n\n\n\n\n\n";
            else if(debugMem == true)
                cout<<"\n\n\n\n\n\n";
            else if(debugSS == true)
                cout<<"\n\n\n";
            else if(debugMem != true and debugSS != true);
        }
    }
    if(debugMem == true) debugMemFunc();
    if(debugSS == true)
    {
        debugSSFunc(source,0);
        int runningError = executeSS(source);
        return runningError;
    }
    else
    {
        int runningError = execute(source); //执行源代码
        return runningError;            //返回
    }
}

void init(int _argc,char* _argv[],bool& _exitOrNo,ifstream& _f_source,string& _sFN)
{
    string _sourceFileName = "";    //源码文件名，以供之后输入
    if(_argc >= 3)
    {
        _f_source.open(_argv[1]);
        for(int i = 2;i != _argc;++i)   //遍历_argv，查看是否有附加参数
        {
            if(_argv[i][0] == '-' and _argv[i][1] == 'd' and _argv[i][2] == 'm' and _argv[i][3] == '\0') debugMem = true; //如果有，debugMem置为true
            if(_argv[i][0] == '-' and _argv[i][1] == 'd' and _argv[i][2] == 's' and _argv[i][3] == 's' and _argv[i][4] == '\0') debugSS = true;//同上，单步调试
            if(_argv[i][0] == '-' and _argv[i][1] == 'c' and _argv[i][2] == '\0') complie = true;
        }
        _exitOrNo = false;
        _sFN = _argv[1];            //将文件名赋值给引用
        return;
    }
    else if(_argc >= 2)             //如果有两个及以上参数（一个为程序名，另一个为参数）
    {
        _f_source.open(_argv[1]);   //打开文件
        _sFN = _argv[1];            //将文件名赋值给引用
        _exitOrNo = false;          //不退出
        return;                     //返回
    }
    else if(_argc == 1)             //如果只有一个参数
    {
        char yn = ' ';
        cout<<endl<<"请输入文件名(按\'Ctrl+Z\'退出)：";   //提示输入文件名
        cin>>_sourceFileName;       //输入
        if(not cin)                 //如果为空（字符串不可能出错）
        {
            _exitOrNo = true;       //退出
            return;                 //返回
        }
        AskDebugMemOrNo:
        cout<<endl<<"是否开启内存监视?(Y/N)";    //询问是否开启Debug模式
        cin>>yn;
        if((not cin) or (yn != 'Y' and yn != 'y' and yn != 'N' and yn != 'n'))
        {
            cout<<endl<<"请输入Y(y)或N(n)。"<<endl;
            goto AskDebugMemOrNo;
        }
        else if(yn == 'Y' or yn == 'y') debugMem = true;
        AskDebugSSOrNo:
        cout<<endl<<"是否开启单步执行?(Y/N)";
        cin>>yn;
        if((not cin) or (yn != 'Y' and yn != 'y' and yn != 'N' and yn != 'n'))
        {
            cout<<endl<<"请输入Y(y)或N(n)。"<<endl;
            goto AskDebugSSOrNo;
        }
        else if(yn == 'Y' or yn == 'y') debugSS = true;
        AskComplieOrNo:
        cout<<endl<<"是否编译?(Y/N)";
        cin>>yn;
        if((not cin) or (yn != 'Y' and yn != 'y' and yn != 'N' and yn != 'n'))
        {
            cout<<endl<<"请输入Y(y)或N(n)。"<<endl;
            goto AskComplieOrNo;
        }
        else if(yn == 'Y' or yn == 'y') complie = true;
        _sFN = _sourceFileName;     //否则将用户输入的函数名赋值给引用
        _f_source.open(_sourceFileName);    //打开文件
        _exitOrNo = false;          //不退出
        return;                     //返回
    }
}

int checkError(string _source)
{
    bool sqBrOK;                    //bool型变量，存储方括号是否匹配
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
            _source[i] == ' ';                  //如果有的话，请成空格
        }
    }
    if(sqBrOK == true) return 0;         //如果无错，返回0
    else if(sqBrOK == false) return 1;   //如果方括号不匹配，返回1
}

int execute(string _source)
{
    for(uint pv_p = 0;_source[pv_p] != '\0';++pv_p)     //遍历程序代码
    {
        switch(_source[pv_p])
        {
        case '<':   //指针减1，带溢出
            --p;
            if(p > 32767) p = 32767;
            break;
        case '>':   //指针加1，带溢出
            ++p;
            if(p > 32767) p = 0;
            break;
        case '+':   //指针指向的值加1
            ++c[p];
            if(debugMem == true) debugMemFunc();
            break;
        case '-':   //指针指向的值减1
            --c[p];
            if(debugMem == true) debugMemFunc();
            break;
        case ',':   //读取输入
            c[p] = _getch();
            if(debugMem == true) debugMemFunc();
            break;
        case '.':   //输出
            printf("%c",c[p]);
            if(debugMem == true) debugMemFunc();
            break;
        case '[':
            if(c[p] != 0) break;    //如果当前指针指向的值不为0，则跳出,否则
            t = 1;                  //t为1
            while(t != 0)       //循环直到t为0
            {
                ++pv_p;             //程序代码的下一个符号
                if(_source[pv_p] == '\0')   //如果到达程序结尾，输出错误信息，返回
                {
                    cout<<"\n\n"<<"语法错误：方括号不匹配。"<<endl;
                    return 1;
                }
                if(_source[pv_p] == '[') ++t;   //如果又出现了一个'['，t加1。
                else if(_source[pv_p] == ']') --t;  //如果出现了一个']'，t减1.
            }
        case ']':
            if(c[p] == 0) break;    //如果当前指针指向的值为0，则跳出，否则
            t = 1;                  //t为1
            while(t != 0)       //循环直到t为0
            {
                --pv_p;
                if(pv_p == -1)
                {
                    cout<<"\n\n"<<"语法错误：方括号不匹配。"<<endl;
                    return 1;
                }
                if(_source[pv_p] == '[') --t;   //如果出现了一个'['，t减1。
                else if(_source[pv_p] == ']') ++t;  //如果又出现了一个']'，t加1.
            }
        case ' ':
            break;
        case '\0':
            return 0;
        }
    }
}

int executeSS(string _source)
{
    ranOrN = true;
    char key = ' ';
    hasinputOrN = false;
    for(uint pv_p = 0;_source[pv_p] != '\0';++pv_p)     //遍历程序代码
    {
        while(key = _getch())
        {
            if(key == stepKey) break;
        }
        debugSSFunc(_source,pv_p);
        switch(_source[pv_p])
        {
        case '<':   //指针减1，带溢出
            --p;
            if(p > 32767) p = 32767;
            hasinputOrN = false;
            break;
        case '>':   //指针加1，带溢出
            ++p;
            if(p > 32767) p = 0;
            hasinputOrN = false;
            break;
        case '+':   //指针指向的值加1
            ++c[p];
            if(debugMem == true) debugMemFunc();
            hasinputOrN = false;
            break;
        case '-':   //指针指向的值减1
            --c[p];
            if(debugMem == true) debugMemFunc();
            hasinputOrN = false;
            break;
        case ',':   //读取输入
            c[p] = _getch();
            if(debugMem == true) debugMemFunc();
            hasinputOrN = true;
            debugSSFunc(_source,pv_p);
            break;
        case '.':   //输出
            cout<<c[p];
            if(debugMem == true) debugMemFunc();
            hasinputOrN = false;
            break;
        case '[':
            if(c[p] != 0)
            {
                hasinputOrN = false;
                break;    //如果当前指针指向的值不为0，则跳出,否则
            }
            t = 1;                  //t为1
            while(t != 0)       //循环直到t为0
            {
                ++pv_p;             //程序代码的下一个符号
                if(_source[pv_p] == '\0')   //如果到达程序结尾，输出错误信息，返回
                {
                    cout<<"\n\n"<<"语法错误：方括号不匹配。"<<endl;
                    return 1;
                }
                if(_source[pv_p] == '[') ++t;   //如果又出现了一个'['，t加1。
                else if(_source[pv_p] == ']') --t;  //如果出现了一个']'，t减1.
            }
            hasinputOrN = false;
        case ']':
            if(c[p] == 0)
            {
                hasinputOrN = false;
                break;    //如果当前指针指向的值为0，则跳出，否则
            }
            t = 1;                  //t为1
            while(t != 0)       //循环直到t为0
            {
                --pv_p;
                if(pv_p == -1)
                {
                    cout<<"\n\n"<<"语法错误：方括号不匹配。"<<endl;
                    return 1;
                }
                if(_source[pv_p] == '[') --t;   //如果出现了一个'['，t减1。
                else if(_source[pv_p] == ']') ++t;  //如果又出现了一个']'，t加1.
            }
            hasinputOrN = false;
        case ' ':
            break;
        case '\0':
            return 0;
        }
    }
}

void debugMemFunc()
{
    int _x,_y;
    getxy(_x,_y);
    if(debugSS == true && putFrame == false)
    {
        gotoxy(0,0);
        printf("|--------");
        setcolor(4,0);
        printf("%05u",mem_m);
        setcolor(7,0);
        printf("-------------------------------------");
        setcolor(4,0);
        printf("%05u",mem_x);
        setcolor(7,0);
        printf("--------|\n|");
        setcolor(4,0);
        printf("  ");
        for(int i = 0;i <= 19;++i)
        {
            printf("%s%02u"," ",i);
        }
        printf(" ");
        setcolor(7,0);
        printf("|\n");
             // |   00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 |
        printf("%s%s%s%s%s%s","|                                                               |\n"
                             ,"|                                                               |\n"
                             ,"|                                                               |\n"
                             ,"|                                                               |\n"
                             ,"|                                                               |\n"
                             ,"-----------------------------------------------------------------");
    }
    else if(debugSS == false && putFrame == false)
    {
        gotoxy(0,0);
        printf("|--------");
        setcolor(4,0);
        printf("%05u",mem_m);
        setcolor(7,0);
        printf("-------------------------------------");
        setcolor(4,0);
        printf("%05u",mem_x);
        setcolor(7,0);
        printf("--------|\n|");
        setcolor(4,0);
        printf("  ");
        for(int i = 0;i <= 19;++i)
        {
            printf("%s%02u"," ",i);
        }
        printf(" ");
        setcolor(7,0);
        printf("|\n");
             // |   00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 |
        printf("|                                                               |\n|                                                               |\n|                                                               |\n|                                                               |\n|                                                               |\n-----------------------------------------------------------------");
        putFrame = true;
    }
    setcolor(4,0);
    for(int i = 1;i <= 5;++i)
    {
        gotoxy(1,i+1);
        printf("%02d",i);
    }
    for(int i = 0;i <= 19;++i)
    {
        for(int j = 1;j <= 5;++j)
        {
            if((20 * ( j - 1 ) + i) % 2 != 0) setcolor(14,0);
            else setcolor(11,0);
            if(mem_x == 32767)
                if(20 * ( j - 1 ) + i > 67)
                continue;
            gotoxy(4+3*i,j+1);
            printf("%02d",(int)c[mem_m + 20 * ( j - 1 ) + i]);
        }
    }
    setcolor(7,0);
    gotoxy(_x,_y);
}

void debugSSFunc(const string& _source,uint _pv_p)
{
    int _x,_y;
    getxy(_x,_y);
    setcolor(7,0);
    if(debugMem == true)
    {
        uint t = _pv_p;
        int m_x = 32;
        if(_y < 10) _y = 10;
        if(putFrame == false)
        {
            gotoxy(0,8);
            printf("%s%s","|                                                               |\n" //x:65
                         ,"-----------------------------------------------------------------");
            putFrame = true;
        }
        gotoxy(m_x,8);
        if(ranOrN == false)
        {
            setcolor(7,9);
            printf("%c",_source[_pv_p]);
            setcolor(7,0);
        }
        else if(_source[_pv_p] == ',' and hasinputOrN == false)
        {
            setcolor(7,12);
            printf("%c",_source[_pv_p]);
            setcolor(7,0);
        }
        else
        {
            setcolor(7,5);
            printf("%c",_source[_pv_p]);
            setcolor(7,0);
        }
        while(true)
        {
            if(_pv_p == 0 or m_x <= 1) break;
            --_pv_p;
            --m_x;
            gotoxy(m_x,8);
            printf("%c",_source[_pv_p]);
        }
        m_x = 32;
        _pv_p = t;
        while(true)
        {
            if(_pv_p == _source.length() or m_x >= 63) break;
            ++_pv_p;
            ++m_x;
            gotoxy(m_x,8);
            printf("%c",_source[_pv_p]);
        }
    }
    else
    {
        uint t = _pv_p;
        int m_x = 32;
        if(_y < 3) _y = 3;
        if(putFrame == false)
        {
            gotoxy(0,0);
            printf("%s%s%s","-----------------------------------------------------------------\n"
                           ,"|                                                               |\n"
                           ,"-----------------------------------------------------------------");
            putFrame = true;
        }
        gotoxy(m_x,1);
        if(ranOrN == false)
        {
            setcolor(7,9);
            printf("%c",_source[_pv_p]);
            setcolor(7,0);
        }
        else if(_source[_pv_p] == ',' and hasinputOrN == false)
        {
            setcolor(7,12);
            printf("%c",_source[_pv_p]);
            setcolor(7,0);
        }
        else
        {
            setcolor(7,5);
            printf("%c",_source[_pv_p]);
            setcolor(7,0);
        }
        while(true)
        {
            if(_pv_p == 0 or m_x <= 1) break;
            --_pv_p;
            --m_x;
            gotoxy(m_x,1);
            printf("%c",_source[_pv_p]);
        }
        m_x = 32;
        _pv_p = t;
        while(true)
        {
            if(_pv_p == _source.length() or m_x >= 63) break;
            ++_pv_p;
            ++m_x;
            gotoxy(m_x,1);
            printf("%c",_source[_pv_p]);
        }
    }
    gotoxy(_x,_y);
}

void complier(const string& sourceFileName, const string& sourceCode)
{
    cout<<"正在生成C语言源文件……"<<endl;
    ofstream cpp(sourceFileName + ".c");
    cpp<<"#include <stdio.h>\n";        //#include <stdio.h>
    cpp<<"#include <conio.h>\n";        //#include <conio.h>
    cpp<<"char c[32767] = \"\";\n";     //char t[32767] = "";
    cpp<<"unsigned int p = 0;\n";       //unsigned int p = 0;
    cpp<<"int main(){\n";               //int main(){
    for(uint pv_p = 0;sourceCode[pv_p] != '\0';++pv_p)     //遍历程序代码
    {
        switch(sourceCode[pv_p])
        {
        case '<':   //指针减1，带溢出
            cpp<<"--p;\n"
               <<"if(p > 32767) p = 32767;\n";
            break;
        case '>':   //指针加1，带溢出
            cpp<<"++p;\n"
               <<"if(p > 32767) p = 0;\n";
            break;
        case '+':   //指针指向的值加1
            cpp<<"++c[p];\n";
            break;
        case '-':   //指针指向的值减1
            cpp<<"--c[p];\n";
            break;
        case ',':   //读取输入
            cpp<<"c[p] = _getch();\n";
            break;
        case '.':   //输出
            cpp<<"putchar(c[p]);\n";
            break;
        case '[':
            cpp<<"while(c[p])\n"
               <<"{\n";
            break;
        case ']':
            cpp<<"}\n";
            break;
        case ' ':
            break;
        case '\0':
            goto OK;
        }
    }
    OK:
        cpp<<"}\n";
        cpp.close();
        cout<<"生成结束。"<<endl;
        string fileNameNoExt = sourceFileName;
        for(string::iterator iter = fileNameNoExt.end();iter != fileNameNoExt.begin();iter--)
        {
            if(*iter == '.')
            {
                fileNameNoExt.erase(iter,fileNameNoExt.end());
                break;
            }
        }
        string temp = ".\\tcc\\tcc.exe " + sourceFileName + ".c -o " + fileNameNoExt + ".exe";
        system(temp.c_str());
        cout<<"正在删除生成的C语言源文件……"<<endl;
        temp = ".\\" + sourceFileName + ".c";
        size_t s = temp.length();
        wchar_t* buffer = new wchar_t[s + 1];
        MultiByteToWideChar(CP_ACP,0,temp.c_str(),s,buffer,s * sizeof(wchar_t));
        buffer[s] = 0;
        DeleteFileW(buffer);
        delete[] buffer;
        cout<<"编译结束。"<<endl;
        return;
}

void gotoxy(int x,int y)
{
    COORD c;
    c.X = x;
    c.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),c);
}

void setcolor(int fg,int bg)
{
    WORD wc;
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    bg <<= 4;
    wc = (fg & 0x000f)|(bg & 0x00f0);
    SetConsoleTextAttribute(h,wc);
}

void getxy(int& x,int& y)
{
    HANDLE hStdout;
    CONSOLE_SCREEN_BUFFER_INFO pBuffer;
    hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleScreenBufferInfo(hStdout,&pBuffer);
    x = pBuffer.dwCursorPosition.X;
    y = pBuffer.dwCursorPosition.Y;
}
