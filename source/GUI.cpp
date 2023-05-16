#include <graphics.h>
#include <set>
#include <fstream>
#include <cmath>
#include <cstdio>
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <cstring>
#include <synchapi.h>
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
bool draw_string(int L,int R,int U,int D,string text){
	setfillcolor(BLACK);
	bar(L,U,R,D);
	setfont(D-U,(R-L)/text.length(),"微软雅黑",0,0,0,0,0,0);
	outtextrect(L,U,R-L+5,D-U+5,text.c_str());
}
void put_text(int L,int R,int U,int D,string text){
	setbkmode(TRANSPARENT);
	setfont(D-U,(R-L)/text.length(),"微软雅黑",0,0,0,0,0,0);
	outtextrect(L,U,R-L+5,D-U+5,text.c_str());
}
bool draw_button(int L,int R,int U,int D,string text,int &last_status){
	//0:无 1:下划线 2:按下 
	int x,y,now_status=0;
	mousepos(&x,&y);
	pos mouse_pos=pos(x,y);
	int height=D-U,length=(R-L)/text.length()/2;
	bool push=0;
	setbkmode(TRANSPARENT);
	if(mouse_pos.x<=R&&mouse_pos.x>=L&&mouse_pos.y<=D&&mouse_pos.y>=U){
		now_status=1;
		if(KEY_DOWN(VK_LBUTTON)) push=1;
		setfont(height,length,"微软雅黑",0,0,0,0,1,0);
	}
	else setfont(height,length,"微软雅黑",0,0,0,0,0,0);
	if(push==1){
		if(last_status!=2){
			bar(L,U,R,D);
			outtextrect(L,U,R-L,D-U,text.c_str());
		}
		last_status=2;
		while(KEY_DOWN(VK_LBUTTON)) delay_ms(5);
		int x,y;
		mousepos(&x,&y);
		if(x<=R&&x>=L&&y<=D&&y>=U) return 1;
		return 0;
	}
	if(last_status==now_status) return 0;
	last_status=now_status;
	bar(L,U,R,D);
	outtextrect(L,U,R-L,D-U,text.c_str());
	return 0;
}
void clean_screen(){
	setfillcolor(WHITE);
	bar(0,0,1000,600);
}

//Printer Part!
void Print_Balloon(string pid){//打印机开始打印 
	if(pid == "test") pid="print_log\\print.txt";
	else pid = "print_log\\print_" + pid + ".txt";
	ShellExecuteA(NULL,"print",pid.c_str(),"",".",0);//无阻塞运行 
	cout<<"Printed.\n";
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
	if(fin.getline(info,sizeof(info))) s=info;
	else s="EOF, PLEASE RETURN.";
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
	while(1){
		//气球id
		new_balloon.pid=read_line(fin);
		if(new_balloon.pid == "EOF, PLEASE RETURN.") break;
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
		fin.getline(info,sizeof(info));
	}
	fin.close();
}
//----------------------


void draw_work(){
	setfillcolor(EGERGB(0x77,0xdd,0xff));
	bar(800,0,1000,600);
}
void delay_stop(int delay_time){
	bool select_1=0,select_2=0,st=0;
	int s1=-1,s2=-1;
	string status[]={"暂 停","继 续"};
	setcolor(WHITE);
	for(int tw=0;tw<=delay_time;++tw){
		delay_ms(10);
		select_1=draw_button(840,1020,460,510,status[st],s1);
		select_2=draw_button(840,1020,530,580,"快 进",s2);
		if(select_1) st^=1,s1=-1;
		else if(select_2){
			setcolor(EGERGB(0x66,0xcc,0xff));
			return ;
		}
		if(st) tw--;
	}
	setcolor(EGERGB(0x66,0xcc,0xff));
}
void Working(){
	clean_screen();
	draw_work();
	setbkmode(TRANSPARENT);
	setfont(50,0,"微软雅黑");
	delay_ms(100);
	while(1){
		Get_Printer();//Get ballons into ballons[]
		for(int i=1;i<=balloon_cnt;++i){
			clean_screen();
			draw_work();
			xyprintf(10,10,"当前打印队伍:");
			xyprintf(10,90,balloons[i].name.c_str());
			string text_rest="当前打印队列剩余气球数: 0000";
			string text_tot="已打印:";
			string text_num="0000";
			
			int num=balloon_cnt-i;
			text_rest[24]+=num/1000;num%=1000;
			text_rest[25]+=num/100;num%=100;
			text_rest[26]+=num/10;num%=10;
			text_rest[27]+=num;
			num=++print_counter;
			text_num[0]+=num/1000;num%=1000;
			text_num[1]+=num/100;num%=100;
			text_num[2]+=num/10;num%=10;
			text_num[3]+=num;
			xyprintf(10,540,text_rest.c_str());//输出剩余气球数量 
			
			setcolor(WHITE);
			xyprintf(810,180,text_tot.c_str());
			xyprintf(810,240,text_num.c_str());
			setcolor(EGERGB(0x66,0xcc,0xff));//输出总计打印数 
			//绘制信息界面------ 
			Load_Printer(balloons[i]);
			Print_Balloon(balloons[i].pid);
			
			delay_stop(100);//每次打印都等待1s,避免出现之前的一次性出太快来不及暂停修bug的情况 
			
		}
		
		//一次获取并打印完成 等待5s左右再进行下一次获取打印
		clean_screen();
		draw_work();
		if(balloon_cnt==0) xyprintf(10,10,"此次打印获取为空");
		else xyprintf(10,10,"冷却中 O x O ......");
		
		//以下为冷却部分 用于暂停或终止程序 
		bool select_1=0,select_2=0,st=0;
		int s1=-1,s2=-1;
		string status[]={"暂 停","继 续"};
		setcolor(WHITE);
		for(int tw=0;tw<=500;++tw){
			delay_ms(10);
			select_1=draw_button(840,1020,460,510,status[st],s1);
			select_2=draw_button(840,1020,530,580,"结 束",s2);
			if(select_1) st^=1;
			else if(select_2) return ;
			if(st) tw--;
		}
		setcolor(EGERGB(0x66,0xcc,0xff));
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

void draw_menu(){
	clean_screen();
	setcolor(EGERGB(0x66,0xcc,0xff));
	put_text(100,900,50,150,"PTA气球打印助手     "); 
}
void draw_test(){
	clean_screen();
	setcolor(EGERGB(0x66,0xcc,0xff));
	put_text(100,900,150,250,"测试打印中..."); 
}
void menu(){
	int s1=-1,s2=-1;
	bool select_1=0,select_2=0;
	draw_menu();
	while(1){
		delay_ms(10);
		select_1=draw_button(380,660,300,350,"开 始",s1);
		select_2=draw_button(330,800,400,450,"测试打印机连接",s2);
		if(select_1){
			s1=s2=-1;
			Working();
			return ;
		}
		else if(select_2){
			s1=s2=-1;
			draw_test();
			Test_Printer();
			draw_menu();
		}
	}
	
}
int main(){
	initgraph(1000,600);
	setcaption("PTA气球打印");
	menu();
	closegraph();
	return 0;
}
