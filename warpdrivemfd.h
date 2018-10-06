//#######################################################################################//
class MFDWarp:public MFD{
public:
 MFDWarp(DWORD w,DWORD h,VESSEL *vessel);
 ~MFDWarp();
 char *ButtonLabel(int bt);
 int ButtonMenu(const MFDBUTTONMENU **menu) const;
 bool ConsumeButton(int bt,int event);
 bool ConsumeKeyBuffered(DWORD key);
 bool ConsumeKeyImmediate(char *kstate);
 void Update(HDC hDC);
 static int MsgProc(UINT msg,UINT mfd,WPARAM wparam,LPARAM lparam);
};
//#######################################################################################//
VESSEL *focus;
VECTOR3 focus_pos,focus_dir,focus_vel,warp_pos,warp_vel,zero_vel;
double set_dist,set_vel;
int gravity_off,warp_on;
//#######################################################################################//
