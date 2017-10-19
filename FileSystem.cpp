#include <QStorageInfo>
#include "FileSystem.h"

FileSystem::FileSystem()
{

}

QStringList FileSystem::mounts()
{
    QStringList items;
    foreach (QStorageInfo info, QStorageInfo::mountedVolumes())
        items.append(info.name());
    return items;
}
