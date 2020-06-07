#define ETH_MAX_WINDOWS 24 // increase it if needed 
#define ETH_MAX_WNAME 64
//window props.
typedef struct eth_window_s {
    int posX; //coords X
    int posY; // y
    int windowW; //width
    int windowH; //height
    int diffx; //difference of mouseX to posX when sticked
    int diffy; //difference of mouseY to posY when sticked
    int isMouseOn;
    int isSticked;
    int isInit; // we must create it first
    vec4_t bgColor;
    vec4_t defBgColor;
} eth_window_t;

// windows list - used to load/save positions etc.
typedef struct eth_winlist_s {
    char winName[ETH_MAX_WNAME]; //name in cfg file
    int winCoords[2]; //x,y
    int winSize[2]; //w,h
    float bgColor[4]; //rgba
    eth_window_t *window;
} eth_winlist_t;

eth_winlist_t eth_winlist[ETH_MAX_WINDOWS];

