constexpr int PORT_NUM = 8111;
constexpr int BUF_SIZE = 200;
constexpr int NAME_SIZE = 20;

constexpr int W_WIDTH = 2000;
constexpr int W_HEIGHT = 2000;
constexpr int VIEW_RANGE = 15;

// Packet ID
constexpr char CS_LOGIN = 0;
constexpr char CS_MOVE = 1;
	
constexpr char SC_LOGIN_OK = 0;
constexpr char SC_LOGIN_FAIL = 1;
constexpr char SC_ADD_PLAYER = 2;
constexpr char SC_REMOVE_PLAYER = 3;
constexpr char SC_MOVE_PLAYER = 4;

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

struct SC_LOGIN_OK_PACKET {
	unsigned char size;
	char	type;
	short	id;
	short	x, y;
};

struct SC_ADD_PLAYER_PACKET {
	unsigned char size;
	char	type;
	short	id;
	short	x, y;
	char	name[NAME_SIZE];
};

struct SC_REMOVE_PLAYER_PACKET {
	unsigned char size;
	char	type;
	short	id;
};

struct SC_MOVE_PLAYER_PACKET {
	unsigned char size;
	char	type;
	short	id;
	short	x, y;
	unsigned move_time;
};

#pragma pack (pop)