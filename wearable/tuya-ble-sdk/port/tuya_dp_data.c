
#include "tuya_dp_data.h"
#include "dp_wearable.h"
#include "dp_tuya_cfg.h"
#include "dp_tuya_panel_info.h"


static uint32_t counter_sn = 0;
uint8_t tx_dp_data[128] = {0};
#define MAX_TX_LEN 128
#define MAX_UTF8_LEN 256
void unicode_2_utf8(uint8_t *unicode_data, uint8_t *utf8_char, uint8_t unicode_len,
                    uint16_t *utf8_len);
void dp_data_send_ack(tuya_ble_dp_data_send_type_t send_type,
                      tuya_ble_dp_data_send_mode_t send_mode, tuya_ble_dp_data_send_ack_t send_resp, uint8_t *data,
                      uint16_t len)
{
    uint32_t res = 0;

    res = tuya_ble_dp_data_send(counter_sn, send_type, send_mode, send_resp, data, len + 4);
    APP_PRINT_INFO1("dp send ack data is %b", TRACE_BINARY(len + 4, data));
    APP_PRINT_INFO1("res is %d", res);

    if (res == 0)
    {
        counter_sn++;
    }
}
void dp_data_write_ftl(void)
{

}

void dp_data_read_ftl(void)
{

}
void dp_sport_data_parse(void)
{

}

void dp_setting_data_parse(void)
{


}
void dp_bool_data_parse(tuya_ble_dp_data_received_data_t *receive_data)
{
    uint8_t dp_id = 0;
    dp_id = receive_data->p_data[0];
    APP_PRINT_INFO1("dp_bool_data_parse dp_id is %d", dp_id);

}
void dp_raw_data_parse(uint8_t   *receive_data, uint16_t len)
{
    uint8_t dp_id = 0;
    dp_id = receive_data[0];
    uint16_t data_len = 0;
    APP_PRINT_INFO1("dp_raw_data_parse dp_id is %d", dp_id);
    APP_PRINT_INFO1("dp_raw_data_parse receive is %b", TRACE_BINARY(len, receive_data + 4));

    switch (dp_id)
    {
    case DP_ID_TARGET_SET:
        APP_PRINT_INFO1("DP_ID_TARGET_SET len is %d", dp_id);
        data_len = sizeof(T_DP_TARGET_SET);
        if (len <= data_len)
        {
            T_DP_TARGET_SET sport_target = {0};
            sport_target.steps_target = receive_data[4];
            sport_target.steps_target = (sport_target.steps_target << 8) + receive_data[4 + 1];
            sport_target.steps_target = (sport_target.steps_target << 8) + receive_data[4 + 2];
            sport_target.steps_target = (sport_target.steps_target << 8) + receive_data[4 + 3];
            sport_target.calories_target = receive_data[8];
            sport_target.calories_target = (sport_target.calories_target << 8) + receive_data[8 + 1];
            sport_target.calories_target = (sport_target.calories_target << 8) + receive_data[8 + 2];
            sport_target.calories_target = (sport_target.calories_target << 8) + receive_data[8 + 3];
            sport_target.distance_target = receive_data[12];
            sport_target.distance_target = (sport_target.distance_target << 8) + receive_data[12 + 1];
            sport_target.distance_target = (sport_target.distance_target << 8) + receive_data[12 + 2];
            sport_target.distance_target = (sport_target.distance_target << 8) + receive_data[12 + 3];
            APP_PRINT_INFO3("sport_target is %d,cal_target is %d, distance target is %d",
                            sport_target.steps_target, sport_target.calories_target, sport_target.distance_target);
            rtk_kvdb_set(KVDB_SPORT_REFS_NAME, KEY_SPORTS_REFS_TARGET_SET, &sport_target, data_len);

            dp_data_send_ack(DP_SEND_TYPE_ACTIVE, DP_SEND_FOR_PANEL, DP_SEND_WITH_RESPONSE, receive_data, len);
        }
        else
        {
            APP_PRINT_ERROR0("dp DP_TARGET_SET len is over");
        }
        break;
    case DP_ID_MILEEAGE_MIN:
        break;
    case DP_ID_INFO_SET:

        if (len <= sizeof(T_DP_INFO_SET))
        {
            T_DP_INFO_SET info_tmp = {0};
            memcpy((uint8_t *)&info_tmp, receive_data + 4, sizeof(info_tmp));

            APP_PRINT_INFO2("info_set is %x,weight is %x, height is %x", info_tmp.weight, info_tmp.height);

            rtk_kvdb_set(KVDB_TUYA_CFG_NAME, KEY_TUYA_CFG_INFO_SET, &info_tmp, sizeof(info_tmp));
            dp_data_send_ack(DP_SEND_TYPE_ACTIVE, DP_SEND_FOR_PANEL, DP_SEND_WITH_RESPONSE, receive_data, len);
        }
        else
        {
            APP_PRINT_ERROR0("dp DP_INFO_SET len is over");
        }
        break;
    case DP_ID_SLEEP_SET:
        {
            data_len = sizeof(T_DP_SLEEP_SET);
            if (len <= data_len)
            {
                T_DP_SLEEP_SET  sleep_set;
                sleep_set.sleep_set_switch = *(receive_data + 4);
                rtk_kvdb_set(KVDB_SPORT_REFS_NAME, KEY_SPORTS_REFS_SLEEP_SET, &sleep_set, data_len);
                dp_data_send_ack(DP_SEND_TYPE_ACTIVE, DP_SEND_FOR_PANEL, DP_SEND_WITH_RESPONSE, receive_data, len);
            }
            else
            {
                APP_PRINT_ERROR0("dp DP_SLEEP_SET len is over");
            }
        }
        break;
    case DP_ID_CLOCK_SET:
        if (len <= sizeof(T_DP_CLOCK_SET))
        {
            T_DP_CLOCK_SET clock_set_tmp = {0};
            memcpy((uint8_t *)&clock_set_tmp, receive_data + 4, len);

            rtk_kvdb_set(KVDB_TUYA_CFG_NAME, KEY_TUYA_CFG_CLOCK_SET, &clock_set_tmp, sizeof(clock_set_tmp));
            dp_data_send_ack(DP_SEND_TYPE_ACTIVE, DP_SEND_FOR_PANEL, DP_SEND_WITH_RESPONSE, receive_data, len);
        }
        else
        {
            APP_PRINT_ERROR0("dp DP_CLOCK_SET len is over");
        }
        break;
    case DP_ID_SCREEN_BRIGHT_VALUE:
        break;
    case DP_ID_NOTIFICATION_PUSH:
        {
            T_DP_NOTIFY_SET notify_data_tmp;
            memcpy(notify_data_tmp.data, receive_data + 4, sizeof(notify_data_tmp));
            APP_PRINT_INFO3("DP_ID_NOTIFICATION_PUSH is 0x%x, 0x0%x,0x%x", notify_data_tmp.data[0], \
                            notify_data_tmp.data[1], notify_data_tmp.data[2]);
            rtk_kvdb_set(KVDB_TUYA_CFG_NAME, KEY_TUYA_CFG_NOTIFY_SET, &notify_data_tmp,
                         sizeof(notify_data_tmp));
            APP_PRINT_INFO1("notification set!!!!! len is %d", len);
            APP_PRINT_INFO1(" message %s", TRACE_STRING(receive_data + 4));
            dp_data_send_ack(DP_SEND_TYPE_ACTIVE, DP_SEND_FOR_PANEL, DP_SEND_WITHOUT_RESPONSE, receive_data,
                             len);
        }
        break;
    default:
        break;
    }
}

void dp_value_data_parse(uint8_t *receive_data, uint16_t len)
{
    uint8_t dp_id = receive_data[0];
    uint16_t data_len;

    APP_PRINT_INFO1("dp_value_data_parse dp_id is %d", dp_id);

    switch (dp_id)
    {
    case DP_ID_SEDENTARY_REMAINDER:
        {
            data_len = sizeof(T_DP_SEDENTARY_REMINDER);
            if (len <= data_len)
            {
                T_DP_SEDENTARY_REMINDER sedentary_deminder = {0};
                memcpy((uint8_t *)&sedentary_deminder, receive_data + 4, data_len);
                APP_PRINT_INFO0("sedentary_reminder_set OK");
                rtk_kvdb_set(KVDB_SPORT_REFS_NAME, KEY_SPORTS_REFS_SEDENTARY_REMINDER, &sedentary_deminder,
                             data_len);
                dp_data_send_ack(DP_SEND_TYPE_ACTIVE, DP_SEND_FOR_PANEL, DP_SEND_WITHOUT_RESPONSE, receive_data,
                                 len);
            }
            else
            {
                APP_PRINT_ERROR0("dp sedentary len is over");
            }
        }
        break;
    case DP_ID_MILEEAGE_MIN:
        break;
    case DP_ID_SCREEN_BRIGHT_VALUE:
        break;
    default:
        break;
    }
}
void dp_enum_data_parse(uint8_t   *receive_data, uint16_t len)
{

    uint8_t dp_id = 0;
    dp_id = receive_data[0];
    APP_PRINT_INFO1("dp_enum_data_parse dp_id is %d", dp_id);
    data_info_def data_set;

    switch (dp_id)
    {
    case DP_ID_TIME_MODE:
        if (len <= sizeof(uint8_t))
        {
            uint8_t time_mode_tmp = {0};
            memcpy((uint8_t *)&time_mode_tmp, receive_data + 4, len);
            rtk_kvdb_set(KVDB_TUYA_CFG_NAME, KEY_TUYA_CFG_TIME_MODE, &time_mode_tmp, sizeof(time_mode_tmp));
            dp_data_send_ack(DP_SEND_TYPE_ACTIVE, DP_SEND_FOR_PANEL, DP_SEND_WITH_RESPONSE, receive_data, len);
        }
        else
        {
            APP_PRINT_ERROR0("dp DP_ID_TIME_MODE len is over");
        }
        break;
    case DP_ID_PHOTO_MODE:
        break;
    case DP_ID_LANGUAGE:
        if (len <= sizeof(uint8_t))
        {
            uint8_t language_set = *(receive_data + 4);
            rtk_kvdb_set(KVDB_TUYA_PANEL_INFO_NAME, KEY_TUYA_PANEL_INFO_LANGUAGE, &language_set,
                         sizeof(language_set));
            dp_data_send_ack(DP_SEND_TYPE_ACTIVE, DP_SEND_FOR_PANEL, DP_SEND_WITH_RESPONSE, receive_data, len);
        }
        else
        {
            APP_PRINT_ERROR0("dp DP_ID_TIME_MODE len is over");
        }
        break;
    case DP_ID_BRIGHT_SET:
        {
            dp_data_send_ack(DP_SEND_TYPE_ACTIVE, DP_SEND_FOR_PANEL, DP_SEND_WITH_RESPONSE, receive_data, len);
        }
        break;
    case DP_ID_EXERICE_CONTROL:
        break;
    case DP_ID_EXERICE_TYPE:
        break;
    case DP_ID_DETECTION_START_SELECT:
        break;
    case DP_ID_EXERICE_START_SELECT:
        break;
    case DP_ID_FUNCTION_SELECT:
        break;
    case DP_ID_SCREEN_TIME_SET:
        break;
    case DP_ID_CONTACT_OPERATE:
        break;
    case DP_ID_BLUETOOTH_STATE:
        break;
    default:
        break;
    }
}
void dp_data_classify(tuya_ble_dp_data_received_data_t *receive_data)
{
    uint8_t dp_id = 0, dp_type = 0;
    dp_id = receive_data->p_data[0];
    dp_type = receive_data->p_data[1];
    uint16_t dp_data_len = receive_data->p_data[2] * 256 + receive_data->p_data[3];

    APP_PRINT_INFO3("dp_data_classify dp_type is %d,len is %d,dp_id is %d", dp_type, dp_data_len,
                    dp_id);
    switch (dp_type)
    {
    case DT_VALUE:
        dp_value_data_parse((uint8_t *)receive_data->p_data, dp_data_len);

        break;
    case DT_RAW:
        dp_raw_data_parse((uint8_t *)receive_data->p_data, dp_data_len);
        break;
    case DT_BOOL:
        dp_bool_data_parse(receive_data);
        break;
    case DT_ENUM:
        dp_enum_data_parse((uint8_t *)receive_data->p_data, dp_data_len);
        break;
    case DT_STRING:
        break;
    default:
        break;
    }
}

void notify_push_parse(tuya_ble_passthrough_data_t receive_data)
{
    uint16_t data_length = 0;
    uint8_t utf8_buf[256] = {0};
    APP_PRINT_INFO1("notify_push_parse!!!!! %d", receive_data.p_data[0]);
    //message_type = receive_data.p_data[0];
    data_length = receive_data.p_data[1] * 0xff + receive_data.p_data[2];
    uint16_t utf8_length = 0;
    APP_PRINT_INFO1("receive_data.p_data[3] is %b", TRACE_BINARY(data_length, &receive_data.p_data[3]));
    unicode_2_utf8(&receive_data.p_data[3], &utf8_buf[1], data_length / 2, &utf8_length);
    APP_PRINT_INFO1("notify_push_parse utf8_buf is %b", TRACE_BINARY(utf8_length, utf8_buf));
    utf8_buf[0] = receive_data.p_data[0];
    set_notification(utf8_length + 1, utf8_buf);
//    extern void new_message_in(void);
//    new_message_in();
}

void All_dp_data_report(void)
{
    uint32_t res = 0;
    uint8_t dp_data_report[256] = {0};
    uint16_t len = 0;
    uint16_t i = 0;

    {
        uint32_t steps_total = 0;
        rtk_kvdb_get(KVDB_SPORT_REFS_NAME, KEY_SPORTS_REFS_STEPS_TOTAL, &steps_total, sizeof(steps_total));
        dp_data_report[i++] = DP_ID_STEPS_TOTAL;
        dp_data_report[i++] = DT_VALUE;
        dp_data_report[i++] = 0;
        dp_data_report[i++] = 0x03;
        dp_data_report[i++] = steps_total >> 16 & 0xff;
        dp_data_report[i++] = steps_total >> 8 & 0xff;
        dp_data_report[i++] = steps_total & 0xff;
    }

    {
        uint32_t calories_total = 0;
        rtk_kvdb_get(KVDB_SPORT_REFS_NAME, KEY_SPORTS_REFS_CALORIES_TOTAL, &calories_total,
                     sizeof(calories_total));
        dp_data_report[i++] = DP_ID_CALORIES_TOTAL;
        dp_data_report[i++] = DT_VALUE;
        dp_data_report[i++] = 0;
        dp_data_report[i++] = 0x03;
        dp_data_report[i++] = calories_total >> 16 & 0xff;
        dp_data_report[i++] = calories_total >> 8 & 0xff;
        dp_data_report[i++] = calories_total & 0xff;
    }

    {
        uint32_t distance_total = 0;
        rtk_kvdb_get(KVDB_SPORT_REFS_NAME, KEY_SPORTS_REFS_DISTANCE_TOTAL, &distance_total,
                     sizeof(distance_total));
        dp_data_report[i++] = DP_ID_DISTANCE_TOTAL;
        dp_data_report[i++] = DT_VALUE;
        dp_data_report[i++] = 0;
        dp_data_report[i++] = 0x03;
        dp_data_report[i++] = distance_total >> 16 & 0xff;
        dp_data_report[i++] = distance_total >> 8 & 0xff;
        dp_data_report[i++] = distance_total & 0xff;
    }

    {
        dp_data_report[i++] = DP_ID_BATTERY_PERCENTAGE;
        dp_data_report[i++] = DT_VALUE;
        dp_data_report[i++] = 0;
        dp_data_report[i++] = 0x01;
        dp_data_report[i++] = 99;
    }

    {
        T_DP_SEDENTARY_REMINDER sedentary_reminder = {0};
        rtk_kvdb_get(KVDB_SPORT_REFS_NAME, KEY_SPORTS_REFS_SEDENTARY_REMINDER, &sedentary_reminder,
                     sizeof(sedentary_reminder));
        dp_data_report[i++] = DP_ID_SEDENTARY_REMAINDER;
        dp_data_report[i++] = DT_VALUE;
        dp_data_report[i++] = 0;
        dp_data_report[i++] = 0x07;
        dp_data_report[i++] = sedentary_reminder.sed_switch;
        dp_data_report[i++] = sedentary_reminder.sed_start_hour;
        dp_data_report[i++] = sedentary_reminder.sed_start_min;
        dp_data_report[i++] = sedentary_reminder.sed_stop_hour;
        dp_data_report[i++] = sedentary_reminder.sed_stop_min;
        dp_data_report[i++] = sedentary_reminder.sed_interval >> 8;
        dp_data_report[i++] = sedentary_reminder.sed_interval & 0xff;
    }

    {
        T_DP_TARGET_SET target_set = {0};
        rtk_kvdb_get(KVDB_SPORT_REFS_NAME, KEY_SPORTS_REFS_TARGET_SET, &target_set, sizeof(target_set));
        dp_data_report[i++] = DP_ID_TARGET_SET;
        dp_data_report[i++] = DT_RAW;
        dp_data_report[i++] = 0;
        dp_data_report[i++] = 0x0C;
        dp_data_report[i++] = target_set.steps_target >> 24 & 0xff;
        dp_data_report[i++] = target_set.steps_target >> 16 & 0xff;
        dp_data_report[i++] = target_set.steps_target >> 8 & 0xff;
        dp_data_report[i++] = target_set.steps_target & 0xff;
        dp_data_report[i++] = target_set.calories_target >> 24 & 0xff;
        dp_data_report[i++] = target_set.calories_target >> 16 & 0xff;
        dp_data_report[i++] = target_set.calories_target >> 8 & 0xff;
        dp_data_report[i++] = target_set.calories_target & 0xff;
        dp_data_report[i++] = target_set.distance_target >> 24 & 0xff;
        dp_data_report[i++] = target_set.distance_target >> 16 & 0xff;
        dp_data_report[i++] = target_set.distance_target >> 8 & 0xff;
        dp_data_report[i++] = target_set.distance_target & 0xff;
    }

    {
        T_DP_SLEEP_SET sleep_set = {0};
        rtk_kvdb_get(KVDB_SPORT_REFS_NAME, KEY_SPORTS_REFS_SLEEP_SET, &sleep_set, sizeof(sleep_set));
        dp_data_report[i++] = DP_ID_SLEEP_SET;
        dp_data_report[i++] = DT_RAW;
        dp_data_report[i++] = 0;
        dp_data_report[i++] = 0x05;
        dp_data_report[i++] = sleep_set.sleep_set_switch;
        dp_data_report[i++] = sleep_set.start_hour;
        dp_data_report[i++] = sleep_set.start_min;
        dp_data_report[i++] = sleep_set.end_hour;
        dp_data_report[i++] = sleep_set.end_min;

        dp_data_report[i++] = DP_ID_NOTIFICATION_PUSH;
        dp_data_report[i++] = DT_RAW;
        dp_data_report[i++] = 0;
        dp_data_report[i++] = 16;
    }

    {
        T_DP_NOTIFY_SET notify_set;
        len = sizeof(notify_set);
        rtk_kvdb_get(KVDB_TUYA_CFG_NAME, KEY_TUYA_CFG_NOTIFY_SET, &notify_set, len);
        memcpy(&dp_data_report[i], notify_set.data, len);
        i += len;
        APP_PRINT_INFO1("sizeof(notify_set.data) IS %d", len);
    }

    {
        dp_data_report[i++] = DP_ID_CLOCK_SET;
        dp_data_report[i++] = DT_RAW;
        dp_data_report[i++] = 0;
        dp_data_report[i++] = 25;
    }

    {
        T_DP_CLOCK_SET clock_set;
        len = sizeof(clock_set);
        rtk_kvdb_get(KVDB_TUYA_CFG_NAME, KEY_TUYA_CFG_CLOCK_SET, &clock_set, len);
        memcpy(dp_data_report + i, (uint8_t *)(&clock_set), len);
        i += len;
    }

    res = tuya_ble_dp_data_send(counter_sn, DP_SEND_TYPE_ACTIVE, DP_SEND_FOR_PANEL,
                                DP_SEND_WITH_RESPONSE, dp_data_report, i);
    APP_PRINT_INFO1("send all dp data is %b", TRACE_BINARY(i, dp_data_report));
    APP_PRINT_INFO1("res is %d", res);
}
void All_dp_data_init(void)
{
    return;
}
void dp_data_report(uint8_t dp_id, void *data_buf, uint16_t dp_len, uint8_t dp_type)
{
    uint8_t dp_data_report[128] = {0};
    if (dp_len > (128 - 4))
    {
        return;
    }
    dp_data_report[0] = dp_id;
    dp_data_report[1] = dp_type;
    dp_data_report[2] = dp_len >> 8;
    dp_data_report[3] = dp_len & 0xff;
    memcpy(dp_data_report + 4, data_buf, dp_len);

}

void gsrData_package(data_info_def *gsr_data_tx)
{
    uint32_t res = 0;
    uint8_t dp_type = gsr_data_tx->data_child_type;
    uint16_t dp_len = gsr_data_tx->data_length;
    tx_dp_data[0] = 0;
    tx_dp_data[1] = DT_VALUE;
    tx_dp_data[2] = dp_len >> 8;
    tx_dp_data[3] = dp_len & 0xff;

    switch (dp_type)
    {
    case GSR_STEPS_TOTAL:
        tx_dp_data[0] = 1;
        break;
    case GSR_CAL_TOTAL:
        tx_dp_data[0] = 2;
        break;
    case GSR_DISC_TOTAL:
        tx_dp_data[0] = 3;
        break;
    case GSR_ALL_TARGET:
        tx_dp_data[0] = 5;
        break;
    case GSR_SEDENTARY_TIME:
        tx_dp_data[0] = 6;
        break;
    case GSR_SLEEP_SET:
        tx_dp_data[0] = 7;
        break;
    case GSR_MIN_CAL:
        tx_dp_data[0] = 11;
        break;
    case GSR_MIN_MILE:
        tx_dp_data[0] = 12;
        break;
    case GSR_MIN_STEP:
        tx_dp_data[0] = 10;
        break;
    default:
        break;
    }
    if (dp_len > MAX_TX_LEN - 4 || (dp_len == 0))
    {
        APP_PRINT_INFO1("dp_len > MAX_TX_LEN - 4 %d", dp_len);
        return;
    }
    memcpy(tx_dp_data + 4, gsr_data_tx->data_buf, dp_len);
    res = tuya_ble_dp_data_send(counter_sn++, DP_SEND_TYPE_ACTIVE, DP_SEND_FOR_PANEL,
                                DP_SEND_WITHOUT_RESPONSE, tx_dp_data, dp_len);
//    APP_PRINT_INFO1("send all dp data is %b", TRACE_BINARY(i, dp_data_report));
    APP_PRINT_INFO1("gsrData_package res is %d", res);
}
void hrsData_package(data_info_def *data_buf_in)
{

}
void dp_data_package(data_info_def *data_buf_in)
{
    uint8_t dp_type = data_buf_in->data_type;
    switch (dp_type)
    {
    case GSR_DATA:
        gsrData_package(data_buf_in);
        break;
    case HRS_DATA:

        break;
    default:
        break;
    }
}

void unicode_2_utf8(uint8_t *unicode_data, uint8_t *utf8_char, uint8_t unicode_len,
                    uint16_t *utf8_len)
{
    uint16_t j = 0;
    for (uint16_t i = 0; i < unicode_len; i++)
    {
        uint32_t uni_tmp = *((uint16_t *)unicode_data + i);
        APP_PRINT_INFO1("uni_tmp IS %x", uni_tmp);
        if (uni_tmp <= 0x0000007f)
        {
            if (j > MAX_UTF8_LEN - 2)
            {
                return;
            }
            *(utf8_char + j) = uni_tmp & 0x7f;
            j += 1;
        }
        else if (uni_tmp >= 0x00000080 && uni_tmp <= 0x000007ff)
        {
            if (j > MAX_UTF8_LEN - 3)
            {
                return;
            }
            *(utf8_char + j + 1) = (uni_tmp & 0x3f) | 0x80;
            *(utf8_char + j) = ((uni_tmp >> 6) & 0x1f) | 0xC0;
            j += 2;
        }
        else if (uni_tmp >= 0x00000800 && uni_tmp <= 0x0000FFFF)
        {
            if (j > MAX_UTF8_LEN - 4)
            {
                return;
            }
            // * U-00000800 - U-0000FFFF:  1110xxxx 10xxxxxx 10xxxxxx
            *(utf8_char + j + 2) = (uni_tmp & 0x3F) | 0x80;
            *(utf8_char + j + 1) = ((uni_tmp >>  6) & 0x3F) | 0x80;
            *(utf8_char + j)   = ((uni_tmp >> 12) & 0x0F) | 0xE0;
            j += 3;
        }
        else if (uni_tmp >= 0x00010000 && uni_tmp <= 0x001FFFFF)
        {
            if (j > MAX_UTF8_LEN - 5)
            {
                return;
            }
            // * U-00010000 - U-001FFFFF:  11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
            *(utf8_char + j + 3) = (uni_tmp & 0x3F) | 0x80;
            *(utf8_char + j + 2) = ((uni_tmp >>  6) & 0x3F) | 0x80;
            *(utf8_char + j + 1) = ((uni_tmp >> 12) & 0x3F) | 0x80;
            *(utf8_char + j)   = ((uni_tmp >> 18) & 0x07) | 0xF0;
            j += 4;
        }
        else if (uni_tmp >= 0x00200000 && uni_tmp <= 0x03FFFFFF)
        {
            if (j > MAX_UTF8_LEN - 6)
            {
                return;
            }
            // * U-00200000 - U-03FFFFFF:  111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
            *(utf8_char + j + 4) = (uni_tmp & 0x3F) | 0x80;
            *(utf8_char + j + 3) = ((uni_tmp >>  6) & 0x3F) | 0x80;
            *(utf8_char + j + 2) = ((uni_tmp >> 12) & 0x3F) | 0x80;
            *(utf8_char + j + 1) = ((uni_tmp >> 18) & 0x3F) | 0x80;
            *(utf8_char + j)   = ((uni_tmp >> 24) & 0x03) | 0xF8;
            j += 5;
        }
        else if (uni_tmp >= 0x04000000 && uni_tmp <= 0x7FFFFFFF)
        {
            if (j > MAX_UTF8_LEN - 7)
            {
                return;
            }
            // * U-04000000 - U-7FFFFFFF:  1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
            *(utf8_char + j + 5) = (uni_tmp & 0x3F) | 0x80;
            *(utf8_char + j + 4) = ((uni_tmp >>  6) & 0x3F) | 0x80;
            *(utf8_char + j + 3) = ((uni_tmp >> 12) & 0x3F) | 0x80;
            *(utf8_char + j + 2) = ((uni_tmp >> 18) & 0x3F) | 0x80;
            *(utf8_char + j + 1) = ((uni_tmp >> 24) & 0x3F) | 0x80;
            *(utf8_char + j)   = ((uni_tmp >> 30) & 0x01) | 0xFC;
            j += 6;
        }

    }
    *(utf8_char + j) = '\0';
    *utf8_len = j + 1;

}
