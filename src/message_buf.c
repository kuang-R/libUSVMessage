#include <stdint.h>
#include <stdlib.h>
#include "include/usv_message.h"
#include "checksum.h"

static const char *msg_search(const char *buf, int ch, int buf_len);

/* 判断有没有报文在缓冲区内
 * msg_start 报文开始位置
 * msg_length 报文长度
 * 0与负数表示没有报文，正数表示有报文
 * 返回值绝对值为出队列字节数 
 */
int msg_parsing(const char *buf, int buf_len, int *msg_start, int *msg_length)
{
	/* 分别表示头位置，长度位置，S字符位置，D字符位置 */
	const char *Hpos, *Lpos, *Spos, *Dpos;

	if (buf_len < MESSAGE_MIN_LEN) {
		return 0;
	}
	/* 查找S字符 */
	if ((Spos = msg_search(buf+8, 'S', buf_len-8)) == NULL) {
		return -(buf_len - 7);
	}
	Hpos = Spos - 8;
	Lpos = Spos - 4;
	/* 设置开始位置和长度 */
	*msg_start = Hpos - buf;
	*msg_length = *(int32_t *)(Lpos);
	/* 检查报文长度 */
	if (*msg_length > MESSAGE_LEN || *msg_length < MESSAGE_MIN_LEN) {
		return -1;
	}
	/* 检查缓冲区长度 */
	if (*msg_length > (buf_len - (Hpos-buf))) {
		return 0;
	}
	/* 检查D字符 */
	Dpos = Spos + 3;
	if (*Dpos != 'D') {
		return -1;
	}
	/* 检查END标识 */
	if ((uint8_t)Hpos[*msg_length-1] != 0xFF) {
		return -(Hpos - buf) - 1;
	}

	return Hpos - buf + *msg_length;
}

/* 提取报文
 * 成功返回0,crc检查失败返回-1
 */
int msg_extract(const char *buf, int msg_length, struct Message *msg)
{
	int i;
	uint16_t real_crc;

	/* check crc */
	if (0 != crc_16(buf, msg_length-1))
		return -1;

	msg->serial = *(uint32_t *)(buf);
	msg->length = *(int32_t *)(buf+4);
	msg->source = *(uint16_t *)(buf+9);
	msg->destination = *(uint16_t *)(buf+12);
	msg->category = *(uint8_t *)(buf+14);
	msg->command = *(uint8_t *)(buf+15);

	for (i = 0; i < msg_length; i++)
		msg->param[i] = buf[16+i];

	return 0;
}

static const char *msg_search(const char *buf, int ch, int buf_len)
{
	int i;

	for (i = 0; i < buf_len; i++)
		if (buf[i] == ch)
			return buf + i;
	return NULL;
}
