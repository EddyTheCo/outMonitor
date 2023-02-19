#include"boutmonitor.hpp"
#include"encoding/qbech32.hpp"
#include"QJsonDocument"
#include"QJsonObject"

using namespace qiota;

Node_Conection* BoutMonitor::connection_=nullptr;

BoutMonitor::BoutMonitor()
{

    QObject::connect(this,&BoutMonitor::connectionChanged,this,[=](){

        connect(connection_,&Node_Conection::connected,this,[=](){

            auto info=connection_->rest_client->get_api_core_v2_info();
            QObject::connect(info,&Node_info::finished,this,[=]( ){
                hrp_=info->bech32Hrp;
                qDebug()<<"READY";
                emit ready();
                info->deleteLater();
            });

        });

    });


}


void BoutMonitor::checkOutput(const qiota::Node_output out)
{
    /*
     *The outputs can be new created or spent ones.
     *
     */

    if(out.output()->type_m==qblocks::Output::Basic_typ)
    {
        emit gotNewBout(out);
    }

}
void BoutMonitor::eventMonitor(void)
{

    auto resp=connection_->mqtt_client->
            get_outputs_unlock_condition_address("address/"+addr());
    connect(resp,&ResponseMqtt::returned,this,[=](QJsonValue data){
        const auto node_output=Node_output(data);
        checkOutput(node_output);
    });
    auto resp2=connection_->mqtt_client->
            get_outputs_unlock_condition_address("address/"+addr_+"/spent");
    connect(resp2,&ResponseMqtt::returned,this,[=](QJsonValue data){
        const auto node_output=Node_output(data);
        checkOutput(node_output);
    });

}
void BoutMonitor::recheck(void)
{
    static qint64 cday=0;

    auto node_outputs_=new Node_outputs();
    auto strfilter=((sender_.isNull())?"":"sender="+sender()+"&")+
            ((addr_.isNull())?"":"address="+addr()+"&") +((tag_.isNull())?"":"tag="+tag_.toHexString() + "&createdAfter="
                                                                          +QString::number(cday));
    qDebug()<<"strfilter:"<<strfilter;
    connection_->rest_client->get_basic_outputs(node_outputs_,strfilter);

    QObject::connect(node_outputs_,&Node_outputs::finished,this,[=]( ){
        for(const auto& v:node_outputs_->outs_)
        {
            checkOutput(v);

            auto resp=connection_->mqtt_client->get_outputs_outputId(v.metadata().outputid_.toHexString());

                    QObject::connect(resp,&ResponseMqtt::returned,this,[=](QJsonValue data)
                    {
                        const auto node_outputs_=Node_output(data);
                        emit outputChanged(node_outputs_);
                        if(node_outputs_.metadata().is_spent_)resp->deleteLater();
                    });
        }
        node_outputs_->deleteLater();

    });
    cday=QDateTime::currentSecsSinceEpoch();
}
void BoutMonitor::restMonitor(void)
{
    recheck();
    connect(&monitorTimer,&QTimer::timeout,this,&BoutMonitor::recheck);
    monitorTimer.start(15000);
}
void BoutMonitor::set_sender(const QString addr)
{

    if(!hrp_.isNull())
    {
        const auto addr_pair=qencoding::qbech32::Iota::decode(addr);
        if(addr_pair.second.size()&&addr_pair.second!=sender_)
        {
            sender_=addr_pair.second;
            emit senderChanged();
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
    if(!hrp_.isNull())
    {
        const auto addr_pair=qencoding::qbech32::Iota::decode(addr_m);
        if(addr_pair.second.size()&&addr_pair.second!=addr_)
        {
            addr_=addr_pair.second;
            emit addrChanged();
        }

    }
}
QString BoutMonitor::sender(void)const
{
    return qencoding::qbech32::Iota::encode(hrp_,sender_);
}
QString BoutMonitor::addr(void)const
{
    return qencoding::qbech32::Iota::encode(hrp_,addr_);
}
QString BoutMonitor::tag(void)const
{
    return QString(tag_);
}
