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
    Q_PROPERTY(OutTypes  outtypes READ outtypes WRITE set_outtypes NOTIFY outtypesChanged)
    Q_PROPERTY(Node_Conection *  connection READ connection WRITE set_connection NOTIFY connectionChanged)

    QML_ELEMENT
public:
    BoutMonitor();
    enum OutTypes {
        Basic=3,
        NFT=6,
    };
    Q_ENUM(OutTypes)
    Q_INVOKABLE void restMonitor(bool fromEpoch=true);
    Q_INVOKABLE void eventMonitor(QString condition="address");
    Q_INVOKABLE void addressMonitor();
    Q_INVOKABLE void folloup(QString outid);

    OutTypes outtypes(void)const{return outtypes_;}
    void set_outtypes(OutTypes sel){if(sel!=outtypes_){outtypes_=sel;emit outtypesChanged();}}

    quint32 rpeat()const{return rpeat_;}
    void set_rpeat(quint32 rpeat_m){if(rpeat_!=rpeat_m){rpeat_=rpeat_m;emit rpeatChanged();}}
    QString sender(void)const;
    QString issuer(void)const;
    QString addr(void)const;
    QString tag(void)const;

    void set_sender(const QString addr);
    void set_issuer(const QString addr);
    void set_addr(const QString addr);
    void set_tag(const QString tag_m);

    Node_Conection* connection(void)const{return connection_;}
    void set_connection(Node_Conection* connection_m){connection_=connection_m;emit connectionChanged();}


signals:
    void connectionChanged(void);
    void senderChanged(void);
    void issuerChanged(void);
    void addrChanged(void);
    void gotNewBout(qiota::Node_output out);
    void outputChanged(qiota::Node_output out);
    void tagChanged();
    void outtypesChanged();
    void rpeatChanged();

private:
    void recheck(bool fromEpoch=true);



    static Node_Conection* connection_;
    qiota::qblocks::c_array sender_,addr_,issuer_;
    qiota::qblocks::tagF tag_;
    QTimer  monitorTimer;
    OutTypes outtypes_;
    quint32 rpeat_;
};
