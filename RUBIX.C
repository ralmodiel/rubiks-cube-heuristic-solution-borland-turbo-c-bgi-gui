#include <stdlib.h>
#include <stdio.h>
#include <graphics.h>
#include <math.h>
#include <mem.h>
#include <window_2.h>
#include <mouse.h>
#include <alloc.h>

#define SZ 70
#define EYEP_Z 150.0
#define ADJF_Y 0.73
#define H 390
#define K 177
#define DTL 25.0
#define ZDim 0.00075
#define Z_Tlevel 12.0
#define	SPEED 4.0  /*must be even for the mouse pointer to show up*/

		FORM F1,F2;
		FONT L1Font,F1Font,BFont;
		BUTTON B1,B2,B3,B4,B5,B6,B7,B8,SBt,ButtonDown;
		FRAME Fr1,Fr2,Fr3,Fr4,Fr5,Fr6,Fr7;
		LABEL L1,L2,L3;
		ARROW A1,A2,A3,A4;
		MOUSE M;
		int *image;

typedef struct
{ int XY[4][2];
	int Z, C;
}Point;

/* 0 front  1 top   2 bottom   3 left   4 right   5 back */
double S[6][9][4][3]; /*side part point {x y z}  */
int    Nb[6][12][2];/* side neighbor {neighbor_side neighbor_part}*/

int    T[4][2]; /*side {x y} */
double ZMid[54], Ztemp[54]; /*Z coordinate of the middle point of a square */
int    Pos[54][2], Ptemp[54][2];
Point  Pnt[54];
int    Q[6][9][3]; /*side part {color orig_side orig_part}*/

double C[8][3]; /* corner {x y z} */
double G[4][4], R[4][4], N[4][4];
int    X_LEFT, X_RIGHT, Y_UP, Y_DOWN;/*viewport of the cube--to be computed*/
int m=1, d=VGA, page=0;
double pi, piovrtu, res, nres;
int front=0, back=5, top=1, bottom=2, left=3, right=4;
int lastside=0, lastdir=2;

int	Nbr0[37]={3,4,5,1,4,7,5,4,3,7,4,1, 0,1,2,2,5,8,8,7,6,6,3,0,
							2,1,0,3,6,7,8,5, 1,4,2,3,5 };
int	Nbr1[37]={5,4,3,5,4,3,5,4,3,5,4,3, 8,7,6,8,7,6,8,7,6,8,7,6,
							6,7,8,5,2,1,0,3, 5,4,0,3,2 };
int	Nbr2[37]={3,4,5,3,4,5,3,4,5,3,4,5, 0,1,2,0,1,2,0,1,2,0,1,2,
							6,7,8,5,2,1,0,3, 0,4,5,3,1 };
int	Nbr3[37]={1,4,7,1,4,7,1,4,7,7,4,1, 2,5,8,2,5,8,2,5,8,6,3,0,
							2,1,0,3,6,7,8,5, 1,0,2,5,4 };
int	Nbr4[37]={7,4,1,1,4,7,7,4,1,7,4,1, 6,3,0,2,5,8,6,3,0,6,3,0,
							2,1,0,3,6,7,8,5, 1,5,2,0,3 };
int	Nbr5[37]={5,4,3,1,4,7,3,4,5,7,4,1, 8,7,6,2,5,8,0,1,2,6,3,0,
							2,1,0,3,6,7,8,5, 1,3,2,4,0 };
int *Nbr=Nbr0, Cur_side=0;
int Clk[8]={0,1,2,5,8,7,6,3};
int Aclk[9]={0,1,2,7,4,3,6,5,4};
int Cclk[8]={2,1,0,3,6,7,8,5};

void initialize_values(void)
{ int k, l;
	for(l=0;l<9;l++)
	{
		Q[0][l][0]=2;	Q[1][l][0]=3;	 Q[2][l][0]=5;
		Q[3][l][0]=6;	Q[4][l][0]=11; Q[5][l][0]=12;
		for (k=0;k<6;k++)
		{	Q[k][l][1]=k;	Q[k][l][2]=l;
		}
	}
	/* 0 front  1 top   2 bottom   3 left   4 right   5 back */
	for (k=0;k<12;k++)
	{ if (k<3)
		{  Nb[0][k][0]=1;   Nb[1][k][0]=5;
			 Nb[2][k][0]=0;   Nb[3][k][0]=1;
			 Nb[4][k][0]=1;   Nb[5][k][0]=1;
			 Nb[0][k][1]=k+6;	Nb[5][k][1]=2-k;
			 Nb[1][k][1]=2-k;	Nb[2][k][1]=k+6;
		}
		else if (k<6)
		{  Nb[0][k][0]=4;   Nb[1][k][0]=4;
			 Nb[2][k][0]=4;   Nb[3][k][0]=0;
			 Nb[4][k][0]=5;   Nb[5][k][0]=3;
			 Nb[1][k][1]=5-k;	Nb[2][k][1]=k+3;
		}
		else if (k<9)
		{  Nb[0][k][0]=2;   Nb[1][k][0]=0;
			 Nb[2][k][0]=5;   Nb[3][k][0]=2;
			 Nb[4][k][0]=2;   Nb[5][k][0]=2;
			 Nb[0][k][1]=8-k;	Nb[5][k][1]=k;
			 Nb[1][k][1]=8-k;	Nb[2][k][1]=k;
		}
		else
		{  Nb[0][k][0]=3;    Nb[1][k][0]=3;
			 Nb[2][k][0]=3;    Nb[3][k][0]=5;
			 Nb[4][k][0]=0;    Nb[5][k][0]=4;
			 Nb[1][k][1]=11-k; Nb[2][k][1]=k-3;
		}
	}

	Nb[4][0][1]=8;Nb[4][1][1]=5;Nb[4][2][1]=2;
	Nb[3][0][1]=0;Nb[3][1][1]=3;Nb[3][2][1]=6;
	Nb[0][3][1]=0;Nb[0][4][1]=3;Nb[0][5][1]=6;
	Nb[3][3][1]=0;Nb[3][4][1]=3;Nb[3][5][1]=6;
	Nb[4][3][1]=0;Nb[4][4][1]=3;Nb[4][5][1]=6;
	Nb[5][3][1]=0;Nb[5][4][1]=3;Nb[5][5][1]=6;
	Nb[4][6][1]=8;Nb[4][7][1]=5;Nb[4][8][1]=2;
	Nb[3][6][1]=0;Nb[3][7][1]=3;Nb[3][8][1]=6;
	Nb[0][9][1]=8;Nb[0][10][1]=5;Nb[0][11][1]=2;
	Nb[3][9][1]=8;Nb[3][10][1]=5;Nb[3][11][1]=2;
	Nb[4][9][1]=8;Nb[4][10][1]=5;Nb[4][11][1]=2;
	Nb[5][9][1]=8;Nb[5][10][1]=5;Nb[5][11][1]=2;


	for (k=0;k<9;k++)
	{ for (l=0;l<4;l++)
		{ S[0][k][l][2]= 1.5*SZ;S[1][k][l][1]= 1.5*SZ;S[2][k][l][1]=-1.5*SZ;
			S[3][k][l][0]=-1.5*SZ;S[4][k][l][0]= 1.5*SZ;S[5][k][l][2]=-1.5*SZ;
		}
	}
	for (k=0;k<9;k++)
	{ for (l=0;l<4;l++)
		{ S[0][k][l][0]=S[1][k][l][0]=S[2][k][l][0]=SZ*(-1.5+k%3+l%2);
			S[0][k][l][1]=S[3][k][l][1]=S[4][k][l][1]=S[5][k][l][1]
									 =SZ*( 1.5-k/3-l/2);
			S[1][k][l][2]=SZ*(-1.5+k/3+l/2);S[2][k][l][2]=SZ*( 1.5-k/3-l/2);
			S[3][k][l][2]=SZ*(-1.5+k%3+l%2);S[4][k][l][2]=SZ*( 1.5-k%3-l%2);
			S[5][k][l][0]=SZ*( 1.5-k%3-l%2);
		}
	}
}/*initialize_values*/
void initialize_matrix(void)
{  int i,j;
	 for (i=0;i<4;i++)
		for (j=0;j<4;j++)
		{ if (i==j) G[i][j]=1;
			else G[i][j]=0;
		}
}

void R_matrix(double theta, char flag)
/* flag==0->x    flag==1->y   flag==2->z */
{ int i,j;
	double c, s;
	if (flag<0 || flag>2) return;
	c=cos(theta);
	s=sin(theta);
	for (i=0;i<4;i++)
		for (j=0;j<4;j++)
		{ if (i==j)
			{  if (i==flag || i==3) R[flag][flag]=1;
				 else R[i][i]=c;
			}
			else R[i][j]=0;
		}
	switch (flag)
	{  case 0: R[2][1]=s; R[1][2]=s*(-1.0);	 break;
		 case 1: R[0][2]=s; R[2][0]=s*(-1.0);	 break;
		 case 2: R[1][0]=s; R[0][1]=s*(-1.0);
	}
	return;
}

void multiply_matrix(void)
{
	int i, j, k;
	double sum;
	for (i=0;i<4;i++)
	 for (j=0;j<4;j++)
	 {  for (k=0,sum=0.0;k<4;k++)
			 sum=sum+R[i][k]*G[k][j];
      N[i][j]=sum;
	 }
	for (i=0;i<4;i++)
	 for (j=0;j<4;j++)
		G[i][j]=N[i][j];
}

void rotate_x(int d, int o, double theta)
{ int i;
	double y_, c=cos(theta), s=sin(theta);
	for(i=0;i<4;i++)
	{ y_=S[d][o][i][1];
		S[d][o][i][1]= S[d][o][i][1] * c  - S[d][o][i][2]  * s;
		S[d][o][i][2]= y_ * s + S[d][o][i][2] * c ;
	}
}

void rotate_y(int d, int o, double theta)
{ int i;
	double x_, c=cos(theta), s=sin(theta);
	for(i=0;i<4;i++)
	{ x_=S[d][o][i][0];
		S[d][o][i][0]= S[d][o][i][0] * c + S[d][o][i][2] * s;
		S[d][o][i][2]= S[d][o][i][2] * c - x_ * s;
	}
}

void rotate_z(int d, int o, double theta)
{ int i;
	double x_, c=cos(theta), s=sin(theta);
	for(i=0;i<4;i++)
	{ x_=S[d][o][i][0];
		S[d][o][i][0]= S[d][o][i][0] * c - S[d][o][i][1] * s;
		S[d][o][i][1]= S[d][o][i][1] * c + x_ * s;
	}
}

double transform(int d, int o)
{ int l;
	double xd, yd, zd,x,y,z,dist, zmid=0.0;
	for (l=0;l<4;l++)
	{ xd=S[Q[d][o][1]][Q[d][o][2]][l][0];
		yd=S[Q[d][o][1]][Q[d][o][2]][l][1];
		zd=S[Q[d][o][1]][Q[d][o][2]][l][2];
		x=xd*G[0][0]+yd*G[0][1]+zd*G[0][2];
		y=xd*G[1][0]+yd*G[1][1]+zd*G[1][2];
		z=xd*G[2][0]+yd*G[2][1]+zd*G[2][2];
		dist=EYEP_Z-z;

		x*=(1.0-dist*ZDim);
		y*=(1.0-dist*ZDim);
    y*=ADJF_Y;
		if (l==2)
		{	T[3][0]=H+(int)(x);
			T[3][1]=K-(int)(y);
		}
		else if (l==3)
		{ T[2][0]=H+(int)(x);
			T[2][1]=K-(int)(y);
		}
		else
		{ T[l][0]=H+(int)(x);
			T[l][1]=K-(int)(y);
		}
		zmid+=z;
	}
	return (zmid/4.0);
}

double Z_facemid(int face,int part)
{ double xd=(S[face][part][0][0]+S[face][part][1][0]+S[face][part][2][0]
						+S[face][part][3][0])/4.0;
	double yd=(S[face][part][0][1]+S[face][part][1][1]+S[face][part][2][1]
						+S[face][part][3][1])/4.0;
	double zd=(S[face][part][0][2]+S[face][part][1][2]+S[face][part][2][2]
						+S[face][part][3][2])/4.0;
	zd=xd*G[2][0]+yd*G[2][1]+zd*G[2][2];
	return zd;
}

void swap_buffers(void)
{  if (page==0) page=1;
		else page=0;
	 setactivepage(page);
}
void show_page(void)
{  setvisualpage(page);
	 delay(200);
}
int check(const void *a, const void *b)
{ Point *c, *d;
	c= (Point *) a;
	d= (Point *) b;
	if ( c->Z<d->Z ) return -1;
	else if ( c->Z>d->Z )	return 1;
	else return 0;
}

void erase_cube(void)
{ int r;char ch;
	setfillstyle(1,LIGHTGRAY);
   if (kbhit())
	 {
		 ch=getch();
		 if (ch==0)
		 {
			ch=getch();
			switch(ch)
			{ case 72 : R_matrix(nres, 0);
								 multiply_matrix();
								 break;
				case 80 : R_matrix(res, 0);
								 multiply_matrix();
								 break;
				case 75 : R_matrix(nres, 1);
									multiply_matrix();
									break;
				case 77 : R_matrix(res, 1);
									multiply_matrix();
									break;
			 }
		}
		else ungetc(ch,stdin);
	 }
	 bar(X_LEFT, Y_UP, X_RIGHT, Y_DOWN);
}
void draw_cube(void)
{  int a, i, j, k=0, *t, rq;
	 for (i=0;i<6;i++)
		 for (j=0;j<9;j++)
		 {
			 Pnt[k].Z=(int)(transform(i,j));
			 Pnt[k].C=Q[i][j][0];
			 for (a=0;a<4;a++)
			 {  Pnt[k].XY[a][0]=T[a][0];
					Pnt[k].XY[a][1]=T[a][1];
			 }
			 k++;
		 }
		qsort(Pnt,54,sizeof(Point),check);
		Hide_Mouse();
		swap_buffers();
		erase_cube();
		for(k=0;k<54;k++)
		{
		 setfillstyle(1,Pnt[k].C);
		 setcolor(13);
		 fillpoly(4,Pnt[k].XY);
		}
		show_page();
		Show_Mouse();
}
/*void update_front()
{ int i, a;
  for (i=0;i<6;i++)
	{ Pnt[i].Z=(int)(transform(i,4));
		Pnt[i].C=Q[i][4][0];
		for (a=0;a<4;a++)
		{  Pnt[i].XY[a][0]=T[a][0];
			 Pnt[i].XY[a][1]=T[a][1];
		}
	 }
	 qsort(Pnt,6,sizeof(Point),check);
}*/
void turn(int side, int dir) /*dir==1 for clockwise ;dir==-1 for counterclk*/
{ int j, a, b, c, s, k, qt;
	double SPD= (-1.0/(dir * SPEED))*piovrtu;
	void (*rotate_f)();

	if (dir!=1 && dir!=-1) return;
	if (side==5 || side==3 || side==2) SPD*=-1;
  switch(side)
		{ case 0:	case 5: rotate_f=rotate_z; break;
			case 1: case 2: rotate_f=rotate_y; break;
			case 3: case 4: rotate_f=rotate_x; break;
		}
	for(s=0;s<SPEED;s++)
		{ for (j=0;j<9;j++)
				rotate_f (Q[side][j][1],Q[side][j][2],SPD);
			for(j=0;j<12;j++)
				rotate_f (Q[Nb[side][j][0]][Nb[side][j][1]][1],
						Q[Nb[side][j][0]][Nb[side][j][1]][2],SPD);
			draw_cube();
		 }
	 if (dir==-1) /*counter-clockwise*/
	 {  for (s=0;s<3;s++)
			{  a=Q[Nb[side][0][0]][Nb[side][0][1]][s];
				 b=Q[Nb[side][1][0]][Nb[side][1][1]][s];
				 c=Q[Nb[side][2][0]][Nb[side][2][1]][s];
				 for (k=0;k<9;k++)
					 Q[Nb[side][k][0]][Nb[side][k][1]][s]
						 =Q[Nb[side][k+3][0]][Nb[side][k+3][1]][s];
				 Q[Nb[side][9 ][0]][Nb[side][9 ][1]][s]=a;
				 Q[Nb[side][10][0]][Nb[side][10][1]][s]=b;
				 Q[Nb[side][11][0]][Nb[side][11][1]][s]=c;
			}
			for (s=0;s<3;s++)
			 {  a=Q[side][0][s]; b=Q[side][1][s];
					Q[side][0][s]=Q[side][2][s];Q[side][2][s]=Q[side][8][s];
					Q[side][8][s]=Q[side][6][s];Q[side][6][s]=a;
          Q[side][1][s]=Q[side][5][s];Q[side][5][s]=Q[side][7][s];
					Q[side][7][s]=Q[side][3][s];Q[side][3][s]=b;
			 }
	 }
	 else /*clockwise*/
	 {  for (s=0;s<3;s++)
			{  a=Q[Nb[side][ 9][0]][Nb[side][ 9][1]][s];
				 b=Q[Nb[side][10][0]][Nb[side][10][1]][s];
				 c=Q[Nb[side][11][0]][Nb[side][11][1]][s];
				 for (k=11;k>2;k--)
					 Q[Nb[side][k][0]][Nb[side][k][1]][s]
						 =Q[Nb[side][k-3][0]][Nb[side][k-3][1]][s];
				 Q[Nb[side][0][0]][Nb[side][0][1]][s]=a;
				 Q[Nb[side][1][0]][Nb[side][1][1]][s]=b;
				 Q[Nb[side][2][0]][Nb[side][2][1]][s]=c;
			}
			for (s=0;s<3;s++)
			 {  a=Q[side][0][s]; b=Q[side][1][s];
					Q[side][0][s]=Q[side][6][s];Q[side][6][s]=Q[side][8][s];
					Q[side][8][s]=Q[side][2][s];Q[side][2][s]=a;
					Q[side][1][s]=Q[side][3][s];Q[side][3][s]=Q[side][7][s];
					Q[side][7][s]=Q[side][5][s];Q[side][5][s]=b;
			 }
	 }
	 lastside=side; lastdir=dir;
	 return;
}/*turn*/


void Initialize_Menu(){
		/*RUBIX Cube - CS 170 AI Project*/
		BFont.size=8.65;
		BFont.fontname=2;
		L1Font.size=9;
		L1Font.fontname=2;
		F1Font.size=7;
		F1Font.fontname=2;

		initialize_form(&F1,"R U B I X   C u b e   -   C S   1 7 0   A I   P r o j e c t",0,0,640,350,1,4);
		initialize_frame(&Fr1,&F1,4,25,150,320);
		initialize_frame(&Fr2,&F1,10,110,138,120);
		initialize_frame(&Fr3,&F1,158,25,476,320);
		initialize_frame(&Fr4,&F1,162,328,468,14);
		initialize_frame(&Fr5,&F1,10,233,138,47);
		initialize_frame(&Fr6,&F1,10,38,138,69);
		initialize_frame(&Fr7,&F1,35,254,88,16);
		initialize_label(&L1,&F1,"Solve up to the ...",20,116,9,7,0,&L1Font);
		initialize_label(&L2,&F1,"Speed",20,240,9,7,0,&L1Font);
		initialize_label(&L3,&F1,"Detail",109,240,9,7,0,&L1Font);
		initialize_button(&B1,&F1,"LOAD",15,45,130,16,4,7);
		initialize_button(&B2,&F1,"SAVE",15,65,130,16,4,7);
		initialize_button(&B3,&F1,"SHUFFLE",15,85,130,16,4,7);
		initialize_button(&B4,&F1,"1st Layer",45,135,75,18,4,7);
		initialize_button(&B5,&F1,"2nd Layer",45,165,75,18,4,7);
		initialize_button(&B6,&F1,"3rd Layer",45,195,75,18,4,7);
		initialize_button(&B7,&F1,"A B O U T",15,300,130,15,4,7);
		initialize_button(&B8,&F1,"Q U I T",15,320,130,15,4,7);
		initialize_button(&SBt,&F1,"",40,256,9,12,4,7);
		initialize_arrow(&A1,&F1,15,260,2);
		initialize_arrow(&A2,&F1,140,260,1);

}

void Draw_Menu(void){
		Set_Font(L1Font);
		Create_Form(&F1);
		Create_Frame(&Fr1);Create_Frame(&Fr2);Create_Frame(&Fr3);
		Create_Frame(&Fr4);Create_Frame(&Fr5);Create_Frame(&Fr6);
		setfillstyle(1,4);
		bar(Fr7.x,Fr7.y+5,Fr7.x+Fr7.width,Fr7.y+Fr7.height-5);
		Create_Label(&L1); Create_Label(&L2); Create_Label(&L3);
		Create_Arrow(&A1); Create_Arrow(&A2);
		Set_Font(BFont);
		Create_Button(&B1);Create_Button(&B2);Create_Button(&B3);
		Create_Button(&B4);Create_Button(&B5);Create_Button(&B6);
		Create_Button(&B7);Create_Button(&B8);Create_Button(&SBt);

}

int About(void){
		FORM F1;
		BUTTON B1;
		LABEL L1,L2,L3;
		int c=1;
		initialize_form(&F1,"About Rubix Cube",220,75,200,200,1,4);
		initialize_button(&B1,&F1,"OK",50,170,100,20,0,7);

		Create_Form(&F1);
		Create_Button(&B1);
		do{
			Show_Mouse();
			if (Mouse_Down(&M)){
					switch(M.Button){
						case 0: if (Within_Control(&F1,&B1,M.x,M.y)){
											Hide_Mouse();
												Button_Down(&B1);
												delay(2000);
												Create_Button(&B1);
												Deactivate_Button(&B1);
												putimage(220,75,image,COPY_PUT);
												c=0;
											Show_Mouse();
										}break;
					}
			}
		}while(c);

}

void Deactivate_All(void){
		Deactivate_Button(&B1);
		Deactivate_Button(&B2);
		Deactivate_Button(&B3);
		Deactivate_Button(&B4);
		Deactivate_Button(&B5);
		Deactivate_Button(&B6);
		Deactivate_Button(&B7);
		Deactivate_Button(&B8);
		Deactivate_Button(&SBt);
}
int sd (int n)
{ if ((n>=0 && n<=2)||(n>=12 && n<=14))      return(Nbr[32]);
	else if ((n>=3 && n<=5)||(n>=15 && n<=17)) return(Nbr[33]);
	else if ((n>=6 && n<=8)||(n>=18 && n<=20)) return(Nbr[34]);
	else if ((n>=9 && n<=11)||(n>=21 && n<=23))return(Nbr[35]);
	else return(Nbr[36]);
}
int csd (int side)
{ return(Q[side][4][0]);
}
int csdn (int n)
{ return (Q[sd(n)][Nb[sd(n)][n%12][0]][0]);
}
int csp (int side,int part)
{ return (Q[side][part][0]);
}
int cp (int part)
{ return (Q[top][part][0]);
}
int cnb(int n)
{ return (Q[Nb[top][n][0]][Nb[top][n][1]][0]);
}
int cnbr(int n)
{ return(Q[sd(n)][Nbr[n]][0]);
}
int cpr(int part)
{ switch (part)
	{ case 0:return(Q[Nb[top][0][0]][Nb[top][0][1]][0]);
		case 1:return(Q[Nb[top][1][0]][Nb[top][1][1]][0]);
		case 2:return(Q[Nb[top][3][0]][Nb[top][3][1]][0]);
		case 3:return(Q[Nb[top][10][0]][Nb[top][10][1]][0]);
		case 5:return(Q[Nb[top][4][0]][Nb[top][4][1]][0]);
		case 6:return(Q[Nb[top][9][0]][Nb[top][9][1]][0]);
		case 7:return(Q[Nb[top][7][0]][Nb[top][7][1]][0]);
		case 8:return(Q[Nb[top][6][0]][Nb[top][6][1]][0]);
	}
}
int cpl(int part)
{ switch (part)
	{ case 0:return(Q[Nb[top][11][0]][Nb[top][11][1]][0]);
		case 1:return(Q[Nb[top][1][0]][Nb[top][1][1]][0]);
		case 2:return(Q[Nb[top][2][0]][Nb[top][2][1]][0]);
		case 3:return(Q[Nb[top][10][0]][Nb[top][10][1]][0]);
		case 5:return(Q[Nb[top][4][0]][Nb[top][4][1]][0]);
		case 6:return(Q[Nb[top][8][0]][Nb[top][8][1]][0]);
		case 7:return(Q[Nb[top][7][0]][Nb[top][7][1]][0]);
		case 8:return(Q[Nb[top][5][0]][Nb[top][5][1]][0]);
	}
}
int cprb(int part)
{ switch (part)
	{ case 0:return(Q[Nbr[32]][Nbr[12]][0]);
		case 1:return(Q[Nbr[32]][Nbr[13]][0]);
		case 2:return(Q[Nbr[33]][Nbr[15]][0]);
		case 3:return(Q[Nbr[35]][Nbr[22]][0]);
		case 5:return(Q[Nbr[33]][Nbr[16]][0]);
		case 6:return(Q[Nbr[35]][Nbr[21]][0]);
		case 7:return(Q[Nbr[34]][Nbr[19]][0]);
		case 8:return(Q[Nbr[34]][Nbr[18]][0]);
	}
}
int cplb(int part)
{ switch (part)
	{ case 0:return(Q[Nbr[35]][Nbr[23]][0]);
		case 1:return(Q[Nbr[32]][Nbr[13]][0]);
		case 2:return(Q[Nbr[32]][Nbr[14]][0]);
		case 3:return(Q[Nbr[35]][Nbr[22]][0]);
		case 5:return(Q[Nbr[33]][Nbr[16]][0]);
		case 6:return(Q[Nbr[34]][Nbr[20]][0]);
		case 7:return(Q[Nbr[34]][Nbr[19]][0]);
		case 8:return(Q[Nbr[33]][Nbr[17]][0]);
	}
}
int cprm(int part)
{ switch (part)
	{ case 0:return(Q[Nbr[32]][Nbr[0]][0]);
		case 1:return(Q[Nbr[33]][Nbr[3]][0]);
		case 2:return(Q[Nbr[33]][Nbr[3]][0]);
		case 3:return(Q[Nbr[32]][Nbr[0]][0]);
		case 5:return(Q[Nbr[34]][Nbr[6]][0]);
		case 6:return(Q[Nbr[35]][Nbr[9]][0]);
		case 7:return(Q[Nbr[35]][Nbr[9]][0]);
		case 8:return(Q[Nbr[34]][Nbr[6]][0]);
	}
}
int cplm(int part)
{ switch (part)
	{ case 0:return(Q[Nbr[35]][Nbr[11]][0]);
		case 1:return(Q[Nbr[32]][Nbr[2]][0]);
		case 2:return(Q[Nbr[32]][Nbr[2]][0]);
		case 3:return(Q[Nbr[35]][Nbr[11]][0]);
		case 5:return(Q[Nbr[33]][Nbr[5]][0]);
		case 6:return(Q[Nbr[34]][Nbr[8]][0]);
		case 7:return(Q[Nbr[34]][Nbr[8]][0]);
		case 8:return(Q[Nbr[33]][Nbr[5]][0]);
	}
}
int cpo(int part)
{ return(Q[bottom][Nbr[Aclk[part]+24]][0]);
}
int spr(int part)
{  switch(part){
	 case 0:return(back);	 case 1:return(right); case 2:return(right);
   case 3:return(back);	 case 5:return(front); case 6:return(left);
	 case 7:return(left);  case 8:return(front);}
}
int spl(int part)
{  switch(part){
	 case 0:return(left);	 case 1:return(left);	 case 2:return(back);
	 case 3:return(front); case 5:return(back);  case 6:return(front);
	 case 7:return(right); case 8:return(right);}
}
int spc(int part)
{  switch(part){
	 case 1:return(back);	 case 3:return(left);
	 case 5:return(right); case 7:return(front);}
}


void hmove1()
{
	turn(spl(0),-1);turn(spr(2),1);
	turn(bottom,-1);turn(spl(0),1);
	turn(bottom,1);turn(spr(2),-1);
	turn(bottom,-1);turn(spl(0),-1);
	turn(bottom,1);turn(spl(0),1);
}

void hmove2()
{
	turn(spl(6),-1);turn(spr(0),1);
	turn(bottom,1);turn(spr(0),-1);
	turn(bottom,-1);turn(spl(6),1);
	turn(bottom,1);turn(spr(0),1);
	turn(bottom,-1);turn(spr(0),-1);
}


void label_cube()
{  switch(Cur_side)
	 { case 0: Nbr=Nbr0;break; case 1: Nbr=Nbr1;break; case 2: Nbr=Nbr2;break;
		 case 3: Nbr=Nbr3;break; case 4: Nbr=Nbr4;break; case 5: Nbr=Nbr5;break;
	 }
	 front=Nbr[34];  back=Nbr[32]; top=Cur_side;
	 bottom=Nbr[36]; left=Nbr[35]; right=Nbr[33];
}
void solve(int up_to)
{  int level_finished=0, state=0, i, I, j, J;
	 Deactivate_All();
	 Cur_side= front;
	 label_cube();
	 while (level_finished<up_to)
	 {
		 switch (state)
		 { case 0: /*first layer ... non-corners*/
				 for (i=1;i<8;i+=2) /*non-corners*/
				 { I=Clk[i];
					 if (   cp(I)==csd(top) && cpr(I)==csd(spc(I))) continue;
					 if (   cp(I)==csd(spc(I)) && cpr(I)==csd(top))
					 {  turn(spr(I),-1);turn(spl(I),1); turn(spc(I),-1);
							turn(spr(I),1); turn(spl(I),-1);turn(bottom,1);
					 }
					 for (j=i+2;j<8;j+=2)
						 { J=Clk[j];
							 if ((  cp(J)==csd(top) && cpr(J)==csd(spc(I)))
									||((cp(J)==csd(spc(I)) && cpr(J)==csd(top)) ) )
							 {  turn(spr(J),-1);turn(spl(J),1); turn(spc(J),-1);
									turn(spr(J),1); turn(spl(J),-1);
									break;
							 }
						 }/*for*/
						for (j=1;j<8;j+=2)
							{ J=Clk[j];
								if ((  cplm(J)==csd(top) && cprm(J)==csd(spc(I)))
									 ||((cplm(J)==csd(spc(I)) && cprm(J)==csd(top)) ) )
								{  turn(spc(J),-1);turn(bottom,-1); turn(spc(J),1);
									 break;
								}/*if*/
							}/*for*/
					 while(1)
					 {  if( ( csp(bottom,Nbr[i+24])==csd(top)
									&& cplb(I)==csd(spc(I) ))
								||( csp(bottom,Nbr[i+24])==csd(spc(I))
									&& cplb(I)==csd(top) ) )
							 break;
							turn(bottom,1);
					 } /*while*/
           if (cplb(I)==csd(top))
					 { turn(bottom,1);turn(spr(I),-1);turn(spl(I),1);turn(spc(I),-1);
						 turn(spr(I),1);turn(spl(I),-1);
					 }
					 else
					 { turn(spr(I),-1);turn(spl(I),1);turn(spc(I),-1);turn(spc(I),-1);
						 turn(spr(I),1);turn(spl(I),-1);
					 }

				 }/*for*/
				 state=1;
				 break;
			 case 1: /*first layer ... corners*/
				 for (i=0;i<8;i+=2) /*corners*/
				 { I=Clk[i];
					 if (    cp(I) ==csd(top) && cpr(I)==csd(spr(I))
								&& cpl(I)==csd(spl(I))) continue;
					 if (  (cp(I)==csd(spl(I)) && cpr(I)==csd(top)
								&& cpl(I)==csd(spr(I)) )
							 ||(cp(I)==csd(spr(I)) && cpr(I)==csd(spl(I))
								&& cpl(I)==csd(top) ))
					 {  turn(spr(I),1);turn(bottom,1);turn(spr(I),-1);
					 }
					 for (j=i+2;j<8;j+=2)
					 {  J=Clk[j];
							if ((cp(J) ==csd(top) && cpr(J)==csd(spr(I))
								&& cpl(J)==csd(spl(I)))
							 ||(cp(J)==csd(spl(I)) && cpr(J)==csd(top)
								&& cpl(J)==csd(spr(I)))
							 ||(cp(J)==csd(spr(I)) && cpr(J)==csd(spl(I))
								&& cpl(J)==csd(top) ))
							{ turn(spr(J),1);turn(bottom,1);turn(spr(J), -1);
								break;
							}
					 }/*for*/
					 while(1)
					 { if( (csp(bottom,Nbr[i+24])==csd(top) && cprb(I)==csd(spl(I))
							 && cplb(I)==csd(spr(I)) )
							|| (csp(bottom,Nbr[i+24])==csd(spr(I)) && cprb(I)==csd(top)
							 && cplb(I)==csd(spl(I)) )
							|| (csp(bottom,Nbr[i+24])==csd(spl(I)) && cprb(I)==csd(spr(I))
							 && cplb(I)==csd(top) ))
							{if (csp(bottom,Nbr[i+24])==csd(top))
								{ turn(spr(I),1);turn(bottom,-1);
									turn(spr(I),-1);turn(bottom,1);
									turn(bottom,1);
								}
							 break;
							}
							turn(bottom,1);
					 }
           if (cplb(I)==csd(top))
					 { turn(bottom,1);turn(spr(I),1);turn(bottom,-1);turn(spr(I),-1);
					 }
					 else
					 { turn(bottom,-1);turn(spl(I),-1);turn(bottom,1);turn(spl(I),1);
					 }
				 }/*for*/
				 state=2;
				 level_finished=1;
				 break;

				case 2:/* second layer */
				 for( i=0; i<8; i+=2)
				 { I=Clk[i];
					 if ( cpl(I)==cplm(I) && cpr(I)==cprm(I) ) continue;
					 if ( cpl(I)==cprm(I) && cpr(I)==cplm(I) )
					 { turn(spl(I),-1);turn(bottom,1);turn(spl(I),1);turn(bottom,1);
						 turn(spr(I),1);turn(bottom,-1);turn(spr(I),-1);
						 turn(bottom,1);
						 turn(spl(I),-1);turn(bottom,1);turn(spl(I),1);turn(bottom,1);
						 turn(spr(I),1);turn(bottom,-1);turn(spr(I),-1);
						 continue;
					 }
					 else /* put down */
					 { if ( cplm(I)!=csd(bottom) && cprm(I)!=csd(bottom) )
						 { while (csd(bottom)!=cplb(Clk[i+1]) && csd(bottom)!=cpo(Clk[i+1]) )
								turn(bottom,-1);
							 turn(bottom,-1);
							 turn(spl(I),-1);turn(bottom,1);turn(spl(I),1);turn(bottom,1);
							 turn(spr(I),1);turn(bottom,-1);turn(spr(I),-1);
						 }
					 }
				 }
				 for( i=0; i<8; i+=2)
				 { I=Clk[i];
					 if ( cpl(I)==cplm(I) && cpr(I)==cprm(I) ) continue;
					 else
					 { while (! ((cpl(I)==cplb(Clk[i+1]) && cpr(I)==cpo(Clk[i+1])) ||
										 (cpr(I)==cplb(Clk[i+1]) && cpl(I)==cpo(Clk[i+1])))	)
							turn(bottom,1);


						 if (cpl(I)==cplb(Clk[i+1]))
						 { turn(bottom,1);turn(bottom,1);turn(spr(I),1);
							 turn(bottom,1);turn(spr(I),-1);turn(bottom,-1);
							 turn(spl(I),-1);turn(bottom,-1);turn(spl(I),1);
						 }
						 else
						 { turn(bottom,-1);turn(spl(I),-1);turn(bottom,1);
							 turn(spl(I),1);turn(bottom,1);turn(spr(I),1);
							 turn(bottom,-1);turn(spr(I),-1);
						 }

					 }
				 }
				 state=3;
				 level_finished=2;
				 break;

				case 3: /* third layer cross */

				 while ( cpo(1)!=csd(bottom) || cpo(3)!=csd(bottom) ||
								 cpo(5)!=csd(bottom) )
				 { if( cpo(3)==csd(bottom)&&cpo(5)==csd(bottom) ||
							 cpo(1)==csd(bottom)&&cpo(5)==csd(bottom) ||
							 cpo(5)==csd(bottom)&&cpo(7)==csd(bottom) ||
							 cpo(1)==csd(bottom)&&cpo(1)==csd(bottom) )
						turn(bottom,1);
					turn(spr(2),-1);turn(bottom,-1);turn(spl(2),-1);turn(bottom,1);
					turn(spl(2),1);turn(spr(2),1);
				 }

				 while ( cprb(1)!=cpr(1) || cprb(3)!=cpr(3) || cprb(5)!=cpr(5) )
				 { if ( ( cprb(1)==cpr(1) && cprb(7)==cpr(7) ) ||
					 ( cprb(3)==cpr(3) && cprb(5)==cpr(5) ) )
					 { turn(spr(2),1);turn(bottom,1);turn(spr(2),-1);turn(bottom,1);
							turn(spr(2),1);turn(bottom,1);turn(bottom,1);turn(spr(2),-1);
					 }
           if ( ( cprb(1)==cpr(1) && cprb(7)==cpr(7) ) ||
						( cprb(3)==cpr(3) && cprb(5)==cpr(5) ) )
					 { turn(spr(2),1);turn(bottom,1);turn(spr(2),-1);turn(bottom,1);
						 turn(spr(2),1);turn(bottom,1);turn(bottom,1);turn(spr(2),-1);
					 }
					 if ( cprb(1)==cpr(1) && cprb(3)==cpr(3) ) J=3;
					 else if ( cprb(1)==cpr(1) && cprb(5)==cpr(5) ) J=1;
					 else if ( cprb(7)==cpr(7) && cprb(3)==cpr(3) ) J=7;
					 else if ( cprb(7)==cpr(7) && cprb(5)==cpr(5) ) J=5;
					 else { turn(bottom,1); continue;}
					 turn(spc(J),1);turn(bottom,1);turn(spc(J),-1);turn(bottom,1);
					 turn(spc(J),1);turn(bottom,1);turn(bottom,1);turn(spc(J),-1);
				 }
				 state=4;
				 level_finished=3;
				 break;

				case 4:
				 if ( !( cpo(0)==csd(bottom) && cprb(0)==cpr(0) &&
								 cpo(2)==csd(bottom) && cprb(2)==cpr(2) &&
								 cpo(6)==csd(bottom) && cprb(6)==cpr(6) &&
								 cpo(8)==csd(bottom) && cprb(8)==cpr(8) ) )
				 { j=0;
					 if ( ( cpo(2)==csd(bottom) && cplb(2)==cpl(0) ) ||
								( cpo(2)==cpr(0) && cplb(2)==csd(bottom) ) ||
								( cpo(2)==cpl(0) && cplb(2)==cpr(0) ) )
					 { turn(spl(8),-1);turn(spr(6),1);
						 turn(bottom,1);turn(spr(6),-1);
						 turn(bottom,-1);turn(spl(8),1);
						 turn(bottom,1);turn(spr(6),1);
						 turn(bottom,-1);turn(spr(6),-1);
					 }

					 while (!( ( cpo(0)==csd(bottom) && cplb(0)==cpl(0) ) ||
									 ( cpo(0)==cpl(0) && cplb(0)==cpr(0) ) ||
									 ( cpo(0)==cpr(0) && cplb(0)==csd(bottom) ) )  )
					 { turn(spl(0),-1);turn(spr(2),1);
						 turn(bottom,1);turn(spr(2),-1);
						 turn(bottom,-1);turn(spl(0),1);
						 turn(bottom,1);turn(spr(2),1);
						 turn(bottom,-1);turn(spr(2),-1);
					 }

					 while (!( cpo(0)==csd(bottom) && cplb(0)==cpl(0) ))
					 { turn(spl(0),-1);turn(spr(2),1);
						 turn(bottom,1);turn(spr(2),-1);
						 turn(bottom,-1);turn(spl(0),1);
						 turn(bottom,1);turn(spr(2),1);
						 turn(bottom,-1);turn(spr(2),-1);

						 turn(spl(2),-1);turn(spr(8),1);
						 turn(bottom,-1);turn(spl(2),1);
						 turn(bottom,1);turn(spr(8),-1);
						 turn(bottom,-1);turn(spl(2),-1);
						 turn(bottom,1);turn(spl(2),1);
					 }

					 while (!( ( cpo(2)==csd(bottom) && cplb(2)==cpl(2) ) ||
									 ( cpo(2)==cpl(2) && cplb(2)==cpr(2) ) ||
									 ( cpo(2)==cpr(2) && cplb(2)==csd(bottom) ) ) )
						hmove1();
					 while (! ( cpo(2)==csd(bottom) && cplb(2)==cpl(2) )  )
					 { hmove1(); hmove2(); }
					 while( !( cpo(0)==csd(bottom) && cprb(0)==cpr(0) &&
								 cpo(2)==csd(bottom) && cprb(2)==cpr(2) &&
								 cpo(6)==csd(bottom) && cprb(6)==cpr(6) &&
								 cpo(8)==csd(bottom) && cprb(8)==cpr(8) ) )
					 { hmove2(); hmove1(); }

				 }
				 state=5;
				 level_finished=4;
				 break;

		 }/*switch*/

	 }/*while*/
}/*solve*/

void ss(void)
{
 turn(right,-1); turn(left,1); turn(front,1); turn(front,1); turn(right,-1);
 turn(left,1); turn(right,-1); turn(left,1); turn(back,1); turn(back,1);
 turn(right,-1); turn(left,1); delay(2000);
 turn(left,-1); turn(front,-1); turn(front,-1); turn(back,1); turn(back,1);
 turn(top,-1); turn(bottom,1); turn(front,1); turn(front,1); turn(top,-1);
 turn(top,-1); turn(bottom,1); turn(bottom,1); turn(back,1); turn(back,1);
 turn(top,-1); turn(bottom,1); turn(left,1);
}
void cross(void)
{ int arian, rox;
 /*arian=random(3);*/
 arian=0;

 if (arian==0)
 { turn(bottom,-1);turn(front,1);turn(top,1);turn(left,-1);turn(left,-1);
	 turn(bottom,-1);turn(left,1);turn(top,-1);turn(top,-1);turn(front,-1);
	 turn(top,1);turn(left,1);turn(right,-1);turn(bottom,1);turn(right,1);
	 turn(bottom,1);turn(front,1);turn(bottom,-1);turn(front,-1);turn(bottom,1);
	 turn(right,-1);turn(bottom,1);turn(right,1);turn(bottom,1);turn(front,1);
	 turn(bottom,-1);turn(front,-1);turn(left,1);turn(bottom,1);turn(left,-1);
	 turn(bottom,-1);turn(front,-1);turn(bottom,-1);turn(front,1);turn(bottom,1);
	 turn(bottom,1);turn(back,1);turn(bottom,1);turn(back,-1);turn(bottom,-1);
	 turn(left,-1);turn(bottom,-1);turn(left,1);turn(left,1);turn(bottom,1);
	 turn(back,1);turn(bottom,-1);turn(back,-1);turn(left,-1);turn(bottom,-1);
	 turn(front,1);turn(back,-1);turn(bottom,1);turn(front,-1);turn(bottom,-1);
	 turn(back,1);turn(bottom,1);turn(front,1);turn(bottom,-1);turn(front,-1);
	 turn(right,1);turn(bottom,1);turn(right,-1);turn(bottom,-1);turn(left,-1);
	 turn(bottom,1);turn(right,1);turn(bottom,-1);turn(right,-1);turn(left,1);
	 turn(back,-1);turn(front,1);turn(bottom,-1);turn(back,1);turn(bottom,1);
	 turn(front,-1);turn(bottom,-1);turn(back,-1);turn(bottom,1);turn(back,1);
	 turn(right,1);turn(left,-1);turn(bottom,1);turn(right,-1);turn(bottom,-1);
	 turn(left,1);turn(bottom,1);turn(right,1);turn(bottom,-1);turn(right,-1);
 }
 else if (arian==1)
 { turn(right,-1);turn(left,1);turn(front,1);turn(front,1);turn(right,-1);
	 turn(left,1);turn(right,-1);turn(left,1);turn(back,1);turn(back,1);
	 turn(right,-1);turn(left,1);turn(left,-1);turn(front,-1);turn(front,-1);
	 turn(back,1);turn(back,1);turn(top,-1);turn(bottom,1);turn(front,1);
	 turn(front,1);turn(top,-1);turn(top,-1);turn(bottom,1);turn(bottom,1);
	 turn(back,1);turn(back,1);turn(top,-1);turn(bottom,1);turn(left,1);
 }
 else
 { for (arian=0;arian<20+random(10);arian++)
	 { rox=random(2); if (!rox) rox=-1;
		 turn(random(6), rox);
	 }
 }
}

void main(void)
{ int i, j, r;
	float rgb_convert;
	char ch=1, chr[100], finished=0;
	struct palettetype pal;
	image=malloc(201*201);
	pi=acos(-1.0);
	piovrtu=pi/2.0;
	res=pi/DTL, nres=-1.0*pi/DTL;
	/*r=(int)(sqrt(SZ*SZ+SZ*SZ));
	r=(int)(sqrt(r*r+r*r))+(int)(SZ/3.0);*/
	r=SZ*2.4;
	Y_UP=K-(int)((r*ADJF_Y));
	X_LEFT=H-r;
	Y_DOWN=K+(int)((r*ADJF_Y));
	X_RIGHT=H+r;
	rgb_convert=63.0/255.0;
   randomize();
	initgraph(&d,&m,"C:\BGI");
	Initialize_Menu();
	Init_Mouse();

	swap_buffers();
	Hide_Mouse();
	Draw_Menu();
	Show_Mouse();Show_Mouse();
	swap_buffers();
	Hide_Mouse();
	Draw_Menu();
	Deactivate_All();
	Show_Mouse();Show_Mouse();

  getpalette(&pal);
  setrgbpalette(pal.colors[MAGENTA],36,7,48);
	setrgbpalette(pal.colors[LIGHTGRAY],45,45,45);
	setrgbpalette(pal.colors[DARKGRAY],30,30,30);
	setrgbpalette(pal.colors[LIGHTCYAN],61,56,51);
	setrgbpalette(pal.colors[RED],63,0,0);
	setrgbpalette(pal.colors[2],102*rgb_convert,102*rgb_convert,255*rgb_convert);
	setrgbpalette(pal.colors[3],255*rgb_convert,201*rgb_convert,135*rgb_convert);
	setrgbpalette(pal.colors[5],255*rgb_convert,255*rgb_convert,0*rgb_convert);
	setrgbpalette(pal.colors[6],60*rgb_convert,255*rgb_convert,0);
	setrgbpalette(pal.colors[11],253*rgb_convert,118*rgb_convert,255*rgb_convert);
	setrgbpalette(pal.colors[12],255*rgb_convert,51*rgb_convert,0);
	setrgbpalette(pal.colors[13],0,0,0);
	setlinestyle(SOLID_LINE,1,NORM_WIDTH);
	initialize_values();
	initialize_matrix();
	setbkcolor(0);
	R_matrix(pi/-5.0, 1);	multiply_matrix();
	R_matrix(pi/5.0, 0);	multiply_matrix();
	draw_cube(); draw_cube();
do
{
	if (kbhit())
	{
		 ch=getch();
		 if (ch==27) finished=1;
		 else if (ch==0)
		 {
			ch=getch();
			for(r=0;r<2;r++)
			{
			switch(ch)
			{ case 72 : R_matrix(nres, 0);
								 multiply_matrix();
								 break;
				case 80 : R_matrix(res, 0);
								 multiply_matrix();
								 break;
				case 75 : R_matrix(nres, 1);
									multiply_matrix();
									break;
				case 77 : R_matrix(res, 1);
									multiply_matrix();
									break;
			 }
			 draw_cube();
			 }
		 }/*if ch==0*/
     else
		 {
				switch(ch)
				{  case 'f': case '1': turn(front ,1);	break; /*front*/
					 case 'r': case '2': turn(right ,1);	break; /*right*/
					 case 't': case '3': turn(top   ,1); break; /*top*/
					 case 'b': case '4': turn(back  ,1);	break; /*back*/
					 case 'l': case '5': turn(left  ,1);	break; /*left*/
					 case 'm': case '6': turn(bottom,1); break; /*bottom*/
					 case 'F': case '7': turn(front ,-1);	break;
					 case 'R': case '8': turn(right ,-1);	break;
					 case 'T': case '9': turn(top   ,-1); break;
					 case 'B': turn(back  ,-1);	break;
					 case 'L': turn(left  ,-1);	break;
					 case 'M': turn(bottom,-1); break;
					 case '0': turn(lastside, -1*lastdir); break; /* undo */

				}
			}

	} else if (Mouse_Down(&M)) {
		switch(M.Button){
			case 0: if (Within_Control(&F1,&B1,M.x,M.y)){       /*LOAD*/
								 Hide_Mouse();
								 Deactivate_Button(&ButtonDown);
								 Button_Down(&B1);
								 delay(2000);
								 Create_Button(&B1);
								 ButtonDown=B1;
								 solve(4);
                 Show_Mouse();
								 delay(2000);
							} else if (Within_Control(&F1,&B2,M.x,M.y)){   /*SAVE*/
								 Hide_Mouse();
                 Deactivate_Button(&ButtonDown);
								 Button_Down(&B2);
								 delay(2000);
								 Create_Button(&B2);
								 Deactivate_Button(&B2);
								 ButtonDown=B2;
								 Show_Mouse();
								 delay(2000);
							} else if (Within_Control(&F1,&B3,M.x,M.y)){    /*SHUFFLE*/
								 Hide_Mouse();
                 Deactivate_Button(&ButtonDown);
								 Button_Down(&B3);
								 delay(2000);
								 Create_Button(&B3);
								 ButtonDown=B3;
								 delay(2000);
                 Deactivate_Button(&ButtonDown);
								 cross();
								 Show_Mouse();

							} else if (Within_Control(&F1,&B4,M.x,M.y)){   /*1st Layer*/
								 Hide_Mouse();
                 Deactivate_Button(&ButtonDown);
								 Button_Down(&B4);
								 delay(2000);
								 Create_Button(&B4);
								 ButtonDown=B4;
                 Deactivate_Button(&ButtonDown);
								 solve(1);
								 Show_Mouse();
								 delay(2000);
							} else if (Within_Control(&F1,&B5,M.x,M.y)){  /*2nd Layer*/
      					 Hide_Mouse();
                 Deactivate_Button(&ButtonDown);
								 Button_Down(&B5);
								 delay(2000);
								 Create_Button(&B5);
								 ButtonDown=B5;
                 Deactivate_Button(&ButtonDown);
								 solve(2);
								 Show_Mouse();
								 delay(2000);
							} else if (Within_Control(&F1,&B6,M.x,M.y)){  /*3rd Layer*/
      					 Hide_Mouse();
                 Deactivate_Button(&ButtonDown);
								 Button_Down(&B6);
								 delay(2000);
								 Create_Button(&B6);
								 ButtonDown=B6;
                 Deactivate_Button(&ButtonDown);
								 solve(4);
								 Show_Mouse();
								 delay(2000);
							} else if (Within_Control(&F1,&B7,M.x,M.y)){  /*About*/
								 Hide_Mouse();
								 getimage(220,75,420,275,image);
                 Deactivate_Button(&ButtonDown);
								 Button_Down(&B7);
								 delay(2000);
								 Create_Button(&B7);
								 Deactivate_Button(&B7);
								 delay(2000);
								 ButtonDown=B7;
								 About();
                 Show_Mouse();
							} else if (Within_Control(&F1,&B8,M.x,M.y)){  /*QUIT*/
      					 Hide_Mouse();
                 Deactivate_Button(&ButtonDown);
								 Button_Down(&B8);
								 delay(2000);
								 Create_Button(&B8);
								 ButtonDown=B8;
                 Show_Mouse();
								 delay(2000);
								 finished=1;
							}
		}/*switch*/
	}

}while (!finished);
	closegraph();
	restorecrtmode();
}