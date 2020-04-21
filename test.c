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
		while ((res = msg_parsing(buf+x, MESSAGE_LEN-x, &start, &len)) != 0) {
			if (res > 0 && msg_extract(buf+x+start, len, &msg) == 0) {
				msg_speed_get(&msg, &param1, &param2);
				assert(param1 == j);
				assert(param2 == k);
				break;
			}
			x += (res > 0) ? res : -res;
		}
	}
	printf("Speed self test pass.\n");

	/* beep */
	printf("Beep self test start.\n");
	for (i = 0; i < 30; i++)
	for (j = 0; j <= 5; j++) {
		msg_beep_construct(buf+i, 1, j);

		x = 0;
		while ((res = msg_parsing(buf+x, MESSAGE_LEN-x, &start, &len)) != 0) {
			if (res > 0 && msg_extract(buf+x+start, len, &msg) == 0) {
				msg_beep_get(&msg, &param1);
				assert(param1 == j);
				break;
			}
			x += (res > 0) ? res : -res;
		}
	}
	printf("Beep self test pass.\n");

	/* salvage */
	printf("Salvage self test start.\n");
	for (i = 0; i < 30; i++) {
		msg_salvage_construct(buf+i, 0);

		x = 0;
		int flag = 0;
		while ((res = msg_parsing(buf+x, MESSAGE_LEN-x, &start, &len)) != 0) {
			if (res > 0 && msg_extract(buf+i+x, len, &msg) == 0)
				flag = 1;
			x += (res > 0) ? res : -res;
		}
		assert(flag);
	}
	printf("Salvage self test pass.\n");

	/* auto_return */
	printf("Auto return self test start.\n");
	for (i = 0; i < 30; i++)
	for (j = 0; j < 2; j++) {
		msg_return_construct(buf+i, 2, j);

		x = 0;
		while ((res = msg_parsing(buf+x, MESSAGE_LEN-x, &start, &len)) != 0) {
			if (res > 0 && msg_extract(buf+x+start, len, &msg) == 0) {
				msg_return_get(&msg, &param1);
				assert(param1 == j);
				break;
			}
			x += (res > 0) ? res : -res;
		}
	}
	printf("Auto return self test pass.\n");

	/* auto_avoid */
	printf("Auto avoid self test start.\n");
	for (i = 0; i < 30; i++)
	for (j = 0; j < 2; j++) {
		msg_avoid_construct(buf+i, 2, j);

		x = 0;
		while ((res = msg_parsing(buf+x, MESSAGE_LEN-x, &start, &len)) != 0) {
			if (res > 0 && msg_extract(buf+x+start, len, &msg) == 0) {
				msg_avoid_get(&msg, &param1);
				assert(param1 == j);
				break;
			}
			x += (res > 0) ? res : -res;
		}
	}
	printf("Auto avoid self test pass.\n");

	struct GPS gps[GPS_NUM];
	float jj, kk;
	/* go_dest */
	printf("Go dest self test start.\n");
	for (i = 0; i < 30; i++)
	for (jj = 0; jj < 50; jj += 0.3)
	for (kk = 50; kk < 100; kk += 0.3) {
		msg_go_dest_construct(buf+i, 1, jj, kk);

		x = 0;
		while ((res = msg_parsing(buf+x, MESSAGE_LEN-x, &start, &len)) != 0) {
			if (res > 0 && msg_extract(buf+x+start, len, &msg) == 0) {
				msg_go_dest_get(&msg, &gps[0].latitude, &gps[0].longtitude);
				assert(gps[0].latitude - jj < 0.000001);
				assert(gps[0].longtitude - kk < 0x000001);
				break;
			}
			x += (res > 0) ? res : -res;
		}
	}
	printf("Go dest self test pass.\n");

	/* cruise */
	printf("Cruise self test start.\n");
	for (i = 0; i < 5; i++)
	for (j = 0; j < GPS_NUM; j++)
	for (jj = 0; jj < 50; jj += 0.3)
	for (kk = 50; kk < 100; kk += 0.3) {
		gps[0].latitude = jj;
		gps[0].longtitude = kk;
		msg_cruise_construct(buf+i, 1, j, gps);

		x = 0;
		int z;
		while ((res = msg_parsing(buf+x, MESSAGE_LEN-x, &start, &len)) != 0) {
			if (res > 0 && msg_extract(buf+x+start, len, &msg) == 0) {
				msg_cruise_get(&msg, &k, gps);
				assert(k == j);
				assert(gps[0].latitude - jj < 0.000001);
				assert(gps[0].longtitude - kk < 0.000001);
				break;
			}
			x += (res > 0) ? res : -res;
		}
	}
	printf("Cruise self test pass.\n");

	/* hover */
	printf("Hover self test start.\n");
	for (i = 0; i < 30; i++) {
		msg_hover_construct(buf+i, 1);
		
		x = 0;
		int flag = 0;
		while ((res = msg_parsing(buf+x, MESSAGE_LEN-x, &start, &len)) != 0) {
			if (res > 0 && msg_extract(buf+x+start, len, &msg) == 0)
				flag = 1;
			x += (res > 0) ? res : -res;
		}
		assert(flag);
	}
	printf("Hover self test pass.\n");
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
