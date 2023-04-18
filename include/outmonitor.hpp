#pragma once

#include<QObject>
#include<QString>
#include<QByteArray>
#include<QJsonArray>
#include<QtQml/qqmlregistration.h>
#include"nodeConnection.hpp"


using namespace qiota::qblocks;
using namespace qiota;

class outMonitor : public QObject
{
    Q_OBJECT
    QML_ELEMENT

public:
    outMonitor(QObject *parent = nullptr);

    template<Output::types outtype>
    void getRestOuts(QString filter)
    {
        if(Node_Conection::state()==Node_Conection::Connected)
        {
            auto node_outputs_=new Node_outputs(reciever);

            connect(node_outputs_,&Node_outputs::finished,reciever,[=]( ){
                emit gotNewOuts(node_outputs_->outs_,add_json(node_outputs_->outs_));
                node_outputs_->deleteLater();
            });

            Node_Conection::rest_client->get_outputs<outtype>(node_outputs_,filter);
        }

    }

    Q_INVOKABLE void getRestBasicOuts(QString filter){getRestOuts<Output::Basic_typ>(filter);};
    Q_INVOKABLE void getRestfoundryOuts(QString filter){getRestOuts<Output::Foundry_typ>(filter);};
    Q_INVOKABLE void getRestNftOuts(QString filter){getRestOuts<Output::NFT_typ>(filter);};
    Q_INVOKABLE void getRestAliasOuts(QString filter){getRestOuts<Output::Alias_typ>(filter);};
    Q_INVOKABLE void subscribe(QString topic);
    Q_INVOKABLE void restart(void);


signals:
    void gotNewOuts(std::vector<qiota::Node_output>  outs,QJsonArray jsonOuts);
    void restarted();

private:
    QJsonArray add_json(const std::vector<qiota::Node_output>&  outs) const;
    QObject * reciever;

};
