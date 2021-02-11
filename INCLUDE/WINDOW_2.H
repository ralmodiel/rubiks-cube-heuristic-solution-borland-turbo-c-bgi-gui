#include"dos.h"
#include"graphics.h"
#include"string.h"
#include"mem.h"
#include"stdarg.h"
#include"stdlib.h"
#include"stdio.h"


#if !defined(__MOUSE__)
#include"mouse.h"
#endif

#define __WINDOW___

#define MYARGS

void interrupt (*old_handler_before_popup)();

#define Install_PopUp() old_handler_before_popup=getvect(0x1C);setvect(0x1C,Handler_PopUp);
#define UnInstall_PopUp() setvect(0x1C,old_handler_before_popup);

typedef struct {
	float size;
	int fontname;
	}FONT;

typedef struct {
	int x,y,width,height,background,foreground;
	char Caption[100];
	}FORM;

typedef struct button{
	int x,y,width,height;
	FORM *Form;
	char background,foreground;
	char Caption[20];
}BUTTON;

typedef struct {
	int x,y,width,height;
	FORM *Form;
	int background,foreground,TopIndex,LeftIndex;
	char *Text;
	int TabLen;
	}LISTBOX;

typedef struct{
	int x,y,width,height;
	FORM *Form;
}FRAME;

typedef struct{
	int x,y,width,height;
	int maxlength;
	FORM *Form;
	int foreground, background;
	char Text[1000];
	int Index,CursorPos;
	int Displaywidth;
	}TEXTBOX;

typedef struct{
	int x,y;
	FORM *Form;
	int foreground,background;
	char Caption[50];
	char _3D;
	FONT *font;
	}LABEL;

typedef struct{
	int x, y;
	char code;
}ARROW;

int ArrowRight[3][6]={{0,0,-14,7,-11,0},{-14,7,-14,-7,-11,0},{-14,-7,-11,0,0,0}};
int ArrowLeft[3][6]={{0,0,14,7,11,0},{14,7,14,-7,11,0},{14,-7,11,0,0,0}};
int ArrowDown[3][6]={{7,-14,0,-11,0,0},{0,0,-7,-14,0,-11},{-7,-14,7,-14,0,-11}};
int ArrowUp[3][6]={{7,14,0,11,0,0},{-7,14,7,14,0,11},{0,0,-7,14,0,11}};

void initialize_form(FORM *F, char *Caption, int x, int y,
		 int width, int height, int foreground, int background)
{ strcpy(F->Caption,Caption);
	F->x=x; F->y=y; F->width=width; F->height=height;
	F->foreground=foreground; F->background=background;
}
void initialize_button(BUTTON *B, FORM *F, char *Caption, int x, int y,
		 int width, int height, int foreground, int background)
{
	B->Form=F;strcpy(B->Caption,Caption);
	B->x=x;	B->y=y; B->width=width; B->height=height;
	B->foreground=foreground;	B->background=background;
}

void initialize_listbox(LISTBOX *L, FORM *F, char *Text, int x, int y,
		 int width, int height, int foreground, int background)
{	L->Form=F;strcpy(L->Text,Text);
	L->x=x;	L->y=y; L->width=width; L->height=height;
	L->foreground=foreground;L->background=background;
	L->TabLen=(int)((float)width/5.75);
	L->TopIndex=1; L->LeftIndex=1;
}
void initialize_textbox(TEXTBOX *T, FORM *Form, char *Text, int x, int y,
		 int width, int height, int foreground, int background, int maxlength)
{ T->Form=Form; strcpy(T->Text,Text);
  T->x=x;	T->y=y;	T->width=width;	T->height=height;
  T->foreground=foreground;	T->background=background;
  T->Index=0;T->CursorPos=0;T->Displaywidth=(int)((float)width/5.75);
  T->maxlength=maxlength;
}
void initialize_frame(FRAME *Fr, FORM *Form, int x, int y, int width, int height)
{  Fr->Form=Form; Fr->x=x; Fr->y=y; Fr->width=width; Fr->height=height;
}
void initialize_arrow(ARROW *A, FORM *Form, int x, int y, int code)
{  A->x=Form->x+x; A->y=Form->y+y; A->code=code;
}
void initialize_label(LABEL *L, FORM *Form, char *Caption, int x, int y,
		 int foreground, int background, char _3D, FONT *font)
{  L->Form=Form; strcpy(L->Caption, Caption);
	 L->x=x; L->y=y; L->foreground=foreground; L->background=background;
	 L->_3D=_3D; L->font=font;
}

void Arrow(int x,int y,int M_[3][6]){
	int i,j,A_[3][6];
	x+=2;y+=1;
	for(i=0;i<3;i++)
	for(j=0;j<6;j++){
		if(j%2==0) A_[i][j]=M_[i][j]+x; else A_[i][j]=M_[i][j]+y;
	}
	setcolor(LIGHTGREEN);
	setfillstyle(1,LIGHTGREEN);
	fillpoly(3,A_[2]);
	setcolor(BLUE);
	setfillstyle(1,BLUE);
	fillpoly(3,A_[0]);
	setcolor(LIGHTBLUE);
	setfillstyle(1,LIGHTBLUE);
	fillpoly(3,A_[1]);
	putpixel(x,y,LIGHTGREEN);
}
void Erase_Arrow(int x,int y,int M_[3][6]){
	int i,j,A_[3][6];
	x+=2;y+=1;
	for(i=0;i<3;i++)
	for(j=0;j<6;j++){
	if(j%2==0) A_[i][j]=M_[i][j]+x; else A_[i][j]=M_[i][j]+y;
	}
	setcolor(LIGHTGRAY);
	setfillstyle(1,LIGHTGRAY);
	fillpoly(3,A_[2]);
	fillpoly(3,A_[0]);
	fillpoly(3,A_[1]);
	putpixel(x,y,LIGHTGRAY);
}

void Arrow_Up(int x,int y,int M_[3][6]){

	int i,j,A_[3][6];
	int B_[6];
	for(i=0;i<3;i++)
	for(j=0;j<6;j++){
		if(j%2==0) A_[i][j]=M_[i][j]+x; else A_[i][j]=M_[i][j]+y;
	}
	B_[0]=A_[0][0]+4;
	B_[1]=A_[0][1]+3;
	B_[2]=A_[1][0]+4;
	B_[3]=A_[1][1]+3;
	B_[4]=A_[2][0]+4;
	B_[5]=A_[2][1]+3;

	setcolor(DARKGRAY);
	setfillstyle(1,DARKGRAY);
	fillpoly(3,B_);

	setcolor(LIGHTGREEN);
	setfillstyle(1,LIGHTGREEN);
	fillpoly(3,A_[2]);
	setcolor(BLUE);
	setfillstyle(1,BLUE);
	fillpoly(3,A_[0]);
	setcolor(LIGHTBLUE);
	setfillstyle(1,LIGHTBLUE);
	fillpoly(3,A_[1]);
	putpixel(x,y,LIGHTGREEN);
}
void Erase_Arrow_Up(int x,int y,int M_[3][6]){
	int i,j,A_[3][6];
	int B_[6];
	for(i=0;i<3;i++)
	for(j=0;j<6;j++){
	if(j%2==0) A_[i][j]=M_[i][j]+x; else A_[i][j]=M_[i][j]+y;
	}
	B_[0]=A_[0][0]+4;
	B_[1]=A_[0][1]+3;
	B_[2]=A_[1][0]+4;
	B_[3]=A_[1][1]+3;
	B_[4]=A_[2][0]+4;
	B_[5]=A_[2][1]+3;
	setcolor(LIGHTGRAY);
	setfillstyle(1,LIGHTGRAY);
	fillpoly(3,B_);
	fillpoly(3,A_[2]);
	fillpoly(3,A_[0]);
	fillpoly(3,A_[1]);
	putpixel(x,y,LIGHTGRAY);
}

void Create_Arrow(ARROW *A)
{ switch (A->code)
  {
     case 1: Arrow_Up(A->x, A->y, ArrowRight);
	     break;
     case 2: Arrow_Up(A->x, A->y, ArrowLeft);
	     break;
     case 3: Arrow_Up(A->x, A->y, ArrowUp);
	     break;
     case 4: Arrow_Up(A->x, A->y, ArrowDown);
	     break;
  }
}
void Erase_Created_Arrow(ARROW *A)
{ switch (A->code)
  {
     case 1: Erase_Arrow_Up(A->x, A->y, ArrowRight);
	     break;
     case 2: Erase_Arrow_Up(A->x, A->y, ArrowLeft);
	     break;
     case 3: Erase_Arrow_Up(A->x, A->y, ArrowUp);
	     break;
     case 4: Erase_Arrow_Up(A->x, A->y, ArrowDown);
	     break;
  }
}
void Arrow_Down(ARROW *A)
{ switch (A->code)
  {  case 1: Arrow(A->x, A->y, ArrowRight);
	     break;
     case 2: Arrow(A->x, A->y, ArrowLeft);
	     break;
     case 3: Arrow(A->x, A->y, ArrowUp);
	     break;
     case 4: Arrow(A->x, A->y, ArrowDown);
	     break;
  }
}
void Erase_Arrow_Down(ARROW *A)
{ switch (A->code)
  {  case 1: Erase_Arrow(A->x, A->y, ArrowRight);
	     break;
     case 2: Erase_Arrow(A->x, A->y, ArrowLeft);
	     break;
     case 3: Erase_Arrow(A->x, A->y, ArrowUp);
	     break;
     case 4: Erase_Arrow(A->x, A->y, ArrowDown);
	     break;
  }
}

int Within_Arrow(ARROW *A, int i, int j)
{
  if (A->code==1) return(i<A->x && i>A->x-15 && j<A->y+10 && j>A->y-10);
  else if(A->code==2) return(i<A->x+15 && i>A->x && j<A->y+10 && j>A->y-10);
  else if(A->code==3) return(i<A->x+10 && i>A->x-10 && j<A->y+15 && j>A->y);
  else return(i<A->x+10 && i>A->x-10 && j<A->y && j>A->y-15);
}


char *concat(int num,...){
	va_list list;
	int i;
	char s[3000];

	memset(s,0,3000);
	va_start(list,num);
	for(i=num;i>0;i--){
		strcat(s,va_arg(list,char *));
	}
	va_end(list);
	return s;


}

char *etoa(double value,int ndig,char *s){
	double x;
	int dec,sign,k;
	char s2[20],s3[20],s4[20],s5[20];
	memset(s2,0,20);
	memset(s3,0,20);
	memset(s4,0,20);

	strcpy(s,ecvt(value,ndig,&dec,&sign));

	if (dec<-30) {
		strcpy(s,"0.0");
		return s;
	}

	if (dec==0){
		if (sign) strcpy(s2,"-0."); else strcpy(s2,"0.");
		strcat(s2,s);
		strcpy(s,s2);
	}

	if (dec>0){

		if (sign) strcpy(s2,"-"); else strcpy(s2,"");
			if (dec<10){
				strncat(s2,s,dec);
				strcat(s2,".");
				strcat(s2,s+dec);
				strcpy(s,s2);
			}else{
				strncat(s2,s,1);
				strcat(s2,".");
				strcat(s2,s+1);
				strcat(s2,"E");
				k=dec-1;
				strcat(s3,ecvt(k,2,&dec,&sign));
				strncat(s2,s3,dec);
				strcpy(s,s2);
			}
	}

	if (dec<0){
		if (sign) strcpy(s2,"-"); else strcpy(s2,"");
		if (dec>-4){
				strcat(s2,"0.");
				memset(s5,0,20);
				memset(s5,'0',abs(dec));
				strcat(s2,s5);
				strncat(s2,s,ndig-abs(dec)-2);
				strcpy(s,s2);
			}else{
				strncat(s2,s,1);
				strcat(s2,".");
				strcat(s2,s+1);
				strcat(s2,"E-");
				k=dec-1;
				strcat(s3,ecvt(k,2,&dec,&sign));
				strncat(s2,s3,dec);
				strcpy(s,s2);
			}
	}

	return s;

}



int Display_Text(TEXTBOX *T1,char Isspecial,char keycode)
{
  struct viewporttype vport;
  char s1[100],s2[100],Text[100],s3[100],c;
  int x1,y1,x,y;

  getviewsettings(&vport);
  memset(s1,0,100);
  memset(s2,0,100);
  memset(s3,0,100);
  memset(Text,0,100);

  x=T1->Form->x+T1->x;
  y=T1->Form->y+T1->y;
  x1=x+T1->width;
  y1=y+T1->height;

	strcpy(Text,T1->Text);
	strncpy(s1,Text,T1->CursorPos+T1->Index);
	strcpy(s2,Text+T1->CursorPos+T1->Index);

  if (Isspecial)
  {
	 switch(keycode)
	 {
		case 'G':/*Home*/
				 T1->CursorPos=0;
				 T1->Index=0;
				 keycode=0;
				 break;
		case 'O': /*End*/
				 if (T1->Displaywidth>strlen(T1->Text))
						 T1->CursorPos=strlen(T1->Text);
				 else T1->CursorPos=T1->Displaywidth;
				 T1->Index=strlen(T1->Text)-T1->Displaywidth;
				 if (T1->Index<0)
						 T1->Index=0;
				 keycode=0;
				 break;
		case 'M':
				 T1->CursorPos++;
				 if(T1->CursorPos>strlen(T1->Text)-T1->Index)
						T1->CursorPos=strlen(T1->Text)-T1->Index;
				 if(T1->CursorPos>T1->Displaywidth)
				 { T1->CursorPos=T1->Displaywidth;
					 T1->Index++;
					 if(T1->Index>strlen(T1->Text)-T1->Displaywidth)
							T1->Index=strlen(T1->Text)-T1->Displaywidth;
				 }
				 keycode=0;
				 break;
		case 'K':
				 T1->CursorPos--;
				 if(T1->CursorPos<0)
				 {
					 T1->CursorPos=0;
					 T1->Index--;
					 if(T1->Index<0) T1->Index=0;
				 }
				 keycode=0;
				 break;
		case 'S':
         if(strlen(s2)>0)
				strcpy(s2,s2+1);
				 keycode=0;
				 break;
		default:
				 keycode=0;
				 break;
		} /* end switch */
	}/*Isspecial*/

	if (!Isspecial && keycode==8 )/* backspace*/
	{
		if (strlen(s1)>0)
		{ strncpy(s3,s1,strlen(s1)-1);
			strcpy(s1,s3);
			T1->CursorPos--;
			if (T1->CursorPos<0 && T1->Index>0)
			{	if(T1->Index==1)
				{ T1->CursorPos=0;
					T1->Index=0;
				}
				else if (T1->Index>=1)
				{	T1->CursorPos=1;
					T1->Index-=2;
				}
			}
		}
		keycode=0;
	}/*backspace*/

	if (keycode!=0)
	{
		 T1->CursorPos++;
		 if (T1->CursorPos>T1->Displaywidth)
			 { T1->CursorPos--;
				 T1->Index++;
			 }
	}

	strncat(s1,&keycode,1);
	strcat(s1,s2);
	strcpy(T1->Text,s1);
	strcpy(Text,T1->Text+T1->Index);
	setfillstyle(SOLID_FILL, WHITE);
	bar(x+2,y+2,x1-2,y1-2);
	setcolor(BLACK);

	setviewport(vport.left+x+2,vport.top+y+2,vport.left+x1-4,vport.top+y1-2,-1);
	for(c=0;c<strlen(Text);c++)
	{ sprintf(s1,"%c",Text[c]);
		outtextxy(2+c*5.75,0,s1);
	}
	setviewport(vport.left,vport.top,vport.right,vport.bottom,vport.clip);
	return(0);
}


int Create_Textbox(TEXTBOX *T1)
{
	int x,y,x1,y1;
	x=T1->x+T1->Form->x;
	y=T1->y+T1->Form->y;
	x1=x+T1->width;
	y1=y+T1->height;

	if (T1->Displaywidth==0) T1->Displaywidth=(T1->width/textwidth("W"))-2;
	setfillstyle(SOLID_FILL,T1->background);
	bar(x,y,x1,y1);
	setcolor(DARKGRAY);
	line(x,y,x1,y);
	line(x,y,x,y1);
	setcolor(BLACK);
	line(x+1,y+1,x1-1,y+1);
	line(x+1,y+1,x+1,y1-1);
	setcolor(LIGHTGRAY);
	line(x+1,y1-1,x1-1,y1-1);
	line(x1-1,y+1,x1-1,y1-1);
	Display_Text(T1,-1,0);
}



int Create_Form(FORM *FORM1){
	int x,y,x1,y1;
	x=FORM1->x;
	y=FORM1->y;
	x1=FORM1->x+FORM1->width;
	y1=FORM1->y+FORM1->height;
	setfillstyle(SOLID_FILL,LIGHTGRAY);
	bar(x,y,x1,y1);
	setcolor(LIGHTGRAY);
	line(x,y,x1,y);
	line(x,y,x,y1);
	setcolor(WHITE);
	line(x+1,y+1,x1-1,y+1);
	line(x+1,y+1,x+1,y1-1);
	setcolor(BLACK);
	line(x,y1,x1,y1);
	line(x1,y,x1,y1);
	setcolor(DARKGRAY);
	line(x+1,y1-1,x1-1,y1-1);
	line(x1-1,y+1,x1-1,y1-1);
	setfillstyle(SOLID_FILL,FORM1->background);
	bar(x+4,y+4,x1-4,y+20);
	setcolor(WHITE);
	line(x+4,y+20,x1-5,y+20);
	line(x1-5,y+4,x1-5,y+20);
	/*setcolor(WHITE);
	outtextxy(x+21,y+11-(textheight("M")/2),FORM1->Caption);*/
	setcolor(FORM1->foreground);
	outtextxy(x+20,y+10-(textheight("M")/2),FORM1->Caption);
}

int Create_Frame(FRAME *F1){
	int x,y,x1,y1;
	x=F1->Form->x+F1->x;
	y=F1->Form->y+F1->y;
	x1=x+F1->width;
	y1=y+F1->height;
	setcolor(DARKGRAY);
	rectangle(x,y,x1,y1);
	setcolor(WHITE);
	line(x+1,y+1,x+1,y1-1);
	line(x+1,y+1,x1-1,y+1);
	line(x,y1+1,x1,y1+1);
	line(x1+1,y,x1+1,y1+1);
}

int Create_Button(BUTTON *B1){
	int x,y,x1,y1,i;
	x=B1->x+B1->Form->x;
	y=B1->y+B1->Form->y;
	x1=x+B1->width;
	y1=y+B1->height;
	setfillstyle(SOLID_FILL,B1->background);
	bar(x,y,x1,y1);
	setcolor(WHITE);
	line(x,y,x1,y);
	line(x,y,x,y1);
	setcolor(BLACK);
	line(x,y1,x1,y1);
	line(x1,y,x1,y1);
	setcolor(LIGHTGRAY);

	line(x-1,y-1,x1,y-1);
	line(x-1,y-1,x-1,y1);

	setcolor(DARKGRAY);
	line(x,y1-1,x1-1,y1-1);
	line(x1-1,y+1,x1-1,y1-1);
	setcolor(B1->foreground);
	outtextxy(x+1+(B1->width-textwidth(B1->Caption))/2,y-1+(B1->height-textheight(B1->Caption))/2,B1->Caption);
	}

int Button_Down(BUTTON *B1){
	int x,y,x1,y1;
	x=B1->Form->x+B1->x;
	y=B1->Form->y+B1->y;
	x1=x+B1->width;
	y1=y+B1->height;
	setfillstyle(SOLID_FILL,B1->background);
	bar(x+2,y+2,x1-2,y1-2);
	setcolor(BLACK);
	rectangle(x-2,y-2,x1+1,y1+1);
	setcolor(DARKGRAY);
	line(x,y,x1,y);
	line(x,y,x,y1);
	line(x+1,y+1,x1-1,y+1);
	line(x+1,y+1,x+1,y1-1);
	setcolor(B1->background);
	line(x1-1,y+1,x1-1,y1-1);
	line(x1,y,x1,y1);
	line(x,y1,x1,y1);
	line(x+1,y1-1,x1-1,y1-1);
	setcolor(B1->foreground);
	outtextxy(x+3+(B1->width-textwidth(B1->Caption))/2,y+1+(B1->height-textheight(B1->Caption))/2,B1->Caption);
}
int Within_Control(FORM *Parent,void far *p,int x,int y)
{
	int x1,y1,width,height;
	void far *Form2;

	x1= peek(FP_SEG(p),FP_OFF(p)) + Parent->x;
	y1= peek(FP_SEG(p),FP_OFF(p)+2) + Parent->y;
	width=peek(FP_SEG(p),FP_OFF(p)+4);
	height=peek(FP_SEG(p),FP_OFF(p)+6);
	if (x>=x1 && x<=(width+x1) && y>=y1 && y<=(y1+height)) return -1;
	else return 0;
}

int Within_Form(FORM P,int x2,int y2){
	int x,y,x1,y1;
	x=P.x;y=P.y;x1=x+P.width,y1=y+P.height;

	if(x2>=x && x2<=x1 && y2>=y && y2<=y1) return -1; else return 0;
}

char *Adjust_String(char *s,int len){
	int len1;
	char s2[100],s3[100];

	memset(s2,0,100);
	memset(s3,0,100);
	len1=abs(len-strlen(s));
	memset(s2,' ',len1);
	strcpy(s3,s);
	strcat(s3,s2);
	strcpy(s,s3);
	return s;


}

int Activate_Button(BUTTON *B1){
	int x,y,x1,y1;
	x=B1->x+B1->Form->x;
	y=B1->y+B1->Form->y;
	x1=x+B1->width;
	y1=y+B1->height;
	setcolor(BLACK);
	rectangle(x-2,y-2,x1+1,y1+1);
	}

int Deactivate_Button(BUTTON *B1){
	int x,y,x1,y1;
	x=B1->x+B1->Form->x;
	y=B1->y+B1->Form->y;
	x1=x+B1->width;
	y1=y+B1->height;
	setcolor(LIGHTGRAY);
	rectangle(x-2,y-2,x1+1,y1+1);
	}

int Set_Font(FONT Font){
	if (Font.fontname<=0) Font.fontname=SMALL_FONT;
	if (Font.size<=0) Font.size=1;
	settextstyle(Font.fontname,HORIZ_DIR,1);
	setusercharsize(Font.size*7,60,Font.size*4,30);

}


int Create_Label(LABEL *L1){

	if (L1->font->fontname<=0) L1->font->fontname=SMALL_FONT;
	if (L1->font->size<=0) L1->font->size=1;
	settextstyle(L1->font->fontname,HORIZ_DIR,1);
	setusercharsize(L1->font->size*7,60,L1->font->size*4,30);
	setcolor(L1->foreground);
	setfillstyle(SOLID_FILL,L1->background);
	bar(L1->x+L1->Form->x,L1->y+L1->Form->y,L1->Form->x+L1->x+textwidth(L1->Caption),L1->Form->y+L1->y+textheight(L1->Caption));
	if (L1->_3D){setcolor(WHITE);outtextxy(L1->x+1+L1->Form->x,1+L1->y+L1->Form->y,L1->Caption);}
	setcolor(L1->foreground);
	outtextxy(L1->x+L1->Form->x,L1->y+L1->Form->y,L1->Caption);
}

int Create_Listbox(LISTBOX *L1){
	int x,y,x1,y1;
	x=L1->x+L1->Form->x;
	y=L1->y+L1->Form->y;
	y1=y+L1->height;
	x1=x+L1->width;
	setfillstyle(SOLID_FILL,L1->background);
	bar(x,y,x1,y1);
	setcolor(DARKGRAY);
	line(x,y,x1,y);
	line(x,y,x,y1);
	setcolor(BLACK);
	line(x+1,y+1,x1-1,y+1);
	line(x+1,y+1,x+1,y1-1);
	setcolor(LIGHTGRAY);
	line(x+1,y1-1,x1-1,y1-1);
	line(x1-1,y+1,x1-1,y1-1);
	setcolor(WHITE);
	line(x,y1,x1,y1);
	line(x1,y,x1,y1);
}

int Update_Listbox(LISTBOX *P1){
	int x, y, x1, y1, t, row, col, m;
	char s1[1];

	Hide_Mouse();
	x=P1->x+P1->Form->x+5;
	y=P1->y+P1->Form->y+5;
	y1=y+P1->height-8;
	x1=x+P1->width-8;
	setfillstyle(SOLID_FILL,P1->background);
	bar(x,y,x1-15,y1);
	setcolor(P1->foreground);
	row=0;
	m=0;
	for (t=0;t<strlen(P1->Text) && m<P1->TopIndex;t++)
	{ if (P1->Text[t]=='\n') m++;
	}
	for (col=0;t<strlen(P1->Text);t++)
	{ if (P1->Text[t]=='\n')
		{ row++;
			col=0;
			continue;
		}
		if (x+col*5.75>x1-20) continue;
		if (y+row*10>y1-10)
		  break;
		sprintf(s1,"%c",P1->Text[t]);
		outtextxy(x+col*5.75,y+row*10,s1);
		col++;
	}
	Show_Mouse();
	return(row);
}