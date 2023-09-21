#include"outmonitor.hpp"
#include"encoding/qbech32.hpp"
#include"QJsonDocument"
#include"QJsonObject"

using namespace qiota;

OutMonitor::OutMonitor(QObject *parent):QObject(parent),reciever(new QObject(this)),restcalls(1),numcalls(0)
{

}
void OutMonitor::subscribe(QString topic)
{
    if(Node_Conection::instance()->state()==Node_Conection::Connected)
    {
        auto resp=Node_Conection::instance()->mqtt()->get_subscription(topic);
        connect(resp,&ResponseMqtt::returned,reciever,[=](QJsonValue data){
            const auto node_output=Node_output(data);
            add_json({node_output});
            emit gotNewOutsMqtt({node_output},add_json({node_output}));
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
    outs.clear();
    jsonOuts=QJsonArray();
    restcalls=1;
    numcalls=0;
    reciever=new QObject(this);
    emit restarted();

}
