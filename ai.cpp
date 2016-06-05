#include "teamstyle17.h"
#include "../communicate/communicate.h"
#include <ctime>
#include<stdlib.h>
#include<iostream>
#include<cmath>
#include<fstream>
using namespace std;
#define pi 3.1415926536
Speed Toward_now={100,200,300};
Position center={kMapSize/2,kMapSize/2,kMapSize/2};
//全局紧急应对开关
bool no_speed=false;
bool drain=false;
bool strong_enemy_contact=false;
bool strong_boss_contact=false;
bool multi_drain_contact=false;
bool fatal_alert=false;

//资源采集策略开关
bool AE_first=true;
bool balance=false;
bool E_first=false;
bool avoid_AE=false;
int first_skill=0;
//战术开关
bool shield_rush=true;
bool boss_kill=false;
bool enemy_kill=false;
bool enemy_swallow=false;
bool boss_duel=false;

bool hard_fight=false;

bool skillupgrade(const Status *status);

double rad=1;

//全局估值参量
double ENERGY_value=1,ADVANCED_ENERGY_value=10;
//寻路状态
struct fight_and_resource{
	int k;
	bool enemy_contact;
	int boss_contact;
	Position target;
};



Position previos;
bool blocked=false;
int anti_blocked_time=0;
bool skill_1=false;

int fight(const Map*map,const Status* status);


Position add(Position a,Position b){
	Position temp;
	temp.x=a.x+b.x;
	temp.y=a.y+b.y;
	temp.z=a.z+b.z;
	return temp;
}


ofstream ofresult("out.txt", ios::app);
struct sphere{
	Position o;
	double r;

};

//计算矢量的夹角
double Vector_cos(Position vec1, Position vec2){
	return(DotProduct(vec1,vec2)/(Norm(vec1)*Norm(vec2)));
}



//阻塞参量计算
bool target_WallDetection(Position vec1,int self_rad){

	if (vec1.x<=self_rad||vec1.x>=kMapSize-self_rad||vec1.y<=self_rad||vec1.y>=kMapSize-self_rad||vec1.z<=self_rad||vec1.z>=kMapSize-self_rad){
		return false;}
	else{
		return true;
	}
}

//精细阻塞位置判断
class edge{

public:
	bool operator ==(edge& a);
	int a;
	int b;
	int c;
	int d;
	int e;
	int f;
private:


};
edge compare={0,0,0,0,0,0};
bool edge::operator ==(edge& k){
	if(k.a!=a||k.b!=b||k.c!=c||k.d!=d||k.e!=e||k.f!=f){
		return false;
	}
	else return true;
}
edge edge_judge(Position vec1,int self_rad){
	edge temp={0,0,0,0,0,0};
	if(vec1.x<=0.5*self_rad){
		temp.a=1;
	}
	if(vec1.y<=0.5*self_rad){
		temp.b=1;
	}
	if(vec1.z<=0.5*self_rad){
		temp.c=1;
	}
	if(vec1.x>=kMapSize-0.5*self_rad){
		temp.d=1;		
	}
	if(vec1.y>=kMapSize-0.5*self_rad){
		temp.e=1;
	}
	if(vec1.z>=kMapSize-0.5*self_rad){
		temp.f=1;
	}
	return temp;
}
inline int add_edge(edge&c){
	return (c.a+c.b+c.c+c.d+c.e+c.f);
}


bool devour_in_route(Position source,Position destiny,Position devour,double radius){
	if(Distance(source,devour)>Distance(source,destiny)&&PointLineDistance(devour,source,destiny)<=1.2*radius&&DotProduct(Displacement(source,devour),Displacement(source,destiny))>0){
		return false;
	}
	else return true;
}


//速度向量标准化
Position speed_std(Position a){
	return(Scale(500/Norm(a),a));
}
bool move_to_boss=true;
Position devour_pos[30];
Position energy_pos[50];
Position source_pos[20];
Position ae_pos[20];


//double energy_density(Position energy_pos[],int energy_num,Position src,Position des){
//	double density=0;
//	for(int i=0;i<energy_num;i++){
//		density+=DotProduct(Displacement(src,des),Displacement(src,energy_pos[i]));
//	}
//	return density/10000;
//}
//double ad_energy_density(Position ae_pos[],int ad_energy_num,Position src,Position des){
//	double density=0;
//	for(int i=0;i<ad_energy_num;i++){
//		density+=DotProduct(Displacement(src,des),Displacement(src,energy_pos[i]));
//	}
//	return density/10000;
//}
//
//double value_evaluate(Position src,Position des,Position *energy_pos,int energy_num,Position *source_pos,int src_num){
//		double edensity=1;
//		double sdensity=1;
//
//	for(int i=0;i<energy_num;i++){
//		edensity*=(1+100*ENERGY_value/Distance(des,*(energy_pos+i)));
//	}
//	for(int i=0;i<src_num;i++){
//		sdensity*=(1+100*ADVANCED_ENERGY_value/Distance(des,*(source_pos+i))/3);
//	}
//	
//		return edensity*sdensity;
//}
//Position energytarget;
double t_atan(double x,double y){
	if(x>=0&&y>=0){
		return atan(y/x);
	}
		if(x<0&&y>=0){
		return pi/2+abs(atan(y/x));
	}
		if(x<0&&y<0){
		return pi+abs(atan(y/x));
	}
		if(x>=0&&y<0){
		return pi*2+atan(y/x);
	}
}
Position hhhh(Position src,Position energy_pos[],int energy_num,Position ae_pos[],int ad_energy_num,Position devour_pos[],int devour_num){
	double fuck[6][12]={0};
	int fucknum[6][12]={0};
	int fuckvalue[6][12][10]={0};
	Position fuckyou[6][12][10]={0};
	for(int i=0;i<energy_num;i++){
		double r;
		double fai;
		double sita;
		r=Distance(src,energy_pos[i]);
		fai=acos((energy_pos[i].z-src.z)/r)/pi*180;
		sita=t_atan(energy_pos[i].x-src.x,energy_pos[i].y-src.y)/pi*180;
		fuck[int(fai/30)][int(sita/30)]+=ENERGY_value/r;
		fuckyou[int(fai/30)][int(sita/30)][fucknum[int(fai/30)][int(sita/30)]]=energy_pos[i];
		fuckvalue[int(fai/30)][int(sita/30)][fucknum[int(fai/30)][int(sita/30)]]=ENERGY_value;
		fucknum[int(fai/30)][int(sita/30)]++;
		
	}
	for(int i=0;i<ad_energy_num;i++){
		double r;
		double fai;
		double sita;
		r=Distance(src,ae_pos[i]);
		fai=acos((ae_pos[i].z-src.z)/r)/pi*180;
		sita=t_atan(ae_pos[i].x-src.x,ae_pos[i].y-src.y)/pi*180;
		fuck[int(fai/30)][int(sita/30)]+=ADVANCED_ENERGY_value/r;
		fuckyou[int(fai/30)][int(sita/30)][fucknum[int(fai/30)][int(sita/30)]]=ae_pos[i];
		fuckvalue[int(fai/30)][int(sita/30)][fucknum[int(fai/30)][int(sita/30)]]=ADVANCED_ENERGY_value;
		fucknum[int(fai/30)][int(sita/30)]++;
	}
		for(int i=0;i<devour_num;i++){
		double r;
		double fai;
		double sita;
		r=Distance(src,devour_pos[i]);
		fai=acos((devour_pos[i].z-src.z)/r)/pi*180;
		sita=t_atan(devour_pos[i].x-src.x,devour_pos[i].y-src.y)/pi*180;
		fuck[int(fai/30)][int(sita/30)]/=(1+2000/r);
		
	}
		int maxi=0;
		int maxj=0;
		int maxk=0;
		double maxpt=0;

	for(int i=0;i<6;i++){
		for(int j=0;j<12;j++){
			if(fuck[i][j]>maxpt){
				maxpt=fuck[i][j];
				maxi=i;
				maxj=j;
			}
            cout<<fuck[i][j]<<"  ";
	}
		cout<<endl;
	}
	cout<<"最大分区："<<"("<<maxi<<","<<maxj<<")"<<endl;

	double maxchoice=0;
	cout<<"最大分区内物体数："<<fucknum[maxi][maxj]<<endl;
	Position choice;
	for(int m=0;m<fucknum[maxi][maxj];m++){
		double temp=fuckvalue[maxi][maxj][m]/Distance(fuckyou[maxi][maxj][m],src);
		if(temp>maxchoice){
			maxchoice=temp;
			choice=fuckyou[maxi][maxj][m];
		}
	}
	return choice;
}



int find_energy(const Map*map,const Status*status){
	fight_and_resource temp;
	temp.boss_contact=0;
	temp.enemy_contact=false;
	temp.k=-1;
	int final_choose=-1;

	double mindis=1e+20;
	int choice=-1;
	int ad_choice=-1;
	double ad_mindis=1e+20;

	int energy_num=0;
	int src_num=0;
	int ad_energy_num=0;
	int devour_num=0;

	for(int i=0;i<map->objects_number;i++){
		if(map->objects[i].type==DEVOUR){
			devour_pos[devour_num]=map->objects[i].pos;
			devour_num++;
			continue;
		}
		if(map->objects[i].type==ENERGY){
			energy_pos[energy_num]=map->objects[i].pos;
			energy_num++;
			continue;
		}
		if(map->objects[i].type==ADVANCED_ENERGY){
			ae_pos[ad_energy_num]=map->objects[i].pos;
			ad_energy_num++;
			continue;
		}
		if(map->objects[i].type==SOURCE){
			source_pos[src_num]=map->objects[i].pos;
			src_num++;
			continue;
		}
	}

	

	for(int i=0;i<map->objects_number;i++){
		if(map->objects[i].id==status->objects[0].id){
	
			continue ;
		}


		if(map->objects[i].type==BOSS){
	if(map->objects[i].type==BOSS&&status->objects[0].radius<0.82*map->objects[i].radius&&(Distance(status->objects[0].pos,map->objects[i].pos)<=2500)){
		Move(status->objects[0].id,speed_std(Displacement(map->objects[i].pos,status->objects[0].pos)));return 1000;
	}
		if(map->objects[i].type==BOSS&&status->objects[0].radius<0.82*map->objects[i].radius&&(Distance(status->objects[0].pos,map->objects[i].pos)>2500)){
			continue;
	}		
		}

				if(map->objects[i].type==BOSS&&map->objects[i].radius<=1450&&status->objects[0].radius<1.2*map->objects[i].radius){
					boss_duel=true;
					move_to_boss=true;
					Move(status->objects[0].id,speed_std(Displacement(map->objects[i].pos,status->objects[0].pos)));return 404;
				}
				if(map->objects[i].type==BOSS&&map->objects[i].radius<=1450&&status->objects[0].radius>=1.2*map->objects[i].radius){
					Move(status->objects[0].id,speed_std(Displacement(status->objects[0].pos,map->objects[i].pos))); return 233; 
				}
	
		if(map->objects[i].type==BOSS&&status->objects[0].health>5400&&(boss_duel==false||status->objects[0].radius>=1.2*map->objects[i].radius)){
			if(move_to_boss==true){
			Move(status->objects[0].id,speed_std(Displacement(status->objects[0].pos,map->objects[i].pos)));move_to_boss=false;return 101; 
			}
			if(status->objects[0].skill_level[0]>0&&Distance(status->objects[0].pos,map->objects[i].pos)-status->objects[0].radius-map->objects[i].radius<=kLongAttackRange[status->objects[0].skill_level[0]]&&status->objects[0].skill_cd[0]==0&&map->objects[i].radius>1375)
		{ 
			LongAttack(status->objects[0].id,map->objects[i].id);return 11;
		}
if(Distance(status->objects[0].pos,map->objects[i].pos)-status->objects[0].radius-map->objects[i].radius<kShortAttackRange[status->objects[0].skill_level[1]])
		{
			if(status->objects[0].skill_cd[1]==0)
			{if(status->objects[0].skill_level[0]<=3&&map->objects[i].radius>1442){
ShortAttack(status->objects[0].id);return 11;
			}
				if(status->objects[0].skill_level[0]==4&&map->objects[i].radius>1474){
					ShortAttack(status->objects[0].id);return 11;
				}
				if(status->objects[0].skill_level[0]==5&&map->objects[i].radius>1490){
					ShortAttack(status->objects[0].id);return 11;
				}
			}
		}
				Move(status->objects[0].id,speed_std(Displacement(status->objects[0].pos,map->objects[i].pos))); return 101; 
		}




if(map->objects[i].type==PLAYER&&map->objects[i].id!=status->objects[0].id){
	rad=status->objects[0].radius/map->objects[i].radius;

		if(status->objects[0].radius>=map->objects[i].radius){
			if(status->objects[0].radius<=1.2*map->objects[i].radius&&status->objects[0].skill_level[1]==0){
				continue;
			}
		if(Distance(status->objects[0].pos,map->objects[i].pos)-status->objects[0].radius-map->objects[i].radius>kShortAttackRange[status->objects[0].skill_level[1]]){
		if(status->objects[0].skill_level[0]>0&&Distance(status->objects[0].pos,map->objects[i].pos)-status->objects[0].radius-map->objects[i].radius<=kLongAttackRange[status->objects[0].skill_level[0]]&&status->objects[0].skill_cd[0]==0)
		{ LongAttack(status->objects[0].id,map->objects[i].id);return 21;}
		Move(status->objects[0].id,speed_std(Displacement(status->objects[0].pos,map->objects[i].pos))); return 20;
		}

		if(Distance(status->objects[0].pos,map->objects[i].pos)-status->objects[0].radius-map->objects[i].radius<kShortAttackRange[status->objects[0].skill_level[1]])
		{if(status->objects[0].skill_cd[1]==0){
			ShortAttack(status->objects[0].id);return 21;
		}
		if(status->objects[0].skill_level[0]>0&&status->objects[0].skill_cd[0]==0)
		{ LongAttack(status->objects[0].id,map->objects[i].id);return 21;}

		if(status->objects[0].skill_level[DASH]>0&&status->objects[0].skill_cd[DASH]==0){
			Dash(status->objects[0].id);
			return DASH;
		}
		Move(status->objects[0].id,speed_std(Displacement(status->objects[0].pos,map->objects[i].pos)));return 20;

        }
}


		if(status->objects[0].radius<map->objects[i].radius&&Distance(status->objects[0].pos,map->objects[i].pos)<4300){

		if(status->objects[0].skill_level[1]>0&&Distance(status->objects[0].pos,map->objects[i].pos)-status->objects[0].radius-map->objects[i].radius<kShortAttackRange[status->objects[0].skill_level[1]]){
			if(status->objects[0].skill_cd[1]==0){
				ShortAttack(status->objects[0].id);return 21;
			}
			}
		if(status->objects[0].skill_level[0]>0&&Distance(status->objects[0].pos,map->objects[i].pos)-status->objects[0].radius-map->objects[i].radius<=kLongAttackRange[status->objects[0].skill_level[0]]&&status->objects[0].skill_cd[0]==0)
		{ LongAttack(status->objects[0].id,map->objects[i].id);return 21;}
		if(status->objects[0].skill_level[DASH]>0&&status->objects[0].skill_cd[DASH]==0){
			Dash(status->objects[0].id);
			return DASH;
		}
		//if(status->objects[0].radius<0.85*map->objects[i].radius){
			for(int j=0;j<map->objects_number;j++){
				if((map->objects[j].type==ADVANCED_ENERGY)&&Vector_cos(Displacement(map->objects[i].pos,status->objects[0].pos),Displacement(status->objects[0].pos,map->objects[i].pos))>0&&(edge_judge(map->objects[i].pos,status->objects[0].radius)==compare)){
					Move(status->objects[0].id,speed_std(Displacement(status->objects[0].pos,map->objects[j].pos))); 
					return 666;
				}
				if((map->objects[j].type==ENERGY)&&Vector_cos(Displacement(map->objects[i].pos,status->objects[0].pos),Displacement(status->objects[0].pos,map->objects[i].pos))>0.5&&(edge_judge(map->objects[i].pos,status->objects[0].radius)==compare)){
					Move(status->objects[0].id,speed_std(Displacement(status->objects[0].pos,map->objects[j].pos))); 
					return 444;
				}
				else{
					continue;
				}
			}
		//}
	//	Position escape;
	//	escape=speed_std(CrossProduct(Displacement(map->objects[i].pos,status->objects[0].pos),Displacement(center,status->objects[0].pos)));
	//Move(status->objects[0].id,escape); 
		Move(status->objects[0].id,speed_std(Displacement(map->objects[i].pos,status->objects[0].pos))); 
		return 30;
}
	if(status->objects[0].radius<map->objects[i].radius&&Distance(status->objects[0].pos,map->objects[i].pos)-status->objects[0].radius-map->objects[i].radius>=2300){
		continue;
	}
		}



/*		if((map->objects[i].type==ADVANCED_ENERGY)&&(edge_judge(map->objects[i].pos,status->objects[0].radius)==compare)){
			bool safe_to_reach=true;
			for(int p=0;p<devour_num;p++){
				if(devour_in_route(status->objects[0].pos,map->objects[i].pos,devour_pos[p],status->objects[0].radius)==false){
					safe_to_reach=false;
					break;
				}
			}
			if(safe_to_reach==false){
				continue;
			}
			//double temp_density=ad_energy_density(ae_pos,ad_energy_num,status->objects[0].pos,map->objects[i].pos);(1.001*abs(temp_density)+temp_density)
			double temp=(Distance(map->objects[i].pos,status->objects[0].pos)-0.99*status->objects[0].radius)/value_evaluate(status->objects[0].pos,map->objects[i].pos,energy_pos,energy_num,source_pos,src_num)/ADVANCED_ENERGY_value;
			//(Distance(map->objects[i].pos,status->objects[0].pos)-0.99*status->objects[0].radius)/ADVANCED_ENERGY_value;
				
			if(temp<ad_mindis){
				//ad_mindis=(Distance(map->objects[i].pos,status->objects[0].pos)-0.99*status->objects[0].radius)/ADVANCED_ENERGY_value;
				ad_mindis=temp;
				ad_choice=i;
			}
		}
		if(map->objects[i].type==ENERGY&&(edge_judge(map->objects[i].pos,status->objects[0].radius)==compare)){

			bool safe_to_reach=true;
			for(int p=0;p<devour_num;p++){
				if(devour_in_route(status->objects[0].pos,map->objects[i].pos,devour_pos[p],status->objects[0].radius)==false){
					safe_to_reach=false;
					break;
				}
			}
			if(safe_to_reach==false){
				continue;
			}
			double temp2=(Distance(map->objects[i].pos,status->objects[0].pos)-0.99*status->objects[0].radius)/ENERGY_value;
				//(Distance(map->objects[i].pos,status->objects[0].pos)-0.99*status->objects[0].radius)/value_evaluate(status->objects[0].pos,map->objects[i].pos,energy_pos,energy_num,source_pos,src_num)/ENERGY_value;
//double temp_density=energy_density(energy_pos,energy_num,status->objects[0].pos,map->objects[i].pos);/(1.001*abs(temp_density)+temp_density)
			if(temp2<mindis){
				
				//mindis=(Distance(map->objects[i].pos,status->objects[0].pos)-0.99*status->objects[0].radius)/ENERGY_value;
				mindis=temp2;
				choice=i;
			}
		}
	}
	if(ad_choice!=-1){
		if(mindis<ad_mindis){
			//energytarget=map->objects[choice].pos;
Move(status->objects[0].id,speed_std(Displacement(status->objects[0].pos,map->objects[choice].pos)));
return 500;
		}
		if(mindis>=ad_mindis){
			//energytarget=map->objects[ad_choice].pos;
		Move(status->objects[0].id,speed_std(Displacement(status->objects[0].pos,map->objects[ad_choice].pos)));
		choice=ad_choice;
		return 501;
	}
	}
	else{

		if(choice!=-1){
				//energytarget=map->objects[choice].pos;
			Move(status->objects[0].id,speed_std(Displacement(status->objects[0].pos,map->objects[choice].pos)));
			return 502;
		}
		else{

		}*/
	}
	double fuck[6][12]={0};
	int fucknum[6][12]={0};
	int fuckvalue[6][12][10]={0};
	Position fuckyou[6][12][10]={0};
	for(int i=0;i<energy_num;i++){
		if((edge_judge(energy_pos[i],status->objects[0].radius)==compare)==false){
			continue;
		}
		double r;
		double fai;
		double sita;
		r=Distance(status->objects[0].pos,energy_pos[i]);
		fai=acos((energy_pos[i].z-status->objects[0].pos.z)/r)/pi*180;
		sita=t_atan(energy_pos[i].x-status->objects[0].pos.x,energy_pos[i].y-status->objects[0].pos.y)/pi*180;
		fuck[int(fai/30)][int(sita/30)]+=ENERGY_value/(r*r);
		fuckyou[int(fai/30)][int(sita/30)][fucknum[int(fai/30)][int(sita/30)]]=energy_pos[i];
		fuckvalue[int(fai/30)][int(sita/30)][fucknum[int(fai/30)][int(sita/30)]]=ENERGY_value;
		fucknum[int(fai/30)][int(sita/30)]++;
		
	}
	for(int i=0;i<ad_energy_num;i++){
		if((edge_judge(ae_pos[i],status->objects[0].radius)==compare)==false){
			continue;
		}
		double r;
		double fai;
		double sita;
		r=Distance(status->objects[0].pos,ae_pos[i]);
		fai=acos((ae_pos[i].z-status->objects[0].pos.z)/r)/pi*180;
		sita=t_atan(ae_pos[i].x-status->objects[0].pos.x,ae_pos[i].y-status->objects[0].pos.y)/pi*180;
		fuck[int(fai/30)][int(sita/30)]+=ADVANCED_ENERGY_value/(r*r);
		fuckyou[int(fai/30)][int(sita/30)][fucknum[int(fai/30)][int(sita/30)]]=ae_pos[i];
		fuckvalue[int(fai/30)][int(sita/30)][fucknum[int(fai/30)][int(sita/30)]]=ADVANCED_ENERGY_value;
		fucknum[int(fai/30)][int(sita/30)]++;
	}
		for(int i=0;i<devour_num;i++){
		double r;
		double fai;
		double sita;
		r=Distance(status->objects[0].pos,devour_pos[i]);
		fai=acos((devour_pos[i].z-status->objects[0].pos.z)/r)/pi*180;
		sita=t_atan(devour_pos[i].x-status->objects[0].pos.x,devour_pos[i].y-status->objects[0].pos.y)/pi*180;
		int xxx=fai/30;
		int yyy=sita/30;
		  fuck[xxx][yyy]/=(1+2000/r);
		//fuck[xxx-1][yyy]/=(1+2000/r);
		//fuck[xxx+1][yyy]/=(1+2000/r);
		//  fuck[xxx][yyy-1]/=(1+2000/r);
		//fuck[xxx-1][yyy-1]/=(1+2000/r);
		//fuck[xxx+1][yyy-1]/=(1+2000/r);
		//  fuck[xxx][yyy+1]/=(1+2000/r);
		//fuck[xxx-1][yyy+1]/=(1+2000/r);
		//fuck[xxx+1][yyy+1]/=(1+2000/r);
	}
		int maxi=0;
		int maxj=0;
		int maxk=0;
		double maxpt=0;

	for(int i=0;i<6;i++){
		for(int j=0;j<12;j++){
			if(fuck[i][j]>maxpt){
				maxpt=fuck[i][j];
				maxi=i;
				maxj=j;
			}
            cout<<fuck[i][j]<<"  ";
	}
		cout<<endl;
	}
	cout<<"最大分区："<<"("<<maxi<<","<<maxj<<")"<<endl;
	double r2=5000;
		double fai2=(maxi*30+15)*pi/180;
		double sita2=(maxj*30+15)*pi/180;
		Position target_speed;
		target_speed.z=r2*cos(fai2);
		target_speed.x=r2*sin(fai2)*cos(sita2);
		target_speed.y=r2*sin(fai2)*sin(sita2);
	//double maxchoice=0;
	//cout<<"最大分区内物体数："<<fucknum[maxi][maxj]<<endl;
	//Position choice3;
	//for(int m=0;m<fucknum[maxi][maxj];m++){
	//	double temp=fuckvalue[maxi][maxj][m]/Distance(fuckyou[maxi][maxj][m],status->objects[0].pos);
	//	if(temp>maxchoice){
	//		maxchoice=temp;
	//		choice3=fuckyou[maxi][maxj][m];
	//	}
	//}
	//Position temp3=hhhh(status->objects[0].pos,energy_pos,energy_num,ae_pos,ad_energy_num,devour_pos,devour_num);
	Move(status->objects[0].id,speed_std(target_speed));
	return 500;
}
fight_and_resource status_now;
int TIME=0;

int edge_addup(edge &e){
	return (e.a+e.b+e.c+e.d+e.e+e.f);
}

Position target_pos;
bool start=false;
bool destination_reached=false;
bool Gotodes=false;
int skill_tree=0;int tree=0;
bool skillupgrade(const Status *status){
	bool flag=false;
	return false;
		if(skill_tree==0&&status->objects[0].ability>=1){
		UpgradeSkill(status->objects[0].id,SHORT_ATTACK);
		skill_tree++;ADVANCED_ENERGY_value=10;
		return true;
		
	}
		if(skill_tree==1&&status->objects[0].ability>=2){
		UpgradeSkill(status->objects[0].id,HEALTH_UP);
		skill_tree++;ADVANCED_ENERGY_value=10;
		return true;
		
	}
	if(skill_tree==2&&status->objects[0].ability>=2){
		UpgradeSkill(status->objects[0].id,SHORT_ATTACK);
		skill_tree++;ADVANCED_ENERGY_value=10;
		return true;
	}
	if(skill_tree==3&&rad>=0.97&&rad<=1.03){
		tree=0;
	}
		if(skill_tree==3&&rad<0.97){
		tree=1;
	}
		if(skill_tree==3&&rad>1.03){
		tree=2;
	}
		if(tree==2){
	if(skill_tree==3&&status->objects[0].ability>=4){
		UpgradeSkill(status->objects[0].id,SHORT_ATTACK);
		skill_tree++;ADVANCED_ENERGY_value=9;
		return true;

	}
    if(skill_tree==4&&status->objects[0].ability>=4){
		UpgradeSkill(status->objects[0].id,LONG_ATTACK);
		skill_tree++;ADVANCED_ENERGY_value=9;
		return true;
	}
		if(skill_tree==5&&status->objects[0].ability>=2){
			UpgradeSkill(status->objects[0].id,LONG_ATTACK);
		skill_tree++;ADVANCED_ENERGY_value=9;
		return true;
	}
	
		if(skill_tree==6&&status->objects[0].ability>=4){
		UpgradeSkill(status->objects[0].id,LONG_ATTACK);
		skill_tree++;ADVANCED_ENERGY_value=7;
		return true;
		
	}
		if(skill_tree==7&&status->objects[0].ability>=2){
		UpgradeSkill(status->objects[0].id,HEALTH_UP);
		skill_tree++;ADVANCED_ENERGY_value=6;
		return true;
		
	}
		if(skill_tree==8&&status->objects[0].ability>=4){
		UpgradeSkill(status->objects[0].id,HEALTH_UP);
		skill_tree++;ADVANCED_ENERGY_value=5;
		return true;
	}

		if(skill_tree==9&&status->objects[0].ability>=8){
		UpgradeSkill(status->objects[0].id,DASH);
		skill_tree++;ADVANCED_ENERGY_value=5;
		return true;
	}		
	    if(skill_tree==10&&status->objects[0].ability>=2){
		UpgradeSkill(status->objects[0].id,DASH);
		skill_tree++;
		
		ADVANCED_ENERGY_value=4;
		return true;
	}
	if(skill_tree==11&&status->objects[0].ability>=4){
		UpgradeSkill(status->objects[0].id,DASH);
		skill_tree++;ADVANCED_ENERGY_value=4;
		return true;
	}

		if(skill_tree==12&&status->objects[0].ability>=8){
			UpgradeSkill(status->objects[0].id,SHORT_ATTACK);
		skill_tree++;ADVANCED_ENERGY_value=6;
		return true;
	}
		if(skill_tree==13&&status->objects[0].ability>=8){
			UpgradeSkill(status->objects[0].id,LONG_ATTACK);
		skill_tree++;ADVANCED_ENERGY_value=5;
		return true;
	}
		if(skill_tree==14&&status->objects[0].ability>=16){
		UpgradeSkill(status->objects[0].id,SHORT_ATTACK);
		skill_tree++;ADVANCED_ENERGY_value=4;
		return true;
	}
		if(skill_tree==15&&status->objects[0].ability>=16){
			UpgradeSkill(status->objects[0].id,LONG_ATTACK);
		skill_tree++;ADVANCED_ENERGY_value=2.5;
		return true;
	}
		if(skill_tree==16&&status->objects[0].ability>=8){
		UpgradeSkill(status->objects[0].id,HEALTH_UP);
		skill_tree++;ADVANCED_ENERGY_value=1.8;
		return true;
	}
		if(skill_tree==17&&status->objects[0].ability>=8){
		UpgradeSkill(status->objects[0].id,DASH);
		skill_tree++;ADVANCED_ENERGY_value=1.6;
		return true;
	}
		if(skill_tree==18&&status->objects[0].ability>=16){
		UpgradeSkill(status->objects[0].id,DASH);
		skill_tree++;ADVANCED_ENERGY_value=1.2;
		return true;
	}
		if(skill_tree==19&&status->objects[0].ability>=16){
		UpgradeSkill(status->objects[0].id,HEALTH_UP);
		skill_tree++;ADVANCED_ENERGY_value=0.9;
		return true;
	}
			if(skill_tree==20&&status->objects[0].ability>=16){
		UpgradeSkill(status->objects[0].id,VISION_UP);
		skill_tree++;ADVANCED_ENERGY_value=0.5;
		return true;
	}
		}


		if(tree==0){
	if(skill_tree==3&&status->objects[0].ability>=4){
		UpgradeSkill(status->objects[0].id,SHORT_ATTACK);
		skill_tree++;ADVANCED_ENERGY_value=9;
		return true;

	}
    if(skill_tree==4&&status->objects[0].ability>=2){
		UpgradeSkill(status->objects[0].id,HEALTH_UP);
		skill_tree++;ADVANCED_ENERGY_value=9;
		return true;
	}
		if(skill_tree==5&&status->objects[0].ability>=4){
			UpgradeSkill(status->objects[0].id,LONG_ATTACK);
		skill_tree++;ADVANCED_ENERGY_value=9;
		return true;
	}
	
		if(skill_tree==6&&status->objects[0].ability>=2){
		UpgradeSkill(status->objects[0].id,LONG_ATTACK);
		skill_tree++;ADVANCED_ENERGY_value=9;
		return true;
		
	}
		if(skill_tree==7&&status->objects[0].ability>=4){
		UpgradeSkill(status->objects[0].id,LONG_ATTACK);
		skill_tree++;ADVANCED_ENERGY_value=8;
		return true;
		
	}
		if(skill_tree==8&&status->objects[0].ability>=8){
		UpgradeSkill(status->objects[0].id,DASH);
		skill_tree++;ADVANCED_ENERGY_value=4;
		return true;
	}

		if(skill_tree==9&&status->objects[0].ability>=2){
		UpgradeSkill(status->objects[0].id,DASH);
		skill_tree++;ADVANCED_ENERGY_value=6;
		return true;
	}		
	    if(skill_tree==10&&status->objects[0].ability>=8){
		UpgradeSkill(status->objects[0].id,SHORT_ATTACK);
		skill_tree++;
		
		ADVANCED_ENERGY_value=3;
		return true;
	}
	if(skill_tree==11&&status->objects[0].ability>=4){
		UpgradeSkill(status->objects[0].id,HEALTH_UP);
		skill_tree++;ADVANCED_ENERGY_value=3;
		return true;
	}

		if(skill_tree==12&&status->objects[0].ability>=4){
			UpgradeSkill(status->objects[0].id,DASH);
		skill_tree++;ADVANCED_ENERGY_value=3;
		return true;
	}
		if(skill_tree==13&&status->objects[0].ability>=8){
			UpgradeSkill(status->objects[0].id,LONG_ATTACK);
		skill_tree++;ADVANCED_ENERGY_value=2.5;
		return true;
	}
		if(skill_tree==14&&status->objects[0].ability>=16){
		UpgradeSkill(status->objects[0].id,SHORT_ATTACK);
		skill_tree++;ADVANCED_ENERGY_value=2.5;
		return true;
	}
		if(skill_tree==15&&status->objects[0].ability>=16){
		UpgradeSkill(status->objects[0].id,LONG_ATTACK);
		skill_tree++;ADVANCED_ENERGY_value=1.5;
		return true;
	}
		if(skill_tree==16&&status->objects[0].ability>=8){
		UpgradeSkill(status->objects[0].id,DASH);
		skill_tree++;ADVANCED_ENERGY_value=1;
		return true;
	}
		if(skill_tree==17&&status->objects[0].ability>=8){
		UpgradeSkill(status->objects[0].id,HEALTH_UP);
		skill_tree++;ADVANCED_ENERGY_value=0.8;
		return true;
	}
		if(skill_tree==18&&status->objects[0].ability>=16){
		UpgradeSkill(status->objects[0].id,DASH);
		skill_tree++;ADVANCED_ENERGY_value=0.7;
		return true;
	}
		if(skill_tree==19&&status->objects[0].ability>=16){
		UpgradeSkill(status->objects[0].id,HEALTH_UP);
		skill_tree++;ADVANCED_ENERGY_value=0.7;
		return true;
	}
			if(skill_tree==20&&status->objects[0].ability>=16){
				UpgradeSkill(status->objects[0].id,VISION_UP);
		skill_tree++;ADVANCED_ENERGY_value=0.4;
		return true;
	}
		}


		if(tree==1){
	if(skill_tree==3&&status->objects[0].ability>=2){
		UpgradeSkill(status->objects[0].id,HEALTH_UP);
		skill_tree++;ADVANCED_ENERGY_value=8;
		return true;

	}
    if(skill_tree==4&&status->objects[0].ability>=4){
		UpgradeSkill(status->objects[0].id,SHORT_ATTACK);
		skill_tree++;ADVANCED_ENERGY_value=9;
		return true;
	}
		if(skill_tree==5&&status->objects[0].ability>=4){
			UpgradeSkill(status->objects[0].id,HEALTH_UP);
		skill_tree++;ADVANCED_ENERGY_value=9;
		return true;
	}
	
		if(skill_tree==6&&status->objects[0].ability>=4){
			UpgradeSkill(status->objects[0].id,LONG_ATTACK);
		skill_tree++;ADVANCED_ENERGY_value=8;
		return true;
		
	}
		if(skill_tree==7&&status->objects[0].ability>=2){
		UpgradeSkill(status->objects[0].id,LONG_ATTACK);
		skill_tree++;ADVANCED_ENERGY_value=8;
		return true;
		
	}
		if(skill_tree==8&&status->objects[0].ability>=4){
		UpgradeSkill(status->objects[0].id,LONG_ATTACK);
		skill_tree++;ADVANCED_ENERGY_value=8;
		return true;
	}

		if(skill_tree==9&&status->objects[0].ability>=8){
			UpgradeSkill(status->objects[0].id,SHORT_ATTACK);
		skill_tree++;ADVANCED_ENERGY_value=9;
		return true;
	}		
	    if(skill_tree==10&&status->objects[0].ability>=8){
		UpgradeSkill(status->objects[0].id,LONG_ATTACK);
		skill_tree++;
		
		ADVANCED_ENERGY_value=5;
		return true;
	}
	if(skill_tree==11&&status->objects[0].ability>=8){
		UpgradeSkill(status->objects[0].id,DASH);
		skill_tree++;ADVANCED_ENERGY_value=4;
		return true;
	}

		if(skill_tree==12&&status->objects[0].ability>=2){
			UpgradeSkill(status->objects[0].id,DASH);
		skill_tree++;ADVANCED_ENERGY_value=3;
		return true;
	}
		if(skill_tree==13&&status->objects[0].ability>=4){
			UpgradeSkill(status->objects[0].id,DASH);
		skill_tree++;ADVANCED_ENERGY_value=5;
		return true;
	}
		if(skill_tree==14&&status->objects[0].ability>=16){
		UpgradeSkill(status->objects[0].id,LONG_ATTACK);
		skill_tree++;ADVANCED_ENERGY_value=5;
		return true;
	}
		if(skill_tree==15&&status->objects[0].ability>=8){
			UpgradeSkill(status->objects[0].id,HEALTH_UP);
		skill_tree++;ADVANCED_ENERGY_value=5;
		return true;
	}
		if(skill_tree==16&&status->objects[0].ability>=16){
			UpgradeSkill(status->objects[0].id,SHORT_ATTACK);
		skill_tree++;ADVANCED_ENERGY_value=2;
		return true;
	}
		if(skill_tree==17&&status->objects[0].ability>=8){
		UpgradeSkill(status->objects[0].id,DASH);
		skill_tree++;ADVANCED_ENERGY_value=1;
		return true;
	}
		if(skill_tree==18&&status->objects[0].ability>=16){
			UpgradeSkill(status->objects[0].id,DASH);
		skill_tree++;ADVANCED_ENERGY_value=0.5;
		return true;
	}
		if(skill_tree==19&&status->objects[0].ability>=16){
		UpgradeSkill(status->objects[0].id,HEALTH_UP);
		skill_tree++;ADVANCED_ENERGY_value=0.5;
		return true;
	}
		}

		

		return flag;
}



void AIMain() {
	int time_before=TIME;
	const Map*_map;
	const Status*_status;
	_map=GetMap();
	_status=GetStatus();
	previos=_status->objects[0].pos;
	if(start==false){
		Move(_status->objects[0].id,Displacement(_status->objects[0].pos,center));
		cout<<"开局启动"<<endl;
		start=true;
	}
		if(_status->objects[0].skill_level[DASH]>=2&&_status->objects[0].skill_cd[DASH]==0){
			Dash(_status->objects[0].id);
			return ;
		}
	int fight_sta;
	
	if(skillupgrade(_status)==true){
		return ;
	}	
	int i=find_energy(_map,_status);	
	cout<<"之前回合:"<<time_before<<endl;
	TIME=GetTime();
	return ;
}	