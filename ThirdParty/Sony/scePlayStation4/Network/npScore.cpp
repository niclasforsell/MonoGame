#include "npScore.h"

#include "../allocator.h"

#include <libsysmodule.h>
#include <memory.h>
#include <assert.h>


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

NpScoreRankings::NpScoreRankings(size_t arrayNum, bool needRanks, bool needComments, bool needGameInfo)
{
	assert(arrayNum <= SCE_NP_SCORE_MAX_RANGE_NUM_PER_REQUEST);
	_arrayNum = arrayNum;

	_rank = needRanks ? mem::alloc_array<SceNpScoreRankData>(arrayNum, 8) : NULL;
	_comment = needComments ? mem::alloc_array<SceNpScoreComment>(arrayNum, 8) : NULL;
	_gameInfo = needGameInfo ? mem::alloc_array<SceNpScoreGameInfo>(arrayNum, 8) : NULL;

	_lastSortDate.tick = 0;
	_totalPlayers = 0;
	_first = 0;
	_last = 0;
	_index = 0;
}

NpScoreRankings::~NpScoreRankings()
{
	mem::free(_rank);
	mem::free(_comment);
	mem::free(_gameInfo);
}

int32_t NpScoreRankings::GetIndex()
{
	return _index;
}

void NpScoreRankings::SetIndex(int32_t index)
{
	assert(index >= 0);
	assert(index < _arrayNum);
	_index = index;
}


size_t NpScoreRankings::GetArrayNum()
{
	return _arrayNum;
}


SceNpScoreRankNumber NpScoreRankings::GetFirstInRange()
{
	return _first;
}

SceNpScoreRankNumber NpScoreRankings::GetLastInRange()
{
	return _last;
}

SceNpScoreRankNumber NpScoreRankings::GetTotalPlayers()
{
	return _totalPlayers;
}

const char* NpScoreRankings::GetUserOnlineIdAtIndex()
{
	if (_rank)
		return _rank[_index].npId.handle.data;
	return NULL;
}

SceNpScorePcId NpScoreRankings::GetPcIdAtIndex()
{
	if (_rank)
		return _rank[_index].pcId;
	return 0;
}

SceNpScoreRankNumber NpScoreRankings::GetSerialRankAtIndex()
{
	if (_rank)
		return _rank[_index].serialRank;
	return 0;
}

SceNpScoreRankNumber NpScoreRankings::GetRankAtIndex()
{
	if (_rank)
		return _rank[_index].rank;
	return 0;
}

SceNpScoreRankNumber NpScoreRankings::GetHighestRankAtIndex()
{
	if (_rank)
		return _rank[_index].highestRank;
	return 0;
}

SceNpScoreValue NpScoreRankings::GetScoreValueAtIndex()
{
	if (_rank)
		return _rank[_index].scoreValue;
	return 0;
}

bool NpScoreRankings::GetHasGameDataAtIndex()
{
	if (_rank)
		return _rank[_index].hasGameData != 0;
	return false;
}

uint64_t NpScoreRankings::GetRecordDateAtIndex()
{
	if (_rank)
		return _rank[_index].recordDate.tick;
	return 0;
}

const char* NpScoreRankings::GetCommentAtIndex()
{
	if (_comment)
		return _comment[_index].utf8Comment;
	return NULL;
}

const char* NpScoreRankings::GetGameInfoIndex()
{
	if (_gameInfo)
		return (const char*)_gameInfo[_index].data;
	return NULL;
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

NpScoreRequest* NpScoreRequest::Create(NpScoreTitleContext *context,  CS_OUT NpCommunityError *error)
{
	auto result = sceNpScoreCreateRequest(context->_titleCtxId);
	if (result < 0)
	{
		*error = (NpCommunityError)result;
		return NULL;
	}

	auto request = new NpScoreRequest();
	request->_requestId = result;
	
	*error = NpCommunityError::Ok;
	return request;
}

NpCommunityError NpScoreRequest::GetRankingsByRange(	SceNpScoreBoardId boardId, 
														SceNpScoreRankNumber startSerialRank,
														NpScoreRankings *results)
{
	assert(results != NULL);

	auto error = sceNpScoreGetRankingByRange(	_requestId, 
												boardId, 
												startSerialRank,
												results->_rank,
												results->_arrayNum * sizeof(SceNpScoreRankData),
												results->_comment,
												results->_arrayNum * sizeof(SceNpScoreComment),
												results->_gameInfo,
												results->_arrayNum * sizeof(SceNpScoreGameInfo),
												results->_arrayNum,
												&results->_lastSortDate,
												&results->_totalPlayers,												
												NULL);
	if (error < 0)
		return (NpCommunityError)error;

	results->_index = 0;
	results->_first = startSerialRank;
	results->_last = startSerialRank + error;

	return NpCommunityError::Ok;
}

NpCommunityError NpScoreRequest::RecordScore(	SceNpScoreBoardId boardId, 
												SceNpScoreValue score, 
												const char *scoreComment, 
												const char *gameInfo,
												uint64_t compareDate,
												CS_OUT SceNpScoreRankNumber *tmpRank)
{
	SceRtcTick tick;
	tick.tick = compareDate;

	SceNpScoreComment comment;
	memset(&comment, 0, sizeof(comment));
	strncpy(comment.utf8Comment, scoreComment, SCE_NP_SCORE_COMMENT_MAXLEN);

	SceNpScoreGameInfo info;
	memset(&info, 0, sizeof(info));
	if (gameInfo != NULL)
	{
		info.infoSize =  MIN(strlen(gameInfo), SCE_NP_SCORE_GAMEINFO_MAXSIZE);
		memcpy(&info.data, gameInfo, info.infoSize);
	}
	

	auto error = sceNpScoreRecordScore(_requestId, boardId, score, &comment, &info, tmpRank, &tick, NULL);
	if (error < 0)
		return (NpCommunityError)error;
}

NpCommunityError NpScoreRequest::RecordScore(	SceNpScoreBoardId boardId, 
												SceNpScoreValue score, 
												const char *scoreComment, 
												const uint8_t *gameInfo,
												int gameInfoLength,
												uint64_t compareDate,
												CS_OUT SceNpScoreRankNumber *tmpRank)
{
	SceRtcTick tick;
	tick.tick = compareDate;

	SceNpScoreComment comment;
	memset(&comment, 0, sizeof(comment));
	strncpy(comment.utf8Comment, scoreComment, SCE_NP_SCORE_COMMENT_MAXLEN);

	SceNpScoreGameInfo info;
	memset(&info, 0, sizeof(info));
	if (gameInfo != NULL)
	{
		info.infoSize =  MIN(gameInfoLength, SCE_NP_SCORE_GAMEINFO_MAXSIZE);
		memcpy(&info.data, gameInfo, info.infoSize);
	}
	

	auto error = sceNpScoreRecordScore(_requestId, boardId, score, &comment, &info, tmpRank, &tick, NULL);
	if (error < 0)
		return (NpCommunityError)error;
}

NpCommunityError NpScoreRequest::CensorComment(const char *comment)
{
	auto error = sceNpScoreCensorComment(_requestId, comment, NULL);
	return (NpCommunityError)error;
}

const char* NpScoreRequest::SanitizeComment(const char *comment, CS_OUT NpCommunityError *error)
{
	*error = (NpCommunityError)sceNpScoreSanitizeComment(_requestId, comment, _sanitizedComment, NULL);
	if ((int)(*error) < 0)
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

NpScoreTitleContext* NpScoreTitleContext::Create(UserServiceUserId userId, CS_OUT NpResult *error)
{
	SceNpId npId;
	auto result = sceNpGetNpId(userId, &npId);
	if (result < 0)
	{
		*error = (NpResult)result;
		return NULL;
	}

	NpScore::_tryInitialize();

	auto context = new NpScoreTitleContext();
	memcpy(&context->_npId, &npId, sizeof(SceNpId));

	*error = NpResult::Ok;
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
