//#######################################################################################//
#define STRICT
#define ORBITER_MODULE
#define _CRT_SECURE_NO_DEPRECATE
//#######################################################################################//
#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include "windows.h"
#include "orbitersdk.h"
#include "warpdrivemfd.h"
//#######################################################################################//
#define light 300000000.0
#define warp_base 3.0
#define warp_accel 1.01
#define lower_limit 5.0
#define upper_limit (light*powf(5,warp_base))
//#######################################################################################//
double random05(){return (unirand2()-0.5);}
int sgn(int a){if(a>=0)return 1; else return -1;}
VECTOR3 tvec(double x,double y,double z){VECTOR3 vec={x,y,z};return vec;}
bool operator !=(VECTOR3 a,VECTOR3 b) {if((a.x!=b.x)||(a.y!=b.y)||(a.z!=b.z)) return true; else return false;}
VECTOR3 operator -(VECTOR3 &a,VECTOR3 &b){return tvec(a.x-b.x,a.y-b.y,a.z-b.z);}
VECTOR3 operator +(VECTOR3 &a,VECTOR3 &b){return tvec(a.x+b.x,a.y+b.y,a.z+b.z);}
VECTOR3 operator /(VECTOR3 a,double b){return tvec(a.x/b,a.y/b,a.z/b);}
VECTOR3 operator *(VECTOR3 &a,double &b){return tvec(a.x*b,a.y*b,a.z*b);}
double modv(VECTOR3 v){return sqrt(v.x*v.x+v.y*v.y+v.z*v.z);}
//#######################################################################################//
VECTOR3 nrvec(VECTOR3 iv)
{
 double md;
 VECTOR3 re;

 md=modv(iv);
 if (absd(md)<0.001) {re=iv; return re;}

 re.x=iv.x/md;
 re.y=iv.y/md;
 re.z=iv.z/md;
 return re;
}
//#######################################################################################//
int g_MFDmode;         //identifier for new MFD mode
MFDWarp *mfd_instance; //Instance of MFD
//#######################################################################################//
DLLCLBK void opcDLLInit(HINSTANCE hDLL)
{
 static char *name="Warp Drive-2";       //MFD mode name
 MFDMODESPEC spec;

 spec.name=name;
 spec.key=OAPI_KEY_W;            //MFD mode selection key
 spec.msgproc=MFDWarp::MsgProc;  //MFD mode callback function

 //Register the new MFD mode with Orbiter
 g_MFDmode=oapiRegisterMFDMode(spec);

 randseed(3456);
 gravity_off=0;
 warp_on=0;
 set_dist=0;
 set_vel=lower_limit;
}
//#######################################################################################//
DLLCLBK void opcDLLExit(HINSTANCE hDLL) {oapiUnregisterMFDMode(g_MFDmode);}
//#######################################################################################//
bool compute_forward_pos(double dt,VESSEL *foc)
{
 VECTOR3 v;

 if (!foc) return false;
 foc->GetGlobalPos(focus_pos);
 foc->GetGlobalVel(focus_vel);
 foc->GlobalRot(_V(0,0,1),focus_dir);

 set_dist=(set_vel-lower_limit)*dt;
 v=focus_dir*set_dist;
 if (gravity_off) {warp_pos=focus_pos  ;warp_vel=zero_vel ;}
             else {warp_pos=focus_pos+v;warp_vel=focus_vel;}

 return true;
}
//#######################################################################################//
void act_warp(VECTOR3 p,VECTOR3 v,VESSEL *foc)
{
 VESSELSTATUS2 vs;
 VECTOR3 vec;

 if (!foc) return;

 vs.version=2;
 vs.flag=0;
 foc->GetStatusEx(&vs);

 oapiGetGlobalPos(oapiGetGbodyByIndex(0),&vec); vs.rpos=p-vec;
 oapiGetGlobalVel(oapiGetGbodyByIndex(0),&vec); vs.rvel=v-vec;
 vs.rbody=oapiGetGbodyByIndex(0);

 foc->DefSetStateEx(&vs);
}
//#######################################################################################//
DLLCLBK void opcPreStep(double SimT,double SimDT,double mjd)
{
 double m,pm,r,f;
 VECTOR3 pv,epp,dv;

 focus=oapiGetFocusInterface();
 if (!focus) return;
 if (warp_on) {
  compute_forward_pos(SimDT,focus);
  act_warp(warp_pos,warp_vel,focus);
 }
 if (gravity_off) {
  m=focus->GetMass();
  pm=oapiGetMass(focus->GetSurfaceRef());
  r=(focus->GetAltitude())+oapiGetSize(focus->GetSurfaceRef());
  f=m*pm*GGRAV/(r*r);
  oapiGetGlobalPos(focus->GetSurfaceRef(),&epp);
  focus->Global2Local(epp,pv);
  dv=nrvec(pv);
  focus->AddForce(-dv*f,_V(0,0,0));
 }
}
//#######################################################################################//
MFDWarp::MFDWarp(DWORD w,DWORD h,VESSEL *vessel):MFD(w,h,vessel){}
MFDWarp::~MFDWarp(){mfd_instance=NULL;}
//#######################################################################################//
char *MFDWarp::ButtonLabel(int bt)
{
 //The labels for the buttons used by the MFD
 static char *label[4]={"WRP","DRP","REV","GRV"};
 return (bt<4?label[bt]:0);
}
//#######################################################################################//
int MFDWarp::ButtonMenu(const MFDBUTTONMENU **menu) const
{
 //The menu descriptions for the buttons
 static const MFDBUTTONMENU mnu[5]={
  {"<- Engage",
   "  (the warp drive)",'W'},
  {"<- Drop out of warp",
   "  (Emergency)",'D'},
  {"<- Reverse the warp vector",
   "  (Don't forget to look behind)",'B'},
  {"<- Gravity toggle",
   "  (For fun?)",'G'}
 };
 if (menu) *menu=mnu;
 return 4; //return the number of buttons used
}
//#######################################################################################//
bool MFDWarp::ConsumeButton(int bt,int event) {

 if (!(event & PANEL_MOUSE_LBDOWN)) return false;

 switch(bt){
  case 3:ConsumeKeyBuffered(OAPI_KEY_G);break; //Gravity
  case 2:ConsumeKeyBuffered(OAPI_KEY_B);break; //Reverse
  case 1:ConsumeKeyBuffered(OAPI_KEY_D);break; //Drop
  case 0:ConsumeKeyBuffered(OAPI_KEY_W);break; //Warp
  default:return false;
 }
 return true;
}
//#######################################################################################//
bool MFDWarp::ConsumeKeyImmediate(char *kstate)
{
 if (*(kstate+OAPI_KEY_MINUS )==-128) {set_vel/=warp_accel;if (abs(set_vel)<lower_limit) set_vel=lower_limit*sgn(set_vel);}
 if (*(kstate+OAPI_KEY_EQUALS)==-128) {set_vel*=warp_accel;if (abs(set_vel)>upper_limit) set_vel=upper_limit*sgn(set_vel);}
 return false;
}
//#######################################################################################//
bool MFDWarp::ConsumeKeyBuffered(DWORD key)
{
 if(!focus)return false;
 switch(key){
  case OAPI_KEY_D:
   set_dist=0;
   set_vel=lower_limit;
   gravity_off=0;
   warp_on=0;
   focus->GetGlobalVel(zero_vel);
   return true;
  case OAPI_KEY_B:set_vel=-set_vel;return true;
  case OAPI_KEY_G:if (!gravity_off) focus->GetGlobalVel(zero_vel);gravity_off=!gravity_off;return true;
  case OAPI_KEY_W:warp_on=!warp_on;return true;
  case OAPI_KEY_0:set_vel=lower_limit;return true;
  case OAPI_KEY_1:set_vel=light*powf(1,warp_base)*sgn(set_vel);return true;
  case OAPI_KEY_2:set_vel=light*powf(2,warp_base)*sgn(set_vel);return true;
  case OAPI_KEY_3:set_vel=light*powf(3,warp_base)*sgn(set_vel);return true;
  case OAPI_KEY_4:set_vel=light*powf(4,warp_base)*sgn(set_vel);return true;
  case OAPI_KEY_5:set_vel=light*powf(5,warp_base)*sgn(set_vel);return true;
  case OAPI_KEY_6:set_vel=light*powf(6,warp_base)*sgn(set_vel);return true;
  case OAPI_KEY_7:set_vel=light*powf(7,warp_base)*sgn(set_vel);return true;
  case OAPI_KEY_8:set_vel=light*powf(8,warp_base)*sgn(set_vel);return true;
  case OAPI_KEY_9:set_vel=light*powf(9,warp_base)*sgn(set_vel);return true;
  default:return false;
 }
}
//#######################################################################################//
#define LINE 16
inline void print (HDC hDC,int i,int *j,LPCTSTR cad){TextOut(hDC,i,*j+=LINE,cad,sizeof(char)*strlen(cad));}
inline void printa(HDC hDC,int i,int *j,LPCTSTR cad){TextOut(hDC,i,*j+=10,  cad,sizeof(char)*strlen(cad));}
inline void print (HDC hDC,int i,int  j,LPCTSTR cad){TextOut(hDC,i, j*LINE, cad,sizeof(char)*strlen(cad));}
//#######################################################################################//
void MFDWarp::Update(HDC hDC)
{
 int line;
 double uv;
 char s[100];

 Title(hDC,"Warp Drive Mk2 MFD");

 SetTextColor(hDC,RGB(255,255,255));

 line=32;
 sprintf(s,"Rel velocity: %.3f Km/s",(set_vel-lower_limit)/1000.0);print(hDC,10,&line,s);
 sprintf(s,"C velocity: %.6f C",     (set_vel-lower_limit)/light );print(hDC,10,&line,s);

 uv=set_vel;
 if (uv<0) uv=-uv;

 sprintf(s,"Warp velocity: Warp %.3f",powf(uv/light,1.0/warp_base));print(hDC,10,&line,s);
 line+=LINE;

 if (set_vel>=0) {SetTextColor(hDC,RGB(128,255,128));sprintf(s,"Prograde");}
            else {SetTextColor(hDC,RGB(255,128,128));sprintf(s,"Retrograde");}
 print(hDC,10,&line,s);

 if (gravity_off) {SetTextColor(hDC,RGB(255,128,128));sprintf(s,"Gravity off");}
             else {SetTextColor(hDC,RGB(128,255,128));sprintf(s,"Gravity on");}
 print(hDC,10,&line,s);

 if (warp_on) {SetTextColor(hDC,RGB(255,128,128));sprintf(s,"Warp Drive on");}
         else {SetTextColor(hDC,RGB(128,255,128));sprintf(s,"Warp Drive off");}
 print(hDC,10,&line,s);
}
//#######################################################################################//
int MFDWarp::MsgProc(UINT msg,UINT mfd,WPARAM wparam,LPARAM lparam){switch(msg){case OAPI_MSG_MFD_OPENED:return(int)(new MFDWarp(LOWORD(wparam),HIWORD(wparam),(VESSEL*)lparam));}return 0;}
//#######################################################################################//
