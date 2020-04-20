#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <stdlib.h>
#include "msg_type.h"

static void self();
static void pressure();

int main(int argc, char *argv[])
{
	srand(time(NULL));

	if (argc < 2) {
		printf("add command\n");
		printf("- self\n");
		printf("- pressure\n");
		return 0;
	}

	if (strcmp(argv[1], "self") == 0)
		self();
	else if (strcmp(argv[1], "pressure") == 0)
		pressure();

	return 0;
}

/* 自测 */
static void self()
{
	int i, j, k, x;
	char buf[MESSAGE_LEN];
	struct Message msg;
	int start, len;
	int res;
	int param1, param2;

	/* speed */
	printf("speed self test start.\n");
	for (i = 0; i < 30; i++)
	for (j = 0; j <= 100; j++)
	for (k = 0; k <= 100; k++) {
		msg_speed_construct(buf+i, 1, j, k);

		x = 0;
		while ((res = msg_parsing(buf+x+i, MESSAGE_LEN-x, &start, &len)) != 0)
			if (res > 0) {
				if (msg_extract(buf+x+i, len, &msg) == 0) {
					msg_speed_get(&msg, &param1, &param2);
					assert(param1 == j);
					assert(param2 == k);
					break;
				}
				k += res;
			}
			else
				k -= res;
	}
	printf("Speed self test pass.\n");

	/* beep */
	printf("Beep self test start.\n");
	for (i = 0; i < 30; i++)
	for (j = 0; j <= 5; j++) {
		msg_beep_construct(buf+i, 1, j);

		x = 0;
		while ((res = msg_parsing(buf+x+i, MESSAGE_LEN-x, &start, &len)) != 0) {
			if (msg_extract(buf+x+i, len, &msg) == 0) {
				msg_beep_get(&msg, &param1);
				assert(param1 == j);
				break;
			}
			k += (res > 0) ? res : -res;
		}
	}
	printf("Beep self test pass.\n");

}

/* 压力测试 */
static void pressure()
{
	int i, j, k, t;
	char buf[MESSAGE_LEN];
	struct Message msg;
	int start, len;
	int crc_check = 0;

	printf("Start parsing test.\n");
	for (i = 0; i < 50; i++)
		/* Bad parsing test */
		for (j = i; j < i+20; j++) {
			msg_speed_construct(buf+i, 1, 0, 100);
			while ((t = (char)rand()) == buf[j])
				;
			buf[j] = t;

			k = 0;
			while ((t = msg_parsing(buf+k, MESSAGE_LEN-k, &start, &len)) != 0) {
				if (t > 0)
					break;
				else
					k -= t;
			}

			if (t > 0) {
				t = msg_extract(buf+start+k, len, &msg);
				if (t != 0)
					++crc_check;
			}
		}

	printf("Crc not check time: %d\n", crc_check);
	printf("Crc check err rage: %lf\n", 1 - crc_check / 1000.0);
	printf("pass\n");
}
