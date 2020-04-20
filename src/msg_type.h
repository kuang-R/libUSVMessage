#ifndef MSGTYPE_H
#define MSGTYPE_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define MESSAGE_MIN_LEN 19
#ifndef MESSAGE_SOURCE
# define MESSAGE_SOURCE 2
#endif  // MESSAGE_SOURCE
#ifndef MESSAGE_LEN
#  define MESSAGE_LEN (64 + MESSAGE_MIN_LEN)
#  define MESSAGE_PARAM_LEN 64
#elif
#  define MESSAGE_PARAM_LEN (MESSAGE_LEN - MESSAGE_MIN_LEN)
#endif  // MESSAGE_LEN

// 报文枚举
enum MCategory
{
	control = 1,
	rtn,
	data
};
enum MCommand {
	// control-有返回报文
	speed = 1,	// usv-427
	salvage,
	beep,
	auto_return,	// usv-host
	auto_advoid,
	go_dest,
	cruise,
	// return
	succeed,
	failure,
	// data-无返回报文
	gps,
	imu,
	ultrasonic,
	battery,
	connect,	// usv-host
	status
};

// 报文
struct Message
{
	uint32_t serial;
	int32_t length;
	uint16_t source;
	uint16_t destination;
	enum MCategory category;
	enum MCommand command;
	char param[MESSAGE_PARAM_LEN];
	uint16_t crc;
};

// 数据
struct IMU
{
    float roll;
    float pitch;
    float yaw;
};
struct GPS
{
    float latitude;
    float longtitude;
    float altitude;
    float speed;
};
struct Ultrasonic
{
    int front;
    int left;
    int right;
};

int msg_parsing(char *buf, int buf_len, int *msg_start, int *msg_length);
int msg_extract(char *buf, int msg_length, struct Message *msg);

void msg_speed_construct(char *buf, uint16_t destination
		, int8_t lspeed, int8_t rspeed);
int msg_speed_get(struct Message *msg, int *lspeed, int *rspeed);
void msg_beep_construct(char *buf, uint16_t destination, int8_t beep);
int msg_beep_get(struct Message *msg, int *beep);

#ifdef __cplusplus
}
#endif  // __cplusplus
#endif  // MSGTYPE_H
