/**
 * @author      : Sam Bell (sam@silverband.com.au)
 * @file        : TemplateString
 * @created     : Monday Feb 24, 2020 12:55:01 AEDT
 */

#ifndef TEMPLATESTRING_H
#define TEMPLATESTRING_H
#include <string>

#include "TemplateObject.h"

class TemplateString : public TemplateObject
{
    public:
        TemplateString (std::string newValue) {
            m_value = newValue;
	}
	std::string toString() { return m_value; }
	std::string type() { return "TemplateString"; }
    private:
        /* private data */
	std::string m_value;
};

#endif /* end of include guard TEMPLATESTRING_H */

