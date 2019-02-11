//
// Created by kahoul on 21/01/19.
//

#ifndef ZIA_REQUEST_PARSER_HPP
#define ZIA_REQUEST_PARSER_HPP

#include <tuple>

        struct request;

        class RequestParser
        {
        public:
            RequestParser();

            void reset();

            enum resultType { good, bad, indeterminate, after_header};

            template <typename InputIterator>
            std::tuple<resultType, InputIterator> parse(request& req,
                                                         InputIterator begin, InputIterator end)
            {
                resultType result;

                while (begin != end)
                {
                    result = consume(req, *begin++);
                    if (result == good || result == bad)
                        return std::make_tuple(result, begin);
                }
                return std::make_tuple(result == after_header ? good : indeterminate, begin);
            }

        private:
            resultType consume(request& req, char input);

            static bool isChar(int c);

            static bool isCtl(int c);

            static bool isTspecial(int c);

            static bool isDigit(int c);

            enum state
            {
                method_start,
                method,
                uri,
                http_version_h,
                http_version_t_1,
                http_version_t_2,
                http_version_p,
                http_version_slash,
                http_version_major_start,
                http_version_major,
                http_version_minor_start,
                http_version_minor,
                expecting_newline_1,
                header_line_start,
                header_lws,
                header_name,
                space_before_header_value,
                header_value,
                expecting_newline_2,
                expecting_newline_3,
                param_line_start,
                param_name,
                param_value
            } _state;
        };

#endif //ZIA_REQUEST_PARSER_HPP
