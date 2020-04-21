#include <stdint.h>
#include "msg_type.h"
#include "checksum.h"

static uint32_t serial = 0;
static void msg_other_construct(char *buf, struct Message *msg);
void msg_speed_construct(char *buf, uint16_t destination, int8_t lspeed, int8_t rspeed);

void msg_speed_construct(char *buf, uint16_t destination, int8_t lspeed, int8_t rspeed)
{
	struct Message msg;

	msg.length = MESSAGE_MIN_LEN + 2;
	msg.destination = destination;
	msg.category = control;
	msg.command = speed;

	*(int8_t *)(msg.param) = lspeed;
	*(int8_t *)(msg.param+1) = rspeed;

	msg_other_construct(buf, &msg);
}
int msg_speed_get(struct Message *msg, int *lspeed, int *rspeed)
{
	if (msg->length != MESSAGE_MIN_LEN+2)
		return -1;

	*lspeed = *(int8_t *)(msg->param);
	*rspeed = *(int8_t *)(msg->param+1);

	return 0;
}

void msg_beep_construct(char *buf, uint16_t destination, int8_t beep)
{
	struct Message msg;

	msg.length = MESSAGE_MIN_LEN + 1;
	msg.destination = destination;
	msg.category = control;
	msg.command = beep;

	*(int8_t *)(msg.param) = beep;

	msg_other_construct(buf, &msg);
}
int msg_beep_get(struct Message *msg, int *beep)
{
	if (msg->length != MESSAGE_MIN_LEN + 1)
		return -1;

	*beep = *(int8_t *)(msg->param);

	return 0;
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
