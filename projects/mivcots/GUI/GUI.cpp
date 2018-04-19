#include "GUI.h"
#include "MIVCOTS\MIVCOTS.h"
#include <iostream>

DECLARE_APP(GUI);
IMPLEMENT_APP(GUI);

bool GUI::OnInit()
{
	frame = new Frame(NULL);

	std::vector<serial::PortInfo> devices_found = serial::list_ports();
	std::vector<serial::PortInfo>::iterator iter = devices_found.begin();
	while (iter != devices_found.end())
	{
		serial::PortInfo device = *iter++;
		frame->comObjects.push_back(device.port);
	}

	frame->comObjects.push_back("COM43");
	activeCars.push_back(0);
	activeCars.push_back(1);


	frame->initFrame(&aMIVCOTS, &activeCars);
	SetTopWindow(frame);
	frame->Show();
	frame->ShowFullScreen(true, wxFULLSCREEN_NOBORDER);

	aMIVCOTS.initialize();
	aMIVCOTS.start();
	
	

	timer = new wxTimer(this, gui_timer);
	timer->Start(1000 / FRAMERATE);

	return true;
}

void GUI::update(wxTimerEvent & event)
{
	//wxLogMessage("updating gui");
	frame->mapPanel.update();
	frame->Show();
	
}

void GUI::onExit(wxCommandEvent & event)
{
	timer->Stop();
	frame->Close(true);
	free(timer);
}

void Frame::comStart(wxCommandEvent & event)
{
	int selection = comComboBox->GetSelection();
	if (!(aMIVCOTS->serialOpen())) {
		if (selection == wxNOT_FOUND) {
			wxLogMessage("No com port selected");
		}
		else {
			aMIVCOTS->initSerial(115200, comObjects.at(selection));
			aMIVCOTS->startSerial();
			openComButton->SetLabel("Stop");
		}
	}
	else {
		wxLogMessage("Closed serial port");
		aMIVCOTS->stopSerial();
		openComButton->SetLabel("Start");
	}
	
	return;
}

void Frame::carSelect(wxCommandEvent & event)
{
	int selection = carComboBox->GetSelection();
	checkForNewCars();
	if (selection == wxNOT_FOUND) {
		wxLogMessage("No car selected");
		return;
	}
	bool found = false;
	for (unsigned int i = 0; i < statusWidgets.size(); i++) {
		if (activeCars->at(selection) == statusWidgets.at(i).getCarID()) {
			found = true;
		}
	}
	if (!found) {
		createStatusWidget(activeCars->at(selection));
	}
	else {
		wxLogMessage("Car%d is already open", activeCars->at(selection));
	}
}

void GUI::OnQuit(wxCloseEvent & evt)
{
	timer->Stop();
	frame->GetParent()->Close(true);
}


Frame::Frame(wxWindow * parent) : wxFrame(parent, -1, _("wxAUI Test"),
	wxDefaultPosition, wxDefaultSize,
	wxDEFAULT_FRAME_STYLE)
{
	
}

Frame::~Frame()
{
	m_mgr.UnInit();
	logTimer->Stop();
}

bool Frame::initFrame(MIVCOTS * aMIVCOTS, std::vector<long>* activeCars)
{
	this->aMIVCOTS = aMIVCOTS;
	this->activeCars = activeCars;

	wxMenu *menuFile = new wxMenu;
	menuFile->Append(wxID_EXIT);

	wxMenu *menuView = new wxMenu;
	menuView->Append(toggleFullscreen, "Toggle fullscreen\tF11", "Toggle fullscreen display");

	wxMenu *menuHelp = new wxMenu;
	menuHelp->Append(wxID_ABOUT);

	wxMenuBar *menuBar = new wxMenuBar;
	menuBar->Append(menuFile, "File");
	menuBar->Append(menuView, "View");
	menuBar->Append(menuHelp, "Help");

	SetMenuBar(menuBar);
	CreateStatusBar();
	SetStatusText("Welcome to MIVCOTS!");

	m_mgr.SetManagedWindow(this);

	log = new wxTextCtrl(this, -1, wxEmptyString,
		wxDefaultPosition, wxSize(200, 150),
		wxNO_BORDER | wxTE_MULTILINE | wxTE_READONLY);

	//adding log to gui
	wxLog::SetActiveTarget(new wxLogTextCtrl(log));
	wxLogMessage("test in gui");

	mapPanel = Map(this);
	mapPanel.initMap(aMIVCOTS, activeCars);

	createUIPanel();

	// add the panes to the manager
	m_mgr.SetFlags(m_mgr.GetFlags() ^ wxAUI_MGR_LIVE_RESIZE);

	m_mgr.AddPane(mapPanel.getPanel(), wxAuiPaneInfo().Center().MinSize(1280, 1280).BestSize(1280, 1280).MaxSize(1280, 1280));
	m_mgr.AddPane(uiPanel, wxBOTTOM, wxT("UI"));
	m_mgr.AddPane(log, wxBOTTOM, wxT("Log"));

	createStatusWidgets();

	// tell the manager to "commit" all the changes just made
	m_mgr.Update();

	logTimer = new wxTimer(this, log_timer);
	logTimer->Start(LOG_FREQUENCY);
	return true;
}

void Frame::onAbout(wxCommandEvent & event)
{
	wxMessageBox("This is the start of a MIVCOTS", "About MIVCOTS",
		wxOK | wxICON_INFORMATION);
}

void Frame::onToggleFullscreen(wxCommandEvent & event)
{
	ShowFullScreen(!IsFullScreen(), wxFULLSCREEN_NOBORDER);
}

void Frame::update(wxTimerEvent & event)
{
	wxString str = log->GetValue();
	if (!str.empty()) {
		log->Clear();
		std::ofstream file;
		file.open("log.txt", std::ios::out | std::ios::app);
		file << str;
		file.close();
	}
}

bool Frame::createUIPanel()
{
	uiPanel = new wxPanel(this, wxID_ANY);
	wxArrayString com_arr;
	wxArrayString car_arr;

	wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);
	wxFlexGridSizer* fgs = new wxFlexGridSizer(2, 3, 10, 10);

	int comSelection = 0;
	for (unsigned int i = 0; i < comObjects.size(); i++) {
		com_arr.Add((comObjects.at(i).c_str()));
		if (comObjects.at(i) == DEFAULT_SERIAL) {
			comSelection = i;
		}
	}
	wxStaticText* comText = new wxStaticText(uiPanel, wxID_ANY, "COM list");
	comComboBox = new wxComboBox(uiPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, com_arr);
	comComboBox->SetSelection(comSelection);
	openComButton = new wxButton(uiPanel, comStartButton, "Start");

	fgs->Add(comText, wxSizerFlags(1).Center());
	fgs->Add(comComboBox, wxSizerFlags(1).Center());
	fgs->Add(openComButton, wxSizerFlags(1).Center());

	for(int i : *activeCars){
		car_arr.Add(std::to_string(i));
	}
	wxStaticText* carText = new wxStaticText(uiPanel, wxID_ANY, "Car list");
	carComboBox = new wxComboBox(uiPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, car_arr);
	carComboBox->SetSelection(0);
	changeCarButton = new wxButton(uiPanel, carSelectButton, "Open");
	
	fgs->Add(carText, wxSizerFlags(1).Center());
	fgs->Add(carComboBox, wxSizerFlags(1).Center());
	fgs->Add(changeCarButton, wxSizerFlags(1).Center());

	hbox->Add(fgs, 1, wxALL | wxEXPAND, 15);
	uiPanel->SetSizer(hbox);
	return false;
}

StatusWidget* Frame::createStatusWidget(long carID)
{
	StatusWidget* statusWidget = new StatusWidget(this);
	statusWidget->initStatusWidget(aMIVCOTS, carID);

	statusWidgets.push_back(*statusWidget);

	wxString cap = wxString(wxT("Car" + std::to_string(carID)));
	m_mgr.AddPane(statusWidget->getPanel(), wxAuiPaneInfo().Name(wxT("Car" + std::to_string(carID))).DestroyOnClose(true).Caption(cap));

	m_mgr.Update();
	return statusWidget;
}

bool Frame::createStatusWidgets()
{
	for (long i : *activeCars) {
		bool found = false;
		for (StatusWidget temp : statusWidgets) {
			if (temp.getCarID() == i) {
				found = true;
			}
		}
		if (!found) {
			createStatusWidget(i);
			}
	}
	return true;
}

void Frame::checkForNewCars()
{
	//bs test code that should be replaced when we can get a list of cars from paul
	//long newCar = activeCars->size();
	//
	//activeCars->push_back(newCar);
	//createStatusWidgets();
	//carComboBox->Append(std::to_string(newCar));
}

void Frame::paneClosed(wxAuiManagerEvent & event)
{
	wxLogMessage("Closed " + event.GetPane()->caption);
	for (unsigned int i = 0; i < statusWidgets.size(); i++) {
		std::string tmp = "Car"+std::to_string(statusWidgets.at(i).getCarID());
		if (event.GetPane()->caption == tmp) {
			statusWidgets.erase(statusWidgets.begin() + i);
		}
	}
}
