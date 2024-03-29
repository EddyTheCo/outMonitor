#pragma once

#include<QObject>
#include<QString>
#include<QByteArray>
#include<QJsonArray>
#include<QtQml/qqmlregistration.h>
#include"nodeConnection.hpp"

#include <QtCore/QtGlobal>

#if defined(WINDOWS_OMONI)
# define OMONI_EXPORT Q_DECL_EXPORT
#else
#define OMONI_EXPORT Q_DECL_IMPORT
#endif

using namespace qiota::qblocks;
using namespace qiota;

class OMONI_EXPORT OutMonitor : public QObject
{
    Q_OBJECT
    QML_ELEMENT

public:
    OutMonitor(QObject *parent = nullptr);

    template<Output::types outtype>
    void getRestOuts(QString filter)
    {
        if(Node_Conection::instance()->state()==Node_Conection::Connected)
        {

            auto node_outputs_=new Node_outputs(reciever);


            connect(node_outputs_,&Node_outputs::finished,reciever,[=]( ){
                numcalls++;
                const auto var=add_json(node_outputs_->outs_);
                const auto vec=node_outputs_->outs_;
                if(node_outputs_->outs_.size())emit gotNewOuts(vec,var);
                for(const auto&v:vec)
                {
                    outs.push_back(v);
                }

                for(const auto& v:var)
                {
                    jsonOuts.append(v);
                }
                if(numcalls==restcalls)
                {
                    emit finished(outs,jsonOuts);
                    numcalls=0;
                }

                node_outputs_->deleteLater();
            });

            Node_Conection::instance()->rest()->get_outputs<outtype>(node_outputs_,filter);
        }

    }

    Q_INVOKABLE void getRestBasicOuts(QString filter){getRestOuts<Output::Basic_typ>(filter);};
    Q_INVOKABLE void getRestfoundryOuts(QString filter){getRestOuts<Output::Foundry_typ>(filter);};
    Q_INVOKABLE void getRestNftOuts(QString filter){getRestOuts<Output::NFT_typ>(filter);};
    Q_INVOKABLE void getRestAliasOuts(QString filter){getRestOuts<Output::Alias_typ>(filter);};
    Q_INVOKABLE void subscribe(QString topic);
    Q_INVOKABLE void restart(void);
    Q_INVOKABLE void setRestCalls(int calls){restcalls=calls;}

signals:
    void gotNewOuts(std::vector<qiota::Node_output>  outs,QJsonArray jsonOuts);
    void gotNewOutsMqtt(std::vector<qiota::Node_output>  outs,QJsonArray jsonOuts);
    void finished(std::vector<qiota::Node_output>  outs,QJsonArray jsonOuts);
    void restarted();

private:
    QJsonArray add_json(const std::vector<qiota::Node_output>&  outs) const;
    QObject * reciever;
    int restcalls, numcalls;
    std::vector<qiota::Node_output>  outs;
    QJsonArray jsonOuts;

};
