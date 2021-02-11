#if !defined(__WINDOW___)

#if !defined(__DOS_DEF_)
#include"dos.h"
#endif
#define __MOUSE__

#define DOWN -1
#define UP   -2
#define MOVE -3


typedef struct{
	   int x,y,Button;
	   char Pass;
	}MOUSE;

int Init_Mouse(void){
	struct REGPACK r;
	r.r_ax=0;
	intr(0x33,&r);
	if(r.r_ax==0) return 0; else return -1;
	}

int Show_Mouse(void){
	struct REGPACK r;
	r.r_ax=1;
	intr(0x33,&r);
}
int Hide_Mouse(void){
	struct REGPACK r;
	r.r_ax=2;
	intr(0x33,&r);
}
void Mouse_to(int x, int y){
	struct REGPACK r;
	r.r_ax=4;
	r.r_cx=x;
	r.r_dx=y;
	intr(0x33,&r);
}
int Mouse_Down(MOUSE *M){
	struct REGPACK r;

	if(M->Pass==DOWN){
		return -1;
	}

	M->Button=-1;
	r.r_ax=5;
	r.r_bx=0;
	intr(0x33,&r);
	if (r.r_bx>0){
		M->Button=0;
		M->x=r.r_cx;
		M->y=r.r_dx;
		return -1;
	};

	r.r_ax=5;
	r.r_bx=1;
	intr(0x33,&r);
	if (r.r_bx>0){
		M->Button=1;
		M->x=r.r_cx;
		M->y=r.r_dx;
		return -1;
	}
	return 0;
}

int Mouse_Move(MOUSE *M){
	struct REGPACK r;

	if(M->Pass==MOVE) return -1;

	r.r_ax=3;
	intr(0x33,&r);
	if (r.r_cx!=M->x && r.r_dx!=M->y){
		 M->x=r.r_cx;
		 M->y=r.r_dx;
		 return -1;
		 }
	return 0;
}

void Get_Mouse_Event(MOUSE *M){
	struct REGPACK r;
	r.r_ax=3;
	intr(0x33,&r);
	M->x=r.r_cx;
	M->y=r.r_dx;
}


int Mouse_Up(MOUSE *M){
	struct REGPACK r;

	if(M->Pass==UP) return -1;
	r.r_ax=6;
	r.r_bx=0;
	intr(0x33,&r);
	if (r.r_bx>0){
		M->Button=0;
        M->x=r.r_cx;
		M->y=r.r_dx;
		return -1;
	};

	r.r_ax=6;
	r.r_bx=1;
	intr(0x33,&r);
	if (r.r_bx>0){
		M->Button=1;
        M->x=r.r_cx;
		M->y=r.r_dx;
		return -1;
	}
	return 0;
}
#endif