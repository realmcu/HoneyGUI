#ifndef _WEAR_PACKET_ENCODE_H_
#define _WEAR_PACKET_ENCODE_H_

#include <pb.h>
#include "wear.pb.h"

#ifdef __cplusplus
extern "C" {
#endif

#define _DEBUG_PRINT_     (0UL)    ///> @add by zhengjunbo 2020.10.20

size_t encode_wear_account_proto(Account_AccountID id, Account *account, uint8_t *p_out_buf,
                                 size_t buffer_len);

size_t encode_wear_system_proto(System_SystemID id, System *system, uint8_t *p_out_buf,
                                size_t buffer_len);

size_t encode_wear_watch_face_proto(WatchFace_WatchFaceID id, WatchFace *watchface,
                                    uint8_t *p_out_buf, size_t buffer_len);

size_t encode_wear_nfc_proto(Nfc_NfcID id, Nfc *nfc, uint8_t *p_out_buf, size_t buffer_len);

size_t encode_wear_notification_proto(Notification_NotificationID id, Notification *notification,
                                      uint8_t *p_out_buf, size_t buffer_len);

size_t encode_wear_fitness_proto(Fitness_FitnessID id, Fitness *fitness, uint8_t *p_out_buf,
                                 size_t buffer_len);

size_t encode_wear_lpa_proto(Lpa_LpaID id, Lpa *lpa, uint8_t *p_out_buf, size_t buffer_len);

size_t encode_wear_weather_proto(Weather_WeatherID id, Weather *weather, uint8_t *p_out_buf,
                                 size_t buffer_len);

size_t encode_wear_stock_proto(Stock_StockID id, Stock *stock, uint8_t *p_out_buf,
                               size_t buffer_len);

size_t encode_wear_calendar_proto(Calendar_CalendarID id, Calendar *calendar, uint8_t *p_out_buf,
                                  size_t buffer_len);

size_t encode_wear_facory_proto(Factory_FactoryID id, Factory *factory, uint8_t *p_out_buf,
                                size_t buffer_len);

size_t encode_wear_aivs_proto(Aivs_AivsID id, Aivs *aivs, uint8_t *p_out_buf, size_t buffer_len);

size_t encode_wear_market_proto(Market_MarketID id, Market *market, uint8_t *p_out_buf,
                                size_t buffer_len);

size_t encode_wear_gnss_proto(Gnss_GnssID id, Gnss *gnss, uint8_t *p_out_buf, size_t buffer_len);

size_t encode_wear_clock_proto(Clock_ClockID id, Clock *clock, uint8_t *p_out_buf,
                               size_t buffer_len);

size_t encode_wear_media_proto(Media_MediaID id, Media *media, uint8_t *p_out_buf,
                               size_t buffer_len);

size_t encode_wear_alexa_proto(Alexa_AlexaID id, Alexa *alexa, uint8_t *p_out_buf,
                               size_t buffer_len);

size_t encode_wear_thirdpartyapp_proto(ThirdpartyApp_ThirdpartyAppID id,
                                       ThirdpartyApp *thirdpartyapp, uint8_t *p_out_buf, size_t buffer_len);

size_t encode_wear_interconnect_proto(Interconnection_InterconnectionID id,
                                      Interconnection *interconnect,  uint8_t *p_out_buf, size_t buffer_len);

size_t encode_wear_errorcode_proto(WearPacket_Type type, uint8_t id, ErrorCode *errorcode,
                                   uint8_t *p_out_buf, size_t buffer_len);

///> just for debug print
void print_test(uint8_t *buf, uint16_t buf_len);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _WEAR_PACKET_ENCODE_H_ */
