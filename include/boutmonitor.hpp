#pragma once

#include<QObject>
#include<QString>
#include<QByteArray>
#include <QtQml/qqmlregistration.h>
#include<nodeConnection.hpp>

/*
 *@ Monitors the basic outputs.
 *
 */
class BoutMonitor : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString  sender READ sender WRITE set_sender NOTIFY senderChanged)
    Q_PROPERTY(QString  addr READ addr WRITE set_addr NOTIFY addrChanged)
    Q_PROPERTY(QString  tag READ tag WRITE set_tag NOTIFY tagChanged)
    Q_PROPERTY(Node_Conection *  connection READ connection WRITE set_connection NOTIFY connectionChanged)

    QML_ELEMENT
public:
    BoutMonitor();
    Q_INVOKABLE void startMonitor(void);
    QString sender(void)const;
    QString addr(void)const;
    QString tag(void)const;

    void set_sender(const QString addr);
    void set_addr(const QString addr);
    void set_tag(const QString tag_m);

    Node_Conection* connection(void)const{return connection_;}
    void set_connection(Node_Conection* connection_m){connection_=connection_m;emit connectionChanged();}


signals:
    void connectionChanged(void);
    void ready(void);
    void senderChanged(void);
    void addrChanged(void);
    void restarted(void);
    void gotNewBout(qiota::Node_output out);
    void tagChanged();

private:
    void checkOutput(const qiota::Node_output out);


    static Node_Conection* connection_;
    QByteArray sender_,addr_;
    qiota::qblocks::tagF tag_;
    QString hrp_;

};
