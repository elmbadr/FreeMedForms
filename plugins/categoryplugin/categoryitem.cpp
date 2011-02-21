/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2010 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
 *  All rights reserved.                                                   *
 *                                                                         *
 *  This program is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 3 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
/**
  \class Category::ICategoryContentItem
*/


/**
  \class Category::CategoryItem
  \brief Represent a category with all its data.
*/


#include "categoryitem.h"

#include <coreplugin/icore.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/iuser.h>

#include <QHash>

#include <QDebug>

using namespace Category;
using namespace Internal;

static inline Core::IPatient *patient()  { return Core::ICore::instance()->patient(); }
static inline QString currentUserUuid() {return Core::ICore::instance()->user()->value(Core::IUser::Uuid).toString();}

namespace Category {
namespace Internal {
class CategoryItemPrivate
{
public:
    CategoryItemPrivate(CategoryItem *parent) : m_Parent(0), m_IsDirty(false), q(parent) {}

    ~CategoryItemPrivate()
    {
    }

public:
    CategoryItem *m_Parent;
    QHash<int, QVariant> m_Data;
    QHash<QString, QString> m_Labels; //K=lang, V=label
    QList<CategoryItem *> m_Children;
    QList<ICategoryContentItem *> m_ContentChildren;
    bool m_IsDirty;

private:
    CategoryItem *q;
};
}
}

/** \brief Creates a valid CategoryItem. */
CategoryItem::CategoryItem() :
        d(new CategoryItemPrivate(this))
{
    d->m_Data.insert(DbOnly_IsValid, true);
}

/** \brief Destructor of CategoryItem. Children categories are not deleted. */
CategoryItem::~CategoryItem()
{
    if (d) {
        delete d;
    }
    d = 0;
}

/**
  \brief When building a CategoryItem tree, define the \e parent of the item.
  Warning, the item is not automatically added to the children of the \e parent. \sa addChild().
  Warning, the ParentId is not automatically defined. \sa setData().
*/
void CategoryItem::setParent(CategoryItem *parent)
{
    d->m_Parent = parent;
    d->m_IsDirty = true;
}

/** \brief Return the CategoryItem parent. */
CategoryItem *CategoryItem::parent() const
{
    return d->m_Parent;
}

/** \brief When building a CategoryItem tree, add the item \e child to the list of children. Warning, adding an item to the children list will not redefine the parent of the \e child. \sa setParent()*/
void CategoryItem::addChild(CategoryItem *child)
{
    d->m_Children.append(child);
}

/** \brief Returns the child number \e number. */
CategoryItem *CategoryItem::child(int number) const
{
    if (number < d->m_Children.count()) {
        return d->m_Children.at(number);
    }
    return 0;
}

/** \brief Returns the children categories list of the CategoryItem. */
QList<CategoryItem *> CategoryItem::children() const
{
    return d->m_Children;
}

/** \brief Returns the children categories list count. */
int CategoryItem::childCount() const
{
    return d->m_Children.count();
}

/** \brief Returns the CategoryItem index of this CategoryItem in the parent list of children. */
int CategoryItem::childNumber() const
{
    if (d->m_Parent)
        return d->m_Parent->children().indexOf(const_cast<CategoryItem*>(this));
    return 0;
}

/** \brief Set data \e value for the CategoryItem for \e ref. */
bool CategoryItem::setData(const int ref, const QVariant &value)
{
    if (d->m_Data.value(ref)==value)
        return true;
    d->m_IsDirty = true;
    d->m_Data.insert(ref, value);
    return true;
}

/** \brief Return the data for the CategoryItem for \e ref. */
QVariant CategoryItem::data(const int ref) const
{
    return d->m_Data.value(ref);
}

/** \brief Return the dirty state. */
bool CategoryItem::isDirty() const
{
    return d->m_IsDirty;
}

/** \brief Set the dirty state. */
void CategoryItem::setDirty(bool state)
{
    d->m_IsDirty = state;
}

/** \brief Set the label of this CategoryItem in the language \e lang. */
bool CategoryItem::setLabel(const QString &label, const QString &lang)
{
    if (d->m_Labels.value(lang) == label)
        return true;
    d->m_IsDirty = true;
    d->m_Labels.insert(lang, label);
    return true;
}

/** \brief Return the label of this CategoryItem in the language \e lang (or the ALL_LANGUAGE if \e lang is not defined). */
QString CategoryItem::label(const QString &lang) const
{
    if (!d->m_Labels.keys().contains(lang))
        return d->m_Labels.value(Trans::Constants::ALL_LANGUAGE);
    return d->m_Labels.value(lang);
}

/** \brief Return all available languages for the label of this CategoryItem. */
QStringList CategoryItem::allLanguagesForLabel() const
{
    return d->m_Labels.keys();
}

/** \brief Clear all labels. */
void CategoryItem::clearLabels()
{
    d->m_Labels.clear();
    d->m_IsDirty = true;
}

/** \brief Remove the label for the language \e lang (\e lang must be formatted in 2char ISO format). */
void CategoryItem::removeLabel(const QString &lang)
{
    d->m_Labels.remove(lang);
    d->m_IsDirty = true;
}

/** \brief Sort CategoryItem children according to the lessThan() member. */
bool CategoryItem::sortChildren()
{
    qSort(d->m_Children.begin(), d->m_Children.end(), CategoryItem::lessThan);
    return true;
}

/** \brief Sort CategoryItem children according to the SortId value. */
bool CategoryItem::lessThan(const CategoryItem *c1, const CategoryItem *c2)
{
    return c1->sortId() < c2->sortId();
}

/** \brief Add an ICategoryContentItem to this CategoryItem. */
void CategoryItem::addContentItem(ICategoryContentItem *data)
{
    d->m_ContentChildren.append(data);
}

/** \brief Return all the ICategoryContentItem of this CategoryItem. */
QList<ICategoryContentItem *> CategoryItem::contentItems() const
{
    return d->m_ContentChildren;
}

/** \brief Remove all ICategoryContentItem without deleting pointers. */
void CategoryItem::clearContentItems()
{
    d->m_ContentChildren.clear();
}
