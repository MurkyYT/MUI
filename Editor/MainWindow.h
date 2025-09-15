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
private:
	std::shared_ptr<GeneratorBase> ParseXMLNode(const pugi::xml_node& node, size_t& index);
	void ParseXML();
	void GenerateCode(std::shared_ptr<GeneratorBase> rootGenerator);
	void UpdatePreview(std::shared_ptr<GeneratorBase> rootGenerator);

	void MainWindow_OnClose(const void* sender, mui::EventArgs_t* e) override;
	void MainWindow_KeyDown(const void* sender, mui::EventArgs_t* e) override;

	void WindowHost_OnResize(const void* sender, mui::EventArgs_t* e) override;

	void Entry_NewLine(const void* sender, mui::EventArgs_t* e) override;
	void Entry_Save(const void* sender, mui::EventArgs_t* e) override;
	void Entry_CharPressed(const void* sender, mui::EventArgs_t* e) override;
};

