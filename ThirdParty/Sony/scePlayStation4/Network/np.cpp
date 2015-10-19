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

