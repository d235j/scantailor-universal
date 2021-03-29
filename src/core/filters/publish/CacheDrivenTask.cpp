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

#include "CacheDrivenTask.h"
#include "DjbzDispatcher.h"
#include "Settings.h"
#include "PageInfo.h"
#include "PageId.h"
#include "ImageId.h"
#include "IncompleteThumbnail.h"
#include "ImageTransformation.h"
#include "ThumbnailBase.h"
#include "AbstractFilterDataCollector.h"
#include "ThumbnailCollector.h"
#include "OutputParams.h"

namespace publish
{

CacheDrivenTask::CacheDrivenTask(IntrusivePtr<Settings> const& settings, OutputFileNameGenerator const& out_file_name_gen)
    : m_ptrSettings(settings), m_refDjbzDispatcher(m_ptrSettings->djbzDispatcher())
{
}

CacheDrivenTask::~CacheDrivenTask()
{
}

bool
CacheDrivenTask::needPageReprocess(const PageId &page_id)
{
    std::unique_ptr<Params> p = m_ptrSettings->getPageParams(page_id);
    if (!p.get()) {
     p.reset(new Params());
    }

    Params::Regenerate val = p->getForceReprocess();
    if (val & Params::RegenerateThumbnail) {
        val = (Params::Regenerate) (val & ~Params::RegenerateThumbnail);
        p->setForceReprocess(val);
        m_ptrSettings->setPageParams(page_id, *p);
        return true;
    }

    const SourceImagesInfo si = p->sourceImagesInfo();
    if (!QFile::exists(si.output_filename()) ||
            !QFile::exists(p->djvuFilename()) ) {
        return true;
    }

    return false;
}

bool
CacheDrivenTask::needDjbzReprocess(const QString& djbz_id)
{
    const DjbzDict dict = m_refDjbzDispatcher.djbzDict(djbz_id);
    for (const PageId& p: dict.pages()) {
        if (p != m_pageId) { // already checked in Task::process
            if (needPageReprocess(p)) {
                return true;
            }
        }
    }
    return false;
}

void
CacheDrivenTask::process(
        PageInfo const& page_info, AbstractFilterDataCollector* collector,
        ImageTransformation const& xform)
{
    m_pageId = page_info.id();

    bool need_reprocess = needPageReprocess(m_pageId);
    if (!need_reprocess) {
        const QString djbz_id = m_refDjbzDispatcher.findDjbzForPage(m_pageId);
        assert(!djbz_id.isEmpty()); // must be set in needPageReprocess
        if (!m_refDjbzDispatcher.isDummyDjbzId(djbz_id)) {
            need_reprocess = needDjbzReprocess(djbz_id);
        }
    }


    if (ThumbnailCollector* thumb_col = dynamic_cast<ThumbnailCollector*>(collector)) {
        QString thumbnail_source;

        std::unique_ptr<Params> params = m_ptrSettings->getPageParams(m_pageId);
        if (!params.get()) {
            params.reset(new Params());
            m_ptrSettings->setPageParams(m_pageId, *params);
            need_reprocess = true;
        }

        if (!need_reprocess) {
            if (params->hasOutputParams()) {
                const Params& used_params = params->outputParams().params();
                need_reprocess = *params != used_params;
            } else {
                need_reprocess = true;
            }
        }

        if (!need_reprocess) {
            const QFileInfo fi(params->djvuFilename());
            if (!fi.exists() || params->djvuSize() != fi.size()
                    || params->djvuLastChanged() != fi.lastModified()) {
                need_reprocess = true;
            }
        }

        if (need_reprocess) {

            thumbnail_source = params->sourceImagesInfo().output_filename();
            if (thumbnail_source.isEmpty() || !QFileInfo::exists(thumbnail_source)) {
                thumbnail_source = page_info.id().imageId().filePath();
            }

            thumb_col->processThumbnail(
                        std::unique_ptr<QGraphicsItem>(
                            new IncompleteThumbnail(
                                thumb_col->thumbnailCache(),
                                thumb_col->maxLogicalThumbSize(),
                                ImageId(thumbnail_source), xform
                                )
                            )
                        );

        } else {

            thumb_col->processThumbnail(
                        std::unique_ptr<QGraphicsItem>(
                            new ThumbnailBase(
                                thumb_col->thumbnailCache(),
                                thumb_col->maxLogicalThumbSize(),
                                ImageId(params->djvuFilename()), xform
                                )
                            )
                        );
        }
    }
}

} // namespace publishing
