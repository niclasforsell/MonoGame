#include "np.h"
#include <assert.h>
#include <memory.h>

using namespace Network;

NpResult Np::_SetNpTitleId(const char* titleId, const uint8_t* titleSecret)
{
	assert(titleId != nullptr);
	assert(titleSecret != nullptr);

	SceNpTitleId id;
	SceNpTitleSecret secret;
	memset(&id, 0, sizeof(SceNpTitleId));
	memset(&secret, 0, sizeof(SceNpTitleSecret));

	assert(strlen(titleId) == SCE_NP_TITLE_ID_LEN);
	memcpy(&id.id, titleId, SCE_NP_TITLE_ID_LEN);
	memcpy(&secret.data, titleSecret, SCE_NP_TITLE_SECRET_SIZE);

	return (NpResult)sceNpSetNpTitleId(&id, &secret);
}

NpResult Np::_NotifyPlusFeature(UserServiceUserId userId, NpPlusFeature features)
{
	SceNpNotifyPlusFeatureParameter param;
	memset(&param, 0, sizeof(SceNpNotifyPlusFeatureParameter));

	param.userId = userId;
	param.features = (uint64_t)features;
	param.size = sizeof(SceNpNotifyPlusFeatureParameter);

	return (NpResult)sceNpNotifyPlusFeature(&param);
}

NpResult Np::_CheckPlus(UserServiceUserId userId, NpPlusFeature features, bool *authorised)
{
	SceNpCheckPlusParameter param;
	memset(&param, 0, sizeof(SceNpCheckPlusParameter));

	param.userId = userId;
	param.features = (uint64_t)features;
	param.size = sizeof(SceNpCheckPlusParameter);

	SceNpCheckPlusResult checkResult;
	memset(&checkResult, 0, sizeof(SceNpCheckPlusResult));
	
	int reqId = sceNpCreateRequest();

	NpResult result = (NpResult)sceNpCheckPlus(reqId, &param, &checkResult);
	(*authorised) = checkResult.authorized;

	sceNpDeleteRequest(reqId);
	return result;
}

static NpResult _SetContentRestriction(int defaultAgeRestriction)
{
	SceNpContentRestriction contentRestriction;
	memset(&contentRestriction, 0, sizeof(contentRestriction));
	contentRestriction.size = sizeof(contentRestriction);
	contentRestriction.defaultAgeRestriction = (int8_t)defaultAgeRestriction;

	// TODO: add support for setting array of SceNpContentRestriction for region specific restrictions
	contentRestriction.ageRestrictionCount = 0;
	contentRestriction.ageRestriction = NULL;

	return (NpResult)sceNpSetContentRestriction(&contentRestriction);
}

static NpResult _CheckNpAvailability(UserServiceUserId userId)
{
	SceNpOnlineId onlineId;
	memset(&onlineId, 0, sizeof(SceNpOnlineId));

	auto onlineIdResult = sceNpGetOnlineId((SceUserServiceUserId)userId, &onlineId);
	if (onlineIdResult != SCE_OK)
		return (NpResult)onlineIdResult;

	int reqId = sceNpCreateRequest();

	auto result = sceNpCheckNpAvailability(reqId, &onlineId, NULL);

	sceNpDeleteRequest(reqId);
	return (NpResult)result;
}

NpResult Np::_GetParentalControlInfo(UserServiceUserId userId, int* age, bool *chatRestriction, bool* ugcRestriction)
{
	SceNpOnlineId onlineId;
	memset(&onlineId, 0, sizeof(SceNpOnlineId));

	auto onlineIdResult = sceNpGetOnlineId((SceUserServiceUserId)userId, &onlineId);
	if (onlineIdResult != SCE_OK)
		return (NpResult)onlineIdResult;

	int8_t ageInfo = 0;
	SceNpParentalControlInfo parentalControlInfo;
	memset(&parentalControlInfo, 0, sizeof(SceNpParentalControlInfo));

	int reqId = sceNpCreateRequest();	
	auto result = sceNpGetParentalControlInfo(reqId, &onlineId, &ageInfo, &parentalControlInfo);
	sceNpDeleteRequest(reqId);

	if (result == SCE_OK)
	{
		(*age) = (int)ageInfo;
		(*chatRestriction) = parentalControlInfo.chatRestriction;
		(*ugcRestriction) = parentalControlInfo.ugcRestriction;
	}
	else
	{
		// https://ps4.scedev.net/resources/documents/SDK/3.000/Np-Reference/0042.html
		// This function itself may return a viewer age restriction error (SCE_NP_ERROR_AGE_RESTRICTION). 
		// If an error occurs for this function including in such cases, appropriate parental control information will not return
		(*age) = 0;
		(*chatRestriction) = true;
		(*ugcRestriction) = true;
	}
	return (NpResult)result;
}

