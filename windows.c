#include"eth.h"

//initWindow - used to load/save window props
void initWindow(const char *wname,int x,int y,int w,int h,float r,float g,float b,float a) {
    int wlnum = 0;
    while (wlnum < ETH_MAX_WINDOWS) {
	if (strlen(eth_winlist[wlnum].winName) < 1) {
	    eth_winlist[wlnum].winCoords[0] = x;
	    eth_winlist[wlnum].winCoords[1] = y;
	    eth_winlist[wlnum].winSize[0] = w;
	    eth_winlist[wlnum].winSize[0] = h;
	    eth_winlist[wlnum].bgColor[0] = r;
	    eth_winlist[wlnum].bgColor[1] = g;
	    eth_winlist[wlnum].bgColor[2] = b;
	    eth_winlist[wlnum].bgColor[3] = a;
	    strcat(eth_winlist[wlnum].winName,wname);
	    return;
	}
	wlnum++;
    }
    printf("eth:initWindow(): too many windows initialized\n");
    exit(-1);
}

//createWindow - used to create in-game window
void createWindow (eth_window_t *win, const char *wname) {
    if(win->isInit) // if initialized just drop this action
	return;
    
    int wnum = 0;    
    while (wnum < ETH_MAX_WINDOWS) {
	if (eth_winlist[wnum].window == NULL) {
	    if(!strncmp(wname,eth_winlist[wnum].winName,strlen(eth_winlist[wnum].winName))) {
		win->posX = eth_winlist[wnum].winCoords[0];
		win->posY = eth_winlist[wnum].winCoords[1];
		win->windowW = eth_winlist[wnum].winSize[0];
		win->windowH = eth_winlist[wnum].winSize[1];
		memcpy(win->bgColor,&eth_winlist[wnum].bgColor,sizeof(vec4_t));
		memcpy(win->defBgColor,&eth_winlist[wnum].bgColor,sizeof(vec4_t));
		win->isInit = 1;
		eth_winlist[wnum].window = win;
		return;	// sucessfuly created so get back
	    }
	}
	wnum++;
    }
    printf("eth:createWindow(): unknown window %s\n",wname);
    exit(-1);
}
		
//getWindowStatus - used to manage in-game window
// - check if mouse pointer activated
// - check if any other window in use is
// - check for mouse coords but not when sticked to pointer
// - drag/drop
void getWindowStatus(eth_window_t *window) {

 if (eth.pointer) {

    if (!window->isSticked && eth.winInUse)
	return;
	
    if (!window->isSticked) {
	if (eth.mouseX > window->posX && eth.mouseX < (window->posX + window->windowW) && eth.mouseY > window->posY && eth.mouseY < (window->posY + window->windowH))
	    window->isMouseOn = 1;
	else 
	    window->isMouseOn = 0;
    }

    memcpy(window->bgColor,window->defBgColor,sizeof(vec4_t));

    if(window->isMouseOn)
	window->bgColor[3]=1.0f;

    if ((window->isMouseOn && eth.mouseButton == Button1) || window->isSticked) {
	if(!window->isSticked) {
	    window->diffx = eth.mouseX - window->posX;
	    window->diffy = eth.mouseY - window->posY;
	    window->isSticked = 1;
	}
	window->posX = eth.mouseX - window->diffx;
	window->posY = eth.mouseY - window->diffy;
	eth.winInUse = 1;
    } 
    if (window->isSticked && (eth.mouseButton == Button2 || eth.mouseButton == Button3)) {
	window->diffx = 0;
	window->diffy = 0;
	window->isSticked = 0;
	eth.winInUse = 0;
    } 
    
    if(window->isSticked) {
	int i=0;	
	for(; i < 4 ; i++)
	    window->bgColor[i]+=0.5f;
    }
 
 }

}
