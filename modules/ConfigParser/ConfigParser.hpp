//
// Created by kahoul on 04/03/19.
//

#ifndef ZIA_CONFIGPARSER_HPP
#define ZIA_CONFIGPARSER_HPP

class ConfigParserModule : public zany::Loader::AbstractModule {
public:
    ConfigParserModule() = default;
    ~ConfigParserModule() = default;

    virtual auto	name() const -> const std::string&
    { static const std::string name("ConfigParser"); return name; }

    virtual void	init() {};

    virtual bool	isAParser() final { return true; }

    virtual auto	parse(std::string const &filename) -> zany::Entity final;
private:

    bool            isJsonFile(const std::string &path) const;
    zany::Entity	parseConfigFile(const std::string &path);

};

#endif //ZIA_CONFIGPARSER_HPP
