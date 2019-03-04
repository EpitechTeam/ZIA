//
// Created by kahoul on 03/03/19.
//

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

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
        this->execPhp(i);
        Utils::printPipelineContent(i);
    }

}


const char **PhpCgiModule::buildEnv(zany::Pipeline::Instance &instance) {
    std::map <std::string, std::string> env;
    std::string script;
    std::string query;
    std::string uri(instance.request.path);
    std::string pathInfo(
            realpath(std::string(boost::filesystem::current_path().native() + "/" + master->getConfig()["docRoot"].value<zany::String>()).c_str(), NULL));

    std::cout << "PASSED: " << __LINE__ << std::endl;
    std::string scriptFileName(pathInfo);
    std::string home(getenv("HOME"));
    std::string path(getenv("PATH"));

    std::cout << "PASSED: " << __LINE__ << std::endl;
    std::size_t pos = uri.find_first_of('?');
    std::size_t size(uri.size());
    std::cout << "PASSED: " << __LINE__ << std::endl;

    const char **e = new const char *[env.size() + 1];
    int i = 0;
    std::map<std::string, std::string>::iterator it;
    std::cout << "PASSED: " << __LINE__ << std::endl;

    if (pos == std::string::npos) {
        script = uri;
    } else {
        script.append(uri.substr(0, pos).c_str(), pos);
        if (size && pos < size)
            query.append(uri.substr(pos + 1).c_str(), size - pos);
    }
    std::cout << "PASSED: " << __LINE__ << std::endl;
    if (script == "/")
        script = "/index.html",
                std::cout << "SCRIPT ASKIP " << script << std::endl;
    std::cout << "PASSED: " << __LINE__ << std::endl;
    scriptFileName = std::string(scriptFileName + script).c_str();
    std::cout << "SCRIPT + PATH ASKIP " << scriptFileName << std::endl;

    std::cout << "PASSED: " << __LINE__ << std::endl;

    env["DOCUMENT_ROOT"] = pathInfo;
    env["GATEWAY_INTERFACE"] = "CGI/1.1";
    env["HOME"] = home;
    std::cout << "PASSED: " << __LINE__ << std::endl;

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


    std::cout << "URI: " << instance.request.fullQuery << " <=> " << instance.request.fullQuery.size() << std::endl;
    //env["QUERY_STRING"] = req.query;
    env["PATH"] = path;
    // env["PATHEXT"] = ".COM;.EXE;.BAT;.CMD;.VBS;.VBE;.JS;.JSE;.WSF;.WSH;.MSC";

    env["PATH_INFO"] = pathInfo;
    env["PATH_TRANSLATED"] = pathInfo;



    //env["REMOTE_ADDR"] = "127.0.0.1";
    //env["REMOTE_PORT"] = "4242";

    std::cout << "URI: " << instance.request.path.c_str() << std::endl;
    //env["REQUEST_URI"] = req.uri;
    env["CONTENT_TYPE"] = "application/x-www-form-urlencoded";
    env["QUERY_STRING_POST"] = instance.request.fullQuery;
    env["REQUEST_METHOD"] = instance.request.methodString;


    env["SCRIPT_FILENAME"] = scriptFileName;
    env["SCRIPT_NAME"] = script;

    //env["SERVER_ADDR"] = "127.0.0.1";
    // env["SERVER_ADMIN"] = "(server admin's email address)";
    //env["SERVER_NAME"] = "zia";
    //env["SERVER_PORT"] = "4242";
    //env["SERVER_PROTOCOL"] = req.http_version_major;
    //env["SERVER_SIGNATURE"] = "";
    //env["SERVER_SOFTWARE"] = "a.out server zia_server";

    env["SYSTEMROOT"] = "/";
    env["REDIRECT_STATUS"] = "true";

    e = new const char *[env.size() + 1];

    for (it = env.begin(); it != env.end(); ++it) {
        std::cout << it->first + " = " + it->second << std::endl;
        std::string s(it->first + "=" + it->second);
        int size = s.size();
        char *buff = new char[size + 1];
        s.copy(buff, size);
        buff[size] = 0;
        e[i] = buff;
        ++i;
    }
    e[i] = NULL;
    return e;
}

void PhpCgiModule::exec1(zany::Pipeline::Instance &i, int pipe1[2]) {
    // input from stdin (already done)
    // output to pipe1
    dup2(pipe1[1], 1);
    // close fds
    close(pipe1[0]);
    close(pipe1[1]);
    // exec
    execlp("/bin/echo", "/bin/echo", (char *) i.request.fullQuery.c_str(), NULL);
    // exec didn't work, exit
    perror("bad exec ps");
    _exit(1);
}


void PhpCgiModule::execPhp(zany::Pipeline::Instance &i) {
    pid_t pid;
    int pipe_fds[2];
    size_t length;
    char *buff;

    std::cout << "ExecPhp" << std::endl;


    int pipe1[2];


    // create pipe1
    if (pipe(pipe1) == -1) {
        perror("bad pipe1");
        exit(1);
    }

    // fork (ps aux)
    if ((pid = fork()) == -1) {
        perror("bad fork1");
        exit(1);
    } else if (pid == 0) {
        // stdin --> ps --> pipe1
        this->exec1(i, pipe1);
    }


    pipe(pipe_fds);
    pid = fork();
    if (pid == -1) {
        close(pipe_fds[0]);
        close(pipe_fds[1]);
        std::cout << "[Error]: Fork return -1." << std::endl;
        return;
    }


    std::cout << "PID =>" << pid << std::endl;
    if (pid == 0) {
        std::string bin("/usr/bin/php-cgi");

        const char *argv[] = {
                bin.c_str(),
                NULL
        };

        std::cout << "BEFORE" << std::endl;

        const char **env = this->buildEnv(i);

        std::cout << "AFTER" << std::endl;
        //connection->stop();
        //close(socket);

        std::cout << "BEFORE close" << std::endl;
        if (close(pipe_fds[0]) == -1) {
            std::cout << "[Error]: close returned -1" << std::endl;
        }

        std::cout << "BEFORE dup2" << std::endl;
        dup2(pipe1[0], 0);

        std::cout << "DEBUG:" << __LINE__ << std::endl;

        close(pipe1[0]);
        std::cout << "DEBUG:" << __LINE__ << std::endl;
        close(pipe1[1]);
        std::cout << "DEBUG:" << __LINE__ << std::endl;

        if (dup2(pipe_fds[1], 1) == -1) {
            std::cout << "[Error]: dup2 returned -1" << std::endl;
        }
        std::cout << "DEBUG:" << __LINE__ << std::endl;

        std::cout << "BEFORE Execve" << std::endl;
        std::string exec_cmd =
                "/bin/echo \"" + (i.request.fullQuery.length() != 0 ? i.request.fullQuery : std::string("")) + std::string("\"  | ") + bin;
        std::cout << "[" << exec_cmd << "]" << std::endl;


        int ret = execve(bin.c_str(), const_cast<char *const *> (argv), const_cast<char *const *> (env));
        std::cout << "Execve => " << ret << std::endl;
        exit(ret);
    } else {
        int child_rt;
        close(pipe_fds[1]);
        waitpid(pid, &child_rt, 0);


        char buff[128];
        std::string bdy;
        int size;
        size = read(pipe_fds[0], buff, 128);

        while (size != -1 && size) {
            bdy.append(buff, size);
            size = read(pipe_fds[0], buff, 128);
        }

        fwrite(buff, size, 1, stdout);

        //printf(" ====> output %s\n", buff);

        if (child_rt != 0) {
            close(pipe_fds[0]);
            std::cout << "[Error]: child_rt != 0." << std::endl;
            return;
        }


        if (size == -1) {
            close(pipe_fds[0]);
            std::cout << "[Error]: Size == -1." << std::endl;
            return;
        }

        std::string header;
        std::string phpBody;

        size_t pos, last_pos;

        pos = bdy.find(EOL
        EOL);

        header = bdy.substr(0, pos);
        if (pos != std::string::npos) {
            phpBody = bdy.substr(pos + 4);

            char *tmp;
            size = phpBody.size();

            tmp = new char[size];
            phpBody.copy(tmp, size);
            std::cout << "SEt body: " << tmp << " of size " << size << "." << std::endl;
            std::cout << "SET content: " << phpBody << std::endl;


          //  scope.content = phpBody;
          //  scope.headers[0].name = "Content-Length";
          //  scope.headers[0].value = std::to_string(scope.content.size());

            i.response.headers.emplace("Content-Length", std::to_string(phpBody.length()));

            //scope = reply::stockReply(reply::ok);

            //phpBody.copy(tmp, size);

            //body.setBody(tmp, size);
        }

        last_pos = pos = 0;
        size_t line_pos;

        do {
            pos = header.find(EOL, last_pos);
            std::string line = header.substr(last_pos, pos - last_pos);

            line_pos = line.find(": ");
            std::string key(line.substr(0, line_pos));
            std::string value(line.substr(line_pos + 2));

            if (key == "Status") {
                size_t pos;
                pos = value.find_first_of(' ');
                std::cout << "Set Status Code: " << value.substr(0, pos) << std::endl;
                // responseHeader.setStatusCode(value.substr(0, pos));
                std::cout << "Set Status Message: " << value.substr(pos + 1) << std::endl;
                // responseHeader.setStatusMessage(value.substr(pos + 1));
            } else {
                std::cout << "Set Value: (" << key << ", " << value << ")" << std::endl;
                i.response.headers.emplace(key, value);

              //  scope.headers[1].name = key;
              //  scope.headers[1].value = value;
                //rep.headers.resize(2);
                // responseHeader.setValue(key, value);
            }

            last_pos = pos + 2;
        } while (std::string::npos != pos);

        std::cout << "WTF: " << std::endl;

        std::cout << "phpBody: " << phpBody << std::endl;
        Utils::writeResponse(i, phpBody);

        /* if (responseHeader.getStatusCode().empty()) {
             responseHeader.setStatusCode("200");
             responseHeader.setStatusMessage("OK");
         }*/

        //responseHeader.setVersion(request.getHeader().getVersion());

        //response.assemble();

        close(pipe_fds[0]);
    }
}


extern "C" ZANY_DLL
        zany::Loader::AbstractModule    *

entrypoint() {
    return new PhpCgiModule();
}