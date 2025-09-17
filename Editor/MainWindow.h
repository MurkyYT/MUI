#pragma once
#include "MainWindow.g.h"
#include "IGenerator.h"
#include "pugixml/pugixml.hpp"

class MainWindow : public MainWindowBase
{
public:
	MainWindow();
private:
	std::shared_ptr<mui::Window> previewWindow;
	std::wstring currentFile;

	std::shared_ptr<GeneratorBase> ParseXMLNode(const pugi::xml_node& node, size_t& index);
	void LoadFile(const std::wstring& path);
	void CloseFile();
	void SaveCurrentFile();
	void MoveLineUp();
	void MoveLineDown();
	void ParseXML(BOOL generateCode = FALSE);
	void GenerateCode(std::shared_ptr<GeneratorBase> rootGenerator, const pugi::xml_node& rootNode);
	void UpdatePreview(std::shared_ptr<GeneratorBase> rootGenerator);

	void MainWindow_OnClose(const void* sender, mui::EventArgs_t* e) override;
	void MainWindow_DragAndDrop(const void* sender, mui::EventArgs_t* e) override;
	void MainWindow_KeyDown(const void* sender, mui::EventArgs_t* e) override;

	void WindowHost_OnResize(const void* sender, mui::EventArgs_t* e) override;

	void File_Open(const void* sender, mui::EventArgs_t* e) override;
	void File_Close(const void* sender, mui::EventArgs_t* e) override;
	void File_Exit(const void* sender, mui::EventArgs_t* e) override;

	void Help_About(const void* sender, mui::EventArgs_t* e) override;

	void Entry_NewLine(const void* sender, mui::EventArgs_t* e) override;
	void Entry_KeyDown(const void* sender, mui::EventArgs_t* e) override;
	void Entry_Save(const void* sender, mui::EventArgs_t* e) override;
	void Entry_CharPressed(const void* sender, mui::EventArgs_t* e) override;
};