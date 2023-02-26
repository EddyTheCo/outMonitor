#pragma once

#include<QObject>
#include<QString>
#include<QByteArray>
#include <QtQml/qqmlregistration.h>
#include<nodeConnection.hpp>
#include<QTimer>

/*
 *@ Monitors the basic outputs.
 *
 */
class BoutMonitor : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString  sender READ sender WRITE set_sender NOTIFY senderChanged)
    Q_PROPERTY(QString  issuer READ issuer WRITE set_issuer NOTIFY issuerChanged)
    Q_PROPERTY(QString  addr READ addr WRITE set_addr NOTIFY addrChanged)
    Q_PROPERTY(QString  tag READ tag WRITE set_tag NOTIFY tagChanged)
    Q_PROPERTY(quint32  rpeat READ rpeat WRITE set_rpeat NOTIFY rpeatChanged)
    Q_PROPERTY(QDateTime  createdAfter READ createdAfter WRITE set_createdAfter NOTIFY createdAfterChanged)
    Q_PROPERTY(OutTypes  outtypes READ outtypes WRITE set_outtypes NOTIFY outtypesChanged)


    QML_ELEMENT
public:
    BoutMonitor(QObject *parent = nullptr);
    enum OutTypes {
        Basic=3,
        NFT=6,
    };
    Q_ENUM(OutTypes)
    Q_INVOKABLE void restMonitor();
    Q_INVOKABLE void eventMonitor(QString condition="address");
    Q_INVOKABLE void addressMonitor();
    Q_INVOKABLE void folloup(QString outid);
    Q_INVOKABLE void restart(void);

    OutTypes outtypes(void)const{return outtypes_;}
    void set_outtypes(OutTypes sel){if(sel!=outtypes_){outtypes_=sel;emit outtypesChanged();}}

    quint32 rpeat()const{return rpeat_;}
    void set_rpeat(quint32 rpeat_m){if(rpeat_!=rpeat_m){rpeat_=rpeat_m;emit rpeatChanged();}}
    QString sender(void)const;
    QString issuer(void)const;
    QString addr(void)const;
    QString tag(void)const;
    QDateTime createdAfter(void)const{return createdAfter_;}

    void set_sender(const QString addr);
    void set_issuer(const QString addr);
    void set_addr(const QString addr);
    void set_tag(const QString tag_m);
    void set_createdAfter(const QDateTime after)
    {if(after!=createdAfter_){createdAfter_=after;emit createdAfterChanged();}};




signals:
    void connectionChanged(void);
    void senderChanged(void);
    void issuerChanged(void);
    void addrChanged(void);
    void createdAfterChanged();
    void gotNewBout(qiota::Node_output out);
    void outputChanged(qiota::Node_output out);
    void tagChanged();
    void outtypesChanged();
    void rpeatChanged();
    void restarted();

private:
    void recheck();


    qiota::qblocks::c_array sender_,addr_,issuer_;
    qiota::qblocks::tagF tag_;
    QTimer  monitorTimer;
    OutTypes outtypes_;
    quint32 rpeat_;
    QObject * reciever;
    QDateTime createdAfter_;
};
