#include "user_aliyun.h"
#include "os.h"
#include "cJSON.h"
#include "main.h"

static const char *TAG = "ALIYUN_MQTT";
u8_t switchValue;

// Search switch value in message
void jsonAnalyse(char *data)
{
    cJSON * root = NULL;
    cJSON * item = NULL;

    root = cJSON_Parse(data);     
    if (!root) 
    {
        printf("Error before: [%s]\n",cJSON_GetErrorPtr());
    }
    else {
        item = cJSON_GetObjectItem(root, "params");
        char *buff = cJSON_Print(item);
        if(buff != NULL) {
            // printf("get params: %s\n", buff);
            switchValue = (u8_t)atoi(os_strchr(buff, ':') + 1);
        } 
        if(root)
            cJSON_Delete(root);
        if(buff)
            free(buff);
    }
}

// Set switch value and send switch status to server
void message_handle(esp_mqtt_event_handle_t event)
{
    esp_mqtt_client_handle_t client = event->client;
    char topic_buff[80];
    cJSON *cj_root = cJSON_Parse(POST_JSON);
    cJSON *cj_item =  cJSON_CreateObject();

    // Analyse the json file and set switch level
    jsonAnalyse(event->data);
    gpio_set_level(SWITCH_PIN, switchValue);

    // Create topic buff
    sprintf(topic_buff, "/sys/%s/%s/thing/event/property/post", PRODUCT_KEY, DEVICE_NAME);

    // Create data buff
    printf("start create json file\n");
    cJSON_AddItemToObject(cj_root, "productKey", cJSON_CreateString(PRODUCT_KEY));
    cJSON_AddItemToObject(cj_root, "deviceName", cJSON_CreateString(DEVICE_NAME));
    cJSON_AddItemToObject(cj_root, "params", cj_item);
    cJSON_AddItemToObject(cj_item, "PowerSwitch", cJSON_CreateNumber(switchValue));

    char *buff = cJSON_Print(cj_root);
    printf("return message:%s\n", buff);
    // Send response
    esp_mqtt_client_publish(client,topic_buff , buff, 0, 1, 0);
    if(buff)
        free(buff);
}

static esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event)
{
    // your_context_t *context = event->context;
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            break;
        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");
            printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
            printf("DATA=%.*s\r\n", event->data_len, event->data);
            message_handle(event);
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            break;
        default:
            ESP_LOGI(TAG, "Other event id:%d", event->event_id);
            break;
    }
    return ESP_OK;
}

void mqtt_app_start(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .uri = CONFIG_BROKER_URL,
        .event_handle = mqtt_event_handler,
        .client_id = CONFIG_CLIENT_ID,
        .username = CONFIG_MQTT_USER_NAME,
        .password = CONFIG_MQTT_PASSWORD
    };
    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_start(client);
}