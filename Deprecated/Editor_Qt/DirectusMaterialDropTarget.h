/*
Copyright(c) 2016 Panos Karabelas

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is furnished
to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

// INCLUDES ========
#include <QLineEdit>
#include <memory>
//==================

//= FORWARD DECLARATIONS =
class DirectusInspector;
class DirectusMaterial;
namespace Directus
{
    class Material;
}
//========================

class DirectusMaterialDropTarget : public QLineEdit
{
    Q_OBJECT
public:
    explicit DirectusMaterialDropTarget(QWidget *parent = 0);
    void Initialize(DirectusInspector* inspector);

    // drop support
    virtual void dragEnterEvent(QDragEnterEvent* event);
    virtual void dragMoveEvent (QDragMoveEvent* event);
    virtual void dropEvent(QDropEvent* event);

private:
    DirectusInspector* m_inspector;

signals:
    void MaterialDropped(std::weak_ptr<Directus::Material> material);

public slots:
};
