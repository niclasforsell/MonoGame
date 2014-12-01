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

	ServerBadRequest = SCE_NP_COMMUNITY_SERVER_ERROR_BAD_REQUEST,
	ServerInvalidTicket = SCE_NP_COMMUNITY_SERVER_ERROR_INVALID_TICKET,
	ServerInvalidSignature = SCE_NP_COMMUNITY_SERVER_ERROR_INVALID_SIGNATURE,
	ServerInvalidNpid = SCE_NP_COMMUNITY_SERVER_ERROR_INVALID_NPID,
	ServerForbidden = SCE_NP_COMMUNITY_SERVER_ERROR_FORBIDDEN,
	ServerInternalError = SCE_NP_COMMUNITY_SERVER_ERROR_INTERNAL_SERVER_ERROR,
	ServerVersionNotSupported = SCE_NP_COMMUNITY_SERVER_ERROR_VERSION_NOT_SUPPORTED,
	ServerServiceUnavailable = SCE_NP_COMMUNITY_SERVER_ERROR_SERVICE_UNAVAILABLE,
	ServerPlayerBanned = SCE_NP_COMMUNITY_SERVER_ERROR_PLAYER_BANNED,
	ServerCensored = SCE_NP_COMMUNITY_SERVER_ERROR_CENSORED,
	ServerRankingRecordForbidden = SCE_NP_COMMUNITY_SERVER_ERROR_RANKING_RECORD_FORBIDDEN,
	ServerUserProfileNotFound = SCE_NP_COMMUNITY_SERVER_ERROR_USER_PROFILE_NOT_FOUND,
	ServerUploaderDataNotFound = SCE_NP_COMMUNITY_SERVER_ERROR_UPLOADER_DATA_NOT_FOUND,
	ServerQuotaMasterNotFound = SCE_NP_COMMUNITY_SERVER_ERROR_QUOTA_MASTER_NOT_FOUND,
	ServerRankingTitleNotFound = SCE_NP_COMMUNITY_SERVER_ERROR_RANKING_TITLE_NOT_FOUND,
	ServerBlacklistedUserId = SCE_NP_COMMUNITY_SERVER_ERROR_BLACKLISTED_USER_ID,
	ServerGameRankingNotFound = SCE_NP_COMMUNITY_SERVER_ERROR_GAME_RANKING_NOT_FOUND,
	ServerRankingStoreNotFound = SCE_NP_COMMUNITY_SERVER_ERROR_RANKING_STORE_NOT_FOUND,
	ServerNotBestScore = SCE_NP_COMMUNITY_SERVER_ERROR_NOT_BEST_SCORE,
	ServerLatestUpdateNotFound = SCE_NP_COMMUNITY_SERVER_ERROR_LATEST_UPDATE_NOT_FOUND,
	ServerRankingBoardMasterNotFound = SCE_NP_COMMUNITY_SERVER_ERROR_RANKING_BOARD_MASTER_NOT_FOUND,
	ServerRankingGameDataMasterNotFound = SCE_NP_COMMUNITY_SERVER_ERROR_RANKING_GAME_DATA_MASTER_NOT_FOUND,
	ServerInvalidAnticheatData = SCE_NP_COMMUNITY_SERVER_ERROR_INVALID_ANTICHEAT_DATA,
	ServerTooLargeData = SCE_NP_COMMUNITY_SERVER_ERROR_TOO_LARGE_DATA,
	ServerNoSuchUserNpid = SCE_NP_COMMUNITY_SERVER_ERROR_NO_SUCH_USER_NPID,
	ServerInvalidEnvironment = SCE_NP_COMMUNITY_SERVER_ERROR_INVALID_ENVIRONMENT,
	ServerInvalidOnlineNameCharacter = SCE_NP_COMMUNITY_SERVER_ERROR_INVALID_ONLINE_NAME_CHARACTER,
	ServerInvalidOnlineNameLength = SCE_NP_COMMUNITY_SERVER_ERROR_INVALID_ONLINE_NAME_LENGTH,
	ServerInvalidAboutMeCharacter = SCE_NP_COMMUNITY_SERVER_ERROR_INVALID_ABOUT_ME_CHARACTER,
	ServerInvalidAboutMeLength = SCE_NP_COMMUNITY_SERVER_ERROR_INVALID_ABOUT_ME_LENGTH,
	ServerInvalidScore = SCE_NP_COMMUNITY_SERVER_ERROR_INVALID_SCORE,
	ServerOverTheRankingLimit = SCE_NP_COMMUNITY_SERVER_ERROR_OVER_THE_RANKING_LIMIT,
	ServerFailToCreateSignature = SCE_NP_COMMUNITY_SERVER_ERROR_FAIL_TO_CREATE_SIGNATURE,
	ServerRankingMasterInfoNotFound = SCE_NP_COMMUNITY_SERVER_ERROR_RANKING_MASTER_INFO_NOT_FOUND,
	ServerOverTheGameDataLimit = SCE_NP_COMMUNITY_SERVER_ERROR_OVER_THE_GAME_DATA_LIMIT,
	ServerSelfDataNotFound = SCE_NP_COMMUNITY_SERVER_ERROR_SELF_DATA_NOT_FOUND,
	ServerUserNotAssigned = SCE_NP_COMMUNITY_SERVER_ERROR_USER_NOT_ASSIGNED,
	ServerGameDataAlreadyExists = SCE_NP_COMMUNITY_SERVER_ERROR_GAME_DATA_ALREADY_EXISTS,
	ServerTooManyResults = SCE_NP_COMMUNITY_SERVER_ERROR_TOO_MANY_RESULTS,
	ServerNotRecordableVersion = SCE_NP_COMMUNITY_SERVER_ERROR_NOT_RECORDABLE_VERSION,
	ServerUserStorageTitleMasterNotFound = SCE_NP_COMMUNITY_SERVER_ERROR_USER_STORAGE_TITLE_MASTER_NOT_FOUND,
	ServerInvalidVirtualUser = SCE_NP_COMMUNITY_SERVER_ERROR_INVALID_VIRTUAL_USER,
	ServerUserStorageDataNotFound = SCE_NP_COMMUNITY_SERVER_ERROR_USER_STORAGE_DATA_NOT_FOUND,
	ServerNonPlusMember = SCE_NP_COMMUNITY_SERVER_ERROR_NON_PLUS_MEMBER,
	ServerUnmatchSequence = SCE_NP_COMMUNITY_SERVER_ERROR_UNMATCH_SEQUENCE,
	ServerSavedataNotFound = SCE_NP_COMMUNITY_SERVER_ERROR_SAVEDATA_NOT_FOUND,
	ServerTooManySavedataFiles = SCE_NP_COMMUNITY_SERVER_ERROR_TOO_MANY_SAVEDATA_FILES,
	ServerTooMuchTotalSavedataSize = SCE_NP_COMMUNITY_SERVER_ERROR_TOO_MUCH_TOTAL_SAVEDATA_SIZE,
	ServerNotYetDownloadable = SCE_NP_COMMUNITY_SERVER_ERROR_NOT_YET_DOWNLOADABLE,
	ServerBlacklistedTitle = SCE_NP_COMMUNITY_SERVER_ERROR_BLACKLISTED_TITLE,
	ServerTooLargeIcondata = SCE_NP_COMMUNITY_SERVER_ERROR_TOO_LARGE_ICONDATA,
	ServerTooLargeSavedata = SCE_NP_COMMUNITY_SERVER_ERROR_TOO_LARGE_SAVEDATA,
	ServerUnmatchSignature = SCE_NP_COMMUNITY_SERVER_ERROR_UNMATCH_SIGNATURE,
	ServerUnmatchMd5sum = SCE_NP_COMMUNITY_SERVER_ERROR_UNMATCH_MD5SUM,
	ServerTooMuchSavedataSize = SCE_NP_COMMUNITY_SERVER_ERROR_TOO_MUCH_SAVEDATA_SIZE,
	ServerRecordDateIsNewerThanCompDate = SCE_NP_COMMUNITY_SERVER_ERROR_RECORD_DATE_IS_NEWER_THAN_COMP_DATE,
	ServerConditionsNotSatisfied = SCE_NP_COMMUNITY_SERVER_ERROR_CONDITIONS_NOT_SATISFIED,
	ServerUnsupportedPlatform = SCE_NP_COMMUNITY_SERVER_ERROR_UNSUPPORTED_PLATFORM,
	ServerExpiredSignature = SCE_NP_COMMUNITY_SERVER_ERROR_EXPIRED_SIGNATURE,
	ServerSavedataUpdated = SCE_NP_COMMUNITY_SERVER_ERROR_SAVEDATA_UPDATED,
	ServerMatchingBeforeService = SCE_NP_COMMUNITY_SERVER_ERROR_MATCHING_BEFORE_SERVICE,
	ServerMatchingEndOfService = SCE_NP_COMMUNITY_SERVER_ERROR_MATCHING_END_OF_SERVICE,
	ServerMatchingMaintenance = SCE_NP_COMMUNITY_SERVER_ERROR_MATCHING_MAINTENANCE,
	ServerRankingBeforeService = SCE_NP_COMMUNITY_SERVER_ERROR_RANKING_BEFORE_SERVICE,
	ServerRankingEndOfService = SCE_NP_COMMUNITY_SERVER_ERROR_RANKING_END_OF_SERVICE,
	ServerRankingMaintenance = SCE_NP_COMMUNITY_SERVER_ERROR_RANKING_MAINTENANCE,
	ServerNoSuchTitle = SCE_NP_COMMUNITY_SERVER_ERROR_NO_SUCH_TITLE,
	ServerTitleUserStorageBeforeService = SCE_NP_COMMUNITY_SERVER_ERROR_TITLE_USER_STORAGE_BEFORE_SERVICE,
	ServerTitleUserStorageEndOfService = SCE_NP_COMMUNITY_SERVER_ERROR_TITLE_USER_STORAGE_END_OF_SERVICE,
	ServerTitleUserStorageMaintenance = SCE_NP_COMMUNITY_SERVER_ERROR_TITLE_USER_STORAGE_MAINTENANCE,
	ServerFsrBeforeService = SCE_NP_COMMUNITY_SERVER_ERROR_FSR_BEFORE_SERVICE,
	ServerFsrEndOfService = SCE_NP_COMMUNITY_SERVER_ERROR_FSR_END_OF_SERVICE,
	ServerFsrMaintenance = SCE_NP_COMMUNITY_SERVER_ERROR_FSR_MAINTENANCE,
	ServerUbsBeforeService = SCE_NP_COMMUNITY_SERVER_ERROR_UBS_BEFORE_SERVICE,
	ServerUbsEndOfService = SCE_NP_COMMUNITY_SERVER_ERROR_UBS_END_OF_SERVICE,
	ServerUbsMaintenance = SCE_NP_COMMUNITY_SERVER_ERROR_UBS_MAINTENANCE,
	ServerBasicBlacklistedUserId = SCE_NP_COMMUNITY_SERVER_ERROR_BASIC_BLACKLISTED_USER_ID,
	ServerErrorUnspecified = SCE_NP_COMMUNITY_SERVER_ERROR_UNSPECIFIED,
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
	uint8_t* GetBinaryGameInfoIndex(void);
};

class CS_API NpScoreRequest
{
	int32_t _requestId;
	char _sanitizedComment[255];

	NpScoreRequest();

	CS_IGNORE NpCommunityError RecordScore(	SceNpScoreBoardId boardId, 
									SceNpScoreValue score, 
									const char *scoreComment,
									const uint8_t *gameInfo,
									int gameInfoLength,
									SceRtcTick *compareDate,
									SceNpScoreRankNumber *tmpRank);

public:

	~NpScoreRequest();

	static NpScoreRequest* Create(NpScoreTitleContext *context, CS_OUT NpCommunityError *error);

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

	const char* SanitizeComment(const char *comment, CS_OUT NpCommunityError *error);

protected:
	NpCommunityError RecordScore(	SceNpScoreBoardId boardId, 
									SceNpScoreValue score, 
									const char *scoreComment,
									const uint8_t *gameInfo,
									int gameInfoLength,
									uint64_t compareDate,
									CS_OUT SceNpScoreRankNumber *tmpRank);

	NpCommunityError RecordScore(SceNpScoreBoardId boardId,
								 SceNpScoreValue score,
								 const char* scoreComment,
								 const uint8_t* gameInfo,
								 int gameInfoLength);

};


class CS_API NpScoreTitleContext
{
	friend class NpScoreRequest;

	SceNpId _npId;

	int _titleCtxId;

	NpScoreTitleContext();

public:

	~NpScoreTitleContext();

	static NpScoreTitleContext* Create(UserServiceUserId userId, CS_OUT NpResult *error);

	NpCommunityError Initialize(SceNpServiceLabel npServiceLabel);

};


} // namespace Network
