#ifndef __PUBLIC_INC_QVOICE_CASR_H__
#define __PUBLIC_INC_QVOICE_CASR_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*qvoice_casr_cb)(const char *res, int len, unsigned int sframe,
                              unsigned int eframe, void *upval, int id,
                              const char *post, int len2);

typedef int (*qvoice_casr_vad_cb)(const int res_type, void *upval);

void *qvoice_casr_new(void);
void qvoice_casr_delete(void *inst);
int qvoice_casr_feed(void *inst, short *audio, int len);
void qvoice_casr_set_notify(void *inst, void *upval, qvoice_casr_cb cb);
void qvoice_casr_set_vad_notify(void *inst, void *upval, qvoice_casr_vad_cb vcb);
int qvoice_casr_feed_end(void *inst);
void qvoice_casr_set_res(const unsigned char *vad,
                         const unsigned char *asr1,//const unsigned char *asr2,
                         const unsigned char *fsm, const unsigned char *sym,
                         const unsigned char *parm);

#ifdef __cplusplus
};
#endif
#endif
