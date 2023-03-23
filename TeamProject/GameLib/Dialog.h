#pragma once
#include "Interface.h"
class Dialog
    : public Interface
{
public:
    virtual bool    Init() override;
    virtual bool    Frame() override;
    virtual bool    Render() override;
    virtual bool    Release() override;

};

