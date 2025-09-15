#include "MainWindow.h"
#include "resource.h"

#include "Generators.h"
#include "GeneratorsManager.h"

std::shared_ptr<GeneratorBase> UIElementGeneratorBase::s_rootGenerator = NULL;
GeneratorsManager generatorsManager;

static void RegisterGenerators()
{
	generatorsManager.RegisterGenerator(L"Window",
		[]() { return std::make_shared<WindowGenerator>(); },
		{ L"Width", L"Height", L"MaxWidth", L"MinWidth", L"MaxHeight", L"MinHeight", L"CaptionColor", L"Title", L"BackgroundColor", L"Class", L"OnClose", L"KeyDown", L"KeyUp" });
	generatorsManager.RegisterGenerator(L"Label",
		[]() { return std::make_shared<LabelGenerator>(); },
		{ L"Text", L"TextAlignment", L"TextColor" });
	generatorsManager.RegisterGenerator(L"Button",
		[]() { return std::make_shared<ButtonGenerator>(); },
		{ L"Text", L"TextColor", L"HoverColor", L"PressedColor", L"BorderColor", L"OnClick" });
	generatorsManager.RegisterGenerator(L"CheckBox",
		[]() { return std::make_shared<CheckBoxGenerator>(); },
		{ L"Text", L"Checked", L"TextColor", L"StateChanged" });
	generatorsManager.RegisterGenerator(L"Entry",
		[]() { return std::make_shared<EntryGenerator>(); },
		{ L"Text", L"Placeholder", L"TextAlignment", L"Multiline", L"TextColor", L"TextChanged", L"NewLine", L"CharPressed", L"Completed", L"Save" });
	generatorsManager.RegisterGenerator(L"StackLayout",
		[]() { return std::make_shared<StackLayoutGenerator>(); },
		{ L"Orientation" });
	generatorsManager.RegisterGenerator(L"Grid",
		[]() { return std::make_shared<GridGenerator>(); },
		{ L"" });
	generatorsManager.RegisterGenerator(L"Grid.RowDefinitions",
		[]() { return std::make_shared<RowDefinitionsGenerator>(); },
		{ L"" });
	generatorsManager.RegisterGenerator(L"RowDefinition",
		[]() { return std::make_shared<RowDefinitionGenerator>(); },
		{ L"Height" });
	generatorsManager.RegisterGenerator(L"Grid.ColumnDefinitions",
		[]() { return std::make_shared<ColumnDefinitionsGenerator>(); },
		{ L"" });
	generatorsManager.RegisterGenerator(L"ColumnDefinition",
		[]() { return std::make_shared<ColumnDefinitionGenerator>(); },
		{ L"Width" });
	generatorsManager.RegisterGenerator(L"TreeView",
		[]() { return std::make_shared<TreeViewGenerator>(); },
		{ L"TextColor", L"ExpandButtonColor", L"SelectedColor", L"RightClick", L"DoubleClick", L"OnReturn", L"SelectionChanged" });
	generatorsManager.RegisterGenerator(L"NativeWindowHost",
		[]() { return std::make_shared<NativeWindowHostGenerator>(); },
		{ L"OnResize" });
	generatorsManager.RegisterGenerator(L"Section",
		[]() { return std::make_shared<SectionGenerator>(); },
		{ L"Text", L"TextColor", L"ExpandButtonColor" });
	generatorsManager.RegisterGenerator(L"ListView",
		[]() { return std::make_shared<ListViewGenerator>(); },
		{ L"TextColor", L"SeparatorColor", L"HoverColor", L"ShowColumns", L"RightClick", L"DoubleClick", L"SelectionChanged" });
	generatorsManager.RegisterGenerator(L"Column",
		[]() { return std::make_shared<ListViewColumnGenerator>(); },
		{ L"Title", L"Width" });

	WIN32_FIND_DATAA findFileData;
	HANDLE hFind = FindFirstFileA("./generators/*.dll", &findFileData);

	if (hFind == INVALID_HANDLE_VALUE) {
		return;
	}

	do {
		std::string fileName = "./generators/";
		fileName += findFileData.cFileName;

		HMODULE hModule = LoadLibraryA(fileName.c_str());
		if (hModule) {
			auto registerFunc = (void(*)(GeneratorsManager*))GetProcAddress(hModule, "RegisterGenerators");
			if (registerFunc) {
				registerFunc(&generatorsManager);
			}
		}
	} while (FindNextFileA(hFind, &findFileData) != 0);

	FindClose(hFind);
}

MainWindow::MainWindow() 
{
	RegisterGenerators();

	InitializeComponent();

	SetTitle(GetTitle() + L" - " + MVD_WVERSION);
} 

void MainWindow::MainWindow_OnClose(const void* sender, mui::EventArgs_t* e)
{
	PostQuitMessage(0);
}

static size_t GetXMLNodeChildCount(const pugi::xml_node& node)
{
	size_t count = 0;
	for (pugi::xml_node child = node.first_child(); child; child = child.next_sibling())
	{
		if (child.type() == pugi::node_element)
			++count;
	}
	return count;
}

static pugi::xml_node GetFirstNodeChild(const pugi::xml_node& node)
{
	for (pugi::xml_node child = node.first_child(); child; child = child.next_sibling())
	{
		if (child.type() == pugi::node_element)
			return child;
	}
}

std::shared_ptr<GeneratorBase> MainWindow::ParseXMLNode(const pugi::xml_node& node, size_t& index)
{
	std::wstring elementName = node.name();
	std::shared_ptr<GeneratorBase> generator = generatorsManager.CreateGenerator(elementName);
	if (!generator)
	{
		MessageBoxW(GetHWND(), (L"Unknown element: " + elementName).c_str(), L"Parse Error", MB_OK | MB_ICONERROR);
		SetFocus(designerEntry->GetHWND());
		designerEntry->SetCaretPos(node.offset_debug());
		return nullptr;
	}

	generator->SetElementName(elementName);
	generator->SetIndex(index++);
	std::vector<std::wstring> supportedProperties = generatorsManager.GetSupportedProperties(elementName);

	for (const auto& attr : node.attributes())
	{
		std::wstring attrName = attr.name();
		std::wstring attrValue = attr.value();

		bool isSupported = std::find(supportedProperties.begin(), supportedProperties.end(), attrName) != supportedProperties.end();
		if (!isSupported)
		{
			std::wstring errorMsg = L"Invalid attribute '" + attrName + L"' for element '" + elementName + L"'";
			MessageBoxW(GetHWND(), errorMsg.c_str(), L"Parse Error", MB_OK | MB_ICONERROR);
			SetFocus(designerEntry->GetHWND());
			designerEntry->SetCaretPos(node.offset_debug());
			return nullptr;
		}

		generator->SetProperty(attrName, attrValue);
	}

	for (const auto& child : node.children())
	{
		if (child.type() == pugi::node_element)
		{
			std::shared_ptr<GeneratorBase> childGenerator = ParseXMLNode(child, index);
			if (childGenerator)
				generator->AddChild(childGenerator);
			else
				return nullptr;
		}
	}

	return generator;
}


void MainWindow::ParseXML()
{
	pugi::xml_document doc;

	pugi::xml_parse_result result = doc.load_string(designerEntry->GetText().c_str());

	if (result.status != pugi::xml_parse_status::status_ok)
	{
		MessageBoxA(GetHWND(), result.description(), "XML Parse Error", MB_OK | MB_ICONERROR);
		SetFocus(designerEntry->GetHWND());
		designerEntry->SetCaretPos(result.offset);
		return;
	}

	size_t rootCount = GetXMLNodeChildCount(doc.root());
	if (rootCount > 1)
	{
		MessageBox(GetHWND(), L"Only one root element allowed", L"Error", MB_OK | MB_ICONERROR);
		SetFocus(designerEntry->GetHWND());
		designerEntry->SetCaretPos(doc.root().first_child().next_sibling().offset_debug() - 1);
		return;
	}

	if (wcscmp(doc.first_child().name(), L"Window") != 0)
	{
		MessageBox(GetHWND(), L"First element should be 'Window'", L"Error", MB_OK | MB_ICONERROR);
		SetFocus(designerEntry->GetHWND());
		designerEntry->SetCaretPos(0);
		return;
	}

	if (!doc.first_child().attribute(L"Class"))
	{
		MessageBox(GetHWND(), L"'Window' element must have a 'Class' attribute", L"Error", MB_OK | MB_ICONERROR);
		SetFocus(designerEntry->GetHWND());
		designerEntry->SetCaretPos(0);
		return;
	}

	size_t index = 0;
	std::shared_ptr<GeneratorBase> rootGenerator = ParseXMLNode(doc.first_child(), index);
	if (rootGenerator)
	{
		UIElementGeneratorBase::s_rootGenerator = rootGenerator;

		GenerateCode(rootGenerator);
		UpdatePreview(rootGenerator);
	}
}

void MainWindow::WindowHost_OnResize(const void* sender, mui::EventArgs_t* e)
{
	if (previewWindow)
	{
		SIZE windowSize = { (LONG)previewWindow->GetWidth(), (LONG)previewWindow->GetHeight() };
		SetWindowPos(previewWindow->GetHWND(), NULL,
			(int)windowHost->GetWidth() / 2 - windowSize.cx / 2,
			(int)windowHost->GetHeight() / 2 - windowSize.cy / 2,
			windowSize.cx, windowSize.cy, SWP_NOACTIVATE);
	}
}

void MainWindow::UpdatePreview(std::shared_ptr<GeneratorBase> rootGenerator)
{
	try {
		std::wstring heightStr = rootGenerator->GetProperty(L"Height");
		std::wstring widthStr = rootGenerator->GetProperty(L"Width");
		size_t height = 500;
		size_t width = 500;

		if (!heightStr.empty())
			height = std::stoi(heightStr.c_str());
		if (!widthStr.empty())
			width = std::stoi(widthStr.c_str());

		SIZE windowSize = { (LONG)width, (LONG)height };

		if (previewWindow)
			previewWindow->Close();

		windowHost->RemoveHostedWindow();
		previewWindow = rootGenerator->CreateWindowPreview();
		SetWindowLongPtr(previewWindow->GetHWND(), GWL_STYLE, WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX ^ WS_MINIMIZEBOX);

		if (previewWindow && rootGenerator->GetChildren().size() == 1)
			previewWindow->SetContent(rootGenerator->GetChildren()[0]->CreateUIElementPreview());
		else if (rootGenerator->GetChildren().size() != 1)
		{
			MessageBox(GetHWND(), L"Only one child allowed in Window", L"Error", MB_OK | MB_ICONERROR);
			SetFocus(designerEntry->GetHWND());
			designerEntry->SetCaretPos(0);
			previewWindow->Close();
			previewWindow = NULL;
			return;
		}

		windowHost->SetHostedWindow(previewWindow->GetHWND());
		SetWindowPos(previewWindow->GetHWND(), NULL, 
			(int)windowHost->GetWidth() / 2 - windowSize.cx / 2,
			(int)windowHost->GetHeight() / 2 - windowSize.cy / 2,
			windowSize.cx, windowSize.cy, SWP_NOACTIVATE);
	}
	catch (const std::exception& e) {
		MessageBoxA(GetHWND(), ("Preview Error: " + std::string(e.what())).c_str(),
			"Preview Error", MB_OK | MB_ICONWARNING);
	}
}

void MainWindow::GenerateCode(std::shared_ptr<GeneratorBase> rootGenerator)
{
	std::wstring generatedCode = rootGenerator->Generate();

	debugEntry->SetText(generatedCode);
}

void MainWindow::MainWindow_KeyDown(const void* sender, mui::EventArgs_t* e)
{
	if ((GetAsyncKeyState(VK_CONTROL) & 0x8000) && e->wParam == 'S') {
		ParseXML();
	}
}

void MainWindow::Entry_Save(const void* sender, mui::EventArgs_t* e)
{
	e->handled = true;
}

void MainWindow::Entry_NewLine(const void* sender, mui::EventArgs_t* e)
{
	HWND hEdit = designerEntry->GetHWND();
	DWORD start = 0;
	SendMessage(hEdit, EM_GETSEL, (WPARAM)&start, NULL);
	DWORD caretPos = start;

	int lineIndex = (int)SendMessage(hEdit, EM_LINEFROMCHAR, caretPos, 0);
	int lineStart = (int)SendMessage(hEdit, EM_LINEINDEX, lineIndex, 0);
	int lineLength = (int)SendMessage(hEdit, EM_LINELENGTH, lineStart, 0);

	std::wstring lineText(lineLength, L'\0');
	*(WORD*)lineText.data() = (WORD)lineLength;
	SendMessage(hEdit, EM_GETLINE, lineIndex, (LPARAM)lineText.data());
	lineText.resize(lineLength);

	int indent = 0;
	while (indent < lineText.size() && (lineText[indent] == L' ' || lineText[indent] == L'\t'))
		indent++;

	std::wstring baseIndent = lineText.substr(0, indent);

	int lineCaretIndex = caretPos - lineStart;
	bool betweenTags = false;
	int nextPos = lineCaretIndex;
	while (nextPos < lineText.size() && lineText[nextPos] == L' ')
		nextPos++;

	if (nextPos < lineText.size() && lineText[nextPos] == L'<' && nextPos + 1 < lineText.size() && lineText[nextPos + 1] == L'/')
	{
		int tagStart = indent;
		while (tagStart < lineCaretIndex && lineText[tagStart] != L'<') tagStart++;
		int tagEnd = tagStart + 1;
		while (tagEnd < lineCaretIndex && (iswalnum(lineText[tagEnd]) || lineText[tagEnd] == L'.' || lineText[tagEnd] == L'-' || lineText[tagEnd] == L'_'))
			tagEnd++;

		std::wstring openTag = lineText.substr(tagStart + 1, tagEnd - tagStart - 1);

		int closeStart = nextPos + 2;
		int closeEnd = closeStart;
		while (closeEnd < lineText.size() && (iswalnum(lineText[closeEnd]) || lineText[closeEnd] == L'.' || lineText[closeEnd] == L'-' || lineText[closeEnd] == L'_'))
			closeEnd++;

		std::wstring closeTag = lineText.substr(closeStart, closeEnd - closeStart);

		if (!openTag.empty() && openTag == closeTag)
			betweenTags = true;
	}

	std::wstring insertText = L"\r\n" + baseIndent;
	int caretInInsert = (int)insertText.size();
	if (betweenTags)
	{
		insertText += L"\t\r\n" + baseIndent;
		caretInInsert += 1;
	}

	SendMessage(hEdit, EM_REPLACESEL, TRUE, (LPARAM)insertText.c_str());
	SendMessage(hEdit, EM_SETSEL, caretPos + caretInInsert, caretPos + caretInInsert);

	e->handled = true;
}

void MainWindow::Entry_CharPressed(const void* sender, mui::EventArgs_t* e)
{
	HWND hEdit = designerEntry->GetHWND();
	DWORD start = 0;
	SendMessage(hEdit, EM_GETSEL, (WPARAM)&start, NULL);
	DWORD caretPos = start;

	int lineIndex = (int)SendMessage(hEdit, EM_LINEFROMCHAR, caretPos, 0);
	int lineStart = (int)SendMessage(hEdit, EM_LINEINDEX, lineIndex, 0);
	int lineLength = (int)SendMessage(hEdit, EM_LINELENGTH, lineStart, 0);

	if (lineLength == 0) return;

	std::wstring lineText(lineLength, L'\0');
	*(WORD*)lineText.data() = (WORD)lineLength;
	SendMessage(hEdit, EM_GETLINE, lineIndex, (LPARAM)lineText.data());
	lineText.resize(lineLength);

	int caretInLine = (int)(caretPos - lineStart);
	if (caretInLine < 1) return;

	if (e->wParam == L'>')
	{
		int pos = caretInLine - 1;
		while (pos >= 0 && lineText[pos] != L'<') pos--;

		if (pos < 0) return;

		std::wstring tagName;
		for (int i = pos + 1; i < caretInLine; ++i)
		{
			wchar_t c = lineText[i];
			if (iswalnum(c) || c == L'-' || c == L'_' || c == L'.')
				tagName += c;
			else if (c == ' ')
				break;
			else
				return;
		}

		if (tagName.empty()) return;

		std::wstring closingTag = L"</" + tagName + L">";

		SendMessage(hEdit, EM_SETSEL, caretPos, caretPos);
		SendMessage(hEdit, EM_REPLACESEL, TRUE, (LPARAM)closingTag.c_str());
		SendMessage(hEdit, EM_SETSEL, caretPos, caretPos);
	}
	else if (e->wParam == L'/')
	{
		int pos = caretInLine - 1;
		if (lineText[caretInLine] == '>') return;

		while (pos >= 0 && lineText[pos] != L'<') pos--;
		if (pos < 0) return;

		int tagStart = pos + 1;
		while (tagStart < caretInLine && lineText[tagStart] == L' ')
			tagStart++;

		if (tagStart >= caretInLine) return;

		SendMessage(hEdit, EM_SETSEL, caretPos, caretPos);
		SendMessage(hEdit, EM_REPLACESEL, TRUE, (LPARAM)L"/>");

		SendMessage(hEdit, EM_SETSEL, caretPos + 2, caretPos + 2);

		e->handled = true;
	}
	else if (e->wParam == L'-')
	{
		if (caretInLine >= 3)
		{
			std::wstring precedingText = lineText.substr(caretInLine - 3, 3);
			if (precedingText == L"<!-")
			{
				std::wstring commentClosing = L"-->";

				SendMessage(hEdit, EM_SETSEL, caretPos, caretPos);
				SendMessage(hEdit, EM_REPLACESEL, TRUE, (LPARAM)commentClosing.c_str());
				SendMessage(hEdit, EM_SETSEL, caretPos, caretPos);
			}
		}
	}
}