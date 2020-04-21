#include <stdio.h>
#include <stdint.h>
#include "msg_type.h"
#include "checksum.h"

static uint32_t serial = 0;
static void msg_other_construct(char *buf, struct Message *msg);
static void msg_fill(struct Message *msg, int len, int destination,
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
int msg_speed_get(struct Message *msg, int *lspeed, int *rspeed)
{
	if (msg->length != MESSAGE_MIN_LEN+2)
		return -1;

	*lspeed = *(int8_t *)(msg->param);
	*rspeed = *(int8_t *)(msg->param+1);
	return 0;
}

unsigned msg_beep_construct(char *buf, uint16_t destination, int8_t beep)
{
	struct Message msg;

	*(int8_t *)(msg.param) = beep;

	msg_fill(&msg, MESSAGE_MIN_LEN+1, destination,
			control, beep);
	msg_other_construct(buf, &msg);
	return msg.length;
}
int msg_beep_get(struct Message *msg, int *beep)
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
int msg_return_get(struct Message *msg, int *status)
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
int msg_avoid_get(struct Message *msg, int *status)
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
int msg_go_dest_get(struct Message *msg,
		float *latitude, float *longtitude)
{
	if (msg->length != MESSAGE_LEN+8)
		return -1;

	*latitude = *(float *)(msg->param);
	*longtitude = *(float *)(msg->param+4);
	return 0;
}

unsigned msg_cruise_construct(char *buf,
		int destination, int gps_num, struct GPS gps[])
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
int msg_cruise_get(struct Message *msg, int *gps_num, struct GPS gps[])
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

unsigned msg_hover_construct(char *buf, int destination)
{
	struct Message msg;

	msg_fill(&msg, MESSAGE_MIN_LEN, destination,
			control, hover);
	msg_other_construct(buf, &msg);
	return msg.length;
}

static void msg_fill(struct Message *msg, int len, int destination,
		enum MCategory category, enum MCommand command)
{
	msg->length = len;
	msg->destination = destination;
	msg->category = category;
	msg->command = command;
}

static void msg_other_construct(char *buf, struct Message *msg)
{
	int i;

	msg->serial = serial++;
	msg->source = MESSAGE_SOURCE;

	*(uint32_t *)(buf) = msg->serial;
	*(int32_t *)(buf+4) = msg->length;
	*(buf+8) = 'S';
	*(uint16_t *)(buf+9) = msg->source;
	*(buf+11) = 'D';
	*(uint16_t *)(buf+12) = msg->destination;
	*(enum MCategory *)(buf+14) = msg->category;
	*(enum MCommand *)(buf+15) = msg->command;
	/* 复制参数 */
	for (i = 0; i < msg->length-MESSAGE_MIN_LEN; i++)
		buf[16+i] = msg->param[i];

	msg->crc = crc_16(buf, msg->length-3);
	*(uint16_t *)(buf+(msg->length-MESSAGE_MIN_LEN+16)) = msg->crc;
	buf[msg->length-1] = 0xFF;
}
