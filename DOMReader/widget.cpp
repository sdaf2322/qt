#include <QtGui>
#include <QtWidgets>
#include <QDomDocument>
#include "widget.h"

DOMReader::DOMReader(QWidget *parent)
    : QWidget(parent)
{
    QStringList labels;
    labels << tr("Name") << tr("Telephone NO.");
    QTreeWidget* treeWidget = new QTreeWidget(this);
    treeWidget->header()->setSectionResizeMode(QHeaderView::Stretch);
    treeWidget->setHeaderLabels(labels);

    QFile file("Sample.xml");
    file.open(QIODevice::ReadOnly | QFile::Text);
    QXmlStreamReader xmlReader(&file);

    xmlReader.readNextStartElement();
    if(xmlReader.name().toString() == "phonebook"){
        while(xmlReader.readNextStartElement()){
            if(xmlReader.name().toString() == "PhoneNumber"){
                QTreeWidgetItem* item = new QTreeWidgetItem(treeWidget);

                foreach (const QXmlStreamAttribute &attr, xmlReader.attributes()) {
                    if(attr.name().toString() == QLatin1String("name"))
                        item->setText(0,attr.value().toString());
                    if(attr.name().toString() == QLatin1String("TelNo"))
                        item->setText(1,attr.value().toString());
                }
            }
            else{
                xmlReader.skipCurrentElement();
            }
        }
    }

    QDomDocument domDocument;
    domDocument.setContent(&file);
    QDomElement docElem = domDocument.documentElement();
    qDebug("%s", qPrintable(docElem.tagName()));

    QDomNode node = docElem.firstChild();
    while(!node.isNull())
    {
        QDomElement element = node.toElement();
        if(!element.isNull())
        {
            if(element.tagName() == "PhoneNumber")
            {
                QTreeWidgetItem* item = new QTreeWidgetItem(treeWidget);
                item->setText(0, element.attribute("Name"));
                item->setText(1, element.attribute("TelNo"));
            }
        }
        node = node.nextSibling();
    }
}

DOMReader::~DOMReader()
{
//    while(!node.isNull())
//    {
//        QDomElement element = node.toElement();
//        if(!element.isNull())
//        {
//            if(element.tagName() == "PhoneNumber")
//            {
//                QTreeWidgetItem* item = new QTreeWidgetItem(treeWidget);
//                item->setText(0, element.attribute("Name"));
//                item->setText(1, element.attribute("TelNo"));
//            }
//        }
//        node = node.nextSibling();
//    }
}
