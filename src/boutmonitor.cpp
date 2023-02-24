#include"boutmonitor.hpp"
#include"encoding/qbech32.hpp"
#include"QJsonDocument"
#include"QJsonObject"

using namespace qiota;

Node_Conection* BoutMonitor::connection_=nullptr;

BoutMonitor::BoutMonitor():outtypes_(Basic),rpeat_(0)
{

}

void BoutMonitor::eventMonitor(QString condition)
{
    auto info=connection_->rest_client->get_api_core_v2_info();
    QObject::connect(info,&Node_info::finished,this,[=]( ){

        auto resp=connection_->mqtt_client->
                get_outputs_unlock_condition_address(condition+"/"+qencoding::qbech32::Iota::encode(info->bech32Hrp,addr_));
        connect(resp,&ResponseMqtt::returned,this,[=](QJsonValue data){
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
void BoutMonitor::folloup(QString outid)
{
    auto resp=connection_->mqtt_client->get_outputs_outputId(outid);
    QObject::connect(resp,&ResponseMqtt::returned,this,[=](QJsonValue data)
    {
        const auto node_outputs_=Node_output(data);
        emit outputChanged(node_outputs_);
        if(node_outputs_.metadata().is_spent_)resp->deleteLater();
    });
}
void BoutMonitor::recheck(bool fromEpoch)
{
    static qint64 cday=(fromEpoch)?0:QDateTime::currentSecsSinceEpoch();

    auto info=connection_->rest_client->get_api_core_v2_info();
    QObject::connect(info,&Node_info::finished,this,[=]( ){

        auto node_outputs_=new Node_outputs();
        auto strfilter=((sender_.isNull())?"":"sender="+qencoding::qbech32::Iota::encode(info->bech32Hrp,sender_)+"&")+
                ((issuer_.isNull())?"":"issuer_="+qencoding::qbech32::Iota::encode(info->bech32Hrp,issuer_)+"&")+
                ((addr_.isNull())?"":"address="+qencoding::qbech32::Iota::encode(info->bech32Hrp,addr_)+"&") +((tag_.isNull())?"":"tag="+tag_.toHexString() + "&createdAfter="
                                                                              +QString::number(cday));
        switch(outtypes_) {
        case Basic:
            connection_->rest_client->get_basic_outputs(node_outputs_,strfilter);
        case NFT:
            connection_->rest_client->get_nft_outputs(node_outputs_,strfilter);
        }

        QObject::connect(node_outputs_,&Node_outputs::finished,this,[=]( ){
            for(const auto& v:node_outputs_->outs_)
            {
                emit gotNewBout(v);
                folloup(v.metadata().outputid_.toHexString());

            }
            node_outputs_->deleteLater();

        });
        info->deleteLater();
    });

}
void BoutMonitor::addressMonitor()
{
    auto info=connection_->rest_client->get_api_core_v2_info();
    QObject::connect(info,&Node_info::finished,this,[=]( ){

        auto node_outputs_=new Node_outputs();
        auto strfilter="address="+qencoding::qbech32::Iota::encode(info->bech32Hrp,addr_);
        switch(outtypes_){
        case Basic:
            connection_->rest_client->get_basic_outputs(node_outputs_,strfilter);
        case NFT:
            connection_->rest_client->get_nft_outputs(node_outputs_,strfilter);
        }

        QObject::connect(node_outputs_,&Node_outputs::finished,this,[=]( ){
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
void BoutMonitor::restMonitor(bool fromEpoch)
{
    recheck(fromEpoch);
    if(rpeat_)
    {
        connect(&monitorTimer,&QTimer::timeout,this,[=]()
        {
                this->recheck(false);
        });
        monitorTimer.start(rpeat_);
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
