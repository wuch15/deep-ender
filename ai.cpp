#include "teamstyle17.h"
#include "../communicate/communicate.h"
#include <ctime>
#include<stdlib.h>
#include<iostream>
#include<cmath>
#include<fstream>
using namespace std;

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
//敌方id记录
//int boss_id;
//int enemy_id;
//int enemy_true_id;
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
//速度向量标准化
Position speed_std(Position a){
	return(Scale(500/Norm(a),a));
}
bool move_to_boss=true;
int find_energy(const Map*map,const Status* status){
	fight_and_resource temp;
	temp.boss_contact=0;
	temp.enemy_contact=false;
	temp.k=-1;
	int final_choose=-1;
	int energy_num=0;
	int ad_energy_num=0;
	double mindis=1000000;
	int choice=-1;
	int ad_choice=-1;
	double ad_mindis=1000000;
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
				if((map->objects[j].type==ADVANCED_ENERGY)&&Vector_cos(Displacement(map->objects[i].pos,status->objects[0].pos),Displacement(status->objects[0].pos,map->objects[i].pos))>0.7&&(edge_judge(map->objects[i].pos,status->objects[0].radius)==compare)){
					Move(status->objects[0].id,speed_std(Displacement(status->objects[0].pos,map->objects[j].pos))); 
					return 666;
				}
				if((map->objects[j].type==ENERGY)&&Vector_cos(Displacement(map->objects[i].pos,status->objects[0].pos),Displacement(status->objects[0].pos,map->objects[i].pos))>0.7&&(edge_judge(map->objects[i].pos,status->objects[0].radius)==compare)){
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
		if((map->objects[i].type==ADVANCED_ENERGY)&&(edge_judge(map->objects[i].pos,status->objects[0].radius)==compare)){
			if(((Distance(map->objects[i].pos,status->objects[0].pos)-0.9*status->objects[0].radius)/ADVANCED_ENERGY_value)<ad_mindis){
				ad_mindis=(Distance(map->objects[i].pos,status->objects[0].pos)-0.9*status->objects[0].radius)/ADVANCED_ENERGY_value;
				ad_choice=i;
			}
		}
		if(map->objects[i].type==ENERGY&&(edge_judge(map->objects[i].pos,status->objects[0].radius)==compare)){
			if(((Distance(map->objects[i].pos,status->objects[0].pos)-0.85*status->objects[0].radius)/ENERGY_value)<mindis){
				mindis=(Distance(map->objects[i].pos,status->objects[0].pos)-0.85*status->objects[0].radius)/ENERGY_value;
				choice=i;
			}
		}
	}
	if(ad_choice!=-1){
		if(mindis<ad_mindis){
Move(status->objects[0].id,speed_std(Displacement(status->objects[0].pos,map->objects[choice].pos)));
return 500;
		}
		if(mindis>=ad_mindis){	
		Move(status->objects[0].id,speed_std(Displacement(status->objects[0].pos,map->objects[ad_choice].pos)));
		choice=ad_choice;
		return 501;
	}
	}
	else{

		if(choice!=-1){
			Move(status->objects[0].id,speed_std(Displacement(status->objects[0].pos,map->objects[choice].pos)));
			return 502;
		}
		else{

		}
	}
	
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