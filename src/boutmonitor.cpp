#include"boutmonitor.hpp"
#include"encoding/qbech32.hpp"
#include"QJsonDocument"
#include"QJsonObject"

using namespace qiota;

BoutMonitor::BoutMonitor(QObject *parent):QObject(parent),outtypes_(Basic),rpeat_(0),
    reciever(new QObject(this))
{

}

void BoutMonitor::eventMonitor(QString condition)
{

    if(Node_Conection::state()==Node_Conection::Connected)
    {
        auto info=Node_Conection::rest_client->get_api_core_v2_info();
        QObject::connect(info,&Node_info::finished,reciever,[=]( ){

            auto resp=Node_Conection::mqtt_client->
                    get_outputs_unlock_condition_address(condition+"/"+qencoding::qbech32::Iota::encode(info->bech32Hrp,addr_));
            connect(resp,&ResponseMqtt::returned,reciever,[=](QJsonValue data){
                const auto node_output=Node_output(data);
                if((int)node_output.output()->type_m==(int)outtypes_)
                {
                    emit gotNewBout(node_output);
                    folloup(node_output.metadata().outputid_.toHexString());
                }

            });
            info->deleteLater();
        });
    }


}
void BoutMonitor::folloup(QString outid)
{

    if(Node_Conection::state()==Node_Conection::Connected)
    {
        auto resp=Node_Conection::mqtt_client->get_outputs_outputId(outid);
        QObject::connect(resp,&ResponseMqtt::returned,reciever,[=](QJsonValue data)
        {
            const auto node_outputs_=Node_output(data);
            emit outputChanged(node_outputs_);
            if(node_outputs_.metadata().is_spent_)resp->deleteLater();
        });
    }


}
void BoutMonitor::recheck()
{
    auto info=Node_Conection::rest_client->get_api_core_v2_info();
    const auto time=createdAfter_;
    QObject::connect(info,&Node_info::finished,this,[=]( ){

        auto node_outputs_=new Node_outputs();
        auto strfilter=
                ((sender_.isNull())?"":"sender="+qencoding::qbech32::Iota::encode(info->bech32Hrp,sender_)+"&")+
                ((issuer_.isNull())?"":"issuer_="+qencoding::qbech32::Iota::encode(info->bech32Hrp,issuer_)+"&")+
                ((addr_.isNull())?"":"address="+qencoding::qbech32::Iota::encode(info->bech32Hrp,addr_)+"&") +
                ((tag_.isNull())?"":"tag="+tag_.toHexString()+"&")+
                ((time.isNull())?"":"createdAfter=" +QString::number(time.toSecsSinceEpoch()));


        switch(outtypes_) {
        case Basic:
            Node_Conection::rest_client->get_basic_outputs(node_outputs_,strfilter);
             break;
        case NFT:
            Node_Conection::rest_client->get_nft_outputs(node_outputs_,strfilter);
             break;
        }

        QObject::connect(node_outputs_,&Node_outputs::finished,reciever,[=]( ){
            for(const auto& v:node_outputs_->outs_)
            {
                emit gotNewBout(v);
                folloup(v.metadata().outputid_.toHexString());

            }
            node_outputs_->deleteLater();

        });
        info->deleteLater();
    });
    createdAfter_=QDateTime::currentDateTime();
}
void BoutMonitor::addressMonitor()
{

    if(Node_Conection::state()==Node_Conection::Connected)
    {
        auto info=Node_Conection::rest_client->get_api_core_v2_info();
        QObject::connect(info,&Node_info::finished,reciever,[=]( ){

            auto node_outputs_=new Node_outputs();
            auto strfilter="address="+qencoding::qbech32::Iota::encode(info->bech32Hrp,addr_);

            switch(outtypes_){
            case Basic:
                Node_Conection::rest_client->get_basic_outputs(node_outputs_,strfilter);
            case NFT:
                Node_Conection::rest_client->get_nft_outputs(node_outputs_,strfilter);
            }

            QObject::connect(node_outputs_,&Node_outputs::finished,reciever,[=]( ){
                for(const auto& v:node_outputs_->outs_)
                {
                    emit gotNewBout(v);
                    folloup(v.metadata().outputid_.toHexString());
                }

                node_outputs_->deleteLater();
                eventMonitor();
            });
            info->deleteLater();
        });
    }


}
void BoutMonitor::restMonitor()
{

    if(Node_Conection::state()==Node_Conection::Connected)
    {
        recheck();
        if(rpeat_)
        {
            connect(&monitorTimer,&QTimer::timeout,this,[=]()
            {
                this->recheck();
            });
            monitorTimer.start(rpeat_);
        }
    }

}

void BoutMonitor::set_tag(const QString tag_m)
{
    auto mtag=qiota::qblocks::tagF(tag_m.toUtf8());

    if(mtag!=tag_)
    {
        tag_=mtag;

        emit tagChanged();
    }
}
void BoutMonitor::set_addr(const QString addr_m)
{

    const auto addr_c=qblocks::c_array::fromHexString(addr_m);
    if(addr_c!=addr_)
    {
        addr_=addr_c;
        emit addrChanged();
    }


}
void BoutMonitor::set_sender(const QString addr_m)
{
    const auto addr_c=qblocks::c_array::fromHexString(addr_m);
    if(addr_c!=sender_)
    {
        sender_=addr_c;
        emit senderChanged();
    }
}
void BoutMonitor::set_issuer(const QString addr_m)
{
    const auto addr_c=qblocks::c_array::fromHexString(addr_m);
    if(addr_c!=issuer_)
    {
        issuer_=addr_c;
        emit issuerChanged();
    }


}
QString BoutMonitor::addr(void)const
{
    return addr_.toHexString();
}
QString BoutMonitor::sender(void)const
{
    return sender_.toHexString();
}
QString BoutMonitor::issuer(void)const
{
    return issuer_.toHexString();
}
QString BoutMonitor::tag(void)const
{
    return QString(tag_);
}
void BoutMonitor::restart(void)
{
    reciever->deleteLater();
    emit restarted();
    reciever=new QObject(this);
}
