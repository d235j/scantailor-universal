/*
    Scan Tailor Universal - Interactive post-processing tool for scanned
    pages. A fork of Scan Tailor by Joseph Artsimovich.
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


#ifndef PUBLISH_OUTPUTPARAMS_H_
#define PUBLISH_OUTPUTPARAMS_H_

#include "Params.h"
#include "DjbzDispatcher.h"

#include <QDomDocument>


class ImageInfo;

namespace publish
{

class OutputParams
{
public:
    // Member-wise copying is OK.

    OutputParams();

    OutputParams(const Params& params, const QString& djbzId,
                 int djbzRevision, const DjbzParams& djbzParams);

    OutputParams(QDomElement const& deps_el);

    ~OutputParams();

    bool matches(OutputParams const& other) const;

    QDomElement toXml(QDomDocument& doc, QString const& name) const;
private:
    Params m_params;
    QString m_djbzId;
    int m_djbzRevision;
    DjbzParams m_djbzParams;
};

} // namespace publish

#endif
