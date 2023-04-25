#include"outmonitor.hpp"
#include"encoding/qbech32.hpp"
#include"QJsonDocument"
#include"QJsonObject"

using namespace qiota;

OutMonitor::OutMonitor(QObject *parent):QObject(parent),reciever(new QObject(this))
{

}
void OutMonitor::setMinDeposit(std::shared_ptr<Output> out)
{

    auto  info=Node_Conection::rest_client->get_api_core_v2_info();
    QObject::connect(info,&Node_info::finished,Node_Conection::rest_client,[=]( ){
        out->amount_=Client::get_deposit(out,info);
        info->deleteLater();
    });
}
void OutMonitor::subscribe(QString topic)
{
    if(Node_Conection::state()==Node_Conection::Connected)
    {
        auto resp=Node_Conection::mqtt_client->get_subscription(topic);
        connect(resp,&ResponseMqtt::returned,reciever,[=](QJsonValue data){
            const auto node_output=Node_output(data);
            add_json({node_output});
            emit gotNewOuts({node_output},add_json({node_output}));
        });
    }

}

QJsonArray OutMonitor::add_json(const std::vector<qiota::Node_output>& outs)const
{
    QJsonArray outsArray_;
    for(const auto& v:outs)
    {
        outsArray_.push_back(v.get_Json());
    }
    return outsArray_;
}
void OutMonitor::restart(void)
{
    reciever->deleteLater();
    emit restarted();
    reciever=new QObject(this);
}
