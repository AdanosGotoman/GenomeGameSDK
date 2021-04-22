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

// INCLUDES =====================
#include <QListView>
#include <QFileSystemModel>
//===============================

class DirectusViewport;
class DirectusHierarchy;
class DirectusIconProvider;
class DirectusFileDialog;
class DirectusInspector;

class DirectusFileExplorer : public QListView
{
    Q_OBJECT
public:
    explicit DirectusFileExplorer(QWidget *parent = 0);
    void Initialize(QWidget* mainWindow,
                    DirectusViewport* directusViewport,
                    DirectusHierarchy* hierarchy,
                    DirectusInspector* inspector
                    );
    void SetRootPath(QString path);
    QFileSystemModel* GetFileSystemModel();

    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void dragEnterEvent(QDragEnterEvent* event);
    virtual void dragMoveEvent (QDragMoveEvent* event);
    virtual void dropEvent(QDropEvent* event);

private:
    void SetRootDirectory(const std::string& directory);
    QString GetRootPath();
    QString GetSelectionPath();

    QFileSystemModel* m_fileModel;
    QPoint m_dragStartPosition;
    DirectusIconProvider* m_directusIconProvider;
    DirectusFileDialog* m_fileDialog;
    DirectusViewport* m_directusViewport;
    DirectusHierarchy* m_hierarchy;
    DirectusInspector* m_inspector;

signals:

public slots:
    void ShowContextMenu(QPoint pos);
    void RenameSelectedItem();
    void DoubleClick(QModelIndex);
    void CreateDirectory_();
    void CreateMaterial();
    void ShowRootPathInExplorer();
    void DeleteSelectedFile();
};
