#include "configuration.h"

Configuration::Configuration() {

}

QString Configuration::language()
{
    return this->m_language;
}

void Configuration::setLanguage(QString language)
{
    this->m_language = language;
}
