#ifndef MESSAGE_H
#define MESSAGE_H

#include <cstdint>
#include <QObject>
#include <QByteArray>
#include <QSharedData>
#include "selftype.h"

// 实现隐式共享，提高效率
class MessageData : public QSharedData
{
public:
    MessageData() {}
    MessageData(const MessageData &other)
        : QSharedData(other), m_message(other.m_message)
        , m_serial(other.m_serial), m_length(other.m_length)
        , m_source(other.m_source), m_distance(other.m_distance)
        , m_category(other.m_category), m_command(other.m_command)
        , m_param(other.m_param), m_crc(other.m_crc)
    {}

    QByteArray m_message;

    uint32_t m_serial;
    int32_t m_length;
    uint16_t m_source;
    uint16_t m_distance;
    enum MCategory m_category;
    enum MCommand m_command;
    QByteArray m_param;
    uint16_t m_crc;
};

class Message
{
public:
    Message() : data(new MessageData) {}
    void construct(QByteArray message);
    void construct(MCommand command, int param1 = 0, int param2 = 0);

    uint32_t serial() const
    { return data->m_serial; }
    const QByteArray byteMessage() const
    { return data->m_message; }
    enum MCategory category() const
    { return data->m_category; }
    enum MCommand command() const
    { return data->m_command; }
    const QByteArray param() const
    { return data->m_param; }

private:
    void other_build();

    QSharedDataPointer<MessageData> data;

    const static int min_length = 19;
};

Q_DECLARE_METATYPE(Message)

#endif // MESSAGE_H
