#pragma once

#include<QObject>
#include<QString>
#include<QByteArray>
#include<QJsonObject>
#include<QJsonDocument>
#include<QtQml/qqmlregistration.h>
#include"block/qblock.hpp"


using namespace qiota::qblocks;
using namespace qiota;

class Converter : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:

    Q_INVOKABLE QString toHex(QString text)
    {
        return c_array(text.toUtf8()).toHexString();
    };
    Q_INVOKABLE QJsonObject fromHex(QString text)
    {

        auto var=c_array::fromHexString(text);
        auto json=QJsonDocument::fromJson(var);

        if(!json.isNull())
        {
            return json.object();
        }
        return QJsonObject();
    };



};
