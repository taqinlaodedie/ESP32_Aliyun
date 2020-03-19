#ifndef USER_ALIYUN_H
#define USER_ALIYUN_H

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event_loop.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt_client.h"

// Aliyun configs
#define CONFIG_BROKER_URL       "mqtt://a1oDFuq2EfB.iot-as-mqtt.cn-shanghai.aliyuncs.com:1883"
#define CONFIG_CLIENT_ID        "test|securemode=3,signmethod=hmacsha1|"
#define CONFIG_MQTT_USER_NAME   "test&a1oDFuq2EfB"
#define CONFIG_MQTT_PASSWORD    "F74A5D716C94EFF40993F9629BB545C7C58E509A"
#define MQTT_UPTOPIC            "/sys/a1oDFuq2EfB/test/thing/event/property/post"
#define PRODUCT_KEY             "a1oDFuq2EfB"
#define DEVICE_NAME             "test"

#define POST_JSON   \
        "{\"version\": \"1.0\",\
            \"id\": \"123\",\
            \"method\": \"thing.event.property.post\"\
        }"

void mqtt_app_start(void);

#endif