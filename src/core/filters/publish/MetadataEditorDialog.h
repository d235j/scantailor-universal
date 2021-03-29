#ifndef METADATAEDITORDIALOG_H
#define METADATAEDITORDIALOG_H

#include <QDialog>
#include <QStyledItemDelegate>

#include "ui_MetadataEditorDialog.h"

namespace publish {

struct DocMetaField {
    const QString name;
    const QString code;
    bool is_default;
    bool is_displayed;
    const QString default_value;
};

class QComboBoxDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    QComboBoxDelegate(QVector<DocMetaField>& knownValues, QTableWidget *parent = nullptr);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override;

    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const override;

    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                              const QModelIndex &index) const override;
private:
    QTableWidget* m_parent;
    QVector<DocMetaField>& m_knownValues;
};


class MetadataEditorDialog : public QDialog, private Ui::MetadataEditorDialog
{
    Q_OBJECT

public:
    explicit MetadataEditorDialog(const QMap<QString, QString>& metadata, QWidget *parent = nullptr);
    ~MetadataEditorDialog();
    QMap<QString, QString> getMetadata();
private slots:
    void on_btnAdd_clicked();
    void on_btnReset_clicked();
    void on_btnRemove_clicked();

    void on_btnNewField_clicked();

private:
    void initDocMeta();
private:
    QComboBoxDelegate* m_delegate;
    QVector<DocMetaField> m_knownDocMeta;
};


} // namespace publish
#endif // METADATAEDITORDIALOG_H
