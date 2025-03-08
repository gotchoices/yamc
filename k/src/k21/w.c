/* Window system
   Copyright (C) 1992 Joseph H. Allen

This file is part of JOE (Joe's Own Editor)

JOE is free software; you can redistribute it and/or modify it under the 
terms of the GNU General Public License as published by the Free Software 
Foundation; either version 1, or (at your option) any later version.  

JOE is distributed in the hope that it will be useful, but WITHOUT ANY 
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more 
details.  

You should have received a copy of the GNU General Public License along with 
JOE; see the file COPYING.  If not, write to the Free Software Foundation, 
675 Mass Ave, Cambridge, MA 02139, USA.  */ 

#include "config.h"
#include "b.h"
#include "scrn.h"
#include "queue.h"
#include "main.h"
#include "poshist.h"
#include "blocks.h"
#include "zstr.h"
#include "w.h"

extern int dspasis;	/* Set to display chars above 127 as-is */
extern int staen;	/* 0 if top-most status line not displayed */

/* Count no. of main windows */

int countmain(t)
SCREEN *t;
 {
 int nmain=1;
 W *m=t->curwin->main;
 W *q;
 for(q=t->curwin->link.next;q!=t->curwin;q=q->link.next)
  if(q->main!=m)
   {
   ++nmain;
   m=q->main;
   }
 return nmain;
 }

/* Redraw a window */

void wredraw(w)
W *w;
 {
 msetI(w->t->t->updtab+w->y,1,w->h);
 }

/* Find first window in a group */

W *findtopw(w)
W *w;
 {
 W *x;
 for(x=w;x->link.prev->main==w->main && x->link.prev!=w;x=x->link.prev);
 return x;
 }

/* Determine height of a family of windows */

int getgrouph(w)
W *w;
 {
 W *x;
 int h;

 /* Find first window in family */
 x=findtopw(w);

 /* Add heights of all windows in family */
 for(w=x, h=(w->reqh?w->reqh:w->hh);
     w->link.next!=x && w->link.next->main==x->main;
     w=w->link.next, h+=(w->reqh?w->reqh:w->hh));

 return h;
 }

/* Determine minimum height of a family */

int getminh(w)
W *w;
 {
 W *x;
 int h;
 x=findtopw(w);
 for(w=x, h=(w->win?1:2);
     w->link.next!=x && w->link.next->main==x->main;
     w=w->link.next, h+=(w->win?1:2));

 return h;
 }

/* Find last window in a group */

W *findbotw(w)
W *w;
 {
 W *x;
 for(x=w;x->link.next->main==w->main && x->link.next!=w;x=x->link.next);
 return x;
 }

/* Demote group of window to end of window list.  Returns true if top window
   was demoted */

int demotegroup(w)
W *w;
 {
 W *top=findtopw(w);
 W *bot=findbotw(w);
 W *next;
 int flg=0;
 for(w=top;w!=bot;w=next)
  {
  next=w->link.next;
  if(w==w->t->topwin) flg=1, w->t->topwin=next;
  else demote(W,link,w->t->topwin,w);
  w->y= -1;
  }
 if(w==w->t->topwin) flg=1;
 else demote(W,link,w->t->topwin,w);
 w->y= -1;
 return flg;
 }

/* Find last window on the screen */

W *lastw(t)
SCREEN *t;
 {
 W *x;
 for(x=t->topwin;x->link.next!=t->topwin && x->link.next->y>=0;x=x->link.next);
 return x;
 }

/* Create a screen object */

SCREEN *scr;

SCREEN *screate(scrn)
SCRN *scrn;
 {
 SCREEN *t=(SCREEN *)malloc(sizeof(SCREEN));
 t->t=scrn;
 t->w=scrn->co;
 t->h=scrn->li;
 t->topwin=0;
 t->curwin=0;
 t->wind=skiptop;
 scr=t;
 return t;
 }

void chsize(t,mul,div)
SCREEN *t;
 {
 W *w;
 w=t->topwin; do
  if(!w->win)
   {
   w->reqh=getgrouph(w)*mul;
   w->reqh=w->reqh/div+(w->reqh%div>=div/2?1:0);
   if(w->reqh<FITHEIGHT) w->reqh=FITHEIGHT;
   w->reqh-=getminh(w)-2;
   if(w->reqh<2) w->reqh=2;
   if(w->reqh>t->h-t->wind) w->reqh=t->h-t->wind;
   }
  while(w=w->link.next, w!=t->topwin);
 wfit(t);
 }

void sresize(t)
SCREEN *t;
 {
 SCRN *scrn=t->t;
 W *w;
 int osize=t->h-t->wind;
 t->w=scrn->co;
 t->h=scrn->li;
 if(t->h-t->wind<FITHEIGHT) t->wind=t->h-FITHEIGHT;
 if(t->wind<0) t->wind=0;
 w=t->topwin; do
  w->y= -1, w->w=t->w-1;
  while(w=w->link.next, w!=t->topwin);
 chsize(t,t->h-t->wind,osize);
 updall();
 }

void updall()
 {
 int y;
 for(y=0;y!=scr->h;++y) scr->t->updtab[y]=1;
 }

void scrins(b,l,n,flg)
B *b;
long l,n;
int flg;
 {
 W *w;
 if(w=scr->topwin) do
  if(w->y>=0)
   {
   if(w->object && w->watom->ins) w->watom->ins(w->object,b,l,n,flg);
   }
  while(w=w->link.next, w!=scr->topwin);
 }

void scrdel(b,l,n,flg)
B *b;
long l,n;
int flg;
 {
 W *w;
 if(w=scr->topwin) do
  if(w->y>=0)
   {
   if(w->object && w->watom->del) w->watom->del(w->object,b,l,n,flg);
   }
  while(w=w->link.next, w!=scr->topwin);
 }

/* Fit as many windows on the screen as is possible beginning with the window
 * at topwin.  Give any extra space which couldn't be used to fit in another
 * window to the last text window on the screen.  This function guarentees
 * to fit on the window with the cursor in it (moves topwin to next group
 * of windows until window with cursor fits on screen).
 */

static int doabort();
extern int dostaupd;

void wfit(t)
SCREEN *t;
 {
 int y;		/* Where next window goes */
 int left;	/* Lines left on screen */
 W *w;		/* Current window we're fitting */
 W *pw;		/* Main window of previous family */
 int req;	/* Amount this family needs */
 int adj;	/* Amount family needs to be adjusted */
 int flg=0;	/* Set if cursor window was placed on screen */
 int ret;
 dostaupd=1;

 tryagain:
 y=t->wind; left=t->h-y; pw=0;

 w=t->topwin; do
  w->ny= -1, w->hh=w->nh=(w->reqh?w->reqh:w->hh);
  while((w=w->link.next)!=t->topwin);

 /* Fit a group of windows on the screen */
 w=t->topwin; do
  {
  req=getgrouph(w);
  if(req>left)		/* If group is taller than lines left */
   adj=req-left;		/* then family gets shorter */
  else adj=0;
  
  /* Fit a family of windows on the screen */
  do
   {
   w->ny=y;			/* Set window's y position */
   if(!w->win) pw=w, w->nh-=adj;	/* Adjust main window of the group */
   if(!w->win && w->nh<2) while(w->nh<2) w->nh+=doabort(w->link.next,&ret);
   if(w==t->curwin) flg=1;	/* Set if we got window with cursor */
   y+=w->nh; left-=w->nh;	/* Increment y value by height of window */
   w=w->link.next;		/* Next window */
   } while(w!=t->topwin && w->main==w->link.prev->main);
  } while(w!=t->topwin && left>=FITHEIGHT);

 /* We can't use extra space to fit a new family on, so give space to parent of
  * previous family */
 pw->nh+=left;

 /* Adjust that family's children which are below the parent */
 while((pw=pw->link.next)!=w) pw->ny+=left;

 /* Make sure the cursor window got on the screen */
 if(!flg)
  {
  t->topwin=findbotw(t->topwin)->link.next;
  goto tryagain;
  }

 /* All of the windows are now on the screen.  Scroll the screen to reflect what
  * happened
  */
 w=t->topwin; do
  if(w->y>=0 && w->ny>=0)
   if(w->ny>w->y)
    {
    W *l=pw=w;
    while(pw->link.next!=t->topwin &&
          (pw->link.next->y<0 || pw->link.next->ny<0 ||
          pw->link.next->ny>pw->link.next->y))
     {
     pw=pw->link.next;
     if(pw->ny>=0 && pw->y>=0) l=pw;
     }
    /* Scroll windows between l and w */
    loop1:
    if(l->ny>=0 && l->y>=0)
     nscrldn(t->t,l->y,l->ny+Umin(l->h,l->nh),l->ny-l->y);
    if(w!=l)
     {
     l=l->link.prev;
     goto loop1;
     }
    w=pw->link.next;
    }
   else if(w->ny<w->y)
    {
    W *l=pw=w;
    while(pw->link.next!=t->topwin &&
          (pw->link.next->y<0 || 
          pw->link.next->ny<0 || 
          pw->link.next->ny<pw->link.next->y))
     {
     pw=pw->link.next;
     if(pw->ny>=0 && pw->y>=0) l=pw;
     }
    /* Scroll windows between l and w */
    loop0:
    if(w->ny>=0 && w->y>=0)
     nscrlup(t->t,w->ny,w->y+Umin(w->h,w->nh),w->y-w->ny);
    if(w!=l)
     {
     w=w->link.next;
     goto loop0;
     }
    w=pw->link.next;
    }
   else w=w->link.next;
  else w=w->link.next;
  while(w!=t->topwin);

 /* Update current height and position values */
 w=t->topwin; do
  {
  if(w->ny>=0)
   {
   if(w->object)
    {
    if(w->watom->move) w->watom->move(w->object,w->x,w->ny);
    if(w->watom->resize) w->watom->resize(w->object,w->w,w->nh);
    }
   if(w->y== -1) msetI(t->t->updtab+w->ny,1,w->nh);
   w->y=w->ny;
   }
  else w->y= -1;
  w->h=w->nh;
  w->reqh=0;
  }
  while(w=w->link.next, w!=t->topwin);
 }

/* Goto next window */

int wnext(t)
SCREEN *t;
 {
 if(t->curwin->link.next!=t->curwin)
  {
  t->curwin=t->curwin->link.next;
  if(t->curwin->y== -1) wfit(t);
  return 0;
  }
 else return -1;
 }

/* Goto previous window */

int wprev(t)
SCREEN *t;
 {
 if(t->curwin->link.prev!=t->curwin)
  {
  t->curwin=t->curwin->link.prev;
  if(t->curwin->y== -1)
   {
   t->topwin=findtopw(t->curwin);
   wfit(t);
   }
  return 0;
  }
 else return -1;
 }

/* Grow window */

int wgrow(w)
W *w;
 {
 W *nextw;
 if((w->link.next==w->t->topwin || w->link.next->y== -1) &&
    w!=w->t->topwin)
  return wshrink(w->link.prev->main);

 /* Is there enough space to grow window? */
 for(nextw=w->link.next;
     nextw->win && nextw!=w->t->topwin;
     nextw=nextw->link.next);
 if(nextw==w->t->topwin) return -1;
 if(nextw->y== -1 || nextw->hh<=FITHEIGHT) return -1;

 w->reqh=w->hh+1;	/* Increase this window's height */
 nextw->reqh=nextw->hh-1;/* Decrease this window's height and move it down */

 wfit(w->t);

 return 0;
 }

/* Shrink window */

int wshrink(w)
W *w;
 {
 W *nextw;
 if((w->link.next==w->t->topwin || w->link.next->y== -1) &&
    w!=w->t->topwin)
  return wgrow(w->link.prev->main);

 /* Is this window too small already? */
 if(w->hh<=FITHEIGHT) return -1;

 /* Is there a window we can grow with this window's space? */
 for(nextw=w->link.next;
     nextw!=w->t->topwin && nextw->win;
     nextw=nextw->link.next);
 if(nextw==w->t->topwin) return -1;

 w->reqh=w->hh-1;		/* Decrease window size */
 nextw->reqh=nextw->hh+1;	/* Give space to this window */

 wfit(w->t);
 return 0;
 }

/* Show all windows */

void wshowall(t)
SCREEN *t;
 {
 int n=0;
 int set;
 W *w=t->topwin; do
  if(!w->win) ++n;
  while(w=w->link.next, w!=t->topwin);
 if((t->h-t->wind)/n>=FITHEIGHT) set=(t->h-t->wind)/n;
 else set=FITHEIGHT;
 w=t->topwin; do
  if(!w->win)
   {
   int h=getminh(w);
   if(h>=set) w->reqh=2;
   else w->reqh=set-(h-2);
   w->orgwin=0;
   }
  while(w=w->link.next, w!=t->topwin);
 wfit(t);
 }

void wspread(t)
SCREEN *t;
 {
 int n=0;
 W *w=t->topwin; do
  if(w->y>=0 && !w->win) ++n;
  while(w=w->link.next, w!=t->topwin);
 if(!n)
  {
  wfit(t);
  return;
  }
 if((t->h-t->wind)/n>=FITHEIGHT) n=(t->h-t->wind)/n;
 else n=FITHEIGHT;
 w=t->topwin; do
  if(!w->win)
   {
   int h=getminh(w);
   if(h>=n) w->reqh=2;
   else w->reqh=n-(h-2);
   w->orgwin=0;
   }
  while(w=w->link.next, w!=t->topwin);
 wfit(t);
 }

/* Show just one family of windows */

void wshowone(w)
W *w;
 {
 W *q=w->t->topwin; do
  if(!q->win)
   q->reqh=w->t->h-(getgrouph(q)-q->hh), q->orgwin=0;
  while(q=q->link.next, q!=w->t->topwin);
 wfit(w->t);
 }

/* Create a window */

W *wcreate(t,watom,where,target,original,height,huh,notify)
SCREEN *t;
WATOM *watom;
W *where, *target, *original;
int height;
char *huh;
int *notify;
 {
 W *new;

 if(height<1) return 0;

 /* Create the window */
 new=(W *)malloc(sizeof(W));
 new->notify=notify;
 new->t=t;
 new->w=t->w-1;
 new->hh=new->h=height;
 new->y= -1;
 new->ny=0; new->nh=0; new->reqh=0;
 new->x=0;
 new->huh=huh;
 new->orgwin=original;
 new->watom=watom;
 new->object=0;
 new->msgb=0;
 new->msgt=0;
 /* Set window's target and family */
 if(new->win=target) new->main=target->main;
 else new->main=new;

 /* Get space for window */
 if(original)
  if(original->hh-height<=2)
   {
   /* Not enough space for window */
   free(new);
   return 0;
   }
  else original->hh-=height;

 /* Create new keyboard handler for window */
 if(watom->context) new->kbd=mkkbd(getcntext(watom->context));
 else new->kbd=0;

 /* Put window on the screen */
 if(where) enquef(W,link,where,new);
 else
  {
  if(t->topwin) enqueb(W,link,t->topwin,new);
  else izque(W,link,new), t->curwin=t->topwin=new;
  }

 return new;
 }

/* Abort group of windows */

static int doabort(w,ret)
W *w;
int *ret;
 {
 int amnt=w->hh;
 W *z;
 w->y= -2;
 if(w->t->topwin==w) w->t->topwin=w->link.next;
 loop:
 z=w->t->topwin; do
  {
  if(z->orgwin==w) z->orgwin=0;
  if((z->win==w || z->main==w) && z->y!= -2)
   {
   amnt+=doabort(z,ret);
   goto loop;
   }
  }
  while(z=z->link.next, z!=w->t->topwin);
 if(w->orgwin)
  {
  if(w->orgwin->reqh) w->orgwin->reqh+=(w->reqh?w->reqh:w->hh);
  else w->orgwin->reqh=w->orgwin->hh+(w->reqh?w->reqh:w->hh);
  }
 if(w->t->curwin==w)
  if(w->t->curwin->win) w->t->curwin=w->t->curwin->win;
  else
   if(w->orgwin) w->t->curwin=w->orgwin;
   else w->t->curwin=w->link.next;
 if(qempty(W,link,w))
  {
  leave=1;
  amnt=0;
  }
 deque(W,link,w);
 if(w->watom->abort && w->object)
  {
  *ret=w->watom->abort(w->object,MAXINT);
  if(w->notify) *w->notify= -1;
  }
 else
  {
  *ret= -1;
  if(w->notify) *w->notify= 1;
  }
 rmkbd(w->kbd);
 free(w);
 windie(w);
 return amnt;
 }

/* Abort a window and its children */

int wabort(w)
W *w;
 {
 SCREEN *t=w->t;
 int ret;
 if(w!=w->main)
  {
  doabort(w,&ret);
  if(!leave) wfit(t);
  }
 else
  {
  doabort(w,&ret);
  if(!leave)
   {
   if(lastw(t)->link.next!=t->topwin) wfit(t);
   else wspread(t);
   }
  }
 return ret;
 }

/* Generate text with formatting escape sequences */

void genfmt(t,x,y,ofst,s,flg)
SCRN *t;
char *s;
 {
 int *scrn=t->scrn+y*t->co+x;
 int atr=0;
 int col=0;
 while(*s)
  {
  int c;
  if(*s=='\\')
   switch(*++s)
    {
    case 'u': case 'U':
    atr^=UNDERLINE; ++s; goto cont;
    
    case 'i': case 'I':
    atr^=INVERSE; ++s; goto cont;
    
    case 'b': case 'B':
    atr^=BOLD; ++s; goto cont;
    
    case 'd': case 'D':
    atr^=DIM; ++s; goto cont;
    
    case 'f': case 'F':
    atr^=BLINK; ++s; goto cont;
    
    case 0: --s;
    }
  if(col++>=ofst) c= (unsigned char)*s++ + atr; else { ++s; continue; }
  if(c!=*scrn) *scrn=c, outatr(t,col-ofst+x-1,y,c);
  ++scrn;
  cont:;
  }
 if(flg) eraeol(t,col-ofst+x,y);
 }

/* Generate text: no formatting */

void gentxt(t,x,y,ofst,s,len,flg)
SCRN *t;
char *s;
 {
 int *scrn;
 int col;
 int c;
 int a;
 for(col=0,scrn=t->scrn+y*t->co+x;col!=len;++col)
  if(col>=ofst)
   {
   a=0;
   c= (unsigned char)s[col];
   if(c>=128 && !dspasis) c-=128, a=INVERSE;
   if(c<32) c+=64, a+=UNDERLINE;
   else if(c==127) c='?', a+=UNDERLINE;
   c+=a;
   if(c!=*scrn) *scrn=c, outatr(t,col-ofst+x,y,c);
   ++scrn;
   }
 if(flg) eraeol(t,col-ofst+x,y);
 }

/* Determine column width of string with format codes */

int fmtlen(s)
char *s;
 {
 int col=0;
 while(*s)
  {
  if(*s=='\\')
   switch(*++s)
    {
    case 'u': case 'i': case 'd': case 'f': case 'b':
    case 'U': case 'I': case 'D': case 'F': case 'B':
    ++s; goto cont;

    case 0:
    --s;
    }
  ++col; ++s;
  cont:;
  }
 return col;
 }

/* Return offset within format string which corresponds to a particular
   column */

int fmtpos(s,goal)
char *s;
 {
 char *org=s;
 int col=0;
 while(*s && col!=goal)
  {
  if(*s=='\\')
   switch(*++s)
    {
    case 'u': case 'i': case 'd': case 'f': case 'b':
    case 'U': case 'I': case 'D': case 'F': case 'B':
    ++s; goto cont;

    case 0:
    --s;
    }
  ++col; ++s;
  cont:;
  }
 return s-org+goal-col;
 }

/* Display a message and skip the next key */

static void mdisp(t,y,s)
SCRN *t;
char *s;
 {
 int ofst;
 int len;
 len=fmtlen(s);
 if(len<=(t->co-1)) ofst=0;
 else ofst=len-(t->co-1);
 genfmt(t,0,y,ofst,s,1);
 t->updtab[y]=1;
 }

void msgout(w)
W *w;
 {
 SCRN *t=w->t->t;
 if(w->msgb)
  {
  mdisp(t,w->y+w->h-1,w->msgb);
  w->msgb=0;
  }
 if(w->msgt)
  {
  mdisp(t,w->y+( (w->h>1 && (w->y || !staen)) ? 1 : 0 ),w->msgt);
  w->msgt=0;
  }
 }

/* Set temporary message */

char msgbuf[80];

void msgnw(w,s)
BASE *w;
char *s;
 {
 w->parent->msgb=s;
 }

void msgnwt(w,s)
BASE *w;
char *s;
 {
 w->parent->msgt=s;
 }

int urtn(b,k)
BASE *b;
 {
 if(b->parent->watom->rtn) return b->parent->watom->rtn(b,k);
 else return -1;
 }

int utype(b,k)
BASE *b;
 {
 if(b->parent->watom->type) return b->parent->watom->type(b,k);
 else return -1;
 }

/* Window user commands */

int uprevw(bw)
BASE *bw;
 {
 return wprev(bw->parent->t);
 }

int unextw(bw)
BASE *bw;
 {
 return wnext(bw->parent->t);
 }

int ugroww(bw)
BASE *bw;
 {
 return wgrow(bw->parent);
 }

int ushrnk(bw)
BASE *bw;
 {
 return wshrink(bw->parent);
 }

int uexpld(bw)
BASE *bw;
 {
 if(bw->parent->t->h-bw->parent->t->wind==getgrouph(bw->parent))
  wshowall(bw->parent->t);
 else wshowone(bw->parent);
 return 0;
 }

int uretyp(bw)
BASE *bw;
 {
 nredraw(bw->parent->t->t);
 return 0;
 }
