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

#include "Settings.h"
#include "Utils.h"
#include "RelinkablePath.h"
#include "AbstractRelinker.h"
#include "ExportSuggestions.h"

namespace publish
{

Settings::Settings(): QObject(nullptr), m_bundledDocFilesize(0)
{
}

Settings::~Settings()
{
}

void
Settings::clear()
{
    QMutexLocker locker(&m_mutex);
    m_perPageParams.clear();
}

void
Settings::performRelinking(AbstractRelinker const& relinker)
{
    QMutexLocker locker(&m_mutex);
    PerPageParams new_params;

    for (PerPageParams::value_type const& kv : m_perPageParams) {
        RelinkablePath const old_path(kv.first.imageId().filePath(), RelinkablePath::File);
        PageId new_page_id(kv.first);
        new_page_id.imageId().setFilePath(relinker.substitutionPathFor(old_path));
        new_params.insert(PerPageParams::value_type(new_page_id, kv.second));
    }

    m_perPageParams.swap(new_params);
}

void
Settings::setPageParams(PageId const& page_id, Params const& params)
{
    QMutexLocker locker(&m_mutex);
    Utils::mapSetValue(m_perPageParams, page_id, params);
}

void
Settings::clearPageParams(PageId const& page_id)
{
    QMutexLocker locker(&m_mutex);
    m_perPageParams.erase(page_id);
}

std::unique_ptr<Params>
Settings::getPageParams(PageId const& page_id) const
{
    QMutexLocker locker(&m_mutex);

    PerPageParams::const_iterator it(m_perPageParams.find(page_id));
    if (it != m_perPageParams.end()) {
        return std::unique_ptr<Params>(new Params(it->second));
    } else {
        return std::unique_ptr<Params>();
    }
}

void
Settings::generateEncoderSettings(const QVector<PageId>& pages, const ExportSuggestions& export_suggestions, QStringList& settings)
{
    settings.clear();
    settings << "(options\n  indirect 1\n) # options";
    settings << "(input-files";
    for (const PageId& p: pages) {
        const Params& params = m_perPageParams[p];
        const SourceImagesInfo si = params.sourceImagesInfo();
        const QString fname = si.export_foregroundFilename().isEmpty() ?
                    si.output_filename() :
                    si.export_foregroundFilename();
        settings << "  (file" <<
                    "    " + fname <<
                    "    (image";
        const ExportSuggestion& es = export_suggestions[p];
        if (!es.hasBWLayer && !es.hasColorLayer) {
            settings << QString("       virtual %1 %2\n"
                                "       dpi     %3\n").arg(es.width, es.height, es.dpi);
        } else {
            settings << QString("       dpi     %1\n"
                                "       smooth  %2\n"
                                "       clean   %3\n"
                                "       erosion %4").arg(
                            QString::number(params.outputDpi().horizontal()),
                            params.smooth() ? "1" : "0",
                            params.clean() ? "1" : "0",
                            params.erosion() ? "1" : "0");
        }
        settings << "    ) #image"
                 << "  ) #file";
    }
    settings << ") #input-files";
}

const DjbzDispatcher &Settings::djbzDispatcherConst() const
{
    return m_ptrDjbzDispatcher;
}

DjbzDispatcher &Settings::djbzDispatcher()
{
    return m_ptrDjbzDispatcher;
}

} // namespace publish
