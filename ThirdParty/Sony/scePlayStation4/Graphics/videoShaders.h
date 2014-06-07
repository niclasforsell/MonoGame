#pragma once

struct VideoShaderConstants
{
	float pos_top;
	float pos_left;
	float pos_right;
	float pos_bottom;

	float tex_top;
	float tex_left;
	float tex_right;
	float tex_bottom;
};

const uint8_t video_vv_pssl_sb[] = {
  0x00, 0x03, 0xe2, 0x19, 0x90, 0x00, 0xf6, 0x27, 0xb4, 0x0b, 0x98, 0x17,
  0x00, 0x01, 0x00, 0x00, 0x7c, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x53, 0x68, 0x64, 0x72, 0x07, 0x00, 0x01, 0x00, 0x01, 0x0c, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x3c, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
  0x30, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0x89, 0x00, 0x0c, 0x00,
  0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x02, 0x00, 0x04, 0x00,
  0x10, 0x00, 0x00, 0x00, 0xff, 0x03, 0xeb, 0xbe, 0x23, 0x00, 0x00, 0x00,
  0x00, 0x05, 0x84, 0xc2, 0x04, 0x05, 0x82, 0xc2, 0x7f, 0x00, 0x8c, 0xbf,
  0x09, 0x02, 0x02, 0x7e, 0x01, 0x03, 0x10, 0x7e, 0x08, 0x02, 0x12, 0x7e,
  0x01, 0x03, 0x18, 0x7e, 0x0b, 0x02, 0x02, 0x7e, 0x01, 0x03, 0x1a, 0x7e,
  0x0a, 0x02, 0x04, 0x7e, 0x02, 0x03, 0x20, 0x7e, 0x08, 0x02, 0x22, 0x7e,
  0x02, 0x03, 0x28, 0x7e, 0x01, 0x03, 0x2a, 0x7e, 0x84, 0x00, 0x00, 0x34,
  0x80, 0x02, 0x0e, 0x7e, 0x80, 0x00, 0x04, 0x7d, 0x07, 0x11, 0x0e, 0x00,
  0x90, 0x00, 0x04, 0x7d, 0x07, 0x19, 0x0e, 0x00, 0xa0, 0x00, 0x04, 0x7d,
  0x07, 0x21, 0x0e, 0x00, 0xb0, 0x00, 0x04, 0x7d, 0x07, 0x29, 0x02, 0x00,
  0x80, 0x02, 0x0e, 0x7e, 0x80, 0x00, 0x04, 0x7d, 0x07, 0x13, 0x0e, 0x00,
  0x90, 0x00, 0x04, 0x7d, 0x07, 0x1b, 0x0e, 0x00, 0xa0, 0x00, 0x04, 0x7d,
  0x07, 0x23, 0x0e, 0x00, 0xb0, 0x00, 0x04, 0x7d, 0x07, 0x2b, 0x04, 0x00,
  0x05, 0x02, 0x06, 0x7e, 0x03, 0x03, 0x30, 0x7e, 0x04, 0x02, 0x32, 0x7e,
  0x03, 0x03, 0x38, 0x7e, 0x07, 0x02, 0x06, 0x7e, 0x03, 0x03, 0x3a, 0x7e,
  0x06, 0x02, 0x08, 0x7e, 0x04, 0x03, 0x40, 0x7e, 0x04, 0x02, 0x42, 0x7e,
  0x04, 0x03, 0x48, 0x7e, 0x03, 0x03, 0x4a, 0x7e, 0x80, 0x02, 0x0e, 0x7e,
  0x80, 0x00, 0x04, 0x7d, 0x07, 0x31, 0x0e, 0x00, 0x90, 0x00, 0x04, 0x7d,
  0x07, 0x39, 0x0e, 0x00, 0xa0, 0x00, 0x04, 0x7d, 0x07, 0x41, 0x0e, 0x00,
  0xb0, 0x00, 0x04, 0x7d, 0x07, 0x49, 0x06, 0x00, 0x80, 0x02, 0x0e, 0x7e,
  0x80, 0x00, 0x04, 0x7d, 0x07, 0x33, 0x0e, 0x00, 0x90, 0x00, 0x04, 0x7d,
  0x07, 0x3b, 0x0e, 0x00, 0xa0, 0x00, 0x04, 0x7d, 0x07, 0x43, 0x0e, 0x00,
  0xb0, 0x00, 0x04, 0x7d, 0x07, 0x4b, 0x00, 0x00, 0x80, 0x02, 0x08, 0x7e,
  0xf2, 0x02, 0x0a, 0x7e, 0xcf, 0x08, 0x00, 0xf8, 0x01, 0x02, 0x04, 0x05,
  0x0f, 0x02, 0x00, 0xf8, 0x03, 0x00, 0x04, 0x05, 0x00, 0x00, 0x81, 0xbf,
  0x02, 0x00, 0x04, 0x00, 0x4f, 0x72, 0x62, 0x53, 0x68, 0x64, 0x72, 0x07,
  0x45, 0x1c, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x90, 0x00, 0xf6, 0x27,
  0xb4, 0x0b, 0x98, 0x17, 0x8c, 0x2f, 0xf0, 0x9b, 0x01, 0x00, 0x00, 0x00,
  0x08, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x01, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x8c, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x16, 0x04, 0x6a, 0x00,
  0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x54, 0x02, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x16, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x44, 0x02, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x2c, 0x02, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x16, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x15, 0x02, 0x00, 0x00,
  0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x16, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0xff, 0x01, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00,
  0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0xea, 0x01, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x16, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xd2, 0x01, 0x00, 0x00,
  0x18, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x16, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0xbb, 0x01, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x00,
  0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0xa5, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x44, 0x01, 0x00, 0x00,
  0x8c, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00,
  0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x28, 0x01, 0x00, 0x00,
  0x68, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00,
  0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0d, 0x01, 0x00, 0x00,
  0x44, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x00,
  0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf3, 0x00, 0x00, 0x00,
  0x20, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00,
  0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xda, 0x00, 0x00, 0x00,
  0xfc, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00,
  0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xbe, 0x00, 0x00, 0x00,
  0xd8, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00,
  0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xa3, 0x00, 0x00, 0x00,
  0xb4, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x00,
  0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x89, 0x00, 0x00, 0x00,
  0x90, 0x00, 0x00, 0x00, 0x0c, 0x2f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x8e, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x03, 0x2e, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x89, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x00,
  0x01, 0x24, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x82, 0x00, 0x00, 0x00,
  0x86, 0x00, 0x00, 0x00, 0x53, 0x68, 0x61, 0x64, 0x65, 0x72, 0x43, 0x6f,
  0x6e, 0x73, 0x74, 0x61, 0x6e, 0x74, 0x73, 0x00, 0x70, 0x6f, 0x73, 0x5f,
  0x74, 0x6f, 0x70, 0x00, 0x70, 0x6f, 0x73, 0x5f, 0x6c, 0x65, 0x66, 0x74,
  0x00, 0x70, 0x6f, 0x73, 0x5f, 0x72, 0x69, 0x67, 0x68, 0x74, 0x00, 0x70,
  0x6f, 0x73, 0x5f, 0x62, 0x6f, 0x74, 0x74, 0x6f, 0x6d, 0x00, 0x74, 0x65,
  0x78, 0x5f, 0x74, 0x6f, 0x70, 0x00, 0x74, 0x65, 0x78, 0x5f, 0x6c, 0x65,
  0x66, 0x74, 0x00, 0x74, 0x65, 0x78, 0x5f, 0x72, 0x69, 0x67, 0x68, 0x74,
  0x00, 0x74, 0x65, 0x78, 0x5f, 0x62, 0x6f, 0x74, 0x74, 0x6f, 0x6d, 0x00,
  0x28, 0x6e, 0x6f, 0x5f, 0x6e, 0x61, 0x6d, 0x65, 0x29, 0x00, 0x76, 0x65,
  0x72, 0x74, 0x65, 0x78, 0x2e, 0x76, 0x69, 0x64, 0x00, 0x6d, 0x61, 0x69,
  0x6e, 0x2e, 0x70, 0x6f, 0x73, 0x00, 0x6d, 0x61, 0x69, 0x6e, 0x2e, 0x75,
  0x76, 0x00, 0x54, 0x45, 0x58, 0x43, 0x4f, 0x4f, 0x52, 0x44, 0x00, 0x00
};

const uint8_t video_p_pssl_sb[] = {
  0x00, 0x03, 0xe2, 0x19, 0xb9, 0x48, 0x0b, 0x27, 0x18, 0x63, 0x5b, 0xa4,
  0x01, 0x01, 0x00, 0x00, 0xa8, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x53, 0x68, 0x64, 0x72, 0x07, 0x00, 0x01, 0x00, 0x02, 0x15, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x44, 0x01, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00,
  0x54, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xc1, 0x00, 0x0c, 0x00,
  0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00,
  0x02, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00,
  0x01, 0x00, 0x00, 0x00, 0x1b, 0x01, 0x02, 0x00, 0x00, 0x00, 0x04, 0x03,
  0x01, 0x00, 0x0c, 0x00, 0x00, 0x01, 0x10, 0x03, 0x01, 0x01, 0x18, 0x00,
  0x10, 0x00, 0x00, 0x00, 0xff, 0x03, 0xeb, 0xbe, 0x24, 0x00, 0x00, 0x00,
  0x7e, 0x04, 0x9c, 0xbe, 0x7e, 0x0a, 0xfe, 0xbe, 0x10, 0x03, 0xfc, 0xbe,
  0x00, 0x00, 0x08, 0xc8, 0x01, 0x00, 0x09, 0xc8, 0x00, 0x01, 0x00, 0xc8,
  0x01, 0x01, 0x01, 0xc8, 0x00, 0x00, 0x02, 0xd0, 0x02, 0x01, 0x01, 0x00,
  0x80, 0x00, 0x08, 0x7c, 0x00, 0x6a, 0x80, 0x88, 0xf2, 0x04, 0x02, 0x7c,
  0x00, 0x6a, 0x80, 0x88, 0xf2, 0x00, 0x02, 0x7c, 0x00, 0x6a, 0xea, 0x88,
  0x6a, 0x24, 0x80, 0xbe, 0x80, 0x02, 0x02, 0x7e, 0x80, 0x02, 0x06, 0x7e,
  0x80, 0x02, 0x08, 0x7e, 0x00, 0x7e, 0xfe, 0x8a, 0x26, 0x00, 0x88, 0xbf,
  0x7e, 0x04, 0xea, 0xbe, 0x7e, 0x0a, 0xfe, 0xbe, 0x6a, 0x04, 0xfe, 0xbe,
  0x08, 0x03, 0x88, 0xc0, 0x00, 0x03, 0xca, 0xc0, 0x00, 0x03, 0x02, 0x7e,
  0x02, 0x03, 0x00, 0x7e, 0x00, 0x01, 0x80, 0xf0, 0x00, 0x02, 0x61, 0x00,
  0x7f, 0x00, 0x8c, 0xbf, 0x00, 0x03, 0x80, 0xf0, 0x00, 0x00, 0x85, 0x00,
  0x71, 0x0f, 0x8c, 0xbf, 0xff, 0x04, 0x04, 0x06, 0x00, 0x00, 0x80, 0xbd,
  0x70, 0x0f, 0x8c, 0xbf, 0xf1, 0x00, 0x00, 0x06, 0xf1, 0x02, 0x02, 0x06,
  0xff, 0x02, 0x06, 0x10, 0x32, 0x77, 0xe5, 0x3f, 0x00, 0x07, 0x06, 0x40,
  0x00, 0x00, 0x00, 0x00, 0x02, 0x07, 0x06, 0x40, 0x0f, 0x0b, 0x95, 0x3f,
  0xff, 0x02, 0x08, 0x10, 0x22, 0x6c, 0x08, 0xbf, 0x00, 0x09, 0x08, 0x40,
  0x51, 0x6b, 0x5a, 0xbe, 0x02, 0x09, 0x08, 0x40, 0x0f, 0x0b, 0x95, 0x3f,
  0x80, 0x02, 0x02, 0x10, 0x00, 0x03, 0x00, 0x40, 0x90, 0x31, 0x07, 0x40,
  0x02, 0x01, 0x00, 0x40, 0x0f, 0x0b, 0x95, 0x3f, 0x03, 0x03, 0x02, 0x7e,
  0x04, 0x03, 0x06, 0x7e, 0x00, 0x03, 0x08, 0x7e, 0x00, 0x04, 0xfe, 0xbe,
  0xf2, 0x02, 0x00, 0x7e, 0x1c, 0x04, 0xfe, 0xbe, 0x01, 0x07, 0x02, 0x5e,
  0x04, 0x01, 0x00, 0x5e, 0x0f, 0x1c, 0x00, 0xf8, 0x01, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x81, 0xbf, 0x1b, 0x01, 0x02, 0x00, 0x00, 0x00, 0x04, 0x03,
  0x01, 0x00, 0x0c, 0x00, 0x00, 0x01, 0x10, 0x03, 0x01, 0x01, 0x18, 0x00,
  0x4f, 0x72, 0x62, 0x53, 0x68, 0x64, 0x72, 0x07, 0x41, 0x14, 0x01, 0x00,
  0x00, 0x05, 0x00, 0x00, 0xb9, 0x48, 0x0b, 0x27, 0x18, 0x63, 0x5b, 0xa4,
  0xc0, 0xa6, 0x38, 0x95, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x54, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x02, 0x05, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x5c, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x02, 0x05, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x4d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x49, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
  0x42, 0x00, 0x00, 0x00, 0x01, 0x24, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
  0x42, 0x00, 0x00, 0x00, 0x4a, 0x00, 0x00, 0x00, 0x03, 0x38, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x47, 0x00, 0x00, 0x00, 0x4e, 0x00, 0x00, 0x00,
  0x79, 0x54, 0x65, 0x78, 0x74, 0x75, 0x72, 0x65, 0x00, 0x63, 0x62, 0x63,
  0x72, 0x54, 0x65, 0x78, 0x74, 0x75, 0x72, 0x65, 0x00, 0x79, 0x53, 0x61,
  0x6d, 0x70, 0x6c, 0x65, 0x72, 0x00, 0x63, 0x62, 0x63, 0x72, 0x53, 0x61,
  0x6d, 0x70, 0x6c, 0x65, 0x72, 0x00, 0x66, 0x72, 0x61, 0x67, 0x6d, 0x65,
  0x6e, 0x74, 0x2e, 0x75, 0x76, 0x00, 0x54, 0x45, 0x58, 0x43, 0x4f, 0x4f,
  0x52, 0x44, 0x00, 0x6d, 0x61, 0x69, 0x6e, 0x2e, 0x63, 0x6f, 0x6c, 0x6f,
  0x72, 0x00, 0x28, 0x6e, 0x6f, 0x5f, 0x6e, 0x61, 0x6d, 0x65, 0x29, 0x00
};


