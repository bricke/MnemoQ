#ifndef MNEMOQ_H
#define MNEMOQ_H

#include <QObject>
#include <QMqttClient>
#include <QMqttTopicName>
#include <QByteArray>
#include <QMap>
#include <QStringList>
#include <QList>

class Mnemoq : public QObject
{
    Q_OBJECT
public:
    explicit Mnemoq(QObject *parent = nullptr);

    void connect();
    void disconnect();
    void send(QMqttTopicName topic, QByteArray message);
    void subscribe(QString topic);
    void subscribe(QStringList topics);
    void unsubscribe(QString topic);
    void unsubscribe(QStringList topics);
    void reload(QList<QMqttTopicName> topics = QList<QMqttTopicName>());

signals:
    void connected();
    void disconnected();
    void received(QString channel, QByteArray message);

private:
    void sendBackup(QList<QByteArray> topicNames = QList<QByteArray>());
    QMqttClient m_client;
    QMap<QMqttTopicName, QByteArray> m_store;

    typedef enum {
        NORMAL,
        REPEAT
    } Command;

    typedef struct {
        QString identity;
        Command command;
        QList<QByteArray> messages;
    } Message;

    QByteArray serialize(Message message);
    Message deserialize(QByteArray message);
};

#endif // MNEMOQ_H
