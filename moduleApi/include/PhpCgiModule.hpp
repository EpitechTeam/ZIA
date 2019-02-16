//
// Created by kahoul on 21/01/19.
//

#ifndef ZIA_PHPCGIMODULE_HPP
#define ZIA_PHPCGIMODULE_HPP

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <map>
#include <string>

#include <boost/filesystem.hpp>

#include "IModule.hpp"
#include "Connection.hpp"

#define EOL "\r\n"

void print_request(request &req) {
    std::cout << "\033[1;31m [" << req.method << "] \033[0m";
    std::cout << "\033[1;37m " << req.uri << " \033[0m\n";
    std::cout << "\033[1;31m major: " << req.http_version_major << " \033[0m";
    std::cout << "\033[1;31m minor: " << req.http_version_minor << " \033[0m\n";
    for (unsigned int i = 0; req.headers.size() != i; i++) {
        std::cout << "\033[1;32m" << req.headers[i].name << " : \033[0m";
        std::cout << "\033[1;37m" << req.headers[i].value << "\033[0m" << std::endl;
    }
    std::cout << "\nPARAMS:" << std::endl;
    for (unsigned int i = 0; req.params.size() != i; i++) {
        std::cout << "\033[1;32m" << req.params[i].name << " : \033[0m";
        std::cout << "\033[1;37m" << req.params[i].value << "\033[0m" << std::endl;
    }
}

class PhpCgiModule : public IModule {
public:
    PhpCgiModule() : IModule("PhpCgiModule") {

    }

    ~PhpCgiModule() = default;

    int catchEvent(const Event &event, request &req, reply &scope, ConnectionPtr connection) {
        switch (event) {
            case AFTER_FILL_RESPONSE:
                //std::cout << "AFTER_FILL_RESPONSE catched by PhpCgiModule." << std::endl;
                print_request(req);
                this->execPhp(req, scope, connection);
                break;
            default:
                //print_request(req);
                //std::cout << "event" << std::endl;
                break;
        }
        return 0;
    }

    const char **buildEnv(const request &req, const reply &scope) {

        std::map <std::string, std::string> env;
        std::string script;
        std::string query;
        std::string uri(req.uri);
        std::string pathInfo(boost::filesystem::current_path().native() + "/assets/PHPForm");


        std::cout << __LINE__ <<   __FUNCTION__<< pathInfo << std::endl;
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
        if(script == "/")
            script = "/index.html",
        std::cout << "SCRIPT ASKIP " << script << std::endl;
        scriptFileName =  scriptFileName + script;
        std::cout << "SCRIPT + PATH ASKIP " << scriptFileName << std::endl;

        env["DOCUMENT_ROOT"] = pathInfo;
        env["GATEWAY_INTERFACE"] = "CGI/1.1";
        env["HOME"] = home;

        for (auto it : req.headers) {

            if (it.name == "Accept") {
                env["HTTP_ACCEPT"] = it.value;
            }

            if (it.name == "Accept-Encoding") {
                env["HTTP_ACCEPT_ENCODING"] = it.value;
            }

            if (it.name == "Accept-Charset") {
                env["HTTP_ACCEPT_CHARSET"] = it.value;
            }

            if (it.name == "Accept-Language") {
                env["HTTP_ACCEPT_LANGUAGE"] = it.value;
            }

            if (it.name == "Connection") {
                env["HTTP_CONNECTION"] = it.value;
            }

            if (it.name == "Host") {
                env["HTTP_HOST"] = it.value;
            }

            if (it.name == "User-Agent") {
                env["HTTP_USER_AGENT"] = it.value;
            }

        }

        env["PATH"] = path;
        // env["PATHEXT"] = ".COM;.EXE;.BAT;.CMD;.VBS;.VBE;.JS;.JSE;.WSF;.WSH;.MSC";
        env["PATH_INFO"] = pathInfo;
        env["PATH_TRANSLATED"] = pathInfo;

        // env["QUERY_STRING"] = query;

        // env["REMOTE_ADDR"] = "127.0.0.1";
        // env["REMOTE_PORT"] = "63555";
        env["REQUEST_METHOD"] = req.method;
        // env["REQUEST_URI"] = requestHeader.getArg();

        env["SCRIPT_FILENAME"] = scriptFileName;
        env["SCRIPT_NAME"] = script;
        printf("file name: %s\n", scriptFileName);

        // env["SERVER_ADDR"] = "127.0.0.1";
        // env["SERVER_ADMIN"] = "(server admin's email address)";
        // env["SERVER_NAME"] = "127.0.0.1";
        // env["SERVER_PORT"] = "80";
        env["SERVER_PROTOCOL"] = req.http_version_major;
        env["SERVER_SIGNATURE"] = "";
        env["SERVER_SOFTWARE"] = "a.out server zia_server";

        env["SYSTEMROOT"] = "/";
        env["REDIRECT_STATUS"] = "true";

        e = new const char *[env.size() + 1];

        for (it = env.begin(); it != env.end(); ++it) {
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

    void execPhp(const request &req, reply &scope, ConnectionPtr connection) {

        pid_t pid;
        int pipe_fds[2];
        size_t length;
        char *buff;

        std::cout << "ExecPhp" << std::endl;
        pipe(pipe_fds);

        pid = fork();
        if (pid == -1) {
            close(pipe_fds[0]);
            close(pipe_fds[1]);
            std::cout << "[Error]: Fork return -1." << std::endl;
            return ;
        }

        std::cout << "PID =>" << pid << std::endl;
        if (pid == 0) {
            std::string bin("/usr/bin/php-cgi");

            const char *argv[] = {
                    bin.c_str(),
                    NULL
            };

            const char **env = this->buildEnv(req, scope);

            //connection->stop();
            //close(socket);

            std::cout << "BEFORE close" << std::endl;

            if (close(pipe_fds[0]) == -1) {
                std::cout << "[Error]: close returned -1" << std::endl;
            }

            std::cout << "BEFORE dup2" << std::endl;

            if (dup2(pipe_fds[1], 1) == -1) {
                std::cout << "[Error]: dup2 returned -1" << std::endl;
            }

            std::cout << "BEFORE Execve" << std::endl;
            int ret = execve(bin.c_str(), const_cast<char *const *> (argv), const_cast<char *const *> (env));
            std::cout << "Execve => " << ret << std::endl;
            exit(ret);
        } else {

            int child_rt;
            close(pipe_fds[1]);

            waitpid(pid, &child_rt, 0);

            /*if (child_rt != 0) {
                close(pipe_fds[0]);
                std::cout << "[Error]: child_rt != 0." << std::endl;
                return ;
            }*/

            char buff[128];
            std::string bdy;
            int size;
            size = read(pipe_fds[0], buff, 128);

            while (size != -1 && size) {
                bdy.append(buff, size);
                size = read(pipe_fds[0], buff, 128);
            }
            printf(" ====> output %s\n", buff);

            if (size == -1) {
                close(pipe_fds[0]);
                std::cout << "[Error]: Size == -1." << std::endl;
                return ;
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
//                scope.content = phpBody;
                scope.content.append("<p> Hello World </p>\n");
                scope = reply::stockReply(reply::ok);

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
                    // responseHeader.setValue(key, value);
                }

                last_pos = pos + 2;
            } while (std::string::npos != pos);

           /* if (responseHeader.getStatusCode().empty()) {
                responseHeader.setStatusCode("200");
                responseHeader.setStatusMessage("OK");
            }*/

            //responseHeader.setVersion(request.getHeader().getVersion());

            //response.assemble();

            close(pipe_fds[0]);
        }
    }
};

#endif //R_TYPE_PHPCGIMODULE_HPP
