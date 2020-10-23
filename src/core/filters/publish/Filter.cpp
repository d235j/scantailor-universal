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

#include "Filter.h"
#include "FilterUiInterface.h"
#include "OptionsWidget.h"
#include "Settings.h"
#include "Task.h"
#include "CacheDrivenTask.h"
#include "PageId.h"
#include "ImageId.h"
#include "ProjectReader.h"
#include "ProjectWriter.h"
#include "XmlMarshaller.h"
#include "XmlUnmarshaller.h"
#ifndef Q_MOC_RUN
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#endif
#include <QString>
#include <QObject>
#include <QCoreApplication>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <iostream>
#include "CommandLine.h"

namespace publish
{

Filter::Filter(IntrusivePtr<ProjectPages> const& pages,
               PageSelectionAccessor const& page_selection_accessor)
    :   m_ptrPages(pages), m_ptrSettings(new Settings), m_ptrDjbzDispatcher(new DjbzDispatcher)
{
    if (CommandLine::get().isGui()) {
        m_ptrOptionsWidget.reset(
            new OptionsWidget(m_ptrSettings, page_selection_accessor)
        );
    }
}

Filter::~Filter()
{
}

QString
Filter::getName() const
{
    return QCoreApplication::translate(
               "publishing::Filter", "Make a book"
           );
}

PageView
Filter::getView() const
{
    return IMAGE_VIEW;
}

void
Filter::performRelinking(AbstractRelinker const& relinker)
{
    m_ptrSettings->performRelinking(relinker);
}

void
Filter::preUpdateUI(FilterUiInterface* ui, PageId const& page_id)
{
    QString filename;
    if (m_ptrOptionsWidget.get()) {
        m_ptrOptionsWidget->preUpdateUI(filename);
        ui->setOptionsWidget(m_ptrOptionsWidget.get(), ui->KEEP_OWNERSHIP);
    }
}

QDomElement
Filter::saveSettings(
    ProjectWriter const& writer, QDomDocument& doc) const
{
    QDomElement filter_el(doc.createElement("publishing"));
    writer.enumPages(
        boost::lambda::bind(
            &Filter::writePageSettings,
            this, boost::ref(doc), boost::lambda::var(filter_el), boost::lambda::_1, boost::lambda::_2
        )
    );

    filter_el.appendChild(m_ptrDjbzDispatcher->toXml(doc, "djbz_dispatcher"));

    return filter_el;
}

void
Filter::loadSettings(ProjectReader const& reader, QDomElement const& filters_el)
{
    m_ptrSettings->clear();

    QDomElement filter_el(filters_el.namedItem("publishing").toElement());
    m_ptrDjbzDispatcher.reset(new DjbzDispatcher(
                                  filters_el.namedItem("djbz_dispatcher").toElement()));

    QString const page_tag_name("page");
    QDomNode node(filter_el.firstChild());
    for (; !node.isNull(); node = node.nextSibling()) {
        if (!node.isElement()) {
            continue;
        }
        if (node.nodeName() != page_tag_name) {
            continue;
        }
        QDomElement const el(node.toElement());

        bool ok = true;
        int const id = el.attribute("id").toInt(&ok);
        if (!ok) {
            continue;
        }

        PageId const page_id(reader.pageId(id));
        if (page_id.isNull()) {
            continue;
        }

        QDomElement const params_el(el.namedItem("params").toElement());
        if (params_el.isNull()) {
            continue;
        }

        Params const params(params_el);
        m_ptrSettings->setPageParams(page_id, params);
    }
}

void
Filter::invalidateSetting(PageId const& page_id)
{
    std::unique_ptr<Params> const params(m_ptrSettings->getPageParams(page_id));
    if (params.get()) {
        Params p(*params.get());
        p.setForceReprocess(Params::RegenerateAll);
        m_ptrSettings->setPageParams(page_id, p);
    }
}

IntrusivePtr<Task>
Filter::createTask(
    PageId const& page_id,
    bool const batch_processing)
{
    QString filename;
    return IntrusivePtr<Task>(
               new Task(
                   filename, page_id, IntrusivePtr<Filter>(this),
                   m_ptrSettings, *m_ptrDjbzDispatcher, batch_processing
               )
           );
}

IntrusivePtr<CacheDrivenTask>
Filter::createCacheDrivenTask()
{
    return IntrusivePtr<CacheDrivenTask>(
               new CacheDrivenTask(m_ptrSettings)
           );
}

void
Filter::writePageSettings(
    QDomDocument& doc, QDomElement& filter_el,
    PageId const& page_id, int const numeric_id) const
{
    std::unique_ptr<Params> const params(m_ptrSettings->getPageParams(page_id));
    if (!params.get()) {
        return;
    }

    QDomElement page_el(doc.createElement("page"));
    page_el.setAttribute("id", numeric_id);
    page_el.appendChild(params->toXml(doc, "params"));

    filter_el.appendChild(page_el);
}

} // namespace publish
