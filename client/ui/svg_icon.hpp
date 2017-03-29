/*
 * Tweeteria - A minimalistic tweet reader.
 * Copyright (C) 2017  Andreas Weis (der_ghulbus@ghulbus-inc.de)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef TWEETERIA_CLIENT_INCLUDE_GUARD_UI_SVG_ICON_HPP
#define TWEETERIA_CLIENT_INCLUDE_GUARD_UI_SVG_ICON_HPP

#include <QSvgWidget>

#include <ratio>

class SvgIcon : public QSvgWidget {
    Q_OBJECT
private:
    QSize m_size;
public:
    SvgIcon(QWidget* parent);

    void setIconSize(QSize s);

    void scaleIcon(int numerator, int denominator);

    QSize sizeHint() const override;
};

#endif
