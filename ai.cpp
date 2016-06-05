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
double fuck[4][8]={0};
	int fucknum[4][8]={0};
	double fuckvalue[4][8][8]={0};
	Position fuckyou[4][8][8]={0};
//全局估值参量
double ENERGY_value=1,ADVANCED_ENERGY_value=3;
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
	if(vec1.x<=1.01*self_rad){
		temp.a=1;
	}
	if(vec1.y<=1.01*self_rad){
		temp.b=1;
	}
	if(vec1.z<=1.01*self_rad){
		temp.c=1;
	}
	if(vec1.x>=kMapSize-1.01*self_rad){
		temp.d=1;		
	}
	if(vec1.y>=kMapSize-1.01*self_rad){
		temp.e=1;
	}
	if(vec1.z>=kMapSize-1.01*self_rad){
		temp.f=1;
	}
	return temp;
}
inline int add_edge(edge&c){
	return (c.a+c.b+c.c+c.d+c.e+c.f);
}


bool devour_in_route(Position source,Position destiny,Position devour,double radius){
	if(Distance(source,devour)<Distance(source,destiny)&&PointLineDistance(devour,source,destiny)<=1.2*radius&&DotProduct(Displacement(source,devour),Displacement(source,destiny))>0){
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

    


int find_energy(const Map*map,const Status*status){
	fight_and_resource temp;
	temp.boss_contact=0;
	temp.enemy_contact=false;
	temp.k=-1;
	int final_choose=-1;

	//double mindis=1e+20;
	//int choice=-1;
	//int ad_choice=-1;
	//double ad_mindis=1e+20;

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

				if(map->objects[i].type==BOSS&&map->objects[i].radius<=1500&&status->objects[0].radius<1.2*map->objects[i].radius){
					boss_duel=true;
					move_to_boss=true;
					Move(status->objects[0].id,speed_std(Displacement(map->objects[i].pos,status->objects[0].pos)));return 404;
				}
				if(map->objects[i].type==BOSS&&map->objects[i].radius<=1500&&status->objects[0].radius>=1.2*map->objects[i].radius){
					Move(status->objects[0].id,speed_std(Displacement(status->objects[0].pos,map->objects[i].pos))); return 233; 
				}
	
		if(map->objects[i].type==BOSS&&status->objects[0].health>5700&&(boss_duel==false||status->objects[0].radius>=1.2*map->objects[i].radius)){
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
			if((status->objects[0].radius<=1.2*map->objects[i].radius&&status->objects[0].skill_level[1]<=2)||(status->objects[0].radius>1.2*map->objects[i].radius&&status->objects[0].skill_level[1]<=1)){
				if((map->objects[i].short_attack_casting!=-1||map->objects[i].long_attack_casting!=-1)&&status->objects[0].skill_level[SHIELD]!=0&&status->objects[0].skill_cd[SHIELD]==0)
					{
						Shield(status->objects[0].id);
						return 48;
				}
				if(status->objects[0].skill_level[1]>0&&Distance(status->objects[0].pos,map->objects[i].pos)-status->objects[0].radius-map->objects[i].radius<kShortAttackRange[status->objects[0].skill_level[1]])
			if(status->objects[0].skill_cd[1]==0&&status->objects[0].short_attack_casting==-1)
			{
			      ShortAttack(status->objects[0].id);return 21;
		     }
			if(status->objects[0].skill_level[1]==0&&Distance(status->objects[0].pos,map->objects[i].pos)-status->objects[0].radius-map->objects[i].radius<2000)
			{
				Move(status->objects[0].id,speed_std(Displacement(map->objects[i].pos,status->objects[0].pos))); return 20;
			}
				else continue;
			}
		
			
			if(status->objects[0].radius>=1.2*map->objects[i].radius){
				if(Distance(status->objects[0].pos,map->objects[i].pos)-status->objects[0].radius-map->objects[i].radius>300+5*kDashSpeed[status->objects[0].skill_level[DASH]]+kShortAttackRange[status->objects[0].skill_level[1]]){
		if(status->objects[0].skill_level[0]>0&&Distance(status->objects[0].pos,map->objects[i].pos)-status->objects[0].radius-map->objects[i].radius<=kLongAttackRange[status->objects[0].skill_level[0]]&&status->objects[0].skill_cd[0]==0)
		{ LongAttack(status->objects[0].id,map->objects[i].id);return 21;}
		Move(status->objects[0].id,speed_std(Displacement(status->objects[0].pos,map->objects[i].pos))); return 20;
		}

		if(Distance(status->objects[0].pos,map->objects[i].pos)-status->objects[0].radius-map->objects[i].radius<300+5*kDashSpeed[status->objects[0].skill_level[DASH]]+kShortAttackRange[status->objects[0].skill_level[1]])
		{
		if(status->objects[0].skill_level[DASH]>0&&status->objects[0].skill_cd[DASH]==0){
			Dash(status->objects[0].id);
			return DASH;
		}
		if(status->objects[0].skill_level[SHIELD]>0&&status->objects[0].skill_cd[SHIELD]==0)
		{
			Shield(status->objects[0].id);
			return SHIELD;
		}
		if(status->objects[0].skill_cd[1]==0&&status->objects[0].short_attack_casting==-1){
			ShortAttack(status->objects[0].id);return 21;
		}
		if(status->objects[0].skill_level[0]>0&&status->objects[0].skill_cd[0]==0)
		{ LongAttack(status->objects[0].id,map->objects[i].id);return 21;}

		
		Move(status->objects[0].id,speed_std(Displacement(status->objects[0].pos,map->objects[i].pos)));return 20;

        }
			}
		
		
		if(status->objects[0].radius<1.2*map->objects[i].radius){
			if(Distance(status->objects[0].pos,map->objects[i].pos)-status->objects[0].radius-map->objects[i].radius>500+5*kDashSpeed[status->objects[0].skill_level[DASH]]+kShortAttackRange[status->objects[0].skill_level[1]]){
		if(status->objects[0].skill_level[0]>0&&Distance(status->objects[0].pos,map->objects[i].pos)-status->objects[0].radius-map->objects[i].radius<=kLongAttackRange[status->objects[0].skill_level[0]]&&status->objects[0].skill_cd[0]==0)
		{ LongAttack(status->objects[0].id,map->objects[i].id);return 21;}
		Move(status->objects[0].id,speed_std(Displacement(status->objects[0].pos,map->objects[i].pos))); return 20;
		}

		if(Distance(status->objects[0].pos,map->objects[i].pos)-status->objects[0].radius-map->objects[i].radius<500+5*kDashSpeed[status->objects[0].skill_level[DASH]]+kShortAttackRange[status->objects[0].skill_level[1]])
		{
		if(status->objects[0].skill_level[DASH]>0&&status->objects[0].skill_cd[DASH]==0){
			Dash(status->objects[0].id);
			return DASH;
		}
		if(status->objects[0].skill_level[SHIELD]>0&&status->objects[0].skill_cd[SHIELD]==0)
		{
			Shield(status->objects[0].id);
			return SHIELD;
		}
		if(status->objects[0].skill_cd[1]==0&&status->objects[0].short_attack_casting==-1){
			ShortAttack(status->objects[0].id);return 21;
		}
		if(status->objects[0].skill_level[0]>0&&status->objects[0].skill_cd[0]==0)
		{ LongAttack(status->objects[0].id,map->objects[i].id);return 21;}

		
		Move(status->objects[0].id,speed_std(Displacement(status->objects[0].pos,map->objects[i].pos)));return 20;

        }
		}
}
		if(status->objects[0].radius<map->objects[i].radius)
		{
			if(status->objects[0].radius>=0.95*map->objects[i].radius&&Distance(status->objects[0].pos,map->objects[i].pos)-status->objects[0].radius-map->objects[i].radius<900+5*kDashSpeed[status->objects[0].skill_level[DASH]]+kShortAttackRange[status->objects[0].skill_level[1]]&&status->objects[0].skill_level[1]>=2)
			{
				if(status->objects[0].skill_level[DASH]>0&&status->objects[0].skill_cd[DASH]==0){
			Dash(status->objects[0].id);
			return DASH;
		}
		if(status->objects[0].skill_level[SHIELD]>0&&status->objects[0].skill_cd[SHIELD]==0)
		{
			Shield(status->objects[0].id);
			return SHIELD;
		}
		if(status->objects[0].skill_cd[1]==0&&status->objects[0].short_attack_casting==-1){
			ShortAttack(status->objects[0].id);return 21;
		}
		
		Move(status->objects[0].id,speed_std(Displacement(status->objects[0].pos,map->objects[i].pos)));return 20;

        }

			else if(Distance(status->objects[0].pos,map->objects[i].pos)-status->objects[0].radius-map->objects[i].radius<=2300){
				if((map->objects[i].short_attack_casting!=-1||map->objects[i].long_attack_casting!=-1)&&status->objects[0].skill_level[SHIELD]!=0&&status->objects[0].skill_cd[SHIELD]==0)
					{
						Shield(status->objects[0].id);
						return 0;
				}
		if(status->objects[0].skill_level[1]>0&&Distance(status->objects[0].pos,map->objects[i].pos)-status->objects[0].radius-map->objects[i].radius<kShortAttackRange[status->objects[0].skill_level[1]])
			if(status->objects[0].skill_cd[1]==0&&status->objects[0].short_attack_casting==-1)
			{
			      ShortAttack(status->objects[0].id);return 21;
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
				if((map->objects[j].type==ENERGY)&&Vector_cos(Displacement(map->objects[i].pos,status->objects[0].pos),Displacement(status->objects[0].pos,map->objects[i].pos))>0&&(edge_judge(map->objects[i].pos,status->objects[0].radius)==compare)){
					Move(status->objects[0].id,speed_std(Displacement(status->objects[0].pos,map->objects[j].pos))); 
					return 444;
				}
				else{
					continue;
				}
			}
	
		continue;
}
	
		if(status->objects[0].radius<map->objects[i].radius&&Distance(status->objects[0].pos,map->objects[i].pos)-status->objects[0].radius-map->objects[i].radius>=2300)
		{
		
		if((map->objects[i].short_attack_casting!=-1||map->objects[i].long_attack_casting!=-1)&&status->objects[0].skill_level[SHIELD]!=0&&status->objects[0].skill_cd[SHIELD]==0)
					{
						Shield(status->objects[0].id);
						return 0;
				}
		if(status->objects[0].skill_level[1]>0&&Distance(status->objects[0].pos,map->objects[i].pos)-status->objects[0].radius-map->objects[i].radius<kShortAttackRange[status->objects[0].skill_level[1]])
			if(status->objects[0].skill_cd[1]==0&&status->objects[0].short_attack_casting==-1)
			{
			      ShortAttack(status->objects[0].id);return 21;
		     }
			else continue;
		}
	}
		}


	}

	for(int i=0;i<4;i++){
		for(int j=0;j<8;j++){
			fuck[i][j]=0;
			fucknum[i][j]=0;
		}
	}
	

	for(int i=0;i<energy_num;i++){
		if((edge_judge(energy_pos[i],status->objects[0].radius)==compare)==false){
			continue;
		}
		double r;
		double fai;
		double sita;
		

		r=Distance(status->objects[0].pos,energy_pos[i])-status->objects[0].radius;
		fai=acos((energy_pos[i].z-status->objects[0].pos.z)/r)/pi*180;
		sita=t_atan(energy_pos[i].x-status->objects[0].pos.x,energy_pos[i].y-status->objects[0].pos.y)/pi*180;
		
		int x2=fai/45;
		int y2=sita/45;
		if(x2>=4){
			x2=3;
		}
		if(y2>=8){
			y2=7;
		}
		if(x2<0){
			x2=0;
		}
		if(y2<0){
			x2=0;
		}
		fuck[x2][y2]+=ENERGY_value/(r*r);
		fuckyou[x2][y2][fucknum[x2][y2]]=energy_pos[i];
		fuckvalue[x2][y2][fucknum[x2][y2]]=ENERGY_value;
		fucknum[x2][y2]++;
		
	}
	
	for(int i=0;i<ad_energy_num;i++){
		if((edge_judge(ae_pos[i],status->objects[0].radius)==compare)==false){
			continue;
		}
		double r;
		double fai;
		double sita;

		r=Distance(status->objects[0].pos,ae_pos[i])-status->objects[0].radius;
		fai=acos((ae_pos[i].z-status->objects[0].pos.z)/r)/pi*180;
		sita=t_atan(ae_pos[i].x-status->objects[0].pos.x,ae_pos[i].y-status->objects[0].pos.y)/pi*180;

		
		int x3=fai/45;
		int y3=sita/45;
		if(x3>=4){
			x3=3;
		}
		if(y3>=8){
			y3=7;
		}
		if(x3<0){
			x3=0;
		}
		if(y3<0){
			x3=0;
		}
		fuck[x3][y3]+=1000*ADVANCED_ENERGY_value/(r*r);
		fuckyou[x3][y3][fucknum[x3][y3]]=ae_pos[i];
		fuckvalue[x3][y3][fucknum[x3][y3]]=ADVANCED_ENERGY_value;
		fucknum[x3][y3]++;
	}
	
		for(int i=0;i<devour_num;i++){
		double r;
		double fai;
		double sita;
		r=Distance(status->objects[0].pos,devour_pos[i])-status->objects[0].radius;
		fai=acos((devour_pos[i].z-status->objects[0].pos.z)/r)/pi*180;
		sita=t_atan(devour_pos[i].x-status->objects[0].pos.x,devour_pos[i].y-status->objects[0].pos.y)/pi*180;
		int xxx=fai/45;
		int yyy=sita/45;
		if(xxx>=4){
			xxx=3;
		}
		if(yyy>=8){
			yyy=7;
		}
				if(xxx<0){
			xxx=0;
		}
		if(yyy<0){
			yyy=0;
		}
		 fuck[xxx][yyy]/=(1+2000/r);
		 if(xxx==0&&yyy==0){
			 fuck[xxx+1][yyy]/=(1+2000/r);
			 fuck[xxx][yyy+1]/=(1+2000/r);
			 fuck[xxx+1][yyy+1]/=(1+2000/r);

		 }
		 if(xxx==3&&yyy==0){
			 fuck[xxx-1][yyy]/=(1+2000/r);
			 fuck[xxx][yyy+1]/=(1+2000/r);
			 fuck[xxx-1][yyy+1]/=(1+2000/r);

		 }
		 if(xxx==0&&yyy==7){
			 fuck[xxx+1][yyy]/=(1+2000/r);
			 fuck[xxx][yyy-1]/=(1+2000/r);
			 fuck[xxx+1][yyy-1]/=(1+2000/r);

		 }
		 		 if(xxx==3&&yyy==7){
			 fuck[xxx-1][yyy]/=(1+2000/r);
			 fuck[xxx][yyy-1]/=(1+2000/r);
			 fuck[xxx-1][yyy-1]/=(1+2000/r);

		 }
			if(xxx==3&&yyy<7&&yyy>0){
			 fuck[xxx-1][yyy]/=(1+2000/r);
			 fuck[xxx][yyy-1]/=(1+2000/r);
			 fuck[xxx-1][yyy-1]/=(1+2000/r);
			  fuck[xxx][yyy+1]/=(1+2000/r);
			 fuck[xxx-1][yyy+1]/=(1+2000/r);

		 }
			if(xxx==0&&yyy<7&&yyy>0){
			 fuck[xxx+1][yyy]/=(1+2000/r);
			 fuck[xxx][yyy-1]/=(1+2000/r);
			 fuck[xxx+1][yyy-1]/=(1+2000/r);
			  fuck[xxx][yyy+1]/=(1+2000/r);
			 fuck[xxx+1][yyy+1]/=(1+2000/r);
		 }
			if(yyy==0&&xxx<3&&xxx>0){
			 fuck[xxx][yyy+1]/=(1+2000/r);
			 fuck[xxx+1][yyy]/=(1+2000/r);
			 fuck[xxx+1][yyy+1]/=(1+2000/r);
			  fuck[xxx-1][yyy]/=(1+2000/r);
			 fuck[xxx-1][yyy+1]/=(1+2000/r);
		 }
			if(yyy==7&&xxx<3&&xxx>0){
			 fuck[xxx][yyy-1]/=(1+2000/r);
			 fuck[xxx+1][yyy]/=(1+2000/r);
			 fuck[xxx+1][yyy-1]/=(1+2000/r);
			  fuck[xxx-1][yyy]/=(1+2000/r);
			 fuck[xxx-1][yyy-1]/=(1+2000/r);
		 }
			if(xxx<3&&xxx>0&&yyy<7&&yyy>0){
				 fuck[xxx][yyy-1]/=(1+2000/r);
			 fuck[xxx+1][yyy]/=(1+2000/r);
			 fuck[xxx+1][yyy-1]/=(1+2000/r);
			  fuck[xxx-1][yyy]/=(1+2000/r);
			 fuck[xxx-1][yyy-1]/=(1+2000/r);
			  fuck[xxx][yyy+1]/=(1+2000/r);
			 fuck[xxx+1][yyy+1]/=(1+2000/r);
			 fuck[xxx-1][yyy+1]/=(1+2000/r);
			}
	}
		
		double maxmax=0;
		Position dest={100,100,100};
		for(int i=0;i<4;i++){
			for(int j=0;j<8;j++){
				for(int k=0;k<fucknum[i][j];k++){
					double mindev=5000;
					for(int m=0;m<devour_num;m++){
if(Distance(devour_pos[m],status->objects[0].pos)<status->objects[0].radius*1.5){
							fuckvalue[i][j][k]*=(8-pow((1+Vector_cos(Displacement(status->objects[0].pos,fuckyou[i][j][k]),Displacement(status->objects[0].pos,devour_pos[m]))),5));
						}
			
					}
			
					double temp=fuck[i][j]*fuckvalue[i][j][k]/(Distance(status->objects[0].pos,fuckyou[i][j][k])-status->objects[0].radius);
					if(temp>maxmax){
						maxmax=temp;
						dest=fuckyou[i][j][k];

					}
				}
			}
		}
		cout<<"**********************************"<<endl;
		cout<<dest.x<<" "<<dest.y<<" "<<dest.z<<endl;
		cout<<"**********************************"<<endl;



	Move(status->objects[0].id,speed_std(Displacement(status->objects[0].pos,dest)));
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
	int add=0;
	status=GetStatus();
	for(int i=0;i<=5;i++){
		add+=status->objects[0].skill_level[i];
	}
	int sklv=add;
	    if(sklv==0&&status->objects[0].ability>=1){
		UpgradeSkill(status->objects[0].id,SHORT_ATTACK);
		 ADVANCED_ENERGY_value=3;
		return true;
		
	}
		if(sklv==1&&status->objects[0].ability>=2){
		UpgradeSkill(status->objects[0].id,HEALTH_UP);
		 ADVANCED_ENERGY_value=3;
		return true;
		
	}
	if(sklv==2&&status->objects[0].ability>=2){
		UpgradeSkill(status->objects[0].id,HEALTH_UP);
		 ADVANCED_ENERGY_value=3;
		return true;
	}

	if(sklv==3&&status->objects[0].ability>=4){
		UpgradeSkill(status->objects[0].id,HEALTH_UP);
		 ADVANCED_ENERGY_value=3;
		return true;

	}
    if(sklv==4&&status->objects[0].ability>=2){
		UpgradeSkill(status->objects[0].id,SHORT_ATTACK);
		 ADVANCED_ENERGY_value=3;
		return true;
	}
		if(sklv==5&&status->objects[0].ability>=4){
			UpgradeSkill(status->objects[0].id,SHORT_ATTACK);
		 ADVANCED_ENERGY_value=2.5;
		return true;
	}
	
		if(sklv==6&&status->objects[0].ability>=4){
			UpgradeSkill(status->objects[0].id,SHIELD);
		 ADVANCED_ENERGY_value=2.5;
		return true;
		
	}
		if(sklv==7&&status->objects[0].ability>=2){
		UpgradeSkill(status->objects[0].id,SHIELD);
		 ADVANCED_ENERGY_value=2.5;
		return true;
		
	}
		if(sklv==8&&status->objects[0].ability>=4){
		UpgradeSkill(status->objects[0].id,SHIELD);
		 ADVANCED_ENERGY_value=2.5;
		return true;
	}

		if(sklv==9&&status->objects[0].ability>=8){
		UpgradeSkill(status->objects[0].id,HEALTH_UP);
		 ADVANCED_ENERGY_value=2.5;
		return true;
	}		
	    if(sklv==10&&status->objects[0].ability>=16){
		UpgradeSkill(status->objects[0].id,HEALTH_UP);
		 
		
		ADVANCED_ENERGY_value=2.5;
		return true;
	}
	if(sklv==11&&status->objects[0].ability>=8){
		UpgradeSkill(status->objects[0].id,SHIELD);
		 ADVANCED_ENERGY_value=2.5;
		return true;
	}

		if(sklv==12&&status->objects[0].ability>=16){
			UpgradeSkill(status->objects[0].id,SHIELD);
		 ADVANCED_ENERGY_value=2.5;
		return true;
	}
		if(sklv==13&&status->objects[0].ability>=8){
			UpgradeSkill(status->objects[0].id,DASH);
		 ADVANCED_ENERGY_value=2.5;
		return true;
	}
		if(sklv==14&&status->objects[0].ability>=2){
		UpgradeSkill(status->objects[0].id,DASH);
		 ADVANCED_ENERGY_value=2;
		return true;
	}
		if(sklv==15&&status->objects[0].ability>=4){
			UpgradeSkill(status->objects[0].id,DASH);
		 ADVANCED_ENERGY_value=2;
		return true;
	}
		if(sklv==16&&status->objects[0].ability>=8){
		UpgradeSkill(status->objects[0].id,SHORT_ATTACK);
		 ADVANCED_ENERGY_value=1.8;
		return true;
	}
		if(sklv==17&&status->objects[0].ability>=8){
		UpgradeSkill(status->objects[0].id,DASH);
		 ADVANCED_ENERGY_value=1.8;
		return true;
	}
		if(sklv==18&&status->objects[0].ability>=16){
		UpgradeSkill(status->objects[0].id,SHORT_ATTACK);
		 ADVANCED_ENERGY_value=1.6;
		return true;
	}
		if(sklv==19&&status->objects[0].ability>=16){
		UpgradeSkill(status->objects[0].id,DASH);
		 ADVANCED_ENERGY_value=1.8;
		return true;
	}
			if(sklv==20&&status->objects[0].ability>=16){
		UpgradeSkill(status->objects[0].id,LONG_ATTACK);
		 ADVANCED_ENERGY_value=1.8;
		return true;
	}
		if(sklv==21&&status->objects[0].ability>=2){
		UpgradeSkill(status->objects[0].id,LONG_ATTACK);
		 ADVANCED_ENERGY_value=1.8;
		return true;
	}
		if(sklv==22&&status->objects[0].ability>=4){
		UpgradeSkill(status->objects[0].id,LONG_ATTACK);
		 ADVANCED_ENERGY_value=1.5;
		return true;
	}
		if(sklv==23&&status->objects[0].ability>=8){
		UpgradeSkill(status->objects[0].id,LONG_ATTACK);
		 ADVANCED_ENERGY_value=1.2;
		return true;
	}
				if(sklv==24&&status->objects[0].ability>=16){
		UpgradeSkill(status->objects[0].id,LONG_ATTACK);
		 ADVANCED_ENERGY_value=0.8;
		return true;
	} 

		return flag;
}

int skilltime=0;


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
	
		if(time_before-skilltime>10&&skillupgrade(_status)==true){
			skilltime=TIME;
		return ;
	}	
		if(_status->objects[0].skill_level[DASH]>=2&&_status->objects[0].skill_cd[DASH]==0){
			Dash(_status->objects[0].id);
			return ;
		}
		if(_status->objects[0].skill_level[SHIELD]>=3&&_status->objects[0].skill_cd[SHIELD]==0&&_status->objects[0].skill_level[HEALTH_UP]==5){
			Shield(_status->objects[0].id);
			return ;
		}
	//int fight_sta;
	int i=find_energy(_map,_status);	
	cout<<"之前回合:"<<time_before<<endl;
	TIME=GetTime();
	return ;
}	