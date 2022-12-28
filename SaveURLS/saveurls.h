#ifndef SAVEURLS_H
#define SAVEURLS_H

#include "SaveURLS_global.h"

//class QStringList;

class SAVEURLS_EXPORT SaveURLS
{
public:
    SaveURLS();

    void save(QStringList);
    QStringList load();
};

#endif // SAVEURLS_H
