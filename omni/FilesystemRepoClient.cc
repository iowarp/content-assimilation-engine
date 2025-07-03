#include "OMNI.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#ifdef USE_POCO
#include <Poco/Net/Context.h>
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPSClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/NetException.h>
#include <Poco/Net/SSLManager.h>
#include <Poco/NullStream.h>
#include <Poco/StreamCopier.h>
#include <Poco/URI.h>
#endif

namespace CAE {

void FilesystemRepoClient::Download(const RepoContext &ctx) {
    // For demonstration, assume ctx.key_ is the URL or file path, and username/passwd are unused.
    const std::string& url = ctx.key_;
    const std::string outputFileName = std::filesystem::path(url).filename();
#ifdef USE_POCO
    try {
        std::string currentUrl = url;
        const std::string caCertFile = "../cacert.pem";
        Poco::Net::Context::Ptr context = new Poco::Net::Context(Poco::Net::Context::CLIENT_USE, "", "", "", Poco::Net::Context::VERIFY_NONE, 9, true, caCertFile);
        int redirectCount = 0;
        int maxRedirects = 20;
        bool redirected = true;
        while (redirected && redirectCount <= maxRedirects) {
            redirected = false;
            redirectCount++;
            Poco::URI uri(currentUrl);
            std::unique_ptr<Poco::Net::HTTPClientSession> session;
            if (uri.getScheme() == "https") {
                session = std::make_unique<Poco::Net::HTTPSClientSession>(uri.getHost(), uri.getPort() == 0 ? 443 : uri.getPort(), context);
            } else if (uri.getScheme() == "http") {
                session = std::make_unique<Poco::Net::HTTPClientSession>(uri.getHost(), uri.getPort());
            } else {
                // Not HTTP/HTTPS, treat as local file
                std::ifstream src(url, std::ios::binary);
                std::ofstream dst(outputFileName, std::ios::binary);
                dst << src.rdbuf();
                std::cout << "Copied local file to " << outputFileName << std::endl;
                return;
            }
            Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, uri.getPathAndQuery(), Poco::Net::HTTPMessage::HTTP_1_1);
            request.set("User-Agent", "POCO HTTP Redirect Client/1.0");
            Poco::Net::HTTPResponse response;
            session->sendRequest(request);
            std::istream& rs = session->receiveResponse(response);
            int status = response.getStatus();
            if (status == Poco::Net::HTTPResponse::HTTP_MOVED_PERMANENTLY ||
                status == Poco::Net::HTTPResponse::HTTP_FOUND ||
                status == Poco::Net::HTTPResponse::HTTP_SEE_OTHER ||
                status == Poco::Net::HTTPResponse::HTTP_TEMPORARY_REDIRECT ||
                status == Poco::Net::HTTPResponse::HTTP_PERMANENT_REDIRECT) {
                if (response.has("Location")) {
                    currentUrl = response.get("Location");
                    redirected = true;
                    Poco::NullOutputStream nullStream;
                    Poco::StreamCopier::copyStream(rs, nullStream);
                } else {
                    std::cerr << "Redirect status received but no Location header found." << std::endl;
                    return;
                }
            } else if (status == Poco::Net::HTTPResponse::HTTP_OK) {
                std::ofstream os(outputFileName, std::ios::binary);
                if (os.is_open()) {
                    Poco::StreamCopier::copyStream(rs, os);
                    os.close();
                    std::cout << "File downloaded successfully to: " << outputFileName << std::endl;
                } else {
                    std::cerr << "Error: Could not open file for writing: " << outputFileName << std::endl;
                }
                return;
            } else {
                std::cerr << "Error: HTTP request failed with status code " << status << std::endl;
                std::string errorBody;
                Poco::StreamCopier::copyToString(rs, errorBody);
                std::cerr << "Response Body: " << errorBody << std::endl;
                return;
            }
        }
        if (redirectCount > maxRedirects) {
            std::cerr << "Error: Maximum redirect limit exceeded." << std::endl;
        }
    } catch (const Poco::Net::NetException& e) {
        std::cerr << "Network Error: " << e.displayText() << std::endl;
    } catch (const Poco::Exception& e) {
        std::cerr << "POCO Error: " << e.displayText() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Standard Error: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Unknown Error occurred." << std::endl;
    }
#else
    // Fallback: just copy local file
    std::ifstream src(url, std::ios::binary);
    std::ofstream dst(outputFileName, std::ios::binary);
    dst << src.rdbuf();
    std::cout << "Copied local file to " << outputFileName << std::endl;
#endif
}

void FilesystemRepoClient::Upload(const RepoContext &ctx, const std::string& local_path) {
    std::filesystem::copy_file(local_path, ctx.key_, std::filesystem::copy_options::overwrite_existing);
    std::cout << "Copied " << local_path << " to " << ctx.key_ << std::endl;
}

} // namespace CAE 