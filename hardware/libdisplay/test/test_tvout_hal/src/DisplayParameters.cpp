/*
**
** Copyright 2008, The Android Open Source Project
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/

#include <utils/Log.h>

#include <string.h>
#include <stdlib.h>
#include <DisplayParameters.h>

namespace android {
// Parameter keys to communicate between jni and java
const char DisplayParameters::KEY_FORMAT[] = "format";
const char DisplayParameters::KEY_AUDIO_CHAN[] = "audio-chan";
const char DisplayParameters::KEY_VIDEO_SCALE[] = "scale";
const char DisplayParameters::KEY_COLORKEY[] = "colorkey";
const char DisplayParameters::KEY_COLOR[] = "color";
const char DisplayParameters::KEY_ALPHA[]="alpha";
const char  DisplayParameters::KEY_HDMI_RES_WIDTH[]="hdmi-res-width";
const char  DisplayParameters::KEY_HDMI_RES_HEIGHT[]="hdmi-res-height";
const char  DisplayParameters::KEY_HDMI_RES_HZ[]="hdmi-res-hz";
const char  DisplayParameters::KEY_HDMI_RES_PG[]="hdmi-res-pg";
const char  DisplayParameters::KEY_HDMI_RES_ASPECT[]="hdmi-res-aspect";

const char  DisplayParameters::KEY_SCALE_X[]="scale-x";
const char  DisplayParameters::KEY_SCALE_Y[]="scale-y";
const char  DisplayParameters::KEY_YPBPR_RES_WIDTH[]="ypbpr-res-width";
const char  DisplayParameters::KEY_YPBPR_RES_HEIGHT[]="ypbpr-res-height";
const char  DisplayParameters::KEY_YPBPR_RES_HZ[]="ypbpr-res-hz";
const char  DisplayParameters::KEY_YPBPR_RES_PG[]="ypbpr-res-pg";
const char  DisplayParameters::KEY_YPBPR_RES_ASPECT[]="ypbpr-res-aspect";
	
// Paramerter keys about Display Device
const char DisplayParameters::KEY_ID[] = "id";
const char DisplayParameters::KEY_WIDTH[] = "width";
const char DisplayParameters::KEY_HEIGHT[] = "height";
const char DisplayParameters::KEY_NAME[] = "name";
const char DisplayParameters::KEY_DES[] = "description";

DisplayParameters::DisplayParameters()
                : mMap()
{
}

DisplayParameters::~DisplayParameters()
{
}

String8 DisplayParameters::flatten() const
{
    String8 flattened("");
    size_t size = mMap.size();

    for (size_t i = 0; i < size; i++) {
        String8 k, v;
        k = mMap.keyAt(i);
        v = mMap.valueAt(i);

        flattened += k;
        flattened += "=";
        flattened += v;
        if (i != size-1)
            flattened += ";";
    }

    return flattened;
}

void DisplayParameters::unflatten(const char *str)
{
    const char *a = str;
    const char *b;

    mMap.clear();

    for (;;) {
        // Find the bounds of the key name.
        b = strchr(a, '=');
        if (b == 0)
            break;

        // Create the key string.
        String8 k(a, (size_t)(b-a));

        // Find the value.
        a = b+1;
        b = strchr(a, ';');
        if (b == 0) {
            // If there's no semicolon, this is the last item.
            String8 v(a);
            mMap.add(k, v);
            break;
        }

        String8 v(a, (size_t)(b-a));
        mMap.add(k, v);
        a = b+1;
    }
}


void DisplayParameters::set(const char *key, const char *value)
{
    // XXX i think i can do this with strspn()
    if (strchr(key, '=') || strchr(key, ';')) {
        //XXX LOGE("Key \"%s\"contains invalid character (= or ;)", key);
        return;
    }

    if (strchr(value, '=') || strchr(key, ';')) {
        //XXX LOGE("Value \"%s\"contains invalid character (= or ;)", value);
        return;
    }

    mMap.replaceValueFor(String8(key), String8(value));
}

void DisplayParameters::setInt(const char *key, int value)
{
    char str[16];
    sprintf(str, "%d", value);
    set(key, str);
}

void DisplayParameters::setFloat(const char *key, float value)
{
    char str[16];  // 14 should be enough. We overestimate to be safe.
    snprintf(str, sizeof(str), "%g", value);
    set(key, str);
}

const char *DisplayParameters::get(const char *key) const
{
    String8 v = mMap.valueFor(String8(key));
    if (v.length() == 0)
        return 0;
    return v.string();
}

int DisplayParameters::getInt(const char *key) const
{
    const char *v = get(key);
    if (v == 0)
        return -1;
    return strtol(v, 0, 0);
}

float DisplayParameters::getFloat(const char *key) const
{
    const char *v = get(key);
    if (v == 0) return -1;
    return strtof(v, 0);
}

void DisplayParameters::remove(const char *key)
{
    mMap.removeItem(String8(key));
}


void DisplayParameters::dump() const
{
    ALOGD("dump: mMap.size = %d", mMap.size());
    for (size_t i = 0; i < mMap.size(); i++) {
        String8 k, v;
        k = mMap.keyAt(i);
        v = mMap.valueAt(i);
        ALOGD("%s: %s\n", k.string(), v.string());
    }
}

status_t DisplayParameters::dump(int fd, const Vector<String16>& args) const
{
    const size_t SIZE = 256;
    char buffer[SIZE];
    String8 result;
    snprintf(buffer, 255, "DisplayParameters::dump: mMap.size = %d\n", mMap.size());
    result.append(buffer);
    for (size_t i = 0; i < mMap.size(); i++) {
        String8 k, v;
        k = mMap.keyAt(i);
        v = mMap.valueAt(i);
        snprintf(buffer, 255, "\t%s: %s\n", k.string(), v.string());
        result.append(buffer);
    }
    write(fd, result.string(), result.size());
    return NO_ERROR;
}

}; // namespace android
