#include "HelpFunction.h"
#include <cstring>
using namespace std;
bool memoryTypeFromProperties(VkPhysicalDeviceMemoryProperties &memoryProperties, uint32_t typeBits, VkFlags requirements_mask, uint32_t *typeIndex) {
    for (uint32_t i = 0; i < 32; i++) {
        if ((typeBits & 1) == 1) {
            if ((memoryProperties.memoryTypes[i].propertyFlags & requirements_mask) == requirements_mask) {
                *typeIndex = i;
                return true;
            }
        }
        typeBits >>= 1;
    }
    return false;
}
int parseInt(const char *token) {
    token += strspn(token, " \t");
    int i = atoi(token);
    return i;
}
float toRadians(float f) {
    return f * 3.1415926f / 180;
}
