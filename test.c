#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <stdlib.h>
#include "msg_type.h"

#define CHECK_TIME 10000000

static void self();
static void crc_check();
static void self_control();
static void self_rtn();
static void self_data();
static void pressure();
static void rtn_test(enum MCommand rtn_command, enum MCommand get_command);

int main(int argc, char *argv[])
{
	srand(time(NULL));

	if (argc < 2) {
		printf("add command\n");
		printf("- crc\n");
		printf("- self\n");
		printf("- pressure\n");
		return 0;
	}

	if (strcmp(argv[1], "self") == 0)
		self();
	else if (strcmp(argv[1], "crc") == 0)
		crc_check();
	else if (strcmp(argv[1], "pressure") == 0)
		pressure();

	return 0;
}
static void self()
{
	self_control();
	printf("\n");
	self_rtn();
	printf("\n");
	self_data();
}

/* 控制报文自测 */
static void self_control()
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

/* 回馈报文自测 */
static void self_rtn()
{
	printf("Return self test start,\n");
	rtn_test(succeed, speed);
	rtn_test(succeed, beep);
	rtn_test(succeed, salvage);
	rtn_test(succeed, auto_return);
	rtn_test(succeed, auto_avoid);
	rtn_test(succeed, go_dest);
	rtn_test(succeed, cruise);
	rtn_test(succeed, hover);

	rtn_test(failure, speed);
	rtn_test(failure, beep);
	rtn_test(failure, salvage);
	rtn_test(failure, auto_return);
	rtn_test(failure, auto_avoid);
	rtn_test(failure, go_dest);
	rtn_test(failure, cruise);
	rtn_test(failure, hover);
	printf("Return self test pass,\n");

}

static void self_data()
{
	int i, j, k;
	float ii, jj, kk, zz;
	struct GPS gps;
	struct IMU imu;
	struct Ultrasonic ultra;
	char buf[MESSAGE_LEN];
	int start, len;
	struct Message msg;

	/* gps */
	printf("Gps self test start\n");
	for (i = 0; i < 30; i++)
	for (ii = 0; ii < 20; ii += 1.3)
	for (jj = 20; jj < 40; jj += 1.3)
	for (kk = 40; kk < 60; kk += 1.3)
	for (zz = 60; zz < 80; zz += 1.3) {
		gps.latitude = ii; gps.longtitude = jj; gps.altitude = kk; gps.speed = zz;
		msg_gps_construct(buf+i, 1, gps);
		k = 0;
		while ((j = msg_parsing(buf+k, MESSAGE_LEN-k, &start, &len)) != 0) {
			if (j > 0 && msg_extract(buf+k+start, len, &msg) == 0) {
				assert(msg_gps_get(&msg, &gps) == 0);
				assert(gps.latitude - ii < 0.000001);
				assert(gps.longtitude - jj < 0.000001);
				assert(gps.altitude - kk < 0.000001);
				assert(gps.speed - zz < 0.000001);
				break;
			}
			k += (j > 0) ? j : -j;
		}
	}
	printf("Gps self test pass.\n");

	/* imu */
	printf("Imu self test start.\n");
	for (i = 0; i < 30; i++)
	for (ii = 0; ii < 30; ii += 1.3)
	for (jj = 30; jj < 60; jj += 1.3)
	for (kk = 60; kk < 90; kk += 1.3) {
		imu.pitch = ii; imu.roll = jj; imu.yaw = kk;
		msg_imu_construct(buf+i, 1, imu);
		k = 0;
		while ((j = msg_parsing(buf+k, MESSAGE_LEN-k, &start, &len)) != 0) {
			if (j > 0 && msg_extract(buf+k+start, len, &msg) == 0) {
				assert(msg_imu_get(&msg, &imu) == 0);
				assert(imu.pitch - ii < 0.000001);
				assert(imu.roll - jj < 0.000001);
				assert(imu.yaw - kk < 0.000001);
				break;
			}
			k += (j > 0) ? j : -j;
		}
	}
	printf("Imu self test pass.\n");

	/* ultrasonic */
	printf("Ultrasonic self test start.\n");
	for (i = 10; i < 30; i++)
	for (ii = 0; ii < 50; ii++)
	for (jj = 50; jj < 100; jj++)
	for (kk = 100; kk <  150; kk++) {
		ultra.front = ii; ultra.left = jj; ultra.right = kk;
		msg_ultrasonic_construct(buf+i, 1, ultra);
		k = 0;
		while ((j = msg_parsing(buf+k, MESSAGE_LEN-k, &start, &len)) != 0) {
			if (j > 0 && msg_extract(buf+k+start, len, &msg) == 0) {
				assert(msg_ultrasonic_get(&msg, &ultra) == 0);
				assert(ultra.front == ii);
				assert(ultra.left == jj);
				assert(ultra.right == kk);
				break;
			}
			k += (j > 0) ? j : -j;
		}
	}
	printf("Ultrasonic self test pass.\n");

	/* battery */
	printf("Battery self test start.\n");
	for (i = 10; i < 30; i++)
	for (ii = 0; ii < 28; ii += 0.12) {
		msg_battery_construct(buf+i, 1, ii);
		k = 0;
		while ((j = msg_parsing(buf+k, MESSAGE_LEN-k, &start, &len)) != 0) {
			if (j > 0 && msg_extract(buf+k+start, len, &msg) == 0) {
				assert(msg_battery_get(&msg, &jj) == 0);
				assert(ii - jj < 0.000001);
				break;
			}
			k += (j > 0) ? j : -j;
		}
	}
	printf("Battery self test pass.\n");

}

/* 压力测试 */
static void pressure()
{

}

/* crc check test */
static void crc_check()
{
	int i, j, k, t;
	char buf[MESSAGE_LEN];
	struct Message msg;
	int start, len;
	int crc_check = 0;
	int res;

	printf("Start crc check test.\n");
	for (i = 0; i < CHECK_TIME; i++) {
		j = (rand() % 50) + 1;
		k = (rand() % 21);

		msg_speed_construct(buf+j, 1, rand() % 101, rand() % 101);
		while ((t = (char)rand()) == buf[j+k])
			;
		buf[j+k] = t;

		t = 0;
		while ((res = msg_parsing(buf+t, MESSAGE_LEN-t, &start, &len)) != 0) {
			if (res > 0 && msg_extract(buf+t+start, len, &msg) == 0) {
				crc_check++;
				break;
			}
			t += (res > 0) ? res : -res;
		}
	}
	printf("Crc check pass times: %d\n", crc_check);
	printf("Crc check times: %d\n", CHECK_TIME);
	printf("Crc check pass rage: %lf\n", crc_check / (double)CHECK_TIME);

}

static void rtn_test(enum MCommand rtn_command, enum MCommand get_command)
{
	int i, j, k;
	struct Message msg;
	char buf[MESSAGE_LEN];
	int start, len;
	enum MCategory category;
	enum MCommand command;

	for (i = 0; i < 30; i++) {
		msg.category = control;
		msg.command = get_command;

		msg_rtn_construct(buf+i, 2, rtn_command, &msg);
		k = 0;
		while ((j = msg_parsing(buf+k, MESSAGE_LEN-k, &start, &len)) != 0) {
			//printf("j %d, k %d\n", j, k);
			if (j > 0 && msg_extract(buf+k+start, len, &msg) == 0) {
				assert(msg.category == rtn);
				assert(msg_rtn_get(&msg, &category,  &command) == 0);
				//printf("%d %d\n", command, get_command);
				assert(category == control);
				assert(command == get_command);
				break;
			}
			k += (j > 0) ? j : -j;
		}
	}

}

