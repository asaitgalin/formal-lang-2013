#ifndef _LOLCODE_VALUE_H_
#define _LOLCODE_VALUE_H_

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <string>
#include <cmath>
#include <cfloat>
#include <sstream>

#include "lolcode_type.h"

void raiseMachineError(const std::string &error);

class Value {
public:
    virtual Type *getType() = 0;
    virtual std::string toString(bool impl = true) const = 0;
    virtual bool toBoolean() const = 0;
    virtual int toInteger(bool &successful, bool impl = true) const = 0;
    virtual float toFloat(bool &successful, bool impl = true) const = 0;
};

class UntypedValue: public Value {
public:

    virtual Type *getType() {
        return Type::_untyped;
    }

    virtual std::string toString(bool impl) const {
        if (impl) {
            raiseMachineError("cannot implicitly cast NOOB to string");
        }
        return std::string();
    }

    virtual bool toBoolean() const {
        return false;
    }

    virtual int toInteger(bool &successful, bool impl) const {
        if (impl) {
            raiseMachineError("cannot implicitly cast NOOB to int");
        }
        return 0;
    }

    virtual float toFloat(bool &successful, bool impl) const {
        if (impl) {
            raiseMachineError("cannot implicitly cast NOOB to float");
        }
        return 0.0f;
    }

};

class IntValue: public Value {
public:

    IntValue(int value):
        value_(value) 
    { }

    virtual Type *getType() {
        return Type::_integer;
    }

    virtual std::string toString(bool impl) const {
        return std::to_string(value_);
    }

    virtual bool toBoolean() const {
        return value_ != 0;
    }

    virtual int toInteger(bool &successful, bool impl) const {
        successful = true;
        return value_;
    }

    virtual float toFloat(bool &successful, bool impl) const {
        successful = true;
        return static_cast<float>(value_);
    }

private:
    int value_;
};

class StringValue: public Value {
public:
    
    StringValue(const char *str) {
        // Skip quotes
        value_ = std::string(str + 1, strlen(str) - 2);
    }

    StringValue(const std::string &str):
        value_(str)
    { }

    virtual Type *getType() {
        return Type::_string;
    }

    virtual std::string toString(bool impl) const {
        return value_;
    }

    virtual bool toBoolean() const {
        return value_.length() != 0;
    }

    virtual int toInteger(bool &successful, bool impl) const {
        int result = 0;
        std::stringstream ss(value_);
        ss >> std::noskipws >> result;
        successful = !ss.fail() && ss.get() == EOF; 
        return result;
    }

    virtual float toFloat(bool &successful, bool impl) const {
        float result = 0.0f;
        std::stringstream ss(value_);
        ss >> std::noskipws >> result;
        successful = !ss.fail() && ss.get() == EOF;
        return result;
    }

private:
    std::string value_;
};

class FloatValue: public Value {
public:
    
    FloatValue(float value):
        value_(value)
    { }

    virtual Type *getType() {
        return Type::_float;
    }

    virtual std::string toString(bool impl) const {
        return std::to_string(value_);
    }

    virtual bool toBoolean() const {
        return fabs(value_) > FLT_EPSILON;
    }

    virtual int toInteger(bool &successful, bool impl) const {
        successful = true;
        return (int)value_;
    }

    virtual float toFloat(bool &successful, bool impl) const {
        successful = true;
        return value_;
    }

private:
    float value_;
};

class BoolValue: public Value {
public:

    BoolValue(bool value):
        value_(value)
    { }

    virtual Type *getType() {
        return Type::_boolean;
    }

    virtual std::string toString(bool impl) const {
        return value_ ? "WIN" : "FAIL";
    }

    virtual bool toBoolean() const {
        return value_;
    }

    virtual int toInteger(bool &successful, bool impl) const {
        successful = true;
        return (int)value_;
    }
    
    virtual float toFloat(bool &successful, bool impl) const {
        return (float)value_;
    }

private:
    bool value_;
};

#endif /* _LOLCODE_VALUE_H_ */
