//
// Created by kahoul on 03/03/19.
//

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <sys/types.h>


#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <limits.h>

#include "PhpCgi.hpp"
#include "Utils.hpp"
#define EOL "\r\n"

void PhpCgiModule::init() {

    garbage << this->master->getPipeline().getHookSet<zany::Pipeline::Hooks::BEFORE_HANDLE_REQUEST>()
            .addTask<zany::Pipeline::Priority::LOW>(
                    std::bind(&PhpCgiModule::_beforeHandleRequest, this, std::placeholders::_1));

    garbage << this->master->getPipeline().getHookSet<zany::Pipeline::Hooks::ON_HANDLE_RESPONSE>()
            .addTask<zany::Pipeline::Priority::MEDIUM>(
                    std::bind(&PhpCgiModule::_onHandleResponse, this, std::placeholders::_1));

}

void PhpCgiModule::_beforeHandleRequest(zany::Pipeline::Instance &i) {
    if (boost::filesystem::extension(i.request.path) == ".php") {
        i.writerID = getUniqueId();
    }

}

void PhpCgiModule::_onHandleResponse(zany::Pipeline::Instance &i) {

    if (i.writerID == this->getUniqueId()
        && i.response.status == 200
        && (i.request.method == zany::HttpRequest::RequestMethods::POST
            || i.request.method == zany::HttpRequest::RequestMethods::GET)) {
        Utils::printPipelineContent(i);
        this->execPhp(i);
        //Utils::printPipelineContent(i);
    }

}


boost::process::environment  PhpCgiModule::buildEnv(zany::Pipeline::Instance &instance) {
    boost::process::environment  env = boost::this_process::environment();

    std::string script;
    std::string query;
    std::string uri(instance.request.path);
    std::string pathInfo(boost::filesystem::path(std::string(/*boost::filesystem::current_path().native() + */"/" + master->getConfig()["docRoot"].value<zany::String>()).c_str()).lexically_normal().string());
    std::string scriptFileName(pathInfo);
    std::string home(getenv("HOME"));
    std::string path(getenv("PATH"));
    std::size_t pos = uri.find_first_of('?');
    std::size_t size(uri.size());

    const char **e = new const char *[env.size() + 1];

    int i = 0;

    std::map<std::string, std::string>::iterator it;
    if (pos == std::string::npos) {
        script = uri;
    } else {
        script.append(uri.substr(0, pos).c_str(), pos);
        if (size && pos < size)
            query.append(uri.substr(pos + 1).c_str(), size - pos);
    }

    scriptFileName = boost::filesystem::path(std::string(scriptFileName + "/" +script).c_str()).lexically_normal().string();

    env["DOCUMENT_ROOT"] = pathInfo;
    env["GATEWAY_INTERFACE"] = "CGI/1.1";
    env["HOME"] = home;
    for (auto it : instance.request.headers) {

        if (it.first == "Accept") {
            env["HTTP_ACCEPT"] = it.second.getData();
        }

        if (it.first == "Accept-Encoding") {
            env["HTTP_ACCEPT_ENCODING"] = it.second.getData();
        }

        if (it.first == "Accept-Charset") {
            env["HTTP_ACCEPT_CHARSET"] = it.second.getData();
        }

        if (it.first == "Accept-Language") {
            env["HTTP_ACCEPT_LANGUAGE"] = it.second.getData();
        }

        if (it.first == "Connection") {
            env["HTTP_CONNECTION"] = it.second.getData();
        }

        if (it.first == "Host") {
            env["HTTP_HOST"] = it.second.getData();
        }
        if (it.first == "User-Agent") {
            env["HTTP_USER_AGENT"] = it.second.getData();
        }

        if (it.first == "Content-Type") {
            env["CONTENT_TYPE"] = it.second.getData();
        }

        if (it.first == "Content-Length") {
            env["CONTENT_LENGTH"] = it.second.getData();
        }

    }
    env["PATH"] = path;
    env["PATH_INFO"] = pathInfo;
    env["PATH_TRANSLATED"] = pathInfo;
    env["CONTENT_TYPE"] = "application/x-www-form-urlencoded";
    env["QUERY_STRING_POST"] = instance.request.fullQuery;
    env["QUERY_STRING"] = instance.request.fullQuery;
    env["REQUEST_METHOD"] = instance.request.methodString;
    env["SCRIPT_FILENAME"] = scriptFileName;
    env["SCRIPT_NAME"] = script;
    env["SYSTEMROOT"] = "/";
    env["REDIRECT_STATUS"] = "true";
    return env;
}



void PhpCgiModule::execPhp(zany::Pipeline::Instance &i) {
    std::string bin("/usr/bin/php-cgi");
    boost::process::environment  env = this->buildEnv(i);
    std::string bdy = "";
    std::string query = "nom=oklm&prenom=oklm";
    std::vector<char> buf;
    std::vector<char> buf2;
    std::string line;
    boost::process::ipstream pipe_stream;


   // int ret = boost::process::system(bin, boost::process::std_out > pipe_stream, env);


    boost::process::ipstream is; //reading pipe-stream
    boost::process::child c(boost::process::search_path("php-cgi"), boost::process::std_out > is, boost::process::std_out > is, env);

    std::vector<std::string> data;


    while (c.running() && std::getline(is, line) && !line.empty())
    {
        std::cout << "line: " << line << std::endl;
        data.push_back(line);
        std::size_t found = line.find("\r");
        if (found!=std::string::npos) {
            std::cout << "end read" << std::endl;
            break;
        }
    }
    c.wait();

    std::string header;
    std::string phpBody;
    size_t pos, last_pos;
    pos = bdy.find(EOL
                   EOL);
    header = bdy.substr(0, pos);

    if (pos != std::string::npos)
    {
        phpBody = bdy.substr(pos + 4);
        char *tmp;
        int size = phpBody.size();
        tmp = new char[size];
        phpBody.copy(tmp, size);

        std::cout << "SEt body: " << tmp << " of size " << size << "." << std::endl;
        std::cout << "SET content: " << phpBody << std::endl;
        i.response.headers.emplace("Content-Length", std::to_string(phpBody.length()));
    }

    phpBody = bdy;
    std::cout << "phpBody: " << phpBody << std::endl;
    i.response.headers.emplace("Content-Length", std::to_string(phpBody.length()));
    i.response.headers.emplace("Content-type", "text/html; charset=UTF-8");
    Utils::writeResponse(i, phpBody);
}


extern "C" ZANY_DLL
        zany::Loader::AbstractModule    *

entrypoint() {
    return new PhpCgiModule();
}