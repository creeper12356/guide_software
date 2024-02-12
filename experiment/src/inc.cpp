#include "inc.h"

bool isRunningInDocker() {
    QFile cgroupFile("/proc/1/cgroup");
    if (cgroupFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString content = cgroupFile.readAll();
        if (content.contains("/docker/")) {
            return true;
        }
    }
    return false;
}
