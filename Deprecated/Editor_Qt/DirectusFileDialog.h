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

//= INCLUDES =========
#include <QFileDialog>
//====================

class DirectusHierarchy;
class DirectusViewport;
class DirectusAssetLoader;

namespace Directus
{
    class Context;
}

class DirectusFileDialog : public QFileDialog
{
    Q_OBJECT
public:
    explicit DirectusFileDialog(QWidget *parent = 0);
    void Initialize(QWidget* mainWindow, DirectusHierarchy* hierarchy, DirectusViewport* directusViewport);
    void Reset();
    void OpenModel();
    void OpenModeImmediatly(const std::string& filePath);
    void OpenScene();
    void SaveScene();
    void SaveSceneAs();

private:
    std::string m_lastSceneFilePath;
    std::string m_assetOperation;

    DirectusAssetLoader* m_assetLoader;
    QWidget* m_mainWindow;
    Directus::Context* m_context;
    DirectusViewport* m_directusViewport;
    DirectusHierarchy* m_hierarchy;

public slots:
    void FileDialogAccepted(QString);

};
