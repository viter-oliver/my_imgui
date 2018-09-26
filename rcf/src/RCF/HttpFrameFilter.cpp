
//******************************************************************************
// RCF - Remote Call Framework
//
// Copyright (c) 2005 - 2013, Delta V Software. All rights reserved.
// http://www.deltavsoft.com
//
// RCF is distributed under dual licenses - closed source or GPL.
// Consult your particular license for conditions of use.
//
// If you have not purchased a commercial license, you are using RCF 
// under GPL terms.
//
// Version: 2.0
// Contact: support <at> deltavsoft.com 
//
//******************************************************************************

#include <RCF/HttpFrameFilter.hpp>

#include <RCF/ClientStub.hpp>
#include <RCF/Exception.hpp>
#include <RCF/ObjectPool.hpp>
#include <RCF/RcfSession.hpp>
#include <RCF/ThreadLocalData.hpp>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/trim.hpp>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <RCF/AsioServerTransport.hpp>

#ifndef BOOST_WINDOWS
#define strnicmp strncasecmp
#define stricmp strcasecmp
#endif

namespace RCF  {

    // Splits a string without making any memory allocations, by reusing data structures.
    void splitString(
        const std::string & stringToSplit, 
        const char * splitAt, 
        std::vector<std::string> & lines)
    {
        std::size_t lineCounter = 0;
        std::size_t splitAtLen = strlen(splitAt);

        std::size_t pos = 0;
        std::size_t posNext = 0;
        while (pos < stringToSplit.size() && pos != std::string::npos && lineCounter < 50)
        {
            posNext = stringToSplit.find(splitAt, pos);
            if (posNext != std::string::npos)
            {
                RCF_ASSERT(lineCounter <= lines.size());
                if ( lineCounter == lines.size() )
                {
                    lines.push_back(std::string());
                }
                lines[lineCounter].assign( stringToSplit.c_str() + pos, posNext - pos);
                ++lineCounter;

                pos = posNext + splitAtLen;
            }
        }

        for ( std::size_t i = lineCounter; i < lines.size(); ++i )
        {
            lines[i].resize(0);
        }
    }

    bool istarts_with(const std::string& line, const std::string& searchFor)
    {
        return 0 == strnicmp(line.c_str(), searchFor.c_str(), searchFor.size());
    }

    bool iequals(const std::string& lhs, const std::string& rhs)
    {
        return 0 == stricmp(lhs.c_str(), rhs.c_str());
    }

    HttpFrameFilter::HttpFrameFilter(std::size_t maxMessageLength) :
        mChunkedResponseMode(false),
        mChunkedResponseCounter(0),
        mServerPort(0),
        mClientSide(false),
        mHttpSessionIndex(0),
        mMaxMessageLength(maxMessageLength)        
    {
        init();
    }

    HttpFrameFilter::HttpFrameFilter(const std::string serverAddr, int serverPort) :
        mChunkedResponseMode(false),
        mChunkedResponseCounter(0),
        mServerAddr(serverAddr),
        mServerPort(serverPort),
        mClientSide(true),
        mHttpSessionIndex(0),
        mMaxMessageLength(0)        
    {
        init();
    }

    HttpFrameFilter::~HttpFrameFilter()
    {
    }

    void HttpFrameFilter::init()
    {
        resetState();

        mOsPtr.reset(new MemOstream());
    }

    void HttpFrameFilter::resetState()
    {
        mWriteBuffers.clear();
        mWritePos = 0;
        mReadBufferPtr = getObjectPool().getReallocBufferPtr();
        mBytesReceived = 0;
        mReadPos = 0;
        mHttpMessage.mHeaderLen = 0;
        mHttpMessage.mContentLen = 0;
        mChunkHeaderLen = 0;
        mChunkLen = 0;
        mHttpMessage.mFrameLen = 0;
        mMaxReadPos = 0;
        mOrigBytesRequested = 0;
        mProtocolChecked = false;

        mPrevHttpSessionIndex = 0;

        if ( mClientSide )
        {
            // Client-side needs to specify a session id to send through in the HTTP header,
            // so the server-side can piece together the RPC stream.

            boost::uuids::uuid uuid = boost::uuids::random_generator()();
            std::string uuidStr = boost::uuids::to_string(uuid);
            mHttpSessionId = uuidStr;

            mHttpSessionIndex = 0;
        }
    }

    void HttpFrameFilter::resizeReadBuffer(std::size_t newSize)
    {
        if ( !mReadBufferPtr )
        {
            mReadBufferPtr = getObjectPool().getReallocBufferPtr();
        }
        mReadBufferPtr->resize(newSize);
    }

    void HttpFrameFilter::read(
        const ByteBuffer &byteBuffer,
        std::size_t bytesRequested)
    {
        RCF_ASSERT(mReadPos <= mMaxReadPos);
        std::size_t bytesAvailableInCurrentFrame = mMaxReadPos - mReadPos;

        if ( !mMaxMessageLength )
        {
            ClientStub * pClientStub = getTlsClientStubPtr();
            if ( pClientStub )
            {
                ClientTransport & clientTransport = pClientStub->getTransport();
                mMaxMessageLength = clientTransport.getMaxMessageLength();
            }
        }
        
        if (bytesRequested == 0)
        {
            // Return any buffers we have, back to the pool.
            mReadBufferPtr.reset();

            mpPostFilter->read(ByteBuffer(), bytesRequested);
        }
        else if ( bytesAvailableInCurrentFrame == 0 )
        {
            // Start reading a new HTTP message.
            mOrigReadBuffer = byteBuffer;
            mOrigBytesRequested = bytesRequested;

            // If we already have bytes available for the next frame, move those bytes to 
            // the front of the buffer.
            if ( mBytesReceived > mHttpMessage.mFrameLen && mReadBufferPtr )
            {
                std::memmove(&(*mReadBufferPtr)[0], &(*mReadBufferPtr)[mHttpMessage.mFrameLen], mBytesReceived - mHttpMessage.mFrameLen);
                mReadBufferPtr->resize(mBytesReceived - mHttpMessage.mFrameLen);
                mBytesReceived = mReadBufferPtr->size();
            }
            else
            {
                mBytesReceived = 0;
            }

            // Prepare for the next frame.
            mHttpMessage.mHeaderLen = 0;
            mHttpMessage.mContentLen = 0;
            mHttpMessage.mFrameLen = 0;
            mReadPos = 0;
            mMaxReadPos = 0;
            mProtocolChecked = false;

            if ( mBytesReceived )
            {
                // Fake a completion with the bytes we already have.
                mBytesReceived = 0;
                onReadCompleted(ByteBuffer(mReadBufferPtr));
            }
            else
            {
                // Read from the network.
                mBytesReceived = 0;
                resizeReadBuffer(1024);
                mpPostFilter->read(
                    ByteBuffer(mReadBufferPtr),
                    mReadBufferPtr->size());
            }
        }
        else
        {
            // Return bytes from currently loaded frame.
            std::size_t bytesToReturn = RCF_MIN(bytesAvailableInCurrentFrame, bytesRequested);
            ByteBuffer byteBuffer_ = byteBuffer;
            if ( byteBuffer_.isEmpty() )
            {
                ReallocBufferPtr reallocBuffer = getObjectPool().getReallocBufferPtr();
                reallocBuffer->resize(bytesToReturn);
                byteBuffer_ = ByteBuffer(reallocBuffer);
            }
            bytesToReturn = RCF_MIN(bytesToReturn, byteBuffer_.getLength());
            memcpy(byteBuffer_.getPtr(), &(*mReadBufferPtr)[mReadPos], bytesToReturn);
            mReadPos += bytesToReturn;
            mpPreFilter->onReadCompleted(ByteBuffer(byteBuffer_, 0, bytesToReturn));
        }
    }

    const char * strnstr(const char * str, std::size_t strSize, const char * searchFor)
    {
        const char * iter = std::search(
            str,
            str + strSize,
            searchFor,
            searchFor + strlen(searchFor));

        if ( iter == str + strSize )
        {
            return NULL;
        }

        return iter;
    }

    static const std::string CrLf               = "\r\n";
    static const std::string CrLfCrLf           = "\r\n\r\n";

    static const std::string ContentLength      = "Content-Length";
    static const std::string Connection         = "Connection";
    static const std::string TransferEncoding   = "Transfer-Encoding";
    static const std::string Chunked            = "chunked";
    static const std::string XRcfSessionId      = "X-RCFSessionId";
    static const std::string XRcfSessionIndex   = "X-RCFSessionIndex";
    static const std::string XRcfError          = "X-RCFError";


    void HttpFrameFilter::tryParseHttpChunkHeader()
    {
        const char * pBody = &(*mReadBufferPtr)[mHttpMessage.mHeaderLen];
        const char * pChar = strnstr(pBody, mBytesReceived - mHttpMessage.mHeaderLen, CrLf.c_str());
        if ( pChar )
        {
            mChunkHeaderLen = pChar - pBody + 2;
            mChunkLen = strtoul(pBody, NULL, 16);

            // TODO: dealing w/ the last zero-size chunk.
            // ...

            RCF_ASSERT(mChunkLen != 0);
            mHttpMessage.mFrameLen = mHttpMessage.mHeaderLen + mChunkHeaderLen + mChunkLen + 2;

            if ( RCF::LogManager::instance().isEnabled(LogNameRcf, LogLevel_3) )
            {
                std::string httpMessageHeader(pBody, mChunkHeaderLen);
                RCF_LOG_3()(this)(mHttpSessionId)(mHttpSessionIndex)(mHttpMessage.mFrameLen)("\n" + httpMessageHeader) << "Received chunked HTTP message";
            }

            // Message length check.
            if ( mMaxMessageLength && mHttpMessage.mFrameLen > mMaxMessageLength )
            {
                int rcfError = mClientSide ? RcfError_ClientMessageLength : RcfError_ServerMessageLength;
                RCF_THROW(Exception(Error(rcfError)))(mMaxMessageLength)(mHttpMessage.mFrameLen);
            }

            mMaxReadPos = mHttpMessage.mFrameLen - 2;
            ++mChunkedResponseCounter;
        }
    }


    void HttpMessage::getHttpStatus(std::string& httpStatus, std::string& httpStatusMsg)
    {
        MemIstream is(mHeaderLines[0].c_str(), mHeaderLines[0].size());
        std::string httpVer;
        is >> httpVer >> httpStatus >> httpStatusMsg;
        boost::trim_left(httpStatus);
    }

    void HttpMessage::getHeaderValue(const std::string& headerName, std::string& headerValue)
    {
        headerValue = "";
        for ( std::size_t i = 0; i < mHeaderList.size(); ++i )
        {
            if ( iequals(headerName, mHeaderList[i].first) )
            {
                headerValue = mHeaderList[i].second;
            }
        }
    }
    
    bool HttpMessage::parseHttpMessage(const char * pFrame, std::size_t bytesAvailable)
    {
        // Search for CRLF CRLF to mark end of HTTP header, and then parse the 
        // lines in the header.

        const char * pChar = strnstr(pFrame, bytesAvailable, CrLfCrLf.c_str());
        if ( !pChar )
        {
            return false;
        }

        mHeaderLen = pChar - pFrame + 4;

        mRequestLine.clear();
        mResponseLine.clear();

        // Store the HTTP header in a string. Round up the size so the same string
        // can fit subsequent headers as well, without new allocations. X-RCFSessionIndex 
        // in particular will make the header size grow.
        std::size_t headerLenRoundedUp = 100 * (1 + mHeaderLen / 100);
        mHttpMessageHeader.reserve(headerLenRoundedUp);
        mHttpMessageHeader.assign(pFrame, mHeaderLen);

        // Split HTTP message into lines.
        splitString(mHttpMessageHeader, CrLf.c_str(), mHeaderLines);

        // Parse request/response line.
        const std::string & firstLine = mHeaderLines.front();
        if ( 0 == strncmp(firstLine.c_str(), "POST", 4) )
        {
            mRequestLine = mHeaderLines.front();
        }
        else if ( 0 == strncmp(firstLine.c_str(), "GET", 3) )
        {
            mRequestLine = mHeaderLines.front();
        }
        else if ( 0 == strncmp(firstLine.c_str(), "HTTP/", 5) )
        {
            mResponseLine = mHeaderLines.front();
        }
        
        // Parse headers.
        mHeaderList.resize(mHeaderLines.size() - 1);
        for ( std::size_t i = 1; i < mHeaderLines.size(); ++i )
        {
            const std::string & line = mHeaderLines[i];

            std::size_t pos = line.find(':');
            if ( pos != std::string::npos )
            {
                const char * pHeaderName = line.c_str();
                const char * pHeaderValue = pHeaderName + pos;
                while ( isspace((int)*(++pHeaderValue)) );

                mHeaderList[i].first.assign(pHeaderName, pos);
                mHeaderList[i].second.assign(pHeaderValue);
            }
        }

        return true;

    }
    

    void HttpFrameFilter::tryParseHttpHeader()
    {
        // Sanity check that we are receiving a HTTP message.
        if ( !mProtocolChecked && mBytesReceived >= 4 )
        {
            char * pBuffer = &(*mReadBufferPtr)[0];

            if ( mClientSide && 0 != strncmp(pBuffer, "HTTP", 4) )
            {
                Exception e(_RcfError_NotHttpResponse());
                onError(e);
            }
            else if ( !mClientSide && 0 == strncmp(pBuffer, "GET", 3) )
            {
                Exception e(_RcfError_NotHttpPostRequest());
                onError(e);
            }
            else if ( !mClientSide && 0 != strncmp(pBuffer, "POST", 4) )
            {
                Exception e(_RcfError_NotHttpRequest());
                onError(e);
            }
            mProtocolChecked = true;
        }

        const char * pFrame = &(*mReadBufferPtr)[0];
        bool parseOk = mHttpMessage.parseHttpMessage(pFrame, mBytesReceived);
        if ( !parseOk )
        {
            return;
        }

        RCF_LOG_3()(this)(mHttpMessage.mFrameLen)("\n" + mHttpMessage.mHttpMessageHeader) << "Received HTTP message";

        // Scan the HTTP headers.

        mPrevHttpSessionId = mHttpSessionId;
        mPrevHttpSessionIndex = mHttpSessionIndex;

        mConnectionHeader.clear();
        mTransferEncoding.clear();
        mHttpSessionId.clear();
        mHttpSessionIndex = 0;
        
        for ( std::size_t i = 0; i < mHttpMessage.mHeaderList.size(); ++i )
        {
            const std::string& headerName = mHttpMessage.mHeaderList[i].first;
            const std::string& headerValue = mHttpMessage.mHeaderList[i].second;
            if ( iequals(headerName, ContentLength) )
            {
                mHttpMessage.mContentLen = atoi(headerValue.c_str());
                mHttpMessage.mFrameLen = mHttpMessage.mHeaderLen + mHttpMessage.mContentLen;
                mMaxReadPos = mHttpMessage.mFrameLen;
            }
            else if ( iequals(headerName, XRcfSessionId) )
            {
                mHttpSessionId = headerValue;
                mPrevHttpSessionId.reserve(mHttpSessionId.size());
            }
            else if ( iequals(headerName, XRcfSessionIndex) )
            {
                boost::uint32_t idx = atoi(headerValue.c_str());
                if ( idx )
                {
                    mHttpSessionIndex = idx;
                }
            }
            else if ( iequals(headerName, Connection) )
            {
                mConnectionHeader = headerValue;
            }
            else if ( iequals(headerName, XRcfError) )
            {
                if ( mClientSide )
                {
                    Exception e(_RcfError_HttpTunnelError(headerValue));
                    RCF_THROW(e);
                }
            }
            else if ( iequals(headerName, TransferEncoding) )
            {
                mTransferEncoding = headerValue;
                if ( iequals(mTransferEncoding, Chunked) )
                {
                    mChunkedResponseMode = true;
                    mChunkedResponseCounter = 0;
                }
            }
        }

        // Message length check.
        if ( mMaxMessageLength && mHttpMessage.mFrameLen > mMaxMessageLength )
        {
            if ( mClientSide )
            {
                RCF_THROW(Exception(Error(RcfError_ClientMessageLength)))(mMaxMessageLength)(mHttpMessage.mFrameLen);
            }
            else
            {
                sendServerError(RcfError_ServerMessageLength);
            }
        }

        // For non-chunked responses, check that the header specified a Content-Length value.
        if ( mHttpMessage.mHeaderLen && !mChunkedResponseMode && mHttpMessage.mContentLen == 0 )
        {
            std::string httpMessage(&(*mReadBufferPtr)[0], mHttpMessage.mHeaderLen);
            if ( mHttpMessage.mResponseLine.size() > 0 )
            {
                onError(Exception(_RcfError_HttpResponseContentLength(mHttpMessage.mResponseLine, httpMessage)));
            }
            else
            {
                onError(Exception(_RcfError_HttpRequestContentLength()));
            }
            return;
        }

        // Client side - check that the response session ID and session index matches with the request.
        if ( mClientSide )
        {
            if (    mPrevHttpSessionId.size() > 0 
                &&  mHttpSessionId.size() > 0 
                &&  mPrevHttpSessionId != mHttpSessionId )
            {
                Exception e(_RcfError_HttpResponseSessionId(mPrevHttpSessionId, mHttpSessionId));
                onError(e);
                return;
            }
            if (    mPrevHttpSessionIndex 
                &&  mHttpSessionIndex 
                &&  mPrevHttpSessionIndex != mHttpSessionIndex )
            {
                Exception e(_RcfError_HttpResponseSessionIndex(mPrevHttpSessionIndex, mHttpSessionIndex));
                onError(e);
                return;
            }
        }

    }

    void HttpFrameFilter::sendServerError(int error)
    {
        RcfSession * pSession = getCurrentRcfSessionPtr();
        if ( pSession )
        {
            NetworkSession& nwSession = pSession->getNetworkSession();
            AsioNetworkSession& asioNwSession = static_cast<AsioNetworkSession&>(nwSession);
            asioNwSession.setCloseAfterWrite();
            RcfServer& server = pSession->getRcfServer();

            // Encode error message.
            std::vector<ByteBuffer> byteBuffers(1);
            encodeServerError(server, byteBuffers.front(), error);

            // Add frame (4 byte length prefix).
            int messageSize = static_cast<int>(RCF::lengthByteBuffers(byteBuffers));
            ByteBuffer &byteBuffer = byteBuffers.front();
            RCF_ASSERT_GTEQ(byteBuffer.getLeftMargin(), 4);
            byteBuffer.expandIntoLeftMargin(4);
            memcpy(byteBuffer.getPtr(), &messageSize, 4);
            RCF::machineToNetworkOrder(byteBuffer.getPtr(), 4, 1);

            write(byteBuffers);
        }
    }

    void HttpFrameFilter::onReadCompleted(const ByteBuffer & byteBuffer)
    {
        if (byteBuffer.isEmpty())
        {
            mpPreFilter->onReadCompleted(byteBuffer);
            return;
        }

        mBytesReceived += byteBuffer.getLength();
        RCF_ASSERT(mBytesReceived <= mReadBufferPtr->size());

        if ( mHttpMessage.mFrameLen )
        {
            std::size_t bytesRemainingInFrame = mHttpMessage.mFrameLen - mBytesReceived;
            if (bytesRemainingInFrame)
            {
                if ( mReadBufferPtr->size() < mHttpMessage.mFrameLen )
                {
                    mReadBufferPtr->resize(mHttpMessage.mFrameLen);
                }
                mpPostFilter->read( 
                    ByteBuffer(ByteBuffer(mReadBufferPtr), mBytesReceived, bytesRemainingInFrame),
                    bytesRemainingInFrame);
            }
            else
            {
                mReadPos = mHttpMessage.mHeaderLen + mChunkHeaderLen;

                ByteBuffer origReadBuffer = mOrigReadBuffer;
                std::size_t origBytesRequested = mOrigBytesRequested;
                mOrigReadBuffer = ByteBuffer();
                mOrigBytesRequested = 0;

                read(origReadBuffer, origBytesRequested);
            }
        }
        else
        {
            RCF_ASSERT(mHttpMessage.mFrameLen == 0);

            // Try to pick out HTTP header or chunk header, to determine frame length.
            if ( mChunkedResponseMode && mChunkedResponseCounter > 0)
            {
                tryParseHttpChunkHeader();
            }
            else
            {
                tryParseHttpHeader();
                if ( mChunkedResponseMode )
                {
                    tryParseHttpChunkHeader();
                }
            }

            if ( mHttpMessage.mFrameLen == 0 )
            {
                // Still don't know the frame length, so do another read on the network.
                if ( mBytesReceived > 10 * 1024 )
                {
                    onError(Exception(_RcfError_InvalidHttpMessage()));
                    return;
                }
                if ( mBytesReceived == mReadBufferPtr->size() )
                {
                    mReadBufferPtr->resize(mReadBufferPtr->size() + 1024);
                }
                mpPostFilter->read(
                    ByteBuffer(ByteBuffer(mReadBufferPtr), mBytesReceived),
                    mReadBufferPtr->size() - mBytesReceived);
            }
            else 
            {
                // We have the frame length.
                RCF_ASSERT(mHttpMessage.mFrameLen);
                if ( mChunkedResponseMode )
                {
                    RCF_ASSERT(mHttpMessage.mFrameLen == mHttpMessage.mHeaderLen + mChunkHeaderLen + mChunkLen + 2);
                }
                else
                {
                    RCF_ASSERT(mHttpMessage.mFrameLen == mHttpMessage.mHeaderLen + mHttpMessage.mContentLen);
                }

                // Read in any remaining bytes.
                if ( mBytesReceived < mHttpMessage.mFrameLen )
                {
                    std::size_t bytesRemainingInFrame = mHttpMessage.mFrameLen - mBytesReceived;
                    if ( mReadBufferPtr->size() < mBytesReceived + bytesRemainingInFrame )
                    {
                        mReadBufferPtr->resize(mBytesReceived + bytesRemainingInFrame);
                    }
                    mpPostFilter->read(
                        ByteBuffer(ByteBuffer(mReadBufferPtr), mBytesReceived, bytesRemainingInFrame),
                        bytesRemainingInFrame);
                }
                else
                {
                    // Entire frame now available.

                    // Client side - check HTTP response status.
                    if ( mClientSide )
                    {
                        std::size_t pos = mHttpMessage.mResponseLine.find("200");
                        if ( pos == std::string::npos )
                        {
                            std::string httpMessage(&(*mReadBufferPtr)[0], mHttpMessage.mHeaderLen);
                            onError(Exception(_RcfError_HttpResponseStatus(mHttpMessage.mResponseLine, httpMessage)));
                            return;
                        }
                    }

                    // Re-issue the original read.
                    mReadPos = mHttpMessage.mHeaderLen + mChunkHeaderLen;
                    read(mOrigReadBuffer, mOrigBytesRequested);
                }
            }
        }
    }

    std::size_t HttpFrameFilter::getFrameSize()
    {
        return mHttpMessage.mContentLen;
    }

    const std::string & HttpFrameFilter::getHttpSessionId()
    {
        return mHttpSessionId;
    }

    boost::uint32_t HttpFrameFilter::getHttpSessionIndex()
    {
        return mHttpSessionIndex;
    }

    const std::string & HttpFrameFilter::getConnectionHeader()
    {
        return mConnectionHeader;
    }

    void HttpFrameFilter::write(const std::vector<ByteBuffer> & byteBuffers)
    {
        mOrigReadBuffer = ByteBuffer();
        mOrigBytesRequested = 0;

        mWriteBuffers = byteBuffers;
        mWritePos = 0;

        unsigned int messageLength = static_cast<unsigned int>(
            lengthByteBuffers(byteBuffers));

        mOsPtr->rewind();

        if (mServerAddr.size() > 0)
        {
            // Client-side request.

            // This needs to work whether or not we are going through a proxy.

            ++mHttpSessionIndex;

            *mOsPtr <<  
                "POST / HTTP/1.1\r\n"
                "Host: " << mServerAddr << ":" << mServerPort << "\r\n"
                "Accept: */*\r\n"
                "Connection: Keep-Alive\r\n"
                //"Connection: close\r\n"
                "X-RCFSessionId: " << mHttpSessionId << "\r\n"
                "X-RCFSessionIndex: " << mHttpSessionIndex << "\r\n"
                "Content-Length: " << messageLength << "\r\n"
                "\r\n";
        }
        else
        {
            // Server-side response.

            if ( mChunkedResponseMode && mChunkedResponseCounter == 0 )
            {
                *mOsPtr <<
                    "HTTP/1.1 200 OK\r\n"
                    "X-RCFSessionId: " << mHttpSessionId << "\r\n"
                    "X-RCFSessionIndex: " << mHttpSessionIndex << "\r\n"
                    "Connection: Keep-Alive\r\n"
                    "Transfer-Encoding: chunked" << "\r\n"
                    //"Connection: close\r\n"
                    "\r\n";

                char buf[255] = { 0 };
                sprintf(buf, "%x", messageLength);
                *mOsPtr << buf << "\r\n";
            }
            else if ( mChunkedResponseMode && mChunkedResponseCounter > 0 )
            {
                char buf[255] = { 0 };
                sprintf(buf, "%x", messageLength);
                *mOsPtr << buf << "\r\n";
            }
            else
            {
                *mOsPtr <<
                    "HTTP/1.1 200 OK\r\n"
                    "X-RCFSessionId: " << mHttpSessionId << "\r\n"
                    "X-RCFSessionIndex: " << mHttpSessionIndex << "\r\n"
                    "Content-Length: " << messageLength << "\r\n"
                    "Connection: Keep-Alive\r\n"
                    //"Connection: close\r\n"
                    "\r\n";
            }
        }   

        ByteBuffer httpMessageHeaderBuf(mOsPtr);
        mWriteBuffers.insert(mWriteBuffers.begin(), httpMessageHeaderBuf);
        if ( mChunkedResponseMode )
        {
            mWriteBuffers.push_back( ByteBuffer( (char*) CrLf.c_str(), 2, true) );
            ++mChunkedResponseCounter;
        }

        if ( RCF::LogManager::instance().isEnabled(LogNameRcf, LogLevel_3) )
        {
            std::string httpMessageHeader(httpMessageHeaderBuf.getPtr(), httpMessageHeaderBuf.getLength());
            std::size_t frameLen = lengthByteBuffers(mWriteBuffers);
            if ( mChunkedResponseMode )
            {
                RCF_LOG_3()(this)(mHttpSessionId)(mHttpSessionIndex)(frameLen)("\n" + httpMessageHeader) << "Sending chunked HTTP message.";
            }
            else
            {
                RCF_LOG_3()(this)(frameLen)("\n" + httpMessageHeader) << "Sending HTTP message.";
            }
        }

        mpPostFilter->write(mWriteBuffers);
    }

    void HttpFrameFilter::onWriteCompleted(std::size_t bytesTransferred)
    {
        mWritePos += bytesTransferred;
        RCF_ASSERT(mWritePos <= lengthByteBuffers(mWriteBuffers));

        if (mWritePos < lengthByteBuffers(mWriteBuffers))
        {
            std::vector<ByteBuffer> slicedBuffers;
            sliceByteBuffers(slicedBuffers, mWriteBuffers, mWritePos);
            mpPostFilter->write(slicedBuffers);
        }
        else
        {
            std::size_t bytesWritten = mWritePos - mWriteBuffers.front().getLength();
            if ( mChunkedResponseMode )
            {
                // Take chunk trailer '\r\n' into account.
                bytesWritten -= mWriteBuffers.back().getLength();
            }
            if ( bytesWritten )
            {
                mWriteBuffers.clear();
                mpPreFilter->onWriteCompleted(bytesWritten);
            }
            else
            {
                // Zero bytes written indicates we sent a HTTP frame level error back to the client.
                // We do  nothing here, which will close the connection.
            }
        }
    }

    int HttpFrameFilter::getFilterId() const
    {
        return RcfFilter_Unknown;
    }   

    void HttpFrameFilter::onError(const Exception& e)
    {
        if ( mClientSide )
        {
            RCF_THROW(e);
        }
        else
        {
            MemOstreamPtr osPtr(new MemOstream());

            *osPtr
                << "HTTP/1.1 400 Bad Request\r\n"
                << "X-RCFError: " << e.getErrorString() << "\r\n"
                << "Content-Length: 0\r\n"
                << "\r\n";

            ByteBuffer buffer(osPtr);
            mWriteBuffers.clear();
            mWriteBuffers.push_back(buffer);
            mWritePos = 0;
            mpPostFilter->write(mWriteBuffers);
        }
    }

} // namespace RCF
