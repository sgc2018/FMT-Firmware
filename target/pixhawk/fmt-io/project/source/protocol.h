/******************************************************************************
 * Copyright 2020 The Firmament Authors. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/

#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

#include "global.h"
#include "stm32f10x.h"

#define IO_BUFFER_SIZE          256

#define IO_PKT_HEAD             0xFA5C
#define IO_REBOOT_MAGIC         0x315C

#define IO_CODE_SYNC            0x00
#define IO_CODE_REBOOT          0x01
#define IO_CODE_W_ACTUATOR      0x02
#define IO_CODE_CTRL_ACTUATOR   0x03
#define IO_CODE_CONFIG_ACTUATOR 0x05
#define IO_CODE_CONFIG_RC       0x06
#define IO_CODE_RC_DATA         0x07
#define IO_CODE_DBG_TEXT        0x08
#define IO_CODE_RC_STATUS       0x09

#define PKT_SIZE(_pkt)          (sizeof(struct IOPacket) - IO_BUFFER_SIZE + (_pkt)->len)

#pragma pack(push, 1)
struct IOPacket {
    uint16_t head;
    uint8_t code;
    uint16_t len;
    uint8_t crc;
    uint8_t data[IO_BUFFER_SIZE];
};
#pragma pack(pop)

typedef enum {
    RXState_HEAD,
    RXState_CODE,
    RXState_LEN,
    RXState_CRC,
    RXState_DATA,
} IO_RXState;

/* fmtio actuator configuration */
typedef struct {
    uint16_t pwm_freq; // pwm output frequency
} IO_ActuatorConfig;

/* fmtio rc configuration */
typedef struct {
    uint16_t protocol; // 1:sbus 2:ppm
    float sample_time; // rc sample time in seconds (-1 for inherits)
} IO_RCConfig;

/* fmtio rc status flags */
#define IO_RC_FLAG_FRAME_LOST (1 << 0) // receiver reported a skipped frame
#define IO_RC_FLAG_FAILSAFE   (1 << 1) // receiver is in failsafe (link lost)

/* fmtio rc link status, sent periodically by the IO */
#pragma pack(push, 1)
typedef struct {
    uint16_t flags;             // flags of the last decoded frame
    uint32_t frame_count;       // frames decoded successfully
    uint32_t frame_lost_count;  // frames flagged frame-lost (values still forwarded)
    uint32_t failsafe_count;    // frames flagged failsafe (not forwarded)
    uint32_t decode_drop_count; // frames the decoder could not parse
    uint32_t ms_since_rx;       // time since the last byte from the receiver
    uint32_t ms_since_frame;    // time since the last decoded frame
} IO_RCStatus;
#pragma pack(pop)

void init_io_pkt(struct IOPacket* pkt);
struct IOPacket* create_io_pkt(void);
void delete_io_pkt(struct IOPacket* pkt);
FMT_Error set_io_pkt(struct IOPacket* pkt, uint8_t code, void* data, uint16_t len);
FMT_Error io_parse_char(struct IOPacket* pkt, uint8_t c);
uint8_t crc_packet(struct IOPacket* pkt);

#endif
