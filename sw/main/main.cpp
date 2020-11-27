/*
Copyright (c) 2020 Josh Watts

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <stdio.h>
#include <esp_log.h>
#include <esp_netif.h>
#include <esp_wifi.h>
#include <esp_http_server.h>
#include <nvs_flash.h>
#include <driver/gpio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "display.h"
#include "rn52.h"

#define USE_RN52 0
#define USE_DISPLAY 1

Display display;
RN52 rn52;

extern "C"
void app_main(void)
{
    printf("Truck Stereo v1.0.0\n");

    ESP_ERROR_CHECK(gpio_install_isr_service(ESP_INTR_FLAG_LOWMED));

#if USE_DISPLAY
    display.enable();
#endif

#if USE_RN52
    rn52.enable();
#endif
    unsigned nextTrackInfoTick = 0;
    MediaInfo mi;

    if (nextTrackInfoTick <= xTaskGetTickCount()) {
        nextTrackInfoTick = xTaskGetTickCount() + 2000 / portTICK_PERIOD_MS;
        mi.setField(MediaInfo::FIELD_TITLE, "LONG TITLE");
        mi.setField(MediaInfo::FIELD_ARTIST, "ARTIST");
        display.setMediaInfo(mi);
    }


    for (;;) {
        vTaskDelay(1);
#if USE_DISPLAY
        display.loop();
#endif
#if USE_RN52
        rn52.loop();
        if (nextTrackInfoTick <= xTaskGetTickCount()) {
            nextTrackInfoTick = xTaskGetTickCount() + 2000 / portTICK_PERIOD_MS;
            if (rn52.getMediaInfo(mi))
                printf("T:%s\nA:%s\n\n", mi.getField(MediaInfo::FIELD_TITLE).c_str(), mi.getField(MediaInfo::FIELD_ARTIST).c_str());
        }
#endif

    }
}
