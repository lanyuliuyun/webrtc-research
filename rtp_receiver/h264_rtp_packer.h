
#ifndef H264_RTP_PACKER_H
#define H264_RTP_PACKER_H

#ifdef __cplusplus
extern "C" {
#endif

struct h264_rtp_packer;
typedef struct h264_rtp_packer h264_rtp_packer_t;

typedef void (*rtp_sink_f)(const unsigned char *rtp_packet, unsigned len, void *userdata);

/* max_playload_len ��ʾrtp���ز����������������RTP��ͷ12�ֽڵ����ݣ�ʹ��������ȷ��RTP������֮������IP��������MTUֵ 
 * �����������RTP���ߴ����Ϊ 12+max_playload_len
 */
h264_rtp_packer_t* h264_rtp_packer_new(unsigned pt, unsigned init_sn, unsigned ssrc, unsigned max_playload_len, rtp_sink_f sink, void *userdata);

void h264_rtp_packer_destroy(h264_rtp_packer_t* packer);

/* ts��ʾnalu������ͼ��֡��ʱ�������λ��RTP��ͷ�е�ʱ����λ��ͬ��
 * is_last��ʾ������nalu�Ƿ��Ƕ�ӦAccess Unit���һ��nalu,����ȷ��rtp�����mark��� 
 */
void h264_rtp_packer_pack(h264_rtp_packer_t* packer, const unsigned char *nalu, unsigned len, unsigned ts, int is_last);
#ifdef __cplusplus
}
#endif

#endif
