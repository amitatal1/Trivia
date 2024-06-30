using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.Numerics;
using System.Text;
using System.Text.Json;
using System.Threading.Tasks;
using TriviaClient.Views;

namespace TriviaClient.Help
{


    public class ResponseInfo
    {
        public byte ResponseCode;
        public byte[] Buffer;

    }

    public class ErrorResponse
    {
        public int status { get; set; }
        public string message { get; set; }
    };

    public class StatusOnlyResponse
    {
        public int status { get; set; }
    }

    struct RoomData
    {
        public int id;
        public string name;
        public uint maxPlayers;
        public uint numOfQuestionsInGame;
        public uint timePerQuestion;
        public uint isActive;
    };

    public struct LoggedUser
    {
        public string _userName;
    }
    public class Room
    {
        RoomData _data;
        Vector<LoggedUser> _users;
    }

    public class SerializationHelper
    {

        const int HEADER_LEN = 5, MESSAGE_CODE_INDEX = 0;



        public enum MessageCode
        {
            ERROR_CODE = 0,

            LOGIN_CODE = 1,
            SIGN_UP_CODE,
            LOGOUT_CODE,

            GET_ROOMS_CODE,
            GET_PLAYERS_CODE,
            JOIN_ROOM_CODE,
            CREATE_ROOM_CODE,
            GET_HIGH_SCORE_CODE,
            GET_PERSONAL_STATS_CODE,

            CLOSE_ROOM_CODE,
            START_GAME_CODE,
            GET_ROOM_STATE_CODE,
            LEAVE_ROOM_CODE,

            SUBMIT_ANSWER_CODE,
            GET_GAME_RESULTS_CODE,
            GET_QUESTION_CODE,
            LEAVE_GAME_CODE
        };

        public enum ResponseStatuses
        {
            SUCCESS_STATUS = 1,
            FAILURE_STATUS = 2
        };

        public static ResponseInfo ReceiveRequestFromClient(Socket clientSocket)
        {
            ResponseInfo info = new ResponseInfo();

            // Receive header
            byte[] header = new byte[HEADER_LEN];
            clientSocket.Receive(header, HEADER_LEN, SocketFlags.None);

            // Parse message code
            info.ResponseCode = header[MESSAGE_CODE_INDEX];

            // Parse length
            int len = BitConverter.ToInt32(header, 1);

            // Receive data
            byte[] data = new byte[len];
            clientSocket.Receive(data, len, SocketFlags.None);

            // Set buffer in RequestInfo
            info.Buffer = data;

            return info;
        }


        public static byte[] SerializePacket<T>(MessageCode code, T dataStruct)
        {

            string jsonString = JsonSerializer.Serialize(dataStruct);
 

            int len = jsonString.Length;

            // Get the byte for code
            byte codeByte = (byte)code;

            // Get bytes for length
            byte[] lenBytes = BitConverter.GetBytes(len);

            // Convert string to bytes using ASCII encoding
            byte[] stringBytes = Encoding.ASCII.GetBytes(jsonString);

            // Check if system is not little-endian, and reverse length bytes if needed
            if (!BitConverter.IsLittleEndian)
            {
                Array.Reverse(lenBytes);
            }

            // Concatenate the three byte arrays into a single byte array
            byte[] serializedPacket = new byte[1 + lenBytes.Length + stringBytes.Length];
            int index = 0;

            // Copy codeByte
            serializedPacket[index] = codeByte;
            index += 1;

            // Copy lenBytes
            Array.Copy(lenBytes, 0, serializedPacket, index, lenBytes.Length);
            index += lenBytes.Length;

            // Copy stringBytes
            Array.Copy(stringBytes, 0, serializedPacket, index, stringBytes.Length);

            return serializedPacket;
        }
    }
}
