#pragma once

#include "predecls.h"
#include "np.h"

#include <sceerror.h>

namespace Network {

enum class NpCommunityError
{
	Ok = SCE_OK,

	AlreadyInitialized = SCE_NP_COMMUNITY_ERROR_ALREADY_INITIALIZED,
	Initialized = SCE_NP_COMMUNITY_ERROR_NOT_INITIALIZED,
	OutOfMemory = SCE_NP_COMMUNITY_ERROR_OUT_OF_MEMORY,
	InvalidArgument = SCE_NP_COMMUNITY_ERROR_INVALID_ARGUMENT,
	NoLogin = SCE_NP_COMMUNITY_ERROR_NO_LOGIN,
	TooManyObjects = SCE_NP_COMMUNITY_ERROR_TOO_MANY_OBJECTS,
	Aborted = SCE_NP_COMMUNITY_ERROR_ABORTED,
	BadResponse = SCE_NP_COMMUNITY_ERROR_BAD_RESPONSE,
	BodyTooLarge = SCE_NP_COMMUNITY_ERROR_BODY_TOO_LARGE,
	HttpServer = SCE_NP_COMMUNITY_ERROR_HTTP_SERVER,
	InvalidSignature = SCE_NP_COMMUNITY_ERROR_INVALID_SIGNATURE,
	InsufficientArgument = SCE_NP_COMMUNITY_ERROR_INSUFFICIENT_ARGUMENT,
	UnknownType = SCE_NP_COMMUNITY_ERROR_UNKNOWN_TYPE,
	InvalidId = SCE_NP_COMMUNITY_ERROR_INVALID_ID,
	InvalidOnlineId = SCE_NP_COMMUNITY_ERROR_INVALID_ONLINE_ID,
	InvalidType = SCE_NP_COMMUNITY_ERROR_INVALID_TYPE,
	TransactionAlreadyEnd = SCE_NP_COMMUNITY_ERROR_TRANSACTION_ALREADY_END,
	InvalidPartition = SCE_NP_COMMUNITY_ERROR_INVALID_PARTITION,
	InvalidAlignment = SCE_NP_COMMUNITY_ERROR_INVALID_ALIGNMENT,
	ClientHandleAlreadyExists = SCE_NP_COMMUNITY_ERROR_CLIENT_HANDLE_ALREADY_EXISTS,
	NoResource = SCE_NP_COMMUNITY_ERROR_NO_RESOURCE,
	RequestBeforeEnd = SCE_NP_COMMUNITY_ERROR_REQUEST_BEFORE_END,
	TooManySlotId = SCE_NP_COMMUNITY_ERROR_TOO_MANY_SLOTID,
	TooManyNpId = SCE_NP_COMMUNITY_ERROR_TOO_MANY_NPID,
	ScoreInvalidSaveDataOwner = SCE_NP_COMMUNITY_ERROR_SCORE_INVALID_SAVEDATA_OWNER,
	TusInvalidSaveDataOwner = SCE_NP_COMMUNITY_ERROR_TUS_INVALID_SAVEDATA_OWNER,

	/*
	Center = SCE_NP_COMMUNITY_SERVER_ERROR_BAD_REQUEST,
	Center = SCE_NP_COMMUNITY_SERVER_ERROR_INVALID_TICKET,
	Center = SCE_NP_COMMUNITY_SERVER_ERROR_INVALID_SIGNATURE,
	Center = SCE_NP_COMMUNITY_SERVER_ERROR_INVALID_NPID,
	Center = SCE_NP_COMMUNITY_SERVER_ERROR_FORBIDDEN,
	Center = SCE_NP_COMMUNITY_SERVER_ERROR_INTERNAL_SERVER_ERROR,
	Center = SCE_NP_COMMUNITY_SERVER_ERROR_VERSION_NOT_SUPPORTED,
	Center = SCE_NP_COMMUNITY_SERVER_ERROR_SERVICE_UNAVAILABLE,
	Center = SCE_NP_COMMUNITY_SERVER_ERROR_PLAYER_BANNED,
	Center = SCE_NP_COMMUNITY_SERVER_ERROR_CENSORED,
	Center = SCE_NP_COMMUNITY_SERVER_ERROR_RANKING_RECORD_FORBIDDEN,
	Center = SCE_NP_COMMUNITY_SERVER_ERROR_USER_PROFILE_NOT_FOUND,
	Center = SCE_NP_COMMUNITY_SERVER_ERROR_UPLOADER_DATA_NOT_FOUND,
	Center = SCE_NP_COMMUNITY_SERVER_ERROR_QUOTA_MASTER_NOT_FOUND,
	Center = SCE_NP_COMMUNITY_SERVER_ERROR_RANKING_TITLE_NOT_FOUND,
	Center = SCE_NP_COMMUNITY_SERVER_ERROR_BLACKLISTED_USER_ID,
	*/
};

class NpScore;
class NpScoreRequest;
class NpScoreRankings;
class NpScoreTitleContext;


class CS_API NpScore
{
	friend class NpScoreTitleContext;

	static void _tryInitialize();

	NpScore() { }

public:


};


class CS_API NpScoreRankings
{
	friend class NpScoreRequest;

	SceNpScoreRankData *_rank;
	SceNpScoreComment *_comment;
	SceNpScoreGameInfo *_gameInfo;

	size_t _arrayNum;

	SceRtcTick _lastSortDate;
	SceNpScoreRankNumber _totalPlayers;

	SceNpScoreRankNumber _first;
	SceNpScoreRankNumber _last;

	int32_t _index;

public:

	NpScoreRankings(size_t arrayNum, bool needRanks, bool needComments, bool needGameInfo);
	~NpScoreRankings();

	int32_t GetIndex();
	void SetIndex(int32_t index);
	
	size_t GetArrayNum();

	SceNpScoreRankNumber GetFirstInRange();
	SceNpScoreRankNumber GetLastInRange();
	SceNpScoreRankNumber GetTotalPlayers();

	const char* GetUserOnlineIdAtIndex();
	SceNpScorePcId GetPcIdAtIndex();
	SceNpScoreRankNumber GetSerialRankAtIndex();
	SceNpScoreRankNumber GetRankAtIndex();
	SceNpScoreRankNumber GetHighestRankAtIndex();
	SceNpScoreValue GetScoreValueAtIndex();
	bool GetHasGameDataAtIndex();
	uint64_t GetRecordDateAtIndex();
	const char* GetCommentAtIndex();
	const char* GetGameInfoIndex();
};

class CS_API NpScoreRequest
{
	int32_t _requestId;
	char _sanitizedComment[255];

	NpScoreRequest();

public:

	~NpScoreRequest();

	static NpScoreRequest* Create(NpScoreTitleContext *context, CS_OUT int32_t *error);

	NpCommunityError GetRankingsByRange(	SceNpScoreBoardId boardId, 
											SceNpScoreRankNumber startSerialRank, 
											NpScoreRankings *results);

	NpCommunityError RecordScore(	SceNpScoreBoardId boardId, 
									SceNpScoreValue score, 
									const char *scoreComment,
									const char *gameInfo,
									uint64_t compareDate,
									CS_OUT SceNpScoreRankNumber *tmpRank);

	NpCommunityError CensorComment(const char *comment);

	const char* SanitizeComment(const char *comment, CS_OUT int32_t *error);

};


class CS_API NpScoreTitleContext
{
	friend class NpScoreRequest;

	SceNpId _npId;

	int _titleCtxId;

	NpScoreTitleContext();

public:

	~NpScoreTitleContext();

	static NpScoreTitleContext* Create(UserServiceUserId userId, CS_OUT int32_t *error);

	NpCommunityError Initialize(SceNpServiceLabel npServiceLabel);

};


} // namespace Network
