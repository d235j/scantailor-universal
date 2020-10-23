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

#include "Params.h"
#include "XmlMarshaller.h"
#include "XmlUnmarshaller.h"
#include "CommandLine.h"
#include "OutputParams.h"
#include "DjbzDispatcher.h"
#include <QDomDocument>
#include <QDomElement>
#include <QString>
#include <QCryptographicHash>

namespace publish
{

Params::Params()
:  RegenParams(),
  m_dpi(CommandLine::get().getDefaultOutputDpi()),
  m_djvuSize(0), m_djbzRevision(0),
  m_clean(false), m_erosion(false), m_smooth(true)
{    
}

Params::Params(QDomElement const& el)
:	RegenParams(),
    m_dpi(XmlUnmarshaller::dpi(el.namedItem("dpi").toElement()))

{
    m_inputImageInfo.fileName = XmlUnmarshaller::string(el.namedItem("input_filename").toElement());
    m_inputImageInfo.imageHash = QByteArray::fromHex(el.attribute("input_image_hash").toLocal8Bit());
    m_inputImageInfo.imageColorMode = (ImageInfo::ColorMode) el.attribute("input_image_color_mode").toUInt();
    m_djvuSize = el.attribute("djvu_filesize").toInt();
    m_djbzId = el.attribute("djbz_id");
    m_djbzRevision = el.attribute("djbz_rev").toInt();
    m_clean = el.attribute("clean").toInt();
    m_erosion = el.attribute("erosion").toInt();
    m_smooth = el.attribute("smooth").toInt();
    QDomNode item = el.namedItem("processed_with");
    if (!item.isNull() && item.isElement()) {
        m_ptrOutputParams.reset(new OutputParams(item.toElement()));
    }
}

QDomElement
Params::toXml(QDomDocument& doc, QString const& name) const
{
	XmlMarshaller marshaller(doc);
	
	QDomElement el(doc.createElement(name));
	el.appendChild(marshaller.dpi(m_dpi, "dpi"));
    el.appendChild(marshaller.string(m_inputImageInfo.fileName, "input_filename"));
    el.setAttribute("input_image_hash", (QString) m_inputImageInfo.imageHash.toHex());
    el.setAttribute("input_image_color_mode", QString::number((int)m_inputImageInfo.imageColorMode));
    el.setAttribute("djvu_filesize", m_djvuSize);
    el.setAttribute("djbz_id", m_djbzId);
    el.setAttribute("djbz_rev", m_djbzRevision);
    el.setAttribute("clean", m_clean);
    el.setAttribute("erosion", m_erosion);
    el.setAttribute("smooth", m_smooth);
    if (m_ptrOutputParams) {
        el.appendChild(m_ptrOutputParams->toXml(doc, "processed_with"));
    }
	return el;
}

bool
Params::operator !=(const Params &other) const
{
    return (m_djvuSize != other.m_djvuSize ||
            m_djbzId != other.m_djbzId ||
            m_djbzRevision != other.m_djbzRevision ||
            m_clean != other.m_clean ||
            m_erosion != other.m_erosion ||
            m_smooth != other.m_smooth ||
            m_dpi != other.m_dpi ||
            m_inputImageInfo != other.m_inputImageInfo);
}

void
Params::rememberOutputParams(const DjbzParams &djbz_params)
{
    m_ptrOutputParams.reset( new OutputParams(*this, m_djbzId, m_djbzRevision, djbz_params
                                 ));
}

} // namespace publish
