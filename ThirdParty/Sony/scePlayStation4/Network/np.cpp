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

