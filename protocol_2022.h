constexpr int PORT_NUM = 4000;
constexpr int BUF_SIZE = 200;
constexpr int NAME_SIZE = 20;
constexpr int CHAT_SIZE = 100;

constexpr int MAX_USER = 10000;
constexpr int MAX_NPC = 100000;				// 추후에 다시 200,000으로 변경

constexpr int W_WIDTH = 2000;
constexpr int W_HEIGHT = 2000;

constexpr int VIEW_RANGE = 15;
constexpr int SECTOR_CNT = 67;
constexpr int SECTOR_SIZE = 30;
constexpr int SECTOR_HALF = SECTOR_SIZE / 2;

constexpr int SYSTEM_CHAT_ID = -10;

// Packet ID
constexpr char CS_LOGIN = 0;
constexpr char CS_MOVE = 1;
constexpr char CS_CHAT = 2;
constexpr char CS_ATTACK = 3;			
constexpr char CS_TELEPORT = 4;			
constexpr char CS_LOGOUT = 5;			
constexpr char CS_INVITE_PARTY = 6;
constexpr char CS_JOIN_PARTY = 7;

constexpr char SC_LOGIN_INFO = 2;
constexpr char SC_ADD_OBJECT = 3;
constexpr char SC_REMOVE_OBJECT = 4;
constexpr char SC_MOVE_OBJECT = 5;
constexpr char SC_CHAT = 6;
constexpr char SC_LOGIN_OK = 7;
constexpr char SC_LOGIN_FAIL = 8;
constexpr char SC_STAT_CHANGE = 9;
constexpr char SC_ATTACK = 10;
constexpr char SC_PLAYER_DIE = 11;
constexpr char SC_INVITE_PARTY = 12;
constexpr char SC_JOIN_PARTY = 13;
constexpr char SC_EXIT_PARTY = 14;

#pragma pack (push, 1)
struct CS_LOGIN_PACKET {
	unsigned char size;
	char	type;
	char	name[NAME_SIZE];
};

struct CS_MOVE_PACKET {
	unsigned char size;
	char	type;
	char	direction;  // 0 : UP, 1 : DOWN, 2 : LEFT, 3 : RIGHT
	unsigned	move_time;
};

struct CS_CHAT_PACKET {
	unsigned char size;
	char	type;
	char	mess[CHAT_SIZE];
};

struct CS_ATTACK_PACKET {
	unsigned char size;
	char type;
};

struct CS_TELEPORT_PACKET {
	unsigned char size;
	char	type;
};

struct CS_LOGOUT_PACKET {
	unsigned char size;
	char	type;
};

struct CS_INVITE_PARTY_PACKET {
	unsigned char size;
	char type;
};

struct CS_JOIN_PARTY_PACKET {
	unsigned char size;
	char type;
	int id;
};

struct SC_LOGIN_INFO_PACKET {
	unsigned char size;
	char	type;
	int		id;
	int		hp;
	int		max_hp;
	int		exp;
	int		level;
	short	x, y;
};

struct SC_ADD_OBJECT_PACKET {
	unsigned char size;
	char	type;
	int		id;
	short	x, y;
	char	name[NAME_SIZE];
};

struct SC_REMOVE_OBJECT_PACKET {
	unsigned char size;
	char	type;
	int		id;
};

struct SC_MOVE_OBJECT_PACKET {
	unsigned char size;
	char	type;
	int		id;
	short	x, y;
	unsigned int move_time;
};

struct SC_CHAT_PACKET {
	unsigned char size;
	char	type;
	int		id;
	char	mess[CHAT_SIZE];
};

struct SC_LOGIN_OK_PACKET {
	unsigned char size;
	char	type;
};

struct SC_LOGIN_FAIL_PACKET {
	unsigned char size;
	char	type;

};

struct SC_STAT_CHANGE_PACKET {
	unsigned char size;
	char	type;
	int		id;
	int		hp;
	int		max_hp;
	int		exp;
	int		level;

};

struct SC_ATTACK_PACKET {
	unsigned char size;
	char type;
	int id;
};

struct SC_INVITE_PARTY_PACKET {
	unsigned char size;
	char type;
	int id;
};

struct SC_JOIN_PARTY_PACKET {
	unsigned char size;
	char type;
	int id;
};

struct SC_EXIT_PARTY_PACKET {
	unsigned char size;
	char type;
	int id;
};

struct SC_PLAYER_DIE_PACKET {
	unsigned char size;
	char type;
	int id;
};

#pragma pack (pop)