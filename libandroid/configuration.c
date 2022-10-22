
typedef struct AConfiguration AConfiguration;
typedef struct AAssetManager AAssetManager;
#include <stddef.h>

AConfiguration* AConfiguration_new() {
    return NULL;
}

void AConfiguration_delete(AConfiguration* config) {

}

void AConfiguration_fromAssetManager(AConfiguration* out, AAssetManager* am) {

}

void AConfiguration_getLanguage(AConfiguration* config, char* outLanguage) {

}

void AConfiguration_getCountry(AConfiguration* config, char* outCountry) {

}
