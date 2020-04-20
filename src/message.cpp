#include "message.h"
#include "crc16.h"
#include <QMutex>
#include <QDebug>
#include "globaldata.h"

static QMutex serial_mutex;
static uint32_t serial = 0;


void Message::construct(QByteArray message)
{
    data->m_message = message;
    const char *cbuf = data->m_message.data();
    uint16_t real_crc;

    data->m_crc = *(uint16_t *)(cbuf + data->m_message.length() - 3);
    real_crc = get_crc(cbuf, data->m_message.length()-3);
    if (real_crc != data->m_crc)
        throw "crc err";

    data->m_serial = *(uint32_t *)(cbuf);
    data->m_length = *(int32_t *)(cbuf + 4);
    data->m_source = *(uint16_t *)(cbuf + 9);
    data->m_distance = *(uint16_t *)(cbuf + 12);
    data->m_category = (enum MCategory)*(uint8_t *)(cbuf + 14);
    data->m_command = (enum MCommand)*(uint8_t *)(cbuf + 15);
    data->m_param = data->m_message.mid(16, data->m_length - min_length);
}

void Message::construct(MCommand command, int param1, int param2)
{
    char param[32];

    data->m_command = command;

    switch (command) {
    case speed: // 速度报文
        data->m_category = control;
        param[0] = static_cast<char>(param1);
        param[1] = static_cast<char>(param2);
        data->m_param = QByteArray(param, 2);
        break;

    case beep: // 蜂鸣报文
        data->m_category = control;
        param[0] = static_cast<char>(param1);
        data->m_param = QByteArray(param, 1);
        break;

    case salvage: // 打捞报文
        data->m_category = control;
        break;

    case connect: // 连接报文
        data->m_category = rtn;
        break;

    case status: // 状态报文
        data->m_category = rtn;
        param[0] = global_data::status;
        param[1] = global_data::autoTrip;
        param[2] = global_data::autoAvoid;
        data->m_param = QByteArray(param, 3);
        break;

    case gps:
        data->m_category = MCategory::data;
        *reinterpret_cast<float *>(param) = global_data::gps.latitude;
        *reinterpret_cast<float *>(param+4) = global_data::gps.longtitude;
        *reinterpret_cast<float *>(param+8) = global_data::gps.altitude;
        *reinterpret_cast<float *>(param+12) = global_data::gps.speed;
        data->m_param = QByteArray(param, 16);
        break;

    case imu:
        data->m_category = MCategory::data;
        *reinterpret_cast<float *>(param) = global_data::imu.roll;
        *reinterpret_cast<float *>(param+4) = global_data::imu.pitch;
        *reinterpret_cast<float *>(param+8) = global_data::imu.yaw;
        data->m_param = QByteArray(param, 12);
        break;

    case ultrasonic:
        data->m_category = MCategory::data;
        *reinterpret_cast<int32_t *>(param) = global_data::ultrasonic.front;
        *reinterpret_cast<int32_t *>(param+4) = global_data::ultrasonic.left;
        *reinterpret_cast<int32_t *>(param+8) = global_data::ultrasonic.right;
        data->m_param = QByteArray(param, 12);
        break;

    case battery:
        data->m_category = MCategory::data;
        *reinterpret_cast<float *>(param) = global_data::battery;
        data->m_param = QByteArray(param, 4);
        break;

    default:
        qWarning() << "construct(): 未知的报文command:" << command;
        return;
    }

    other_build();
}

void Message::other_build()
{
    serial_mutex.lock();
    data->m_serial = (::serial)++;
    serial_mutex.unlock();
    data->m_length = static_cast<int32_t>(min_length + data->m_param.length());
    data->m_source = 1;
    data->m_distance = 0;

    char *arr = new char[data->m_length];
    *reinterpret_cast<uint32_t *>(arr) = data->m_serial;
    *reinterpret_cast<int32_t *>(arr+4) = data->m_length;
    *(arr+8) = 'S';
    *reinterpret_cast<uint16_t *>(arr+9) = data->m_source;
    *(arr+11) = 'R';
    *reinterpret_cast<uint16_t *>(arr+12) = data->m_distance;
    *reinterpret_cast<MCategory *>(arr+14) = data->m_category;
    *reinterpret_cast<MCommand *>(arr+15) = data->m_command;
    for (int i = 0; i < data->m_param.length(); ++i)
        *(arr + 16 + i) = data->m_param.at(i);

    data->m_crc = get_crc(arr, data->m_param.length()+min_length-3);
    *reinterpret_cast<uint16_t *>(arr+16+data->m_param.length()) = data->m_crc;
    *(arr+min_length+data->m_param.length()-1) = static_cast<char>(0xFF);

    data->m_message = QByteArray(arr, data->m_length);

    delete[] arr;
}
