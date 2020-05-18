#include <stdio.h>
#include <stdint.h>
#include "include/usv_message.h"
#include "checksum.h"

static volatile uint32_t serial = 0;

static void msg_other_construct(char *buf, struct Message *msg);
static void msg_fill(struct Message *msg, int len, uint16_t destination,
		enum MCategory category, enum MCommand command);

unsigned msg_speed_construct(char *buf, uint16_t destination,
		int8_t lspeed, int8_t rspeed)
{
	struct Message msg;

	*(int8_t *)(msg.param) = lspeed;
	*(int8_t *)(msg.param+1) = rspeed;

	msg_fill(&msg, MESSAGE_MIN_LEN+2, destination,
			control, speed);
	msg_other_construct(buf, &msg);
	return msg.length;
}
int msg_speed_get(const struct Message *msg, int *lspeed, int *rspeed)
{
	if (msg->length != MESSAGE_MIN_LEN+2)
		return -1;

	*lspeed = *(int8_t *)(msg->param);
	*rspeed = *(int8_t *)(msg->param+1);
	return 0;
}

unsigned msg_beep_construct(char *buf, uint16_t destination, int8_t in_beep)
{
	struct Message msg;

	*(int8_t *)(msg.param) = in_beep;

	msg_fill(&msg, MESSAGE_MIN_LEN+1, destination,
			control, beep);
	msg_other_construct(buf, &msg);
	return msg.length;
}
int msg_beep_get(const struct Message *msg, int *beep)
{
	if (msg->length != MESSAGE_MIN_LEN + 1)
		return -1;

	*beep = *(int8_t *)(msg->param);
	return 0;
}

unsigned msg_salvage_construct(char *buf, uint16_t destination)
{
	struct Message msg;

	msg_fill(&msg, MESSAGE_MIN_LEN, destination,
			control, salvage);
	msg_other_construct(buf, &msg);
	return msg.length;
}

unsigned msg_return_construct(char *buf, uint16_t destination,
		uint8_t status)
{
	struct Message msg;

	msg.param[0] = status;

	msg_fill(&msg, MESSAGE_MIN_LEN+1, destination,
			control, auto_return);
	msg_other_construct(buf, &msg);
	return msg.length;
}
int msg_return_get(const struct Message *msg, int *status)
{
	if (msg->length != MESSAGE_MIN_LEN+1)
		return -1;

	*status = msg->param[0];
	return 0;
}

unsigned msg_avoid_construct(char *buf, uint16_t destination,
		uint8_t status)
{
	struct Message msg;

	msg.param[0] = status;

	msg_fill(&msg, MESSAGE_MIN_LEN+1, destination,
			control, auto_avoid);
	msg_other_construct(buf, &msg);
	return msg.length;
}
int msg_avoid_get(const struct Message *msg, int *status)
{
	if (msg->length != MESSAGE_MIN_LEN+1)
		return -1;

	*status = msg->param[0];
	return 0;
}

unsigned msg_go_dest_construct(char *buf, uint16_t destination,
		float latitude, float longtitude)
{
	struct Message msg;

	*(float *)(msg.param) = latitude;
	*(float *)(msg.param+4) = longtitude;

	msg_fill(&msg, MESSAGE_MIN_LEN+8, destination,
			control, go_dest);
	msg_other_construct(buf, &msg);
	return msg.length;
}
int msg_go_dest_get(const struct Message *msg,
		float *latitude, float *longtitude)
{
	if (msg->length != MESSAGE_LEN+8)
		return -1;

	*latitude = *(float *)(msg->param);
	*longtitude = *(float *)(msg->param+4);
	return 0;
}

unsigned msg_cruise_construct(char *buf,
		uint16_t destination, int gps_num, struct GPS gps[])
{
	int i;
	struct Message msg;

	if (MESSAGE_PARAM_LEN < gps_num * 8)
		return 0;

	for (i = 0; i < gps_num; i++) {
		*(float *)(msg.param+i*8) = gps[i].latitude;
		*(float *)(msg.param+i*8+4) = gps[i].longtitude;
	}

	msg_fill(&msg, MESSAGE_MIN_LEN+gps_num*8, destination,
			control, cruise);
	msg_other_construct(buf, &msg);
	return msg.length;
}
int msg_cruise_get(const struct Message *msg, int *gps_num, struct GPS gps[])
{
	int i;
	
	if ((msg->length - MESSAGE_MIN_LEN) % 8 != 0)
		return -1;

	*gps_num = (msg->length - MESSAGE_MIN_LEN) / 8;
	for (i = 0; i < (msg->length - MESSAGE_MIN_LEN) % 8; i++) {
		gps[i].latitude = *(float *)(msg->param+i*8);
		gps[i].longtitude = *(float *)(msg->param+i*8+4);
	}
	return 0;
}

unsigned msg_hover_construct(char *buf, uint16_t destination)
{
	struct Message msg;

	msg_fill(&msg, MESSAGE_MIN_LEN, destination,
			control, hover);
	msg_other_construct(buf, &msg);
	return msg.length;
}

unsigned msg_feedback_construct(char *buf, uint16_t destination,
		enum MCommand command, const struct Message *get_msg)
{
	struct Message msg;

	*(uint32_t *)(msg.param) = get_msg->serial;

	msg_fill(&msg, MESSAGE_MIN_LEN+4, destination,
			feedback, command);
	msg_other_construct(buf, &msg);
	return msg.length;
}
int msg_feedback_get(const struct Message *msg, uint32_t *serial)
{
	if (msg->length != MESSAGE_MIN_LEN+4)
		return -1;

	*serial = *(uint32_t *)(msg->param);
	return 0;
}

unsigned msg_gps_construct(char *buf, uint16_t destination, struct GPS post_gps)
{
	struct Message msg;

	*(float *)(msg.param) = post_gps.latitude;
	*(float *)(msg.param+4) = post_gps.longtitude;
	*(float *)(msg.param+8) = post_gps.altitude;
	*(float *)(msg.param+12) = post_gps.speed;

	msg_fill(&msg, MESSAGE_MIN_LEN+16, destination,
			data, gps);
	msg_other_construct(buf, &msg);
	return msg.length;
}
int msg_gps_get(const struct Message *msg, struct GPS *gps)
{
	if (msg->length != MESSAGE_MIN_LEN+16)
		return -1;

	gps->latitude = *(float *)(msg->param);
	gps->longtitude = *(float *)(msg->param+4);
	gps->altitude = *(float *)(msg->param+8);
	gps->speed = *(float *)(msg->param+12);
	return 0;
}

unsigned msg_imu_construct(char *buf, uint16_t destination, struct IMU post_imu)
{
	struct Message msg;

	*(float *)(msg.param) = post_imu.roll;
	*(float *)(msg.param+4) = post_imu.pitch;
	*(float *)(msg.param+8) = post_imu.yaw;

	msg_fill(&msg, MESSAGE_MIN_LEN+12, destination,
			data, imu);
	msg_other_construct(buf, &msg);
	return msg.length;
}
int msg_imu_get(const struct Message *msg, struct IMU *imu)
{
	if (msg->length != MESSAGE_MIN_LEN+12)
		return -1;

	imu->roll = *(float *)(msg->param);
	imu->pitch = *(float *)(msg->param+4);
	imu->yaw = *(float *)(msg->param+8);
	return 0;
}

unsigned msg_ultrasonic_construct(char *buf, uint16_t destination,
		struct Ultrasonic ultra)
{
	struct Message msg;

	*(int32_t *)(msg.param) = ultra.front;
	*(int32_t *)(msg.param+4) = ultra.left;
	*(int32_t *)(msg.param+8) = ultra.right;

	msg_fill(&msg, MESSAGE_MIN_LEN+12, destination,
			data, ultrasonic);
	msg_other_construct(buf, &msg);
	return msg.length;
}
int msg_ultrasonic_get(const struct Message *msg, struct Ultrasonic *ultra)
{
	if (msg->length != MESSAGE_MIN_LEN+12)
		return -1;

	ultra->front = *(int32_t *)(msg->param);
	ultra->left = *(int32_t *)(msg->param+4);
	ultra->right = *(int32_t *)(msg->param+8);
	return 0;
}

unsigned msg_battery_construct(char *buf, uint16_t destination, float in_battery)
{
	struct Message msg;

	*(float *)(msg.param) = in_battery;

	msg_fill(&msg, MESSAGE_MIN_LEN+4, destination,
			data, battery);
	msg_other_construct(buf, &msg);
	return msg.length;
}
int msg_battery_get(const struct Message *msg, float *battery)
{
	if (msg->length != MESSAGE_MIN_LEN+4)
		return -1;

	*battery = *(float *)(msg->param);
	return 0;
}

unsigned msg_connect_construct(char *buf, uint16_t destination)
{
	struct Message msg;

	msg_fill(&msg, MESSAGE_MIN_LEN, destination,
			data, connect);
	msg_other_construct(buf, &msg);
	return msg.length;
}


static void msg_fill(struct Message *msg, int len, uint16_t destination,
		enum MCategory category, enum MCommand command)
{
	msg->serial = serial++;
	msg->source = MESSAGE_SOURCE;

	msg->length = len;
	msg->destination = destination;
	msg->category = category;
	msg->command = command;
}

static void msg_other_construct(char *buf, struct Message *msg)
{
	int i;

	*(uint32_t *)(buf) = msg->serial;
	*(int32_t *)(buf+4) = msg->length;
	*(buf+8) = 'S';
	*(uint16_t *)(buf+9) = msg->source;
	*(buf+11) = 'D';
	*(uint16_t *)(buf+12) = msg->destination;
	*(uint8_t *)(buf+14) = msg->category;
	*(uint8_t *)(buf+15) = msg->command;
	/* 复制参数 */
	for (i = 0; i < msg->length-MESSAGE_MIN_LEN; i++)
		buf[16+i] = msg->param[i];

	msg->crc = crc_16(buf, msg->length-3);
	*(uint16_t *)(buf+(msg->length-MESSAGE_MIN_LEN+16)) = msg->crc;
	buf[msg->length-1] = 0xFF;
}
