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

#ifndef DJBZDISPATCHER_H
#define DJBZDISPATCHER_H

#include "PageId.h"

#include <QMap>
#include <QSet>

class QDomDocument;
class QDomElement;

namespace publish {

class DjbzParams
{
public:
    enum ClassifierType {
        Original,
        FullScan,
        MaximalScan
    };

    DjbzParams();
    DjbzParams(QDomElement const& el);

    bool match() const { return m_match; }
    void setMatch(bool match) { m_match = match; }

    bool prototypes() const { return m_usePrototypes; }
    void setPrototypes(bool prototypes) { m_usePrototypes = prototypes; }

    bool averaging() const { return m_useAveraging; }
    void setAveraging(bool averaging) { m_useAveraging = averaging; }

    int agression() const { return m_aggression; }
    void setAgression(int agression) { m_aggression = agression; }

    int classifierType() const { return m_classifier; }
    void setClassifierType(ClassifierType classifierType) { m_classifier = classifierType; }

    QDomElement toXml(QDomDocument& doc, QString const& name) const;

    bool operator !=(const DjbzParams &b) const;

    ~DjbzParams(){}
private:
    bool m_match;
    bool m_usePrototypes;
    bool m_useAveraging;
    int  m_aggression;
    ClassifierType m_classifier;
};


class DjbzDict
{
public:
    enum Type {
        AutoFill, // page can automatically added to this djbz
        Locked,   // only user can add pages to this djbz
        None      // that's a virtual djbz. Page won't be added to any djbz
    };
    DjbzDict(): type(AutoFill), revision(0) {}

    Type type;
    int revision; // shall be increased at any change

    void addPage(const PageId& page) {
        m_pages += page;
        revision++;
    }
    void removePage(const PageId& page) {
        if (m_pages.remove(page)) {
            revision++;
        }
    }
    const QSet<PageId>& pages() const { return m_pages; };

    void setParams(const DjbzParams& params) {
        if (m_params != params) {
            m_params = params;
            revision++;
        }
    }
    const DjbzParams& params() const { return m_params; }

private:
    QSet<PageId> m_pages;
    DjbzParams m_params;
};

typedef QMap<QString, DjbzDict > DjbzContent;
typedef QMap<PageId, QString> PageToDjbz;

class DjbzDispatcher
{
public:
    DjbzDispatcher();
    DjbzDispatcher(QDomElement const& el);

    int defaultPagesPerDict() const { return m_default_pages_per_dict; }
    void setDefaultPagesPerDict(int val) { m_default_pages_per_dict = val; }

    QString findDjbzForPage(PageId const& page_id) const;
    DjbzDict djbzDictForPage(const QString &dict_id) const;
    QStringList listAllDjbz() const;
    QMap<QString, int> listAllDjbzAndTheirSize() const;
    QString addNewPage(PageId const& page_id);
    void deleteFromDjbz(PageId const& page_id);
    void setToDjbz(PageId const& page_id, QString const& new_djbz);
    void moveToDjbz(PageId const& page_id, QString const& new_djbz);
    void resetAllDicts(QVector<PageId> const&);

    QDomElement toXml(QDomDocument& doc, QString const& name) const;
public:
    static QString dummyDjbzId;
private:
    QString nextDjbzId() const;
private:
    int m_default_pages_per_dict;
    DjbzContent m_dictionaries;
    PageToDjbz m_page_to_dict;
    mutable QAtomicInt m_refCounter;
};

}

#endif // DJBZDISPATCHER_H
