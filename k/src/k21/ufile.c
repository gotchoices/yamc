/* User file operations
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

#include <stdio.h>
#include <time.h>
#include <pwd.h>
#include <unistd.h>
#include "config.h"
#include "b.h"
#include "bw.h"
#include "scrn.h"
#include "tw.h"
#include "w.h"
#include "pw.h"
#include "qw.h"
#include "ublock.h"
#include "main.h"
#include "zstr.h"
#include "vs.h"
#include "va.h"
#include "menu.h"
#include "path.h"
#include "ublock.h"
#include "tty.h"
#include "tab.h"
#include "uerror.h"
#include "ufile.h"

int exask=0;
extern int orphan;
char *backpath=0;			/* Place to store backup files */

static B *filehist=0;			/* History of file names */

/* Write highlighted block to a file */

int ublksave(bw)
BW *bw;
 {
 if(markb && markk && markb->b==markk->b &&
    (markk->byte-markb->byte)>0 &&
    (!square || piscol(markk)>piscol(markb)))
  {
  if(wmkpw(bw,
         "Name of file to write (^C to abort): ",&filehist,dowrite,"Names",NULL,cmplt,NULL,NULL)) return 0;
  else return -1;
  }
 msgnw(bw,"No block");
 return -1;
 }

/* Shell escape */

int ushell(bw)
BW *bw;
 {
 nescape(bw->parent->t->t);
 ttsusp();
 nreturn(bw->parent->t->t);
 return 0;
 }

/* Make backup file if it needs to be made
 * Returns 0 if backup file was made or didn't need to be made
 * Returns 1 for error
 */

int nobackups=0;

static int backup(bw)
BW *bw;
 {
 if(!bw->b->backup && !nobackups && bw->b->name && bw->b->name[0])
  {
  char *name=0;
  char *s=0;

  if(backpath)
   {
   char *n=namprt(bw->b->name);
   name=vsncpy(name,0,sz(backpath));
   if(name[sLEN(name)-1]!='/') name=vsadd(name,'/');
   name=vsncpy(name,sLEN(name),sz(getlogin()));
   if(name[sLEN(name)-1]!='/') name=vsadd(name,'/');
   
   s=vsncpy(s,0,sc("/bin/mkdir -p "));
   s=vsncpy(s,sLEN(s),sz(name));
   s=vsncpy(s,sLEN(s),sc("; "));
   
   name=vsncpy(sv(name),sv(n));
   vsrm(n);
   }
  else 
   {
   name=vsncpy(name,0,sz(bw->b->name));
   s=vsncpy(s,0,sc(""));
   }
  name=vsadd(name,'~');

  /* Create command string */
  s=vsncpy(s,sLEN(s),sc("/bin/cp \""));
  s=vsncpy(s,sLEN(s),sz(bw->b->name));
  s=vsncpy(s,sLEN(s),sc("\" \""));

#ifdef UNIQUE_BACKUPS
  {
  char ts[32];
  time_t t;
  struct passwd *pwp = getpwuid(getuid());	
  time(&t);
  strftime(ts,32,"%d-%b-%y_%H:%M:%S~",localtime(&t));
  name=vsncpy(name,sLEN(name),sz(ts));
  name=vsncpy(name,sLEN(name),sz(pwp->pw_name));
  }
#else
  unlink(name);  /* Attempt to delete backup file */
#endif

  s=vsncpy(s,sLEN(s),sv(name));
  s=vsncpy(s,sLEN(s),sc("\" 2>/dev/null"));
  tickoff();
  if(system(s))
   {
   char *ms = 0;
   tickon();
   ms=vsncpy(ms,0,sc("Can't create backup file: "));
   ms=vsncpy(ms,sLEN(ms),sv(name));
   msgnw(bw,ms);
   vsrm(ms);
   vsrm(s);
   vsrm(name);
   return 1;
   }
  else
   {
   tickon();
   bw->b->backup=1;
   vsrm(s);
   vsrm(name);
   return 0;
   }
  }
 else return 0;
 }

/* Write file */

static int dosave(bw,s,object,notify)
BW *bw;
char *s;
void *object;
int *notify;
 {
 int fl;
 if(notify) *notify=1;
 if(backup(bw)) { vsrm(s); return -1; }
 if(fl=bsave(bw->b->bof,s,bw->b->eof->byte))
  {
  msgnw(bw,msgs[fl+MSGS]);
  vsrm(s);
  return -1;
  }
 else
  {
  if(!bw->b->name) bw->b->name=zdup(s);
  if(!zcmp(bw->b->name,s))
   {
   bw->b->changed=0;
   saverr(bw->b->name);
   }
  sprintf(msgbuf,"File %s saved",s);
  msgnw(bw,msgbuf);
  vsrm(s);
  return 0;
  }
 }

int usave(bw)
BW *bw;
 {
 BW *pbw=wmkpw(bw,
                "Name of file to save (^C to abort): ",&filehist,dosave,"Names",NULL,cmplt,NULL,NULL);
 if(pbw && bw->b->name)
  {
  binss(pbw->cursor,bw->b->name);
  pset(pbw->cursor,pbw->b->eof);
  pbw->cursor->xcol=piscol(pbw->cursor);
  }
 if(pbw) return 0;
 else return -1;
 }

/* Load file to edit */

int doedit(bw,s,obj,notify)
BW *bw;
char *s;
void *obj;
int *notify;
 {
 int ret=0;
 int er;
 void *object;
 W *w;
 B *b;
 if(notify) *notify=1;
 if(bw->pid)
  {
  msgnw(bw,"Process running in this window");
  return -1;
  }
 b=bfind(s,1);
 er=error;
 if(er == -5)
  {
  msgnw(bw,msgs[MSGS+er]);
  return -1;
  }
 if(bw->b->count==1 && (bw->b->changed || bw->b->name))
  if(orphan) orphit(bw);
  else
   {
   if(uduptw(bw))
    {
    brm(b);
    return -1;
    }
   bw=(BW *)maint->curwin->object;
   }
 if(er)
  {
  msgnwt(bw,msgs[er+MSGS]);
  if(er!= -1) ret= -1;
  }
 object=bw->object;
 w=bw->parent;
 bwrm(bw);
 w->object=(void *)(bw=bwmk(w,b,0));
 wredraw(bw->parent);
 bw->object=object;
 vsrm(s);
 return ret;
 }

int okrepl(bw)
BW *bw;
 {
 if(bw->b->count==1 && bw->b->changed)
  {
  msgnw(bw,"Can't replace modified file");
  return -1;
  }
 else return 0;
 }

int uedit(bw)
BW *bw;
 {
 if(wmkpw(bw,"Name of file to edit (^C to abort): ",&filehist,doedit,"Names",NULL,cmplt,NULL,NULL)) return 0;
 else return -1;
 }

/* Load file into buffer: can result in an orphaned buffer */

int dorepl(bw,s,obj,notify)
BW *bw;
char *s;
void *obj;
int *notify;
 {
 void *object=bw->object;
 int ret=0;
 W *w=bw->parent;
 B *b;
 if(notify) *notify=1;
 if(bw->pid)
  {
  msgnw(bw,"Process running in this window");
  return -1;
  }
 b=bfind(s,1);
 if(error)
  {
  msgnwt(bw,msgs[error+MSGS]);
  if(error!= -1) ret= -1;
  }
 if(bw->b->count==1 && (bw->b->changed || bw->b->name))
  orphit(bw);
 bwrm(bw);
 w->object=(void *)(bw=bwmk(w,b,0));
 wredraw(bw->parent);
 bw->object=object;
 vsrm(s);
 return ret;
 }

/* Switch to next buffer in window */

int unbuf(bw)
BW *bw;
 {
 void *object=bw->object;
 W *w=bw->parent;
 B *b;
 if(bw->pid)
  {
  msgnw(bw,"Process running in this window");
  return -1;
  }
 b=bnext();
 if(b==bw->b) b=bnext();
 if(b==bw->b) return -1;
 if(!b->orphan) ++b->count;
 else b->orphan=0;
 if(bw->b->count==1) orphit(bw);
 bwrm(bw);
 w->object=(void *)(bw=bwmk(w,b,0));
 wredraw(bw->parent);
 bw->object=object;
 return 0;
 }

int upbuf(bw)
BW *bw;
 {
 void *object=bw->object;
 W *w=bw->parent;
 B *b;
 if(bw->pid)
  {
  msgnw(bw,"Process running in this window");
  return -1;
  }
 b=bprev();
 if(b==bw->b) b=bprev();
 if(b==bw->b) return -1;
 if(!b->orphan) ++b->count;
 else b->orphan=0;
 if(bw->b->count==1) orphit(bw);
 bwrm(bw);
 w->object=(void *)(bw=bwmk(w,b,0));
 wredraw(bw->parent);
 bw->object=object;
 return 0;
 }

int uinsf(bw)
BW *bw;
 {
 if(wmkpw(bw,
           "Name of file to insert (^C to abort): ",&filehist,doinsf,"Names",NULL,cmplt,NULL,NULL)) return 0;
 else return -1;
 }

/* Save and exit */

static int doex(bw,s,object,notify)
BW *bw;
char *s;
void *object;
int *notify;
 {
 bw->b->name=zdup(s); 
 if(dosave(bw,s,object,notify)) { free(bw->b->name); bw->b->name=0; return -1; }
 exmsg=vsncpy(NULL,0,sz("File "));
 exmsg=vsncpy(exmsg,sLEN(exmsg),sz(bw->b->name));
 exmsg=vsncpy(exmsg,sLEN(exmsg),sz(" saved."));
 bw->b->changed=0;
 saverr(bw->b->name);
 uabort(bw,MAXINT);
 return 0;
 }

int uexsve(bw)
BW *bw;
 {
 if(!bw->b->changed) { uabort(bw,MAXINT); return 0; }
 else if(bw->b->name && !exask)
  {
  if(dosave(bw,vsncpy(NULL,0,sz(bw->b->name)),NULL,NULL)) return -1;
  exmsg=vsncpy(NULL,0,sz("File "));
  exmsg=vsncpy(exmsg,sLEN(exmsg),sz(bw->b->name));
  exmsg=vsncpy(exmsg,sLEN(exmsg),sz(" saved."));
  bw->b->changed=0;
  saverr(bw->b->name);
  uabort(bw,MAXINT);
  return 0;
  } 
 else
  {
  BW *pbw=wmkpw(bw,
                "Name of file to save (^C to abort): ",&filehist,doex,"Names",NULL,cmplt,NULL,NULL);
  if(pbw && bw->b->name)
   {
   binss(pbw->cursor,bw->b->name);
   pset(pbw->cursor,pbw->b->eof);
   pbw->cursor->xcol=piscol(pbw->cursor);
   }
  if(pbw) return 0;
  else return -1;
  }
 }

/* If buffer is modified, prompt for saving */

static int donask(bw,s,object,notify)
BW *bw;
char *s;
void *object;
int *notify;
 {
 if(!bw->b->name) bw->b->name=zdup(s); 
 if(dosave(bw,s,object,notify)) { bw->b->name=0; return -1; }
 exmsg=vsncpy(NULL,0,sz("File "));
 exmsg=vsncpy(exmsg,sLEN(exmsg),sz(bw->b->name));
 exmsg=vsncpy(exmsg,sLEN(exmsg),sz(" saved."));
 bw->b->changed=0;
 saverr(bw->b->name);
 return 0;
 }

static int nask(bw,c,object,notify)
BW *bw;
void *object;
int *notify;
 {
 if(c=='y' || c=='Y')
  if(bw->b->name) return donask(bw,vsncpy(NULL,0,sz(bw->b->name)),object,notify);
  else
   {
   BW *pbw=wmkpw(bw,
                 "Name of file to save (^C to abort): ",&filehist,donask,"Names",NULL,cmplt,object,notify);
   if(pbw) return 0;
   else return -1;
   }
 else if(c=='n' || c=='N')
  {
  if(notify) *notify=1;
  return 0;
  }
 else
  if(bw->b->count==1 && bw->b->changed)
   {
   if(mkqw(bw,sc("Save changes to this file (y,n,^C)? "),nask,NULL,object,notify)) return 0;
   else return -1;
   }
  else
   {
   if(notify) *notify=1;
   return 0;
   }
 }

int uask(bw)
BW *bw;
 {
 return nask(bw,0,NULL,NULL);
 }

/* If buffer is modified, ask if it's ok to lose changes */

int dolose(bw,c,object,notify)
BW *bw;
void *object;
int *notify;
 {
 W *w;
 if(notify) *notify=1;
 if(c!='y' && c!='Y') return -1;
 if(bw->b->count==1) bw->b->changed=0;
 object=bw->object; w=bw->parent;
 bwrm(bw);
 w->object=(void *)(bw=bwmk(w,bfind("",0),0));
 wredraw(bw->parent);
 bw->object=object;
 return 0;
 }

int ulose(bw)
BW *bw;
 {
 if(bw->pid) return ukillpid(bw);
 if(bw->b->count==1 && bw->b->changed)
  if(mkqw(bw,sc("Lose changed to this file (y,n,^C)? "),dolose,NULL,NULL,NULL)) return 0;
  else return -1;
 else return dolose(bw,'y',NULL,NULL);
 }

/* Buffer list */

int dobuf(m,x,s)
MENU *m;
char **s;
 {
 char *name;
 BW *bw=m->parent->win->object;
 int *notify=m->parent->notify;
 m->parent->notify=0;
 name=vsdup(s[x]);
 uabort(m,MAXINT);
 return dorepl(bw,name,NULL,notify);
 }

int abrtb(m,x,s)
MENU *m;
char **s;
 {
 varm(s);
 return -1;
 }

int ubufed(bw)
BW *bw;
 {
 char **s=getbufs();
 vasort(av(s));
 if(mkmenu(bw,s,dobuf,abrtb,NULL,0,s,NULL)) return 0;
 else
  {
  varm(s);
  return -1;
  }
 }
