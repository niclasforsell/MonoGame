#pragma once

#include "predecls.h"
#include <sceerror.h>
#include <camera.h>
#include <user_service.h>

#if SCE_ORBIS_SDK_VERSION >= 0x01700081u // SDK Version 1.7
#include <device_service/device_service_error.h>
#endif

namespace Input {

enum class CameraResult
{
	Ok = SCE_OK,

	ErrorParam = SCE_CAMERA_ERROR_PARAM,
	ErrorAlreadyInit = SCE_CAMERA_ERROR_ALREADY_INIT,
	ErrorNotInit = SCE_CAMERA_ERROR_NOT_INIT,
	ErrorAlreadyOpen = SCE_CAMERA_ERROR_ALREADY_OPEN,
	ErrorNotOPen = SCE_CAMERA_ERROR_NOT_OPEN,
	ErrorAlreadyStart = SCE_CAMERA_ERROR_ALREADY_START,
	ErrorNotStart = SCE_CAMERA_ERROR_NOT_START,
	ErrorFormatUnknown = SCE_CAMERA_ERROR_FORMAT_UNKNOWN,
	ErrorResolutionUnknown = SCE_CAMERA_ERROR_RESOLUTION_UNKNOWN,
	ErrorBadFramerate = SCE_CAMERA_ERROR_BAD_FRAMERATE,
	ErrorTimeout = SCE_CAMERA_ERROR_TIMEOUT,
	ErrorAttributeUnknown = SCE_CAMERA_ERROR_ATTRIBUTE_UNKNOWN,
	ErrorBusy = SCE_CAMERA_ERROR_BUSY,
	ErrorUnknownConfig = SCE_CAMERA_ERROR_UNKNOWN_CONFIG,
	ErrorAlreadyRead = SCE_CAMERA_ERROR_ALREADY_READ,
	ErrorNotConnected = SCE_CAMERA_ERROR_NOT_CONNECTED,
	ErrorNotSupported = SCE_CAMERA_ERROR_NOT_SUPPORTED,
	ErrorInvalidConfig = SCE_CAMERA_ERROR_INVALID_CONFIG,
	ErrorMaxHandle = SCE_CAMERA_ERROR_MAX_HANDLE,
	ErrorMaxProcess = SCE_CAMERA_ERROR_MAX_PROCESS,
	ErrorCopyOutFailed = SCE_CAMERA_ERROR_COPYOUT_FAILED,
	ErrorCopyInFailed = SCE_CAMERA_ERROR_COPYIN_FAILED,
	ErrorKProcCreate = SCE_CAMERA_ERROR_KPROC_CREATE,
	ErrorFatal = SCE_CAMERA_ERROR_FATAL,

#if SCE_ORBIS_SDK_VERSION >= 0x01700081u // SDK Version 1.7
    ErrorInvalidUser = SCE_DEVICE_SERVICE_ERROR_INVALID_USER,
    ErrorInvalidDeviceType = SCE_DEVICE_SERVICE_ERROR_INVALID_DEVICE_TYPE,
    ErrorInvalidIndex = SCE_DEVICE_SERVICE_ERROR_INVALID_INDEX,
    ErrorMemory = SCE_DEVICE_SERVICE_ERROR_MEMORY,
    ErrorInvalidParam = SCE_DEVICE_SERVICE_ERROR_INVALID_PARAM,
    ErrorUserNotLogin = SCE_DEVICE_SERVICE_ERROR_USER_NOT_LOGIN,
    ErrorUserOverMax = SCE_DEVICE_SERVICE_ERROR_USER_OVER_MAX,
    ErrorDeviceOverMax = SCE_DEVICE_SERVICE_ERROR_DEVICE_OVER_MAX,
    ErrorBusOverMax = SCE_DEVICE_SERVICE_ERROR_BUS_OVER_MAX,
    ErrorPortOverMax = SCE_DEVICE_SERVICE_ERROR_PORT_OVER_MAX,
    ErrorInternal = SCE_DEVICE_SERVICE_ERROR_INTERNAL,
#endif
};

enum class CameraBaseFormat
{
	Yuv422 = SCE_CAMERA_FORMAT_YUV422,
	Raw16 = SCE_CAMERA_FORMAT_RAW16,
	Raw8 = SCE_CAMERA_FORMAT_RAW8,
	NoUse = SCE_CAMERA_FORMAT_NO_USE,
	Unknown = SCE_CAMERA_FORMAT_UNKNOWN
};

enum class CameraChannel
{
	Channel0 = SCE_CAMERA_CHANNEL_0,
	Channel1 = SCE_CAMERA_CHANNEL_1,
	ChannelBoth = SCE_CAMERA_CHANNEL_BOTH
};

enum class CameraConfigType
{
	Type1 = SCE_CAMERA_CONFIG_TYPE1,
	Type2 = SCE_CAMERA_CONFIG_TYPE2,
	Type3 = SCE_CAMERA_CONFIG_TYPE3,
	Type4 = SCE_CAMERA_CONFIG_TYPE4,
	Extension = SCE_CAMERA_CONFIG_EXTENTION
};

enum class CameraFrameRate
{
	Unknown = SCE_CAMERA_FRAMERATE_UNKNOWN,
	Fps_7_5 = SCE_CAMERA_FRAMERATE_7_5,
	Fps_15 = SCE_CAMERA_FRAMERATE_15,
	Fps_30 = SCE_CAMERA_FRAMERATE_30,
	Fps_60 = SCE_CAMERA_FRAMERATE_60,
	Fps_120 = SCE_CAMERA_FRAMERATE_120,
	Fps_240 = SCE_CAMERA_FRAMERATE_240,
};

CS_FLAGS enum class CameraFrameFormat
{
	Level0 = SCE_CAMERA_FRAME_FORMAT_LEVEL0,
	Level1 = SCE_CAMERA_FRAME_FORMAT_LEVEL1,
	Level2 = SCE_CAMERA_FRAME_FORMAT_LEVEL2,
	Level3 = SCE_CAMERA_FRAME_FORMAT_LEVEL3,
	All = SCE_CAMERA_FRAME_FORMAT_LEVEL_ALL
};

enum class CameraResolution
{
	Res_1280x800 = SCE_CAMERA_RESOLUTION_1280X800,
	Res_640x400 = SCE_CAMERA_RESOLUTION_640X400,
	Res_320x200 = SCE_CAMERA_RESOLUTION_320X200,
	Res_160x100 = SCE_CAMERA_RESOLUTION_160X100,
	SpecifiedWidthHeight = SCE_CAMERA_RESOLUTION_SPECIFIED_WIDTH_HEIGHT,
	Unknown = SCE_CAMERA_RESOLUTION_UNKNOWN,
};

enum class CameraScaleFormat
{
	Yuv422 = SCE_CAMERA_SCALE_FORMAT_YUV422,
	Y16 = SCE_CAMERA_SCALE_FORMAT_Y16,
	Y8 = SCE_CAMERA_SCALE_FORMAT_Y8,
	NoUse = SCE_CAMERA_SCALE_FORMAT_NO_USE,
	Unknown = SCE_CAMERA_SCALE_FORMAT_UNKNOWN
};

enum class CameraFrameMemoryType
{
	Onion = SCE_CAMERA_FRAME_MEMORY_TYPE_ONION,
	Garlic = SCE_CAMERA_FRAME_MEMORY_TYPE_GARLIC
};

enum class CameraFrameWaitMode
{
	NextFrameOn = SCE_CAMERA_FRAME_WAIT_NEXTFRAME_ON,
	NextFrameOff = SCE_CAMERA_FRAME_WAIT_NEXTFRAME_OFF,

#if SCE_ORBIS_SDK_VERSION >= 0x01700081u // SDK Version 1.7
	NextFrameOnMode1 = SCE_CAMERA_FRAME_WAIT_NEXTFRAME_ON_MODE1,
	NextFrameOffMode1 = SCE_CAMERA_FRAME_WAIT_NEXTFRAME_OFF_MODE1,
#endif
};

enum class CameraStatus
{
	IsActive = SCE_CAMERA_STATUS_IS_ACTIVE,
	IsNotActive = SCE_CAMERA_STATUS_IS_NOT_ACTIVE,
	IsAlreadyRead = SCE_CAMERA_STATUS_IS_ALREADY_READ,
	IsNotStable = SCE_CAMERA_STATUS_IS_NOT_STABLE,
	IsInvalidFrame  = SCE_CAMERA_STATUS_IS_INVALID_FRAME,
	IsInvalidMetaData = SCE_CAMERA_STATUS_IS_INVALID_META_DATA,
};

struct CS_VALUE_TYPE CameraFrameData
{
	void* Frame_0_0;
	void* Frame_0_1;
	void* Frame_0_2;
	void* Frame_0_3;

	void* Frame_1_0;
	void* Frame_1_1;
	void* Frame_1_2;
	void* Frame_1_3;

	time_t Timestamp;

	uint32_t Frame_0_0_Size;
	uint32_t Frame_0_1_Size;
	uint32_t Frame_0_2_Size;
	uint32_t Frame_0_3_Size;

	uint32_t Frame_1_0_Size;
	uint32_t Frame_1_1_Size;
	uint32_t Frame_1_2_Size;
	uint32_t Frame_1_3_Size;

	CameraStatus Status0;
	CameraStatus Status1;
};

class CS_API Camera
{
	CS_IGNORE int32_t _handle;

	Camera();
	~Camera();

public:
	static bool  IsAttached();

	CameraResult Open();
	CameraResult Stop();
	CameraResult Close();

	CameraResult SetConfigType(CameraConfigType configType);

	CameraResult Start(CameraFrameFormat camera1format, CameraFrameFormat camera2Format);

	CameraResult GetExposureGain(CameraChannel channel,
		CS_OUT uint32_t* exposureControl,
		CS_OUT uint32_t* exposure,
		CS_OUT uint32_t* gain,
		CS_OUT uint32_t* mode);

	CameraResult SetExposureGain(CameraChannel channel,
		uint32_t exposureControl,
		uint32_t exposure,
		uint32_t gain,
		uint32_t mode);

	CameraResult GetWhiteBalance(CameraChannel channel,
		CS_OUT uint32_t* whiteBalanceControl,
		CS_OUT uint32_t* gainRed,
		CS_OUT uint32_t* gainBlue,
		CS_OUT uint32_t* gainGreen);

	CameraResult SetWhiteBalance(CameraChannel channel,
		uint32_t whiteBalanceControl,
		uint32_t gainRed,
		uint32_t gainBlue,
		uint32_t gainGreen);

	CameraResult GetGamma(CameraChannel channel,
		CS_OUT uint32_t* gammaControl,
		CS_OUT uint32_t* value);

	CameraResult SetGamma(CameraChannel channel,
		uint32_t gammaControl,
		uint32_t value);

	CameraResult GetSaturation(CameraChannel channel, CS_OUT uint32_t* saturation);
	CameraResult SetSaturation(CameraChannel channel, uint32_t saturation);

	CameraResult GetContrast(CameraChannel channel, CS_OUT uint32_t* contrast);
	CameraResult SetContrast(CameraChannel channel, uint32_t contrast);

	CameraResult GetSharpness(CameraChannel channel, CS_OUT uint32_t* sharpness);
	CameraResult SetSharpness(CameraChannel channel, uint32_t sharpness);

	CameraResult GetHue(CameraChannel channel, CS_OUT int32_t* hue);
	CameraResult SetHue(CameraChannel channel, int32_t hue);

	CameraResult GetDefectivePixelCancellation(CameraChannel channel, CS_OUT uint32_t* isEnabled);
	CameraResult SetDefectivePixelCancellation(CameraChannel channel, uint32_t isEnabled);

	CameraResult GetLensCorrection(CameraChannel channel, CS_OUT uint32_t* isEnabled);
	CameraResult SetLensCorrection(CameraChannel channel, uint32_t isEnabled);

	CameraResult GetFrameData(CameraFrameMemoryType readMode,
		CameraFrameWaitMode waitMode,
		CS_OUT CameraFrameData* outData,
		CS_OUT bool* isValid);
};

} // namespace Input
