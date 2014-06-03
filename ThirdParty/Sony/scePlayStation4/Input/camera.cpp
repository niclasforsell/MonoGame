#include "camera.h"
#include <assert.h>
#include <sceerror.h>
#include <memory.h>

using namespace Input;

Camera::Camera()
	: _handle(-1)
{
}

Camera::~Camera()
{
	_handle = -1;
}

bool Camera::IsAttached()
{
	return sceCameraIsAttached(SCE_USER_SERVICE_USER_ID_SYSTEM);
}

CameraResult Camera::Open()
{
	auto ret = sceCameraOpen(SCE_USER_SERVICE_USER_ID_SYSTEM, 0, 0, NULL);
	if (ret < 0)
		return (CameraResult)_handle;

	_handle = ret;

	return CameraResult::Ok;
}

CameraResult Camera::Stop()
{
	return (CameraResult)sceCameraStop(_handle);
}

CameraResult Camera::Close()
{
	auto ret = sceCameraClose(_handle);
	if (ret == SCE_OK)
		_handle = -1;

	return (CameraResult)ret;
}

CameraResult Camera::SetConfigType(CameraConfigType configType)
{
	SceCameraConfig config;
	memset(&config, 0, sizeof(SceCameraConfig));
	config.sizeThis = sizeof(SceCameraConfig);
	config.configType = (SceCameraConfigType)configType;

	return (CameraResult)sceCameraSetConfig(_handle, &config);
}

CameraResult Camera::Start(CameraFrameFormat camera1Format, CameraFrameFormat camera2Format)
{
	SceCameraStartParameter param;
	memset(&param, 0, sizeof(SceCameraStartParameter));
	param.sizeThis = sizeof(SceCameraStartParameter);
	param.formatLevel[0] = (uint32_t)camera1Format;
	param.formatLevel[1] = (uint32_t)camera2Format;

	return (CameraResult)sceCameraStart(_handle, &param);
}

CameraResult Camera::GetExposureGain(CameraChannel channel,
									 CS_OUT uint32_t* exposureControl,
									 CS_OUT uint32_t* exposure,
									 CS_OUT uint32_t* gain,
									 CS_OUT uint32_t* mode)
{
	SceCameraExposureGain result;
	auto ret = sceCameraGetExposureGain(_handle, (SceCameraChannel)channel, &result, NULL);
	*exposureControl = result.exposureControl;
	*exposure = result.exposure;
	*gain = result.gain;
	*mode = result.mode;
	return (CameraResult)ret;
}

CameraResult Camera::SetExposureGain(CameraChannel channel,
									 uint32_t exposureControl,
									 uint32_t exposure,
									 uint32_t gain,
									 uint32_t mode)
{
	SceCameraExposureGain param;
	memset(&param, 0, sizeof(SceCameraExposureGain));
	param.exposureControl = exposureControl;
	param.exposure = exposure;
	param.gain = gain;
	param.mode = mode;
	return (CameraResult)sceCameraSetExposureGain(_handle, (SceCameraChannel)channel, &param, NULL);
}

CameraResult Camera::GetWhiteBalance(CameraChannel channel,
									 CS_OUT uint32_t* whiteBalanceControl,
									 CS_OUT uint32_t* gainRed,
									 CS_OUT uint32_t* gainBlue,
									 CS_OUT uint32_t* gainGreen)
{
	SceCameraWhiteBalance result;
	auto ret = sceCameraGetWhiteBalance(_handle, (SceCameraChannel)channel, &result, NULL);
	*whiteBalanceControl = result.whiteBalanceControl;
	*gainRed = result.gainRed;
	*gainBlue = result.gainBlue;
	*gainGreen = result.gainGreen;
	return (CameraResult)ret;
}

CameraResult Camera::SetWhiteBalance(CameraChannel channel,
									 uint32_t whiteBalanceControl,
									 uint32_t gainRed,
									 uint32_t gainBlue,
									 uint32_t gainGreen)
{
	SceCameraWhiteBalance param;
	memset(&param, 0, sizeof(SceCameraWhiteBalance));
	param.whiteBalanceControl = whiteBalanceControl;
	param.gainRed = gainRed;
	param.gainBlue = gainBlue;
	param.gainGreen = gainGreen;
	return (CameraResult)sceCameraSetWhiteBalance(_handle, (SceCameraChannel)channel, &param, NULL);
}

CameraResult Camera::GetGamma(CameraChannel channel,
							  CS_OUT uint32_t* gammaControl,
							  CS_OUT uint32_t* value)
{
	SceCameraGamma result;
	auto ret = sceCameraGetGamma(_handle, (SceCameraChannel)channel, &result, NULL);
	*gammaControl = result.gammaControl;
	*value = result.value;
	return (CameraResult)ret;
}

CameraResult Camera::SetGamma(CameraChannel channel,
							  uint32_t gammaControl,
							  uint32_t value)
{
	SceCameraGamma param;
	memset(&param, 0, sizeof(SceCameraGamma));
	param.gammaControl = gammaControl;
	param.value = value;
	return (CameraResult)sceCameraSetGamma(_handle, (SceCameraChannel)channel, &param, NULL);
}

CameraResult Camera::GetSaturation(CameraChannel channel, CS_OUT uint32_t* saturation)
{
	return (CameraResult)sceCameraGetSaturation(_handle, (SceCameraChannel)channel, saturation, NULL);
}

CameraResult Camera::SetSaturation(CameraChannel channel, uint32_t saturation)
{
	return (CameraResult)sceCameraSetSaturation(_handle, (SceCameraChannel)channel, saturation, NULL);
}

CameraResult Camera::GetContrast(CameraChannel channel, CS_OUT uint32_t* contrast)
{
	return (CameraResult)sceCameraGetContrast(_handle, (SceCameraChannel)channel, contrast, NULL);
}

CameraResult Camera::SetContrast(CameraChannel channel, uint32_t contrast)
{
	return (CameraResult)sceCameraSetContrast(_handle, (SceCameraChannel)channel, contrast, NULL);
}

CameraResult Camera::GetSharpness(CameraChannel channel, CS_OUT uint32_t* sharpness)
{
	return (CameraResult)sceCameraGetSharpness(_handle, (SceCameraChannel)channel, sharpness, NULL);
}

CameraResult Camera::SetSharpness(CameraChannel channel, uint32_t sharpness)
{
	return (CameraResult)sceCameraSetSharpness(_handle, (SceCameraChannel)channel, sharpness, NULL);
}

CameraResult Camera::GetHue(CameraChannel channel, CS_OUT int32_t* hue)
{
	return (CameraResult)sceCameraGetHue(_handle, (SceCameraChannel)channel, hue, NULL);
}

CameraResult Camera::SetHue(CameraChannel channel, int32_t hue)
{
	return (CameraResult)sceCameraSetHue(_handle, (SceCameraChannel)channel, hue, NULL);
}

CameraResult Camera::GetDefectivePixelCancellation(CameraChannel channel, CS_OUT uint32_t* isEnabled)
{
	return (CameraResult)sceCameraGetDefectivePixelCancellation(_handle, (SceCameraChannel)channel, isEnabled, NULL);
}

CameraResult Camera::SetDefectivePixelCancellation(CameraChannel channel, uint32_t isEnabled)
{
	return (CameraResult)sceCameraSetDefectivePixelCancellation(_handle, (SceCameraChannel)channel, isEnabled, NULL);
}

CameraResult Camera::GetLensCorrection(CameraChannel channel, CS_OUT uint32_t* isEnabled)
{
	return (CameraResult)sceCameraGetLensCorrection(_handle, (SceCameraChannel)channel, isEnabled, NULL);
}

CameraResult Camera::SetLensCorrection(CameraChannel channel, uint32_t isEnabled)
{
	return (CameraResult)sceCameraSetLensCorrection(_handle, (SceCameraChannel)channel, isEnabled, NULL);
}

CameraResult Camera::GetFrameData(CameraFrameMemoryType readMode, CameraFrameWaitMode waitMode, CS_OUT CameraFrameData* outData, CS_OUT bool* isValid)
{
	SceCameraFrameData result;
	memset(&result, 0, sizeof(SceCameraFrameData));
	result.sizeThis = sizeof(SceCameraFrameData);
	result.readMode = (uint32_t)(readMode) | (uint32_t)waitMode;

	auto ret = sceCameraGetFrameData(_handle, &result);

	outData->Frame_0_0 = result.pFramePointerList[0][0];
	outData->Frame_0_1 = result.pFramePointerList[0][1];
	outData->Frame_0_2 = result.pFramePointerList[0][2];
	outData->Frame_0_3 = result.pFramePointerList[0][3];

	outData->Frame_1_0 = result.pFramePointerList[1][0];
	outData->Frame_1_1 = result.pFramePointerList[1][1];
	outData->Frame_1_2 = result.pFramePointerList[1][2];
	outData->Frame_1_3 = result.pFramePointerList[1][3];

	outData->Frame_0_0_Size = result.frameSize[0][0];
	outData->Frame_0_1_Size = result.frameSize[0][1];
	outData->Frame_0_2_Size = result.frameSize[0][2];
	outData->Frame_0_3_Size = result.frameSize[0][3];

	outData->Frame_1_0_Size = result.frameSize[1][0];
	outData->Frame_1_1_Size = result.frameSize[1][1];
	outData->Frame_1_2_Size = result.frameSize[1][2];
	outData->Frame_1_3_Size = result.frameSize[1][3];

	outData->Timestamp = result.meta.timestamp[0];

	outData->Status0 = (CameraStatus)result.status[0];
	outData->Status1 = (CameraStatus)result.status[1];

	*isValid = sceCameraIsValidFrameData(_handle, &result) == 1;

	return (CameraResult)ret;
}
