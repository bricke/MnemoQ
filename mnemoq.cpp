#include "mnemoq.h"
#include <QUuid>
#include <QDataStream>
#include <QMqttSubscriptionProperties>
#include <QDebug>

#define ADMIN QStringLiteral("admin")
#define PORT 9898
#define BROKER_URL QStringLiteral("localhost")

Mnemoq::Mnemoq(QObject *parent)
    : QObject{parent}, m_client(this)
{
    QObject::connect(&m_client, &QMqttClient::connected, this, [=]() {
        m_client.subscribe(ADMIN);
        emit connected();
    });

    QObject::connect(&m_client, &QMqttClient::disconnected, this, &Mnemoq::disconnected);
    QObject::connect(&m_client, &QMqttClient::messageReceived, this, [=](const QByteArray &message, const QMqttTopicName &topic) {
        Message msg = deserialize(message);
        if (msg.identity.compare(m_client.clientId()) == 0) return;
        switch(msg.command) {
        case REPEAT: sendBackup(msg.messages); break;
        default: emit received(topic.name(), msg.messages.first()); break;
        }
    });

    m_client.setClientId(QUuid::createUuid().toString());
    m_client.setHostname(BROKER_URL);
    m_client.setPort(PORT);
}

void Mnemoq::connect()
{
    m_client.connectToHost();
}

void Mnemoq::disconnect()
{
    m_client.disconnectFromHost();
}

void Mnemoq::send(QMqttTopicName topic, QByteArray message)
{
    Message msg;
    msg.identity = m_client.clientId();
    msg.command = NORMAL;
    msg.messages.append(message);

    m_client.publish(topic, serialize(msg));
    m_store.insert(topic, message);
}

void Mnemoq::subscribe(QString topic)
{
    QMqttSubscriptionProperties properties;
    properties.setNoLocal(true); //only works in v5 brokers
    if (m_client.state() == QMqttClient::Connected) {
        m_client.subscribe(topic, properties);
    }
}

void Mnemoq::subscribe(QStringList topics)
{
    foreach (auto topic, topics) {
        subscribe(topic);
    }
}

void Mnemoq::unsubscribe(QString topic)
{
    if (m_client.state() == QMqttClient::Connected) {
        m_client.unsubscribe(topic);
    }
}

void Mnemoq::unsubscribe(QStringList topics)
{
    foreach (auto topic, topics) {
        unsubscribe(topic);
    }
}

void Mnemoq::reload(QList<QMqttTopicName> topics)
{
    if (m_client.state() == QMqttClient::Connected) {
        Message msg;
        msg.identity = m_client.clientId();
        msg.command = REPEAT;
        foreach (auto topic, topics) {
            msg.messages.append(topic.name().toLatin1());
        }
        m_client.publish(ADMIN, serialize(msg));
    }
}

void Mnemoq::sendBackup(QList<QByteArray> topicNames)
{
    // Sending data from the stored value
    if (!topicNames.isEmpty()) {
        foreach (QByteArray topicName, topicNames) {
            if (!m_store.value(QString(topicName)).isEmpty()) {
                send(QString(topicName), m_store.value(QString(topicName)));
            }
        }
    } else {
        // return them all
        for (auto it = m_store.begin(); it != m_store.end(); ++it) {
                send(it.key(), it.value());
        }
    }
}

QByteArray Mnemoq::serialize(Message message)
{
    QByteArray ret;
    QDataStream stream(&ret, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_6_9);

    stream << message.identity
           << message.command
           << message.messages;

    return ret;
}

Mnemoq::Message Mnemoq::deserialize(QByteArray message)
{
    Message ret;
    QDataStream stream(message);
    stream.setVersion(QDataStream::Qt_6_9);

    stream  >> ret.identity
            >> ret.command
            >> ret.messages;

    return ret;
}
