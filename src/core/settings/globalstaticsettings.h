/*
    Scan Tailor - Interactive post-processing tool for scanned pages.
    Copyright (C) Joseph Artsimovich <joseph_a@mail.ru>

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

#ifndef GLOBALSTATICSETTINGS_H
#define GLOBALSTATICSETTINGS_H
#include "settings/hotkeysmanager.h"
#include "settings/ini_keys.h"
#include <memory>
#include <QColor>
#include <QResource>
#include <QRegularExpression>

class GlobalStaticSettings
{
public:
    inline static bool doDrawDeviants()
    {
        return m_drawDeviants;
    }

    inline static void setDrawDeskewDeviants(bool val)
    {
        if (m_drawDeskewDeviants != val) {
            m_drawDeskewDeviants = val;
            updateParams();
        }
    }

    inline static void setDrawContentDeviants(bool val)
    {
        if (m_drawContentDeviants != val) {
            m_drawContentDeviants = val;
            updateParams();
        }
    }

    inline static void setDrawMarginDeviants(bool val)
    {
        if (m_drawMarginDeviants != val) {
            m_drawMarginDeviants = val;
            updateParams();
        }
    }

    inline static void stageChanged(int i)
    {
        if (m_currentStage != i) {
            m_currentStage = i;
            updateParams();
        }
    }

    static void updateSettings()
    {
        QSettings settings;
        setDrawDeskewDeviants(settings.value(_key_deskew_deviant_enabled, _key_deskew_deviant_enabled_def).toBool());
        setDrawContentDeviants(settings.value(_key_select_content_deviant_enabled, _key_select_content_deviant_enabled_def).toBool());
        setDrawMarginDeviants(settings.value(_key_margins_deviant_enabled, _key_margins_deviant_enabled_def).toBool());

        m_tiff_compr_method = settings.value(_key_tiff_compr_method, _key_tiff_compr_method_def).toString();
        updateTiffCompression(m_tiff_compr_method);
        m_binrization_threshold_control_default = settings.value(_key_output_bin_threshold_default, _key_output_bin_threshold_default_def).toInt();
        m_use_horizontal_predictor = settings.value(_key_tiff_compr_horiz_pred, _key_tiff_compr_horiz_pred_def).toBool();
        m_disable_bw_smoothing = settings.value(_key_mode_bw_disable_smoothing, _key_mode_bw_disable_smoothing_def).toBool();
        m_zone_editor_min_angle = settings.value(_key_zone_editor_min_angle, _key_zone_editor_min_angle_def).toReal();
        m_picture_detection_sensitivity = settings.value(_key_picture_zones_layer_sensitivity, _key_picture_zones_layer_sensitivity_def).toInt();
        m_deskew_controls_color.setNamedColor(settings.value(_key_deskew_controls_color, _key_deskew_controls_color_def).toString());
        m_deskew_controls_color_pen = m_deskew_controls_color;
        m_deskew_controls_color_pen.setAlpha(255); // ignore transparency
        m_content_sel_content_color.setNamedColor(settings.value(_key_content_sel_content_color, _key_content_sel_content_color_def).toString());
        m_content_sel_content_color_pen = m_content_sel_content_color;
        m_content_sel_content_color_pen.setAlpha(255); // ignore transparency
        m_deskew_controls_color_thumb = m_deskew_controls_color;
        if (m_deskew_controls_color_thumb.alpha() > 70) {
            m_deskew_controls_color_thumb.setAlpha(70);
        }

        m_highlightColorAdjustment = 100 + settings.value(_key_thumbnails_non_focused_selection_highlight_color_adj, _key_thumbnails_non_focused_selection_highlight_color_adj_def).toInt();

        m_thumbsListOrderAllowed = settings.value(_key_thumbnails_multiple_items_in_row, _key_thumbnails_multiple_items_in_row_def).toBool();
        m_thumbsMinSpacing = settings.value(_key_thumbnails_min_spacing, _key_thumbnails_min_spacing_def).toInt();
        m_thumbsBoundaryAdjTop = settings.value(_key_thumbnails_boundary_adj_top, _key_thumbnails_boundary_adj_top_def).toInt();
        m_thumbsBoundaryAdjBottom = settings.value(_key_thumbnails_boundary_adj_bottom, _key_thumbnails_boundary_adj_bottom_def).toInt();
        m_thumbsBoundaryAdjLeft = settings.value(_key_thumbnails_boundary_adj_left, _key_thumbnails_boundary_adj_left_def).toInt();
        m_thumbsBoundaryAdjRight = settings.value(_key_thumbnails_boundary_adj_right, _key_thumbnails_boundary_adj_right_def).toInt();
        m_fixedMaxLogicalThumbSize = settings.value(_key_thumbnails_fixed_thumb_size, _key_thumbnails_fixed_thumb_size_def).toBool();
        m_displayOrderHints = settings.value(_key_thumbnails_display_order_hints, _key_thumbnails_display_order_hints_def).toBool();

        m_dewarpAutoVertHalfCorrection = settings.value(_key_dewarp_auto_vert_half_correction, _key_dewarp_auto_vert_half_correction_def).toBool();
        m_dewarpAutoDeskewAfterDewarp = settings.value(_key_dewarp_auto_deskew_after_dewarp, _key_dewarp_auto_deskew_after_dewarp_def).toBool();

        m_simulateSelectionModifierHintEnabled = settings.value(_key_thumbnails_simulate_key_press_hint, _key_thumbnails_simulate_key_press_hint_def).toBool();

        m_DontUseNativeDialog = settings.value(_key_dont_use_native_dialog, _key_dont_use_native_dialog_def).toBool();

        m_djvu_bin_c44 = settings.value(_key_djvu_bin_c44, _key_djvu_bin_c44_def).toString();
        m_djvu_bin_minidjvu = settings.value(_key_djvu_bin_minidjvu, _key_djvu_bin_minidjvu_def).toString();
        m_djvu_bin_djvuextract = settings.value(_key_djvu_bin_djvuextract, _key_djvu_bin_djvuextract_def).toString();
        m_djvu_bin_djvumake = settings.value(_key_djvu_bin_djvumake, _key_djvu_bin_djvumake_def).toString();
        m_djvu_bin_djvused = settings.value(_key_djvu_bin_djvused, _key_djvu_bin_djvused_def).toString();
        m_djvu_bin_tesseract = settings.value(_key_djvu_bin_tesseract, _key_djvu_bin_tesseract_def).toString();
        m_djvu_bin_djvm = settings.value(_key_djvu_bin_djvm, _key_djvu_bin_djvm_def).toString();
        m_djvu_pages_subfolder = settings.value(_key_djvu_pages_subfolder, _key_djvu_pages_subfolder_def).toString();
        m_djvu_layers_subfolder = settings.value(_key_djvu_layers_subfolder, _key_djvu_layers_subfolder_def).toString();
        m_djvu_pages_per_djbz  = settings.value(_key_djvu_pages_per_djbz, _key_djvu_pages_per_djbz_def).toInt();
        m_djvu_djbz_erosion    = settings.value(_key_djvu_djbz_erosion, _key_djvu_djbz_erosion_def).toBool();
        m_djvu_djbz_use_prototypes = settings.value(_key_djvu_djbz_use_prototypes, _key_djvu_djbz_use_prototypes_def).toBool();
        m_djvu_djbz_use_averaging = settings.value(_key_djvu_djbz_use_averaging, _key_djvu_djbz_use_averaging_def).toBool();
        m_djvu_djbz_aggression = settings.value(_key_djvu_djbz_aggression, _key_djvu_djbz_aggression_def).toInt();
        m_djvu_djbz_classifier = settings.value(_key_djvu_djbz_classifier, _key_djvu_djbz_classifier_def).toInt();
        m_djvu_djbz_extension  = settings.value(_key_djvu_djbz_extension, _key_djvu_djbz_extension_def).toString();

        m_default_bsf = settings.value(_key_scale_bsf, _key_scale_bsf_def).toUInt();
        m_default_scale_filter = (FREE_IMAGE_FILTER) settings.value(_key_scale_filter, _key_scale_filter_def).toUInt();
    }

    static void updateHotkeys()
    {
        m_hotKeyManager.load();
    }

    static QKeySequence createShortcut(const HotKeysId& id, int idx = 0)
    {
        const HotKeySequence& seq = m_hotKeyManager.get(id)->sequences()[idx];
        QString str = QHotKeys::hotkeysToString(seq.m_modifierSequence, seq.m_keySequence);
        return QKeySequence(str);
    }

    static QString getShortcutText(const HotKeysId& id, int idx = 0)
    {
        const HotKeySequence& seq = m_hotKeyManager.get(id)->sequences()[idx];
        return QHotKeys::hotkeysToString(seq.m_modifierSequence, seq.m_keySequence);
    }

    static bool checkKeysMatch(const HotKeysId& id, const Qt::KeyboardModifiers& modifiers, const Qt::Key& key)
    {
        const HotKeyInfo* i = m_hotKeyManager.get(id);
        for (const HotKeySequence& seq : i->sequences()) {
            if (seq.m_modifierSequence == modifiers &&
                    seq.m_keySequence.count() == 1 &&
                    seq.m_keySequence[0] == key) {
                return true;
            }
        }
        return false;
    }

    static bool checkModifiersMatch(const HotKeysId& id, const Qt::KeyboardModifiers& modifiers)
    {
        if (modifiers == Qt::NoModifier) {
            return false;
        }

        const HotKeyInfo* i = m_hotKeyManager.get(id);
        for (const HotKeySequence& seq : i->sequences()) {
            if (seq.m_modifierSequence == modifiers) {
                return true;
            }
        }
        return false;
    }

    // TIFF compression
    static int getCompressionVal(const QString& compression)
    {
        if (m_tiff_list.isEmpty()) {
            const QResource tiff_data(":/TiffCompressionMethods.tsv");
            m_tiff_list = QString::fromUtf8((char const*)tiff_data.data(), tiff_data.size()).split('\n');
        }

        QStringList data = m_tiff_list.filter(QRegularExpression("^" + compression + "\t.*"));

        if (data.empty()) {
            throw std::runtime_error("Settings: unknown compression.");
        }

        Q_ASSERT(data.size() == 1);
        data = data[0].split('\t');
        Q_ASSERT(data.size() >= 3);
        return data[1].toInt();
    }

    static int updateTiffCompression(const QString& name)
    {

        if (m_tiff_compr_method == name) {
            return m_tiff_compression_id;
        }

        m_tiff_compr_method = name;
        m_tiff_compression_id = getCompressionVal(name);
        return m_tiff_compression_id;
    }

    static void setTiffCompression(QString const& compression)
    {
        if (m_tiff_compr_method != compression) {
            m_tiff_compression_id = getCompressionVal(compression);
            m_tiff_compr_method = compression;
        }
        // QSettings might be out of sync
        QSettings().setValue(_key_tiff_compr_method, m_tiff_compr_method);
    }

private:
    inline static void updateParams()
    {
        switch (m_currentStage) {
        case 2:
            m_drawDeviants = m_drawDeskewDeviants;
            break;
        case 3:
            m_drawDeviants = m_drawContentDeviants;
            break;
        case 4:
            m_drawDeviants = m_drawMarginDeviants;
            break;
        default:
            m_drawDeviants = false;
        }
    }

    GlobalStaticSettings() {}

    static bool m_drawDeviants;
    static bool m_drawDeskewDeviants;
    static bool m_drawContentDeviants;
    static bool m_drawMarginDeviants;
    static int m_currentStage;
    static QStringList m_tiff_list;
public:
    static QString m_tiff_compr_method;
    static int m_tiff_compression_id;
    static int m_binrization_threshold_control_default;
    static bool m_use_horizontal_predictor;
    static bool m_disable_bw_smoothing;
    static qreal m_zone_editor_min_angle;
    static float m_picture_detection_sensitivity;
    static QColor m_deskew_controls_color;
    static QColor m_deskew_controls_color_pen;
    static QColor m_deskew_controls_color_thumb;
    static QColor m_content_sel_content_color;
    static QColor m_content_sel_content_color_pen;
    static QHotKeys m_hotKeyManager;
    static int m_highlightColorAdjustment;

    static bool m_thumbsListOrderAllowed;
    static int m_thumbsMinSpacing;
    static int m_thumbsBoundaryAdjTop;
    static int m_thumbsBoundaryAdjBottom;
    static int m_thumbsBoundaryAdjLeft;
    static int m_thumbsBoundaryAdjRight;
    static bool m_fixedMaxLogicalThumbSize;
    static bool m_displayOrderHints;

    static bool m_dewarpAutoVertHalfCorrection;
    static bool m_dewarpAutoDeskewAfterDewarp;
    static bool m_simulateSelectionModifier;
    static bool m_simulateSelectionModifierHintEnabled;
    static bool m_inversePageOrder;

    static bool m_DontUseNativeDialog;

    static QString m_djvu_bin_minidjvu;
    static QString m_djvu_bin_c44;
    static QString m_djvu_bin_djvuextract;
    static QString m_djvu_bin_djvumake;
    static QString m_djvu_bin_djvused;
    static QString m_djvu_bin_tesseract;
    static QString m_djvu_bin_djvm;
    static QString m_djvu_pages_subfolder;
    static QString m_djvu_layers_subfolder;
    static int m_djvu_pages_per_djbz;
    static bool m_djvu_djbz_erosion;
    static bool m_djvu_djbz_use_prototypes;
    static bool m_djvu_djbz_use_averaging;
    static int m_djvu_djbz_aggression;
    static int m_djvu_djbz_classifier;
    static QString m_djvu_djbz_extension;

    static uint m_default_bsf;
    static FREE_IMAGE_FILTER m_default_scale_filter;

};

#endif // GLOBALSTATICSETTINGS_H
