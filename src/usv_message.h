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
#else
#  if MESSAGE_LEN > MESSAGE_MIN_LIN
#    define MESSAGE_PARAM_LEN (MESSAGE_LEN - MESSAGE_MIN_LEN)
#  else
#    undef MESSAGE_LEN
#    define MESSAGE_LEN MESSAGE_MIN_LEN
#    define MESSAGE_PARAM_LEN 0
#  endif
#endif  // MESSAGE_LEN

#define GPS_NUM ((int)(MESSAGE_PARAM_LEN / 8))

// 报文枚举
enum MCategory
{
	control = 1,
	feedback,
	data
};
enum MCommand
{
	// control-有返回报文
	speed = 1,	// usv-427
	salvage,
	beep,
	auto_return,	// usv-host
	auto_avoid,
	auto_salvage,
	go_dest,
	cruise,
	hover,
	background,
	// return
	succeed = 50,
	failure,
	// data-无返回报文
	gps = 100,
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
    int32_t front;
    int32_t left;
    int32_t right;
};

enum USVStatus {
	status_hover, status_motor_control, status_go_dest, status_cruise
};

/* 解析报文 */
int msg_parsing(const char *buf, int buf_len, int *msg_start, int *msg_length);
int msg_extract(const char *buf, int msg_length, struct Message *msg);

/* 构造报文-获取报文信息 */
/* speed */
unsigned msg_speed_construct(char *buf, uint16_t destination,
		int8_t lspeed, int8_t rspeed);
int msg_speed_get(const struct Message *msg, int *lspeed, int *rspeed);
/* beep */
unsigned msg_beep_construct(char *buf, uint16_t destination, int8_t beep);
int msg_beep_get(const struct Message *msg, int *beep);
/* salvage */
unsigned msg_salvage_construct(char *buf, uint16_t destination);
/* auto_return */
unsigned msg_return_construct(char *buf, uint16_t destination,
		uint8_t status);
int msg_return_get(const struct Message *msg, int *status);
/* auto_avoid */
unsigned msg_avoid_construct(char *buf, uint16_t destination, uint8_t status);
int msg_avoid_get(const struct Message *msg, int *status);
/* go_dest */
unsigned msg_go_dest_construct(char *buf, uint16_t destination,
		float latitude, float longtitude);
int msg_go_dest_get(const struct Message *msg,
		float *latitude, float *longtitude);
/* cruise */
unsigned msg_cruise_construct(char *buf,
		uint16_t destination, int gps_num, struct GPS gps[]);
int msg_cruise_get(const struct Message *msg, int *gps_num, struct GPS gps[]);
/* hover */
unsigned msg_hover_construct(char *buf, uint16_t destination);
/* background */
unsigned msg_background_construct(char *buf, uint16_t destination, int8_t flag);
int msg_background_get(const struct Message *msg, int *flag);

/* return */
unsigned msg_feedback_construct(char *buf, uint16_t destination,
		enum MCommand command, const struct Message *get_msg);
int msg_feedback_get(const struct Message *msg, uint32_t *serial);

/* gps */
unsigned msg_gps_construct(char *buf, uint16_t destination, struct GPS gps);
int msg_gps_get(const struct Message *msg, struct GPS *gps);
/* imu */
unsigned msg_imu_construct(char *buf, uint16_t destination, struct IMU imu);
int msg_imu_get(const struct Message *msg, struct IMU *imu);
/* ultrasonic */
unsigned msg_ultrasonic_construct(char *buf, uint16_t destination,
		struct Ultrasonic ultra);
int msg_ultrasonic_get(const struct Message *msg, struct Ultrasonic *ultra);
/* battery */
unsigned msg_battery_construct(char *buf, uint16_t destination, float battery);
int msg_battery_get(const struct Message *msg, float *battery);
/* connect */
unsigned msg_connect_construct(char *buf, uint16_t destination);
/* status */
unsigned msg_status_construct(char *buf, uint16_t destination, enum USVStatus status,
		uint8_t auto_return, uint8_t auto_avoid);
int msg_status_get(const struct Message *msg, enum USVStatus *status,
		int *auto_return, int *auto_avoid);
/* auto salvage */
unsigned msg_auto_salvage_construct(char *buf, uint16_t destination, uint8_t status);
int msg_auto_salvage_get(const struct Message *msg, int *status);

#ifdef __cplusplus
}
#endif  // __cplusplus
#endif  // MSGTYPE_H
