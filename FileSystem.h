#ifndef FILESYSTEM_H
#define FILESYSTEM_H

class QStringList;

class FileSystem
{
public:
    FileSystem();

    static QStringList mounts();
};

#endif // FILESYSTEM_H
