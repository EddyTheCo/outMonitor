import QtQuick.Controls
import QtQuick
import bout_monitor
import nodeConection

ApplicationWindow {
    visible: true
    width:700
    height:700

    background: Rectangle {
        color:"#1e1e1e"
    }

    Node_Connections
    {
        id:node_con
        visible:true
        anchors.fill: parent
    }

    BoutMonitor
    {
        id:outMon
        connection:Node_Conection
        tag:"Testing monitor"
        onReady: outMon.startMonitor();

    }
}
