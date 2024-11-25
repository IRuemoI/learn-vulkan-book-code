#include "HelpFunction.h"
#include <cstdlib>
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
float random(float start, float end) {
    return start + (end - start) * rand() / (RAND_MAX + 1.0);
}