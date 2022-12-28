#ifndef SAVEURLS_H
#define SAVEURLS_H
#include <QStringList>

//class QStringList;

class SaveURLS
{
public:
    SaveURLS();

    void save(QStringList);
    QStringList load();
};

#endif // SAVEURLS_H
