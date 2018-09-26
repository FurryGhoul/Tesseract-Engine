#include "Application.h"
#include "ModuleGUI.h"
#include "ImGui\imgui.h"
#include "ImGui\imgui_impl_sdl.h"
#include "ImGui\imgui_impl_opengl2.h"
#include "Panel.h"
#include "PanelConfiguration.h"
#include "PanelConsole.h"
#include "PanelAbout.h"
#include "PanelHardwareInfo.h"

#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleAudio.h"
#include "ModuleSceneIntro.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModulePhysics3D.h"

ModuleGUI::ModuleGUI(bool start_enabled) : Module(start_enabled)
{
}

ModuleGUI::~ModuleGUI()
{
}

bool ModuleGUI::Init(rapidjson::Document& document)
{

	ImGui::CreateContext();
	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->context);
	ImGui_ImplOpenGL2_Init();

	rapidjson::Value& panels_aux = document["panels"]["Hardware_Info"];
	panels.push_back(hardwareInfo = new PanelHardwareInfo(panels_aux["name"].GetString(),panels_aux["pos_X"].GetFloat(), panels_aux["pos_Y"].GetFloat(), panels_aux["width"].GetFloat(), panels_aux["height"].GetFloat()));
	
	panels_aux = document["panels"]["Console"];
	panels.push_back(console = new PanelConsole(panels_aux["name"].GetString(), panels_aux["pos_X"].GetFloat(), panels_aux["pos_Y"].GetFloat(), panels_aux["width"].GetFloat(), panels_aux["height"].GetFloat()));
	
	panels_aux = document["panels"]["Configuration"];
	panels.push_back(configuration = new PanelConfiguration(panels_aux["name"].GetString(), panels_aux["pos_X"].GetFloat(), panels_aux["pos_Y"].GetFloat(), panels_aux["width"].GetFloat(), panels_aux["height"].GetFloat()));
	
	panels_aux = document["panels"]["About"];
	panels.push_back(about = new PanelAbout(panels_aux["name"].GetString(), panels_aux["pos_X"].GetFloat(), panels_aux["pos_Y"].GetFloat(), panels_aux["width"].GetFloat(), panels_aux["height"].GetFloat()));

	return true;
}

bool ModuleGUI::Start()
{	

	return true;
}

update_status ModuleGUI::PreUpdate(float dt)
{
	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();

	return update_status(UPDATE_CONTINUE);
}

update_status ModuleGUI::Update(float dt)
{
	update_status status= UPDATE_CONTINUE;

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{	
			if (ImGui::MenuItem("Save", "ctrl+s"))
				App->SaveGame();
			if (ImGui::MenuItem("Load"))
				App->LoadGame();
			if (ImGui::MenuItem("Close", "ESC"))
				status = UPDATE_STOP;

			ImGui::EndMenu();
		}
		
		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("Hardware Info",NULL, hardwareInfo->isActive()))
				hardwareInfo->toggleActive();
			if (ImGui::MenuItem("Console", NULL, console->isActive()))
				console->toggleActive();
			if (ImGui::MenuItem("Config", NULL, configuration->isActive()))
				configuration->toggleActive();
			if (ImGui::MenuItem("About", NULL, about->isActive()))
				about->toggleActive();
			ImGui::EndMenu();
		}
	}
	ImGui::EndMainMenuBar();

	return status;
}

update_status ModuleGUI::PostUpdate(float dt)
{
	

	return update_status(UPDATE_CONTINUE);
}

bool ModuleGUI::CleanUp()
{
	for (std::list<Panel*>::iterator it_p = panels.begin(); it_p != panels.end(); it_p++)
	{
		if (*it_p != nullptr)
			delete *it_p;
		*it_p = nullptr;
	}

	panels.clear();
	console = nullptr;

	ImGui_ImplSDL2_Shutdown();
	ImGui_ImplOpenGL2_Shutdown();

	return true;
}

void ModuleGUI::logFPS(float fps, float ms)
{
	if (configuration != nullptr)
		configuration->addFPS(fps, ms);
}

void ModuleGUI::AddLog(const char* log)
{
	if(App->gui && console!=nullptr)
		console->AddLog(log);
}

void ModuleGUI::handleInput(SDL_Event * event)
{
	ImGui_ImplSDL2_ProcessEvent(event);
}

bool ModuleGUI::Save(rapidjson::Document& document, rapidjson::FileWriteStream& os) {
	
	rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
	document.AddMember("Name", 2, allocator);
	document.AddMember("Pepe", "paco", allocator);

	rapidjson::Value Obj(rapidjson::kObjectType);
	Obj.AddMember("tete", "gym\n", allocator);
	rapidjson::Value Ovj(rapidjson::kObjectType);
	Ovj.AddMember("tinc", "gana", allocator);
	Obj.AddMember("pilota", Ovj, allocator);
	document.AddMember("chistorra", Obj, allocator);

	rapidjson::Writer<rapidjson::FileWriteStream> writer(os);

	return true;
}
bool ModuleGUI::Load(rapidjson::Document& document) {
	return true;
}

void ModuleGUI::Draw()
{
	for (std::list<Panel*>::iterator it_p = panels.begin(); it_p != panels.end(); it_p++)
	{
		if ((*it_p)->isActive())
		{
			ImGui::SetNextWindowPos({ (*it_p)->posX, (*it_p)->posY });
			ImGui::SetNextWindowSize({ (*it_p)->width, (*it_p)->height });
			(*it_p)->Draw();
		}
	}

	ImGui::Render();
	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
}