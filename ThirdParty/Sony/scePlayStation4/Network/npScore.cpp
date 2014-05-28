#include "npScore.h"

#include <libsysmodule.h>
#include <memory.h>


using namespace Network;

void NpScore::_tryInitialize()
{
	if (sceSysmoduleIsLoaded(SCE_SYSMODULE_NP_SCORE_RANKING) == SCE_SYSMODULE_LOADED)
		return;

	auto ret = sceSysmoduleLoadModule(SCE_SYSMODULE_NP_SCORE_RANKING);
	if (ret < 0)
	{
		printf("ERROR: Couldn't initialize NpScore library: 0x%08X\n", ret);
		return;
	}
}


NpScoreRequest::NpScoreRequest()
{
	_requestId = -1;
}

NpScoreRequest::~NpScoreRequest()
{
	if (_requestId != -1)
	{
		sceNpScoreDeleteRequest(_requestId);
		_requestId = -1;
	}
}

NpScoreRequest* NpScoreRequest::Create(NpScoreTitleContext *context,  CS_OUT int32_t *error)
{
	auto result = sceNpScoreCreateRequest(context->_titleCtxId);
	if (result < 0)
	{
		*error = (int32_t)(NpCommunityError)result;
		return NULL;
	}

	auto request = new NpScoreRequest();
	request->_requestId = result;
	
	*error = (int32_t)NpCommunityError::Ok;
	return request;
}

NpCommunityError NpScoreRequest::RecordScore(	SceNpScoreBoardId boardId, 
												SceNpScoreValue score, 
												const char *scoreComment, 
												uint64_t compareDate,
												CS_OUT SceNpScoreRankNumber *tmpRank)
{
	SceRtcTick tick;
	tick.tick = compareDate;

	SceNpScoreComment comment;
	memset(&comment, 0, sizeof(comment));
	strncpy(comment.utf8Comment, scoreComment, SCE_NP_SCORE_COMMENT_MAXLEN);

	auto error = sceNpScoreRecordScore(_requestId, boardId, score, &comment, NULL, tmpRank, &tick, NULL);
	if (error < 0)
		return (NpCommunityError)error;
}

NpCommunityError NpScoreRequest::CensorComment(const char *comment)
{
	auto error = sceNpScoreCensorComment(_requestId, comment, NULL);
	return (NpCommunityError)error;
}

const char* NpScoreRequest::SanitizeComment(const char *comment, CS_OUT int32_t *error)
{
	*error = sceNpScoreSanitizeComment(_requestId, comment, _sanitizedComment, NULL);
	if (error < 0)
		return NULL;

	return _sanitizedComment;
}


NpScoreTitleContext::NpScoreTitleContext()
{
	memset(&_npId, 0, sizeof(_npId));
	_titleCtxId = -1;
}

NpScoreTitleContext::~NpScoreTitleContext()
{
	if (_titleCtxId != -1)
	{
		sceNpScoreDeleteNpTitleCtx(_titleCtxId);
		_titleCtxId = -1;
	}
}

NpScoreTitleContext* NpScoreTitleContext::Create(UserServiceUserId userId, CS_OUT int32_t *error)
{
	SceNpId npId;
	auto result = sceNpGetNpId(userId, &npId);
	if (result < 0)
	{
		*error = (int32_t)(NpResult)result;
		return NULL;
	}

	NpScore::_tryInitialize();

	auto context = new NpScoreTitleContext();
	memcpy(&context->_npId, &npId, sizeof(SceNpId));

	*error = (int32_t)NpResult::Ok;
	return context;
}

NpCommunityError NpScoreTitleContext::Initialize(SceNpServiceLabel npServiceLabel)
{	
	// If the context is already created then delete it.
	if (_titleCtxId != -1)
	{
		sceNpScoreDeleteNpTitleCtx(_titleCtxId);
		_titleCtxId = -1;
	}

	// Create the context.
	auto error = sceNpScoreCreateNpTitleCtx(npServiceLabel, &_npId);
	if (error < 0)
		return (NpCommunityError)error;

	_titleCtxId = error;
	return NpCommunityError::Ok;
}
