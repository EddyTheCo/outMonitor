#include"outmonitor.hpp"
#include"encoding/qbech32.hpp"
#include"QJsonDocument"
#include"QJsonObject"

using namespace qiota;

outMonitor::outMonitor(QObject *parent):QObject(parent),reciever(new QObject(this))
{

}

void outMonitor::subscribe(QString topic)
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

QJsonArray outMonitor::add_json(const std::vector<qiota::Node_output>& outs)const
{
    QJsonArray outsArray_;
    for(const auto& v:outs)
    {
        outsArray_.push_back(v.get_Json());
    }
    return outsArray_;
}
void outMonitor::restart(void)
{
    reciever->deleteLater();
    emit restarted();
    reciever=new QObject(this);
}
