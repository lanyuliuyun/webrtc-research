
#ifndef H264_RTP_PACKER_H
#define H264_RTP_PACKER_H

#ifdef __cplusplus
extern "C" {
#endif

struct h264_rtp_packer;
typedef struct h264_rtp_packer h264_rtp_packer_t;

typedef void (*rtp_sink_f)(const unsigned char *rtp_packet, unsigned len, void *userdata);

/* max_playload_len 表示rtp负载部分最大容量，不含RTP包头12字节的内容，使用者自行确定RTP封包完毕之后，整体IP包不超过MTU值 
 * 最终最输出的RTP包尺寸最大为 12+max_playload_len
 */
h264_rtp_packer_t* h264_rtp_packer_new(unsigned pt, unsigned init_sn, unsigned ssrc, unsigned max_playload_len, rtp_sink_f sink, void *userdata);

void h264_rtp_packer_destroy(h264_rtp_packer_t* packer);

/* ts表示nalu所属的图像帧的时间戳，单位与RTP包头中的时戳单位相同，
 * is_last表示给定的nalu是否是对应Access Unit最后一个nalu,用于确定rtp封包的mark标记 
 */
void h264_rtp_packer_pack(h264_rtp_packer_t* packer, const unsigned char *nalu, unsigned len, unsigned ts, int is_last);
#ifdef __cplusplus
}
#endif

#endif
