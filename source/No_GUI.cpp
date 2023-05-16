#include <bits/stdc++.h>
#include <conio.h>
#include <windows.h>
#define  KEY_DOWN(VK_NONAME) ((GetAsyncKeyState(VK_NONAME) & 0x8000) ? 1:0) 
#define  KEY_UP(VK_NONAME) ((GetAsyncKeyState(VK_NONAME) & 0x8000) ? 0:1) 
using namespace std;
set<string> balloon_pid;
const int MAX_TEAM=10000;
const int MAX_LEN=2000;
int balloon_cnt;//队列的气球数
int print_counter;//打印了多少气球 用于判断纸张用量 
struct Balloon{
	mutable string pid;
	mutable string location;
	mutable string name,problem,color;
	mutable bool first;
	Balloon(string id="",string a="",string b="",string c="",string d="",bool e=0):
	pid(id),location(a),name(d),problem(c),color(d),first(e){}
	
	Balloon operator = (const Balloon &o) const{
		pid=o.pid;
		location=o.location;
		name=o.name;problem=o.problem;
		color=o.color;
		first=o.first;
		return o;
	}
}balloons[MAX_TEAM];

struct pos{
	mutable int x,y;
	pos(int a=0,int b=0):x(a),y(b){}
	pos operator = (const pos &o) const{
		x=o.x;y=o.y;
		return *this;
	}
	bool operator == (const pos &o) const{
		if(x==o.x&&y==o.y) return 1;
		return 0;
	}
}Candy;
int Best_score=0;

//Printer Part!
void Print_Balloon(string pid){//打印机开始打印 
	if(pid == "test") pid="print_log\\print.txt";
	else pid = "print_log\\print_" + pid + ".txt";
	ShellExecuteA(NULL,"print",pid.c_str(),"",".",0);//无阻塞运行 
	cout<<"\nPrinted.\n";
	return ;
}
void Load_Printer(Balloon p){//把气球写入print.txt 
	ofstream fout;
	string pid = p.pid;
	pid = "print_log\\print_" + pid + ".txt";
	fout.open(pid.c_str());
	fout<<"\n----气球打印小票----\n";
	fout<<"队伍名称:"<<p.name<<"\n";
	fout<<"通过题目:"<<p.problem<<"\n";
	fout<<"气球颜色:"<<p.color<<"\n";
	fout<<"派送到:"<<p.location<<"\n";
	if(p.first) fout<<"\n该队为 "<<p.problem<<" 题的首A！"<<"\n请为他送上字母气球!";
	fout<<"\n\n";
	fout.close();
}
string read_line(ifstream &fin){
	string s;
	char info[MAX_LEN];
	fin.getline(info,sizeof(info));s=info;
	return s;
}
void Get_Printer(){//获取气球打印列表 
	system("python get_balloon.py");
	Sleep(1000);
	ifstream fin;
	fin.open("print_info\\printer.txt");
	
	char info[MAX_LEN];
	string balloon;
	balloon_cnt=0;
	int pos;
	Balloon new_balloon;
	string tmp;
	
	fin.getline(info,sizeof(info));tmp=info;
	if(tmp!="BALLOON_STATUS"){
		fin.close();
		return ;
	}
	while(fin.getline(info,sizeof(info))){
		if(tmp=="EOF") break;
		//气球id
		new_balloon.pid=read_line(fin);
		if(new_balloon.pid=="EOF") break;
		//队伍名称 
		new_balloon.name=read_line(fin);
		//队伍位置 
		new_balloon.location=read_line(fin);
		//题目
		new_balloon.problem=read_line(fin);
		//气球颜色
		new_balloon.color=read_line(fin);
		//是否首A 
		new_balloon.first=(read_line(fin)=="True"?1:0);
		
		if(balloon_pid.find(new_balloon.pid)==balloon_pid.end()){
			balloons[++balloon_cnt]=new_balloon;
			balloon_pid.insert(new_balloon.pid);
		}
	}
	fin.close();
}
//----------------------


inline int choice(const char *s1,const char *s2,int &pos,int delay_time=-1){
	string s[10];
	bool ok=0;
	s[0]=s1;s[1]=s2;
	while(s[0].length()<23) s[0]=" "+s[0]+" ";
	while(s[1].length()<23) s[1]=" "+s[1]+" ";
	cout<<s[0]<<s[1]<<endl;
	if(pos==0) printf("    ****************                                      ");
	if(pos==1) printf("                            ****************              ");
	
	int dt=1;
	if(delay_time==-1) dt=0,delay_time=1;
	for(int i=1;i<=delay_time;i+=dt){
		if(_kbhit()){
			int ch=_getch();//左75 右77
			if(ch==13) return pos;
			if(ok&&(ch==75||ch==77)) pos^=1;
			else if(ch==224) ok=1;
			else{
				ok=0;continue;
			}
			printf("\r");
			printf("                                                                     ");
			printf("\r");
			if(pos==0) printf("    ****************                                      ");
			if(pos==1) printf("                            ****************              ");
		}
		Sleep(10);
	}
	return -1;
}

void Working(){
	int pos=0,x=0;
	while(1){
		system("cls");
		Get_Printer();//Get ballons into ballons[]
		for(int i=1;i<=balloon_cnt;++i){
			cout<<"当前打印队伍:"<<balloons[i].name<<endl;
			cout<<"当前打印队列剩余气球数: "<<balloon_cnt-i<<endl;
			cout<<"已打印: "<<++print_counter<<endl;
			
			Load_Printer(balloons[i]);
			Print_Balloon(balloons[i].pid);
			int w = choice("暂停","快进",pos,100);//每次打印都等待1s,避免出现之前的一次性出太快来不及暂停修bug的情况 
			if(w==0){
				system("cls");
				while(choice("继续","不继续",x)) system("cls");
			}
			
		}
		
		//一次获取并打印完成 等待5s左右再进行下一次获取打印
		if(balloon_cnt==0) cout<<"此次打印获取为空\n";
		else cout<<"冷却中 O x O ......\n";
		
		//以下为冷却部分 用于暂停或终止程序 
		if(choice("暂停","立即继续",x,500)==0){
			system("cls");
			int w = choice("继续","结束",x);
			if(w==1) return ;
		}
	}
}

void Test_Printer(){
	ofstream fout;
	fout.open("print_log\\print.txt");
	fout<<"测试信息\n如果你能看见这段话\n说明打印机已经\n成功安装并连接\nOvO可以正常开始\n运行程序";
	fout.close();
	Sleep(500);
	Print_Balloon("test");
	Sleep(500);
	return ;
}
void menu(){
	int x=0;
	bool select_1=0,select_2=0;
	while(1){
		system("cls");
		int w = choice("START","TEST",x);
		if(w==0){
			Working();
			return ;
		}
		else Test_Printer();
		Sleep(1000);
	}
	
}
int main(){
	menu();
	return 0;
}
