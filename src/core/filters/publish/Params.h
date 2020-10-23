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

#ifndef PUBLISH_PARAMS_H_
#define PUBLISH_PARAMS_H_

#include "Dpi.h"
#include "RegenParams.h"
#include "PageId.h"
#include "ImageInfo.h"
#include <QVariantMap>
#include <QFileInfo>
#include <QDir>
#include <QImage>
#include <settings/globalstaticsettings.h>

class QDomDocument;
class QDomElement;

namespace publish
{

class OutputParams;

class Params: public RegenParams
{
public:
    Params();
    Params(QDomElement const& el);
    bool isNull() const { return m_inputImageInfo.fileName.isEmpty() ||
                m_inputImageInfo.imageColorMode == ImageInfo::ColorMode::Unknown; }

    Dpi const& outputDpi() const { return m_dpi; }
    void setOutputDpi(Dpi const& dpi) { m_dpi = dpi; }

    QString const imageFilename() const { return m_inputImageInfo.fileName; }
    void setImageFilename(QString const& val) { m_inputImageInfo.fileName = val; }

    QString const djvuFilename() const {
        QFileInfo info(m_inputImageInfo.fileName);
        return info.path() + QDir::separator() + GlobalStaticSettings::m_djvu_pages_subfolder + QDir::separator() + info.completeBaseName() + ".djv";
    }

    void setInputImageHash(const QByteArray & val) { m_inputImageInfo.imageHash = val; }
    const QByteArray & inputImageHash() const { return m_inputImageInfo.imageHash; }

    void setInputImageColorMode(ImageInfo::ColorMode val) { m_inputImageInfo.imageColorMode = val; }
    ImageInfo::ColorMode inputImageColorMode() const { return m_inputImageInfo.imageColorMode; }

    void setImageInfo(const ImageInfo& info) { m_inputImageInfo = info; }
    ImageInfo imageInfo() const { return m_inputImageInfo; }

    void setDjVuSize(int size) { m_djvuSize = size; }
    int djvuSize() const { return m_djvuSize; }

    void setDjbzId(QString const& DjbzId) { m_djbzId = DjbzId; }
    QString djbzId() const { return m_djbzId; }

    void setDjbzRevision(int val) { m_djbzRevision = val; }
    int djbzRevision() const { return m_djbzRevision; }

    void setClean(bool clean) { m_clean = clean; }
    bool clean() const { return m_clean; }

    void setErosion(bool erosion) { m_erosion = erosion; }
    bool erosion() const { return m_erosion; }

    void setSmooth(bool smooth) { m_smooth = smooth; }
    bool smooth() const { return m_smooth; }

    bool hasOutputParams() const { return m_ptrOutputParams != nullptr; }
    const OutputParams& outputParams() const { return *m_ptrOutputParams; }

    QDomElement toXml(QDomDocument& doc, QString const& name) const;

    ~Params(){}

    bool operator !=(const Params &other) const;

    void rememberOutputParams(const DjbzParams &djbz_params);

private:
    Dpi m_dpi;
    ImageInfo m_inputImageInfo;
    int m_djvuSize;
    QString m_djbzId;
    int m_djbzRevision;
    bool m_clean;
    bool m_erosion;
    bool m_smooth;
    std::unique_ptr<OutputParams> m_ptrOutputParams;
};

} // namespace publish

#endif
