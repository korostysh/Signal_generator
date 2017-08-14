#include "inputvalidatortableitem.h"
#include <QLineEdit>

InputValidatorTableItem::InputValidatorTableItem(QValidator *validator, QObject *parent) :
    QItemDelegate(parent), m_validator(validator)
{

}

QWidget *InputValidatorTableItem::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QLineEdit *editor = new QLineEdit(parent);
    editor->setValidator(m_validator);
    return editor;
}

void InputValidatorTableItem::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QString value = index.model()->data(index, Qt::EditRole).toString();
    QLineEdit *line = static_cast<QLineEdit*>(editor);
    line->setText(value);
}

void InputValidatorTableItem::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QLineEdit *line = static_cast<QLineEdit*>(editor);
    QString value = line->text();
    model->setData(index, value);
}

void InputValidatorTableItem::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    editor->setGeometry(option.rect);
}
