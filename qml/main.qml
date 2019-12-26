import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Window 2.14
import QtQuick.Controls.Styles 1.4

import UrlScanner 1.0

ApplicationWindow
{
    id: urlScannerWindow
    visible: true
    width: Screen.width / 2
    height: Screen.height / 2
    title: qsTr("URL Scanner")

    Text
    {
        id: textStartUrl
        text: qsTr("Start url:")
        width: parent.width / 4
        height: parent.height / 12
        anchors.top: parent.top
        anchors.left: parent.left
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }

    TextArea
    {
        id: startUrl
        placeholderText: qsTr("Enter start url")
        height: textStartUrl.height
        anchors.top: textStartUrl.top
        anchors.left: textStartUrl.right
        anchors.right: parent.right
        verticalAlignment: Text.AlignVCenter

        wrapMode: TextEdit.Wrap
    }

    Text
    {
        id: textMaxThreadCount
        text: qsTr("Maximum number of threads:")
        width: parent.width / 4
        height: parent.height / 12
        anchors.top: textStartUrl.bottom
        anchors.left: parent.left
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }

    TextArea
    {
        id: maxThreadCount
        placeholderText: qsTr("Enter maximum number of threads")
        height: textMaxThreadCount.height
        anchors.top: textMaxThreadCount.top
        anchors.left: textMaxThreadCount.right
        anchors.right: parent.right
        verticalAlignment: Text.AlignVCenter

        wrapMode: TextEdit.Wrap
    }

    Text
    {
        id: textSearchText
        text: qsTr("Search text:")
        width: parent.width / 4
        height: parent.height / 12
        anchors.top: textMaxThreadCount.bottom
        anchors.left: parent.left
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }

    TextArea
    {
        id: searchText
        placeholderText: qsTr("Enter search text")
        height: textSearchText.height
        anchors.top: textSearchText.top
        anchors.left: textSearchText.right
        anchors.right: parent.right
        verticalAlignment: Text.AlignVCenter

        wrapMode: TextEdit.Wrap
    }

    Text
    {
        id: textMaxUrlsCount
        text: qsTr("Maximum number of urls:")
        width: parent.width / 4
        height: parent.height / 12
        anchors.top: textSearchText.bottom
        anchors.left: parent.left
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }

    TextArea
    {
        id: maxUrlsCount
        placeholderText: qsTr("Enter maximum number of urls")
        height: textMaxUrlsCount.height
        anchors.top: textMaxUrlsCount.top
        anchors.left: textMaxUrlsCount.right
        anchors.right: parent.right
        verticalAlignment: Text.AlignVCenter

        wrapMode: TextEdit.Wrap
    }

    Button
    {
        id: start
        width: parent.width / 3
        height: parent.height / 12
        anchors.top: textMaxUrlsCount.bottom
        anchors.left: parent.left

        text: qsTr("start")
        background: Rectangle {
            border.width: 2
            color: start.down ? "grey" : "lightgrey"
        }

        onClicked:
        {
            urlScanner.start_url = startUrl.text
            urlScanner.max_thread_count = maxThreadCount.text
            urlScanner.search_text = searchText.text
            urlScanner.max_urls_count = maxUrlsCount.text
            urlScanner.start()
            start.enabled = false
            pause.enabled = true
            stop.enabled = true
        }
    }

    Button
    {
        id: pause
        width: parent.width / 3
        height: parent.height / 12
        anchors.top: textMaxUrlsCount.bottom
        anchors.left: start.right
        enabled: false

        text: qsTr("pause")
        background: Rectangle {
            border.width: 2
            color: pause.down ? "grey" : "lightgrey"
        }

        onClicked:
        {
            text == "pause" ? urlScanner.pause() : urlScanner.resume()
            text == "pause" ? text = qsTr("resume") : text = qsTr("pause")
        }
    }

    Button
    {
        id: stop
        width: parent.width / 3
        height: parent.height / 12
        anchors.top: textMaxUrlsCount.bottom
        anchors.left: pause.right
        enabled: false

        text: qsTr("stop")
        background: Rectangle {
            border.width: 2
            color: stop.down ? "grey" : "lightgrey"
        }

        onClicked:
        {
            urlScanner.stop()
            pause.enabled = false
            stop.enabled = false
        }
    }

    Text {
        id: textUrl
        width: parent.width / 2
        height: parent.height / 12
        anchors.top: start.bottom
        anchors.left: parent.left
        verticalAlignment: Text.AlignVCenter

        text: qsTr("Url")
    }

    Text {
        id: textUrlStatus
        width: parent.width / 2
        height: parent.height / 12
        anchors.top: textUrl.top
        anchors.left: textUrl.right
        verticalAlignment: Text.AlignVCenter

        text: qsTr("Scan status")
    }

    TableView {
        id: tableView
        anchors.top: textUrl.bottom
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        columnSpacing: 1
        rowSpacing: 1
        clip: true
        ScrollIndicator.horizontal: ScrollIndicator { }
        ScrollIndicator.vertical: ScrollIndicator { }

        model: UrlScanner {
            id: urlScanner
        }

        delegate: Rectangle {
            implicitWidth: parent.width / 2
            implicitHeight: 20
            Text {
                text: model.display
            }
        }
    }

}
