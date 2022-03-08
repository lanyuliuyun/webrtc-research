# webrtc研究工程

包括模块的独立剥离和功能测试研究。使用的webrtc基础代码版本为 
http://120.92.49.206:3232/chromiumsrc/webrtc.git@gitlab commit 069934e68d6b321edb0da0c23e5dd0bc0581ba52

各个目录说明如下
- common_base 业务无关的公共基础模块
- common_audio 音频相关的公共基础DSP模块
- nack_module 测试nack模块，判断丢包重传
- rtp_receiver 测试RTP收包处理，含ulpfec/red包恢复
- neteq 音频neteq模块的独立剥离
- aec3 音频AEC3模块独立