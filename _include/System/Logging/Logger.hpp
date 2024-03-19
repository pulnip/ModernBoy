#pragma once

#include <memory>
#include "System/Logging/Types.hpp" 

class Logger{
  protected:
    using Info=Logging::Info;
    using String=Logging::String;
    using Integer=Logging::Integer;
    using Double=Logging::Double;
    using Level=Logging::Level;

  public:
    virtual void log(Level, const Info& who, const std::string& msg)=0;
    virtual void log(Level, const Info& who, const String& data)=0;
    virtual void log(Level, const Info& who, const Integer& data)=0;
    virtual void log(Level, const Info& who, const Double& data)=0;

    virtual void setCurrLevel(Level level)=0;
    virtual Level getCurrLevel()=0;

    virtual bool isShown(Level set) noexcept=0;

    static std::shared_ptr<Logger> null;
};

class NullLogger final: public Logger{
  public:
    void log(Level, const Info&, const std::string&) override{}
    void log(Level, const Info&, const String&) override{}
    void log(Level, const Info&, const Integer&) override{}
    void log(Level, const Info&, const Double&) override{}

    void setCurrLevel(Level level) override{
        currLevel=level;
    }
    Level getCurrLevel() override{
        return currLevel;
    }
    bool isShown(Level set) noexcept override{
        return static_cast<int>(currLevel) <= static_cast<int>(set);
    }

  private:
    Level currLevel;
};
