#include <shlwapi.h>

#pragma comment(lib, "shlwapi.lib")

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
		{ L"WndProc", L"DragAndDrop", L"Width", L"Height", L"MaxWidth", L"MinWidth", L"MaxHeight", L"MinHeight", L"CaptionColor", L"Title", L"BackgroundColor", L"Class", L"OnClose", L"KeyDown", L"KeyUp"});
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
	generatorsManager.RegisterGenerator(L"ListView.Column",
		[]() { return std::make_shared<ListViewColumnGenerator>(); },
		{ L"Title", L"Width" });
	generatorsManager.RegisterGenerator(L"Menu",
		[]() { return std::make_shared<MenuGenerator>(); },
		{ L"TextColor", L"BackgroundColor", L"BottomBarColor", L"ItemBackgroundColor", L"ItemBackgroundHotColor",
		  L"ItemBackgroundSelectedColor", L"Name" });
	generatorsManager.RegisterGenerator(L"MenuItem",
		[]() { return std::make_shared<MenuItemGenerator>(); },
		{ L"Text", L"Enabled", L"Checked", L"Shortcut", L"OnClick", L"Name" });
	generatorsManager.RegisterGenerator(L"MenuSeparator",
		[]() { return std::make_shared<MenuSeparatorGenerator>(); },
		{ L"" });

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
} 

void MainWindow::MainWindow_OnClose(const void* sender, mui::EventArgs_t* e)
{
	PostQuitMessage(0);
}

void MainWindow::MainWindow_DragAndDrop(const void* sender, mui::EventArgs_t* e)
{
	HDROP hdrop = (HDROP)e->wParam;

	size_t files = DragQueryFile(hdrop, 0xFFFFFFFF, NULL, NULL);

	if (files > 1)
	{
		MessageBox(GetHWND(), L"Only one file allowed for now", L"Error", MB_OK | MB_ICONERROR);
		return;
	}

	for (UINT i = 0; i < files; i++)
	{
		UINT neededSize = DragQueryFile(hdrop, i, NULL, 0);
		std::wstring buf(neededSize, L'\0');
		DragQueryFile(hdrop, i, (LPWSTR)buf.data(), neededSize + 1);
		LoadFile(buf);
	}
}

void MainWindow::CloseFile()
{
	if (currentFile.empty()) return;

	designerEntry->SetText(L"");
	if (previewWindow)
	{
		previewWindow->Close();
		previewWindow = NULL;
	}

	windowHost->RemoveHostedWindow();

	windowTitleLabel->SetText(L"");
	fakeWindowGrid->SetVisible(FALSE);

	SetTitle(L"MUI Visual Designer");

	currentFile = L"";
}

void MainWindow::LoadFile(const std::wstring& path)
{
	CloseFile();

	if (path.substr(path.size() - 4, 4) != L"muix") {

		MessageBox(GetHWND(), L"Wrong file extention, should be 'muix'", L"Error", MB_OK | MB_ICONERROR);
		return;
	}

	HANDLE hFile = CreateFileW(
		path.c_str(),
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		MessageBox(GetHWND(), L"Error occurred when opening the file", L"Error", MB_OK | MB_ICONERROR);
		return;
	}

	DWORD fileSize = GetFileSize(hFile, NULL);
	if (fileSize == INVALID_FILE_SIZE) {
		CloseHandle(hFile);
		MessageBox(GetHWND(), L"Error occurred when reading file size", L"Error", MB_OK | MB_ICONERROR);
		return;
	}

	std::string utf8(fileSize, '\0');
	DWORD bytesRead = 0;

	if (!ReadFile(hFile, (void*)utf8.data(), fileSize, &bytesRead, NULL)) {
		CloseHandle(hFile);
		MessageBox(GetHWND(), L"Error occurred when reading file content", L"Error", MB_OK | MB_ICONERROR);
		return;
	}

	CloseHandle(hFile);

	if (utf8.size() >= 3 &&
		(unsigned char)utf8[0] == 0xEF &&
		(unsigned char)utf8[1] == 0xBB &&
		(unsigned char)utf8[2] == 0xBF)
	{
		utf8.erase(0, 3);
	}

	int wlen = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(),
		(int)utf8.size(), nullptr, 0);

	std::wstring wstr(wlen, L'\0');
	MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(),
		(int)utf8.size(), &wstr[0], wlen);

	SetTitle(L"MUI Visual Designer - " + path);
	currentFile = path;
	designerEntry->SetText(wstr);
	fakeWindowGrid->SetVisible(TRUE);
	ParseXML();
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
			return NULL;
		}

		generator->SetProperty(attrName, attrValue);
	}

	for (const auto& child : node.children())
	{
		if (child.type() == pugi::node_element)
		{
			std::shared_ptr<GeneratorBase> childGenerator = ParseXMLNode(child, index);
			if (childGenerator) 
			{
				generator->AddChild(childGenerator);
				childGenerator->SetParent(generator);
			}
			else
				return NULL;
		}
	}

	return generator;
}


void MainWindow::ParseXML(BOOL generateCode)
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

		try {
			if (generateCode)
				GenerateCode(rootGenerator, doc.first_child());

			UpdatePreview(rootGenerator);
		}
		catch (std::runtime_error e)
		{
			MessageBoxA(GetHWND(), e.what(), "Error", MB_OK | MB_ICONERROR);
		}
	}
}

void MainWindow::WindowHost_OnResize(const void* sender, mui::EventArgs_t* e)
{
	if (previewWindow)
	{
		SIZE windowSize = { (LONG)previewWindow->GetWidth(), (LONG)previewWindow->GetHeight() };
		SetWindowPos(previewWindow->GetHWND(), NULL,
			0 ,
			0,
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

		SetWindowLongPtr(previewWindow->GetHWND(), GWL_STYLE, WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX ^ WS_MINIMIZEBOX ^ WS_CAPTION);

		std::shared_ptr<GeneratorBase> menuChild = NULL;
		std::shared_ptr<GeneratorBase> contentChild = NULL;

		for (const auto& child : rootGenerator->GetChildren()) {
			if (child->GetElementName() == L"Menu") {
				menuChild = child;
			}
			else {
				contentChild = child;
			}
		}

		if (rootGenerator->GetChildren().size() > 2) {
			MessageBox(GetHWND(), L"Window can have at most 2 children: one Menu and one content element", L"Error", MB_OK | MB_ICONERROR);
			SetFocus(designerEntry->GetHWND());
			designerEntry->SetCaretPos(0);
			previewWindow->Close();
			previewWindow = NULL;
			return;
		}

		if (rootGenerator->GetChildren().size() == 2 && !menuChild) {
			MessageBox(GetHWND(), L"When Window has 2 children, one must be a Menu", L"Error", MB_OK | MB_ICONERROR);
			SetFocus(designerEntry->GetHWND());
			designerEntry->SetCaretPos(0);
			previewWindow->Close();
			previewWindow = NULL;
			return;
		}

		if (menuChild) {
			auto menuGen = std::dynamic_pointer_cast<MenuGenerator>(menuChild);
			if (menuGen) {
				auto menu = menuGen->CreateMenuPreview();
				if (menu) {
					previewWindow->SetMenu(menu);
				}
			}
		}

		if (contentChild) {
			auto contentElement = contentChild->CreateUIElementPreview();
			if (contentElement) {
				previewWindow->SetContent(contentElement);
			}
		}
		else if (rootGenerator->GetChildren().size() == 1 && !menuChild) {
			auto contentElement = rootGenerator->GetChildren()[0]->CreateUIElementPreview();
			if (contentElement) {
				previewWindow->SetContent(contentElement);
			}
		}

		windowHost->SetHostedWindow(previewWindow->GetHWND());
		SetWindowPos(previewWindow->GetHWND(), NULL,
			0, 0,
			windowSize.cx, windowSize.cy, SWP_NOACTIVATE | SWP_FRAMECHANGED);

		COLORREF bgColor = previewWindow->GetCaptionColor();
		BYTE r = GetRValue(bgColor);
		BYTE g = GetGValue(bgColor);
		BYTE b = GetBValue(bgColor);

		double brightness = 0.299 * r + 0.587 * g + 0.114 * b;
		COLORREF textColor = (brightness < 128) ? RGB(255, 255, 255) : RGB(0, 0, 0);

		windowTitleLabel->SetText(L"      " + previewWindow->GetTitle());
		windowCaption->SetBackgroundColor(bgColor);
		windowTitleLabel->SetTextColor(textColor);
	}
	catch (const std::exception& e) {
		MessageBoxA(GetHWND(), ("Preview Error: " + std::string(e.what())).c_str(),
			"Preview Error", MB_OK | MB_ICONWARNING);
	}
}

void MainWindow::File_Close(const void* sender, mui::EventArgs_t* e)
{
	CloseFile();
}

void MainWindow::File_Open(const void* sender, mui::EventArgs_t* e)
{
	OPENFILENAME ofn = { 0 };
	wchar_t fileName[MAX_PATH] = { 0 };

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = GetHWND();
	ofn.lpstrFilter = L"MUI XML Files (*.muix)\0*.muix\0All Files (*.*)\0*.*\0";
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	ofn.lpstrDefExt = L"muix";

	if (GetOpenFileName(&ofn))
	{
		std::wstring path = fileName;
		LoadFile(path);
	}
}

void MainWindow::File_Exit(const void* sender, mui::EventArgs_t* e)
{
	PostQuitMessage(0);
}

void MainWindow::Help_About(const void* sender, mui::EventArgs_t* e)
{
	MessageBox(GetHWND(), L"MUI Visual Designer (" MVD_WVERSION L")\nCreated by: Murky\nBuilt at: " __TIMESTAMP__ , L"About", MB_OK | MB_ICONINFORMATION);
}

void MainWindow::GenerateCode(std::shared_ptr<GeneratorBase> rootGenerator, const pugi::xml_node& rootNode)
{
	std::wstring generatedCode = rootGenerator->Generate();
	std::wstring basePath = currentFile;
	wchar_t pathBuffer[MAX_PATH];
	wcsncpy_s(pathBuffer, basePath.c_str(), _TRUNCATE);

	PathRemoveFileSpecW(pathBuffer);
	basePath = pathBuffer;

	std::wstring newFilePath = basePath + L"\\" +
		std::wstring(rootNode.attribute(L"Class").value()) + L".g.h";

	FILE* f;
	if (_wfopen_s(&f, newFilePath.c_str(), L"wb") == 0 && f)
	{
		unsigned char bom[] = { 0xEF, 0xBB, 0xBF };
		fwrite(bom, 1, sizeof(bom), f);

		int len = WideCharToMultiByte(CP_UTF8, 0,
			generatedCode.c_str(),
			(int)generatedCode.size(),
			nullptr, 0, nullptr, nullptr);

		std::string utf8(len, '\0');
		WideCharToMultiByte(CP_UTF8, 0,
			generatedCode.c_str(),
			(int)generatedCode.size(),
			(LPSTR)utf8.data(), len,
			nullptr, nullptr);

		fwrite(utf8.data(), 1, utf8.size(), f);

		fclose(f);
	}
}

void MainWindow::SaveCurrentFile()
{
	FILE* f;
	if (_wfopen_s(&f, currentFile.c_str(), L"wb") == 0 && f)
	{
		std::wstring content = designerEntry->GetText();
		unsigned char bom[] = { 0xEF, 0xBB, 0xBF };
		fwrite(bom, 1, sizeof(bom), f);

		int len = WideCharToMultiByte(CP_UTF8, 0,
			content.c_str(),
			(int)content.size(),
			nullptr, 0, nullptr, nullptr);

		std::string utf8(len, '\0');
		WideCharToMultiByte(CP_UTF8, 0,
			content.c_str(),
			(int)content.size(),
			(LPSTR)utf8.data(), len,
			nullptr, nullptr);

		fwrite(utf8.data(), 1, utf8.size(), f);

		fclose(f);
	}
}

void MainWindow::MoveLineUp()
{
	HWND hEdit = designerEntry->GetHWND();
	DWORD start, end;
	SendMessage(hEdit, EM_GETSEL, (WPARAM)&start, (LPARAM)&end);

	int currentLineIndex = (int)SendMessage(hEdit, EM_LINEFROMCHAR, start, 0);
	if (currentLineIndex == 0) return;

	int textLength = (int)SendMessage(hEdit, WM_GETTEXTLENGTH, 0, 0);
	std::wstring allText(textLength + 1, L'\0');
	SendMessage(hEdit, WM_GETTEXT, textLength + 1, (LPARAM)allText.data());
	allText.resize(textLength);

	int currentLineStart = (int)SendMessage(hEdit, EM_LINEINDEX, currentLineIndex, 0);
	int prevLineStart = (int)SendMessage(hEdit, EM_LINEINDEX, currentLineIndex - 1, 0);
	int nextLineStart = (int)SendMessage(hEdit, EM_LINEINDEX, currentLineIndex + 1, 0);
	if (nextLineStart == -1) nextLineStart = textLength;

	std::wstring prevLine = allText.substr(prevLineStart, currentLineStart - prevLineStart);
	std::wstring currentLine = allText.substr(currentLineStart, nextLineStart - currentLineStart);

	if (prevLine.size() >= 2 && prevLine.compare(prevLine.size() - 2, 2, L"\r\n") == 0) {
		prevLine = prevLine.substr(0, prevLine.length() - 2);
	}
	if (currentLine.size() >= 2 && currentLine.compare(currentLine.size() - 2, 2, L"\r\n") == 0) {
		currentLine = currentLine.substr(0, currentLine.length() - 2);
	}

	std::wstring newText = currentLine + L"\r\n" + prevLine;
	if (nextLineStart < textLength) {
		newText += L"\r\n";
	}

	SendMessage(hEdit, EM_SETSEL, prevLineStart, nextLineStart);
	SendMessage(hEdit, EM_REPLACESEL, TRUE, (LPARAM)newText.c_str());

	int caretOffset = start - currentLineStart;
	int newCaretPos = prevLineStart + caretOffset;
	SendMessage(hEdit, EM_SETSEL, newCaretPos, newCaretPos);
}

void MainWindow::MoveLineDown()
{
	HWND hEdit = designerEntry->GetHWND();
	DWORD start, end;
	SendMessage(hEdit, EM_GETSEL, (WPARAM)&start, (LPARAM)&end);

	int currentLineIndex = (int)SendMessage(hEdit, EM_LINEFROMCHAR, start, 0);
	int totalLines = (int)SendMessage(hEdit, EM_GETLINECOUNT, 0, 0);
	if (currentLineIndex >= totalLines - 1) return;

	int textLength = (int)SendMessage(hEdit, WM_GETTEXTLENGTH, 0, 0);
	std::wstring allText(textLength + 1, L'\0');
	SendMessage(hEdit, WM_GETTEXT, textLength + 1, (LPARAM)allText.data());
	allText.resize(textLength);

	int currentLineStart = (int)SendMessage(hEdit, EM_LINEINDEX, currentLineIndex, 0);
	int nextLineStart = (int)SendMessage(hEdit, EM_LINEINDEX, currentLineIndex + 1, 0);
	int nextNextLineStart = (int)SendMessage(hEdit, EM_LINEINDEX, currentLineIndex + 2, 0);
	if (nextNextLineStart == -1) nextNextLineStart = textLength;

	std::wstring currentLine = allText.substr(currentLineStart, nextLineStart - currentLineStart);
	std::wstring nextLine = allText.substr(nextLineStart, nextNextLineStart - nextLineStart);

	if (currentLine.size() >= 2 && currentLine.compare(currentLine.size() - 2, 2, L"\r\n") == 0) {
		currentLine = currentLine.substr(0, currentLine.length() - 2);
	}
	if (nextLine.size() >= 2 && nextLine.compare(nextLine.size() - 2, 2, L"\r\n") == 0) {
		nextLine = nextLine.substr(0, nextLine.length() - 2);
	}

	std::wstring newText = nextLine + L"\r\n" + currentLine;
	if (nextNextLineStart < textLength) {
		newText += L"\r\n";
	}

	SendMessage(hEdit, EM_SETSEL, currentLineStart, nextNextLineStart);
	SendMessage(hEdit, EM_REPLACESEL, TRUE, (LPARAM)newText.c_str());

	int caretOffset = start - currentLineStart;
	int newCaretPos = currentLineStart + (int)nextLine.length() + 2 + caretOffset;
	SendMessage(hEdit, EM_SETSEL, newCaretPos, newCaretPos);
}

void MainWindow::MainWindow_KeyDown(const void* sender, mui::EventArgs_t* e)
{
	if ((GetAsyncKeyState(VK_CONTROL) & 0x8000) && e->wParam == 'S') {
		SaveCurrentFile();
		ParseXML(TRUE);
	}
	else if ((GetAsyncKeyState(VK_MENU) & 0x8000)) {
		if (e->wParam == VK_UP) {
			MoveLineUp();
			e->handled = true;
		}
		else if (e->wParam == VK_DOWN) {
			MoveLineDown();
			e->handled = true;
		}
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
	while (indent < lineText.size() && lineText[indent] == L'\t')
		indent++;

	std::wstring baseIndent = lineText.substr(0, indent);

	int lineCaretIndex = caretPos - lineStart;
	bool betweenTags = false;
	int nextPos = lineCaretIndex;
	while (nextPos < lineText.size() && lineText[nextPos] == L'\t')
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