#ifndef INPUTVALIDATORTABLEITEM_H
#define INPUTVALIDATORTABLEITEM_H

#include <QObject>
#include <QItemDelegate>

class InputValidatorTableItem : public QItemDelegate
{
    Q_OBJECT
    QValidator *m_validator;

public:
    InputValidatorTableItem(QValidator *validator = 0, QObject *parent = 0);

protected:


    // QAbstractItemDelegate interface
public:
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

#endif // INPUTVALIDATORTABLEITEM_H
