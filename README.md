## 报文库
该库用于实现usv报文的解析，提取，构造。提供了方便易用的接口。
### 语言要求
c99以上
### 配置
MESSAGE_LEN 设定报文长度，默认83

MESSAGE_SOURCE 设定报文源地址，默认2
### 宏
MESSAGE_LEN 报文长度，创建存放一条报文的缓冲区时使用

GPS_NUM 一条cruise报文最多包含的gps点，创建GPS数组时使用
### 多机控制实现
用地址实现，STM32F427的地址固定为0,上位机地址固定为1。
nano地址为无人艇地址，取值2～65536。
### 构造报文接口
函数名规律为 msg_xxx_construct。
第一个参数为构造好的报文缓冲区，建议长度为MESSAGE_LEN。第二个参数为目的地址。其余参数为报文携带信息。

返回0表示构造失败，返回值为报文字节长度。
``` C
unsigned msg_speed_construct(char *buf, uint16_t destination, int8_t lspeed, int8_t rspeed);
unsigned msg_beep_construct(char *buf, uint16_t destination, int8_t beep);
unsigned msg_salvage_construct(char *buf, uint16_t destination);
unsigned msg_return_construct(char *buf, uint16_t destination,uint8_t status);
unsigned msg_avoid_construct(char *buf, uint16_t destination, uint8_t status);
unsigned msg_go_dest_construct(char *buf, uint16_t destination,float latitude, float longtitude);
unsigned msg_cruise_construct(char *buf, uint16_t destination, int gps_num, struct GPS gps[]);
unsigned msg_hover_construct(char *buf, uint16_t destination);

unsigned msg_rtn_construct(char *buf, uint16_t destination, enum MCommand command, struct Message *get_msg);

unsigned msg_gps_construct(char *buf, uint16_t destination, struct GPS gps);
unsigned msg_imu_construct(char *buf, uint16_t destination, struct IMU imu);
unsigned msg_ultrasonic_construct(char *buf, uint16_t destination,struct Ultrasonic ultra);
unsigned msg_battery_construct(char *buf, uint16_t destination, float battery);
```
### 解析报文接口
包含两部分
- 缓冲区解析算法
- 报文信息提取接口
#### 报文解析文档
首先使用msg_parsing分析缓冲区内是否存在报文。
如果存在，就从缓冲区中解析报文。
最后移除无用缓冲区。
``` C
/* 原型 */
int msg_parsing(const char *buf, int buf_len, int *msg_start, int *msg_length);
int msg_extract(const char *buf, int msg_length, struct Message *msg);

/* 例子-具体使用参考test.c */
int pos = 0, res;
int start, len;
char buf[BUF_SIZE];
struct Message msg;
while ((res = msg_parsing(buf+pos, BUF_SIZE-pos, &start, &len)) != 0) {
    if (res > 0 && msg_exteract(buf+pos, len, &msg) == 0) {
        /* 根据类型进行处理 */
    }
    pos += (res > 0) ? res : -res;
}
```
#### 信息提取
函数名规律为msg_xxx_get。第一个参数传入提取出的Message，接下来的参数传入信息。

返回0表示提取成功，-1表示提取失败。
``` C
int msg_speed_get(struct Message *msg, int *lspeed, int *rspeed);
int msg_beep_get(struct Message *msg, int *beep);
int msg_return_get(struct Message *msg, int *status);
int msg_avoid_get(struct Message *msg, int *status);
int msg_go_dest_get(struct Message *msg,float *latitude, float *longtitude);
int msg_cruise_get(struct Message *msg, int *gps_num, struct GPS gps[]);

int msg_rtn_get(struct Message *msg, enum MCategory *category, enum MCommand *command);

int msg_gps_get(struct Message *msg, struct GPS *gps);
int msg_imu_get(struct Message *msg, struct IMU *imu);
int msg_ultrasonic_get(struct Message *msg, struct Ultrasonic *ultra);
int msg_battery_get(struct Message *msg, float *battery);
```
### crc16
使用压力测试，对21字节长的报文随机一个字节进行随机替换（非原字符）。
测试得出通过率小于0.0001%
