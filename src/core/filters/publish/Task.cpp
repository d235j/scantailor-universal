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

#include "Task.h"
#include "Filter.h"
#include "OptionsWidget.h"
#include "Settings.h"
#include "FilterData.h"
#include "ImageTransformation.h"
#include "TaskStatus.h"
#include "ImageView.h"
#include "FilterUiInterface.h"
#include "OutputParams.h"
#include "DjbzDispatcher.h"

#include <iostream>

namespace publish
{

class Task::UiUpdater : public FilterResult
{
public:
    UiUpdater(IntrusivePtr<Filter> const& filter, QString const& filename,
              bool batch_processing);

    virtual void updateUI(FilterUiInterface* wnd);

    virtual IntrusivePtr<AbstractFilter> filter()
    {
        return m_ptrFilter;
    }
private:
    QString const& m_filename;
    IntrusivePtr<Filter> m_ptrFilter;
    bool m_batchProcessing;
};

Task::Task(
        QString const& filename,
        PageId const& page_id,
        IntrusivePtr<Filter> const& filter,
        IntrusivePtr<Settings> const& settings,
        DjbzDispatcher& djbzDispatcher,
        bool const batch_processing)
    :
      m_filename(filename),
      m_pageId(page_id),
      m_ptrFilter(filter),
      m_ptrSettings(settings),
      m_refDjbzDispatcher(djbzDispatcher),
      m_batchProcessing(batch_processing)
{
}

Task::~Task()
{
}

FilterResultPtr
Task::process(TaskStatus const& status, FilterData const& data)
{
    status.throwIfCancelled();

    QString djbz_id = m_refDjbzDispatcher.findDjbzForPage(m_pageId);
    if (djbz_id.isEmpty()) {
        djbz_id = m_refDjbzDispatcher.addNewPage(m_pageId);
    }

    const DjbzDict dict = m_refDjbzDispatcher.djbzDictForPage(djbz_id);

    std::unique_ptr<Params> params(m_ptrSettings->getPageParams(m_pageId));
    bool need_reprocess(!params.get());
    if (!need_reprocess) {
        Params::Regenerate val = params->getForceReprocess();
        need_reprocess = val & Params::RegeneratePage;
        if (need_reprocess) {
            val = (Params::Regenerate)(val & ~Params::RegeneratePage);
            params->setForceReprocess(val);
        }
    }

    if (!need_reprocess) {
        if (params->hasOutputParams()) {
            OutputParams const output_params_to_use(*params, djbz_id, dict.revision, dict.params());
            OutputParams const& output_params_was_used(params->outputParams());
            need_reprocess = !output_params_was_used.matches(output_params_to_use);
        } else {
            need_reprocess = true;
        }
    }

    if (need_reprocess) {
        params->setDjbzId(djbz_id);
        params->setDjbzRevision(dict.revision);
        params->rememberOutputParams(dict.params());
    }



}

/*============================ Task::UiUpdater ========================*/

Task::UiUpdater::UiUpdater(
        IntrusivePtr<Filter> const& filter,
        QString const& filename,
        bool const batch_processing)
    :   m_ptrFilter(filter),
      m_filename(filename),
      m_batchProcessing(batch_processing)
{
}

void
Task::UiUpdater::updateUI(FilterUiInterface* ui)
{
    // This function is executed from the GUI thread.
    OptionsWidget* const opt_widget = m_ptrFilter->optionsWidget();
    if (!m_batchProcessing) {
        opt_widget->postUpdateUI();
    }
    ui->setOptionsWidget(opt_widget, ui->KEEP_OWNERSHIP);

    //    ui->invalidateThumbnail(m_filename);

    if (m_batchProcessing) {
        return;
    }

    QImage image;
    QImage downscaled_image;
    QRect r; Dpi d;
    ImageTransformation xform(r, d);

    //ImageView* view = new ImageView(image, downscaled_image, xform);
    //ui->setImageWidget(view, ui->TRANSFER_OWNERSHIP);
}

} // namespace publish
