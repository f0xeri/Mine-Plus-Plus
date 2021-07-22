//
// Created by Yaroslav on 19.07.2021.
//

#ifndef MINE_MINENETCLIENT_HPP
#define MINE_MINENETCLIENT_HPP

#include <MineNet.h>
#include <MineNetClient.hpp>
#include <glm/glm.hpp>

enum class MineMsgTypes : uint32_t
{
    ServerPing,
    ServerMessage,
    ServerMessageAll,
    ClientAccepted,
    ClientAssignID,
    ClientRegisterWithServer,
    ClientUnregisterWithServer,
    ClientGetWorldChanges,
    WorldAddPlayer,
    WorldRemovePlayer,
    WorldUpdatePlayer,
    WorldChunkModified,
    WorldChanges,
    ServerSaveWorldChange,
};

struct ChunkModifyData
{
    int iendx, iendy, iendz;
    int normx, normy, normz;
    int blockId;
};

struct ChunkChangesSave
{
    int cx, cy, cz;
    int blockNumber;
    int newBlockId;
};


class MineNetClient : public MineNet::IClient<MineMsgTypes>
{
public:
    uint32_t playerID = 0;
    bool waitingForConnection = true;

    void messageAll()
    {
        MineNet::message<MineMsgTypes> msg;
        msg.header.id = MineMsgTypes::ServerMessageAll;
        send(msg);
    }
};


#endif //MINE_MINENETCLIENT_HPP
