#ifndef NET_H_
#define NET_H_

// ET/q3 related
#define NET_PROTOCOL "84"
#define NET_Q3MARKER "\xff\xff\xff\xff"
#define NET_STATUS_REQUEST (NET_Q3MARKER "getstatus")
#define NET_STATUS_RESPONSE (NET_Q3MARKER "statusResponse")
#define NET_STATUS_TIMEOUT 1750
#define NET_SERVERS_REQUEST (NET_Q3MARKER "getservers " NET_PROTOCOL)
#define NET_SERVERS_RESPONSE (NET_Q3MARKER "getserversResponse")
#define NET_SERVERS_RESPONSE_END "\\EOT"
#define NET_SERVERS_TIMEOUT 3000
#define NET_ET_MASTER_SERVER (server_t){ "etmaster.idsoftware.com", 27950 }

// irc related
#define IRC_NOTICE "NOTICE"
#define IRC_JOIN "JOIN"
#define IRC_NICK "NICK"
#define IRC_PART "PART"
#define IRC_QUIT "QUIT"
#define IRC_PING "PING "
#define IRC_PONG "PONG "
#define IRC_PRIVMSG "PRIVMSG"
#define IRC_REGISTRED "001"
#define IRC_LIST_NAMES "353"
#define IRC_TOPIC "332"
#define IRC_TOPIC_CHANGE "TOPIC"
#define IRC_TOPIC_SIZE 390 // Rizon
#define IRC_CTCP_VERSION "\1VERSION\1"
#define IRC_NICK_SIZE 30
#define IRC_RETRY_TIME 10	// Time in seconds to wait before retry a new connection

// irc gui
#define IRC_MAX_MSGS 7
#define IRC_MAX_NICK_LEN 30 // Rizon
#define IRC_MAX_MSG_LEN 400 // No need to set at real server's max length of msg as it won't be drawn anyway if it's too long.

#define IRC_MSGTYPE_CHAN 1
#define IRC_MSGTYPE_QUERY 2

typedef struct {
	char nick[IRC_MAX_NICK_LEN];
	char msg[IRC_MAX_MSG_LEN];
	int type;
} ircmsgs_t;

typedef struct {
	qboolean show;
	int lastCheck;
} ircMsgsGeneral_t;

extern ircMsgsGeneral_t ircMsgsGeneral;
extern ircmsgs_t ircMsgs[IRC_MAX_MSGS];

// irc bot
#define IRC_BOT_PREFIX "eth_"
#define IRC_BOT_STATUS "!status"
#define IRC_BOT_INVITE "!invite"

typedef int socket_t;

typedef struct server_s {
	char hostname[64];
	int port;
} server_t;

// socket helper functions
socket_t serverConnect(server_t *server, int socketType);
qboolean netSendText(socket_t socket, char *format, ...);

//socket helper even more :)
typedef enum sAction_s {
    SACTION_INIT,
    SACTION_SEND,
    SACTION_GET,
    SACTION_CLOSE
} sAction_t;

typedef struct netMsg_s {
    socket_t socket;
    sAction_t sLastAction; 
    int lastCallTime;
} netMsg_t;

// Set a random name
#define MAX_SERVERS 4096
#define MAX_NAMES 128
qboolean setRandomName();

// Get spectators
void getSpectators();

// irc functions
void ircNextEvent();
qboolean netSendIrcChannel(char *format, ...);
qboolean netSendIrcStatus();
void registerIrcCommands();
void acceptInvite(int slot);

// CG_INIT
void ircCGinit();

// all irc buddies on the channel
#define IRC_MAX_BUDDIES 25
typedef struct ircBuddy_s {
	qboolean infoValid;
	char nick[IRC_NICK_SIZE];
	int playerID;
	server_t server;
	char ftString[128];
	char etVersion[12];
	qboolean invite;
} ircBuddy_t;
extern ircBuddy_t ircBuddies[IRC_MAX_BUDDIES];

#endif /*NET_H_*/
