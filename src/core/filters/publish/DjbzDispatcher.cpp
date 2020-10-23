/*
    Scan Tailor - Interactive post-processing tool for scanned pages.
    Copyright (C) 2020 Alexander Trufanov <trufanovan@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "DjbzDispatcher.h"
#include "settings/globalstaticsettings.h"

#include <QDomDocument>

namespace publish {

DjbzParams::DjbzParams():
    m_match(GlobalStaticSettings::m_key_djvu_djbz_match),
    m_usePrototypes(GlobalStaticSettings::m_key_djvu_djbz_use_prototypes),
    m_useAveraging(GlobalStaticSettings::m_key_djvu_djbz_use_averaging),
    m_aggression(GlobalStaticSettings::m_key_djvu_djbz_aggression),
    m_classifier((ClassifierType) GlobalStaticSettings::m_key_djvu_djbz_classifier)
{
}

DjbzParams::ClassifierType
ClassifierTypeFromString(QString const& str)
{
    if (str == "original") return DjbzParams::ClassifierType::Original;
    if (str == "full") return DjbzParams::ClassifierType::FullScan;
    return DjbzParams::ClassifierType::MaximalScan;
}

QString
ClassifierTypeToString(DjbzParams::ClassifierType val)
{
    switch (val) {
    case DjbzParams::ClassifierType::Original: return "original";
    case DjbzParams::ClassifierType::FullScan: return "full";
    case DjbzParams::ClassifierType::MaximalScan: return "maximal";
    default: return "maximal";
    }
}

DjbzParams::DjbzParams(QDomElement const& el)
{
    m_match = el.attribute("match").toInt();
    m_usePrototypes = el.attribute("prototypes").toInt();
    m_useAveraging = el.attribute("averaging").toInt();
    m_aggression = el.attribute("aggression").toInt();
    m_classifier = ClassifierTypeFromString(el.attribute("type").toLocal8Bit());
}

QDomElement
DjbzParams::toXml(QDomDocument& doc, QString const& name) const
{
    QDomElement el(doc.createElement(name));
    el.setAttribute("match", m_match);
    el.setAttribute("prototypes", m_usePrototypes);
    el.setAttribute("averaging", m_useAveraging);
    el.setAttribute("aggression", m_aggression);
    el.setAttribute("type", ClassifierTypeToString(m_classifier));
    return el;
}

bool
DjbzParams::operator !=(const DjbzParams &b) const
{
    return (m_match != b.m_match ||
            m_useAveraging != b.m_useAveraging ||
            m_usePrototypes != b.m_usePrototypes ||
            m_aggression != b.m_aggression ||
            m_classifier != b.m_classifier);
}


/// DjbzDispatcher





QString DjbzDispatcher::dummyDjbzId = "<none>";

DjbzDispatcher::DjbzDispatcher():
    m_default_pages_per_dict(GlobalStaticSettings::m_djvu_pages_per_djbz)
{
    m_dictionaries[DjbzDispatcher::dummyDjbzId].type = DjbzDict::Type::None;
}

QString
DjbzDispatcher::findDjbzForPage(PageId const& page_id) const
{
    if (m_page_to_dict.contains(page_id)) {
        return m_page_to_dict[page_id];
    }
    return QString();
}

DjbzDict
DjbzDispatcher::djbzDictForPage(QString const& dict_id) const
{
    if (m_dictionaries.contains(dict_id)) {
        return m_dictionaries[dict_id];
    }
    return DjbzDict();
}

QStringList
DjbzDispatcher::listAllDjbz() const
{
    return m_dictionaries.keys();
}

QMap<QString, int>
DjbzDispatcher::listAllDjbzAndTheirSize() const
{
    QMap<QString, int> res;
    const QStringList sl = listAllDjbz();
    for(const QString& s: sl) {
        res[s] = m_dictionaries[s].pages().count();
    }
    return res;
}


QString
DjbzDispatcher::nextDjbzId() const
{
    QString djbz = QString::number(m_dictionaries.count());
    while (djbz.length() < 4) {
        djbz.prepend("0");
    }
    return djbz + ".djbz";
}

QString
DjbzDispatcher::addNewPage(PageId const& page_id)
{
    if (m_default_pages_per_dict < 2) {
        m_dictionaries[DjbzDispatcher::dummyDjbzId].addPage(page_id);
        m_page_to_dict[page_id] = DjbzDispatcher::dummyDjbzId;
        return DjbzDispatcher::dummyDjbzId;
    }

    QString djbz = findDjbzForPage(page_id);
    if (djbz.isEmpty()) {
        // Find some dict that still isn't full enough
        for (QString const& dict: qAsConst(m_dictionaries).keys()) {
            DjbzDict const& vec = m_dictionaries[dict];
            if (vec.type == DjbzDict::Type::AutoFill
                    && vec.pages().size() < m_default_pages_per_dict) {
                djbz = dict;
                break;
            }
        }

        if (djbz.isEmpty()) {
            // page goes to a new djbz
            djbz = nextDjbzId();
        }

        m_dictionaries[djbz].addPage(page_id);
        m_page_to_dict[page_id] = djbz;
    }
    return djbz;
}

void
DjbzDispatcher::deleteFromDjbz(PageId const& page_id)
{
    if (m_page_to_dict.contains(page_id)) {
        m_dictionaries[m_page_to_dict[page_id]].removePage(page_id);
        m_page_to_dict.remove(page_id);
    }
}

void
DjbzDispatcher::setToDjbz(PageId const& page_id, QString const& new_djbz)
{
    if (m_page_to_dict[page_id] != new_djbz) {
        m_dictionaries[new_djbz].addPage(page_id);
        m_page_to_dict[page_id] = new_djbz;
    }
}

void
DjbzDispatcher::moveToDjbz(PageId const& page_id, QString const& new_djbz)
{
    deleteFromDjbz(page_id);
    setToDjbz(page_id, new_djbz);
}

void
DjbzDispatcher::resetAllDicts(QVector<PageId> const& pages)
{
    m_dictionaries.clear();
    m_page_to_dict.clear();

    m_dictionaries[DjbzDispatcher::dummyDjbzId].type = DjbzDict::Type::None;

    if (m_default_pages_per_dict < 2) {
        return;
    }

    int cur_djbz = 0;
    QString djbz = nextDjbzId();
    for (PageId const& page_id: pages) {
        if (++cur_djbz > m_default_pages_per_dict) {
            djbz = nextDjbzId();
            cur_djbz = 1;
        }

        m_dictionaries[djbz].addPage(page_id);
        m_page_to_dict[page_id] = djbz;
    }
}

QString type2str(DjbzDict::Type t) {
    if (t == DjbzDict::Type::Locked) return "locked";
    else if (t == DjbzDict::Type::None) return "no_dict";

    return "auto";
}

DjbzDict::Type str2type( QString const& s) {
    if (s == "locked") return DjbzDict::Type::Locked;
    else if (s == "no_dict") return DjbzDict::Type::None;

    return DjbzDict::Type::AutoFill;
}

QDomElement
DjbzDispatcher::toXml(QDomDocument& doc, QString const& name) const
{
    QDomElement root_el(doc.createElement(name));
    root_el.attribute("pages_per_dict", QString::number(m_default_pages_per_dict));
    const QStringList keys = m_dictionaries.keys();
    for (QString const& id: keys) {
        QDomElement el(doc.createElement(id));
        DjbzDict const& dict = m_dictionaries[id];
        el.setAttribute("type", type2str(dict.type));
        el.setAttribute("rev", QString::number(dict.revision));
        el.appendChild(dict.params().toXml(doc, "djbz_params"));
        root_el.appendChild(el);
    }

    return root_el;
}

DjbzDispatcher::DjbzDispatcher(QDomElement const& el)
{
    m_default_pages_per_dict = el.attribute("pages_per_dict",
                                            QString::number(GlobalStaticSettings::m_djvu_pages_per_djbz)).toInt();
    const QDomNodeList list = el.childNodes();
    for (int i = 0; i < list.size(); i++) {
        const QDomNode d = list.item(i);

        DjbzDict & dict = m_dictionaries[d.nodeName()];
        dict.type = str2type(d.toElement().attribute("type", "auto"));
        DjbzParams params(d.namedItem("djbz_params").toElement());
        dict.setParams(params);
        dict.revision = d.toElement().attribute("rev", "0").toInt();
    }

    // should be read, but just to make sure that project isn't damaged or created with old ST version
    m_dictionaries[DjbzDispatcher::dummyDjbzId].type = DjbzDict::Type::None;
}

}
