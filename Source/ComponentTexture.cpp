#include "Application.h"
#include "ComponentTexture.h"
#include "ModuleTextures.h"
#include "ModuleResource.h"
#include "Resource.h"
#include "ResourceTexture.h"
#include "ModuleFileSystem.h"

ComponentTexture::~ComponentTexture()
{
}

bool ComponentTexture::Update()
{
	ResourceTexture* tex = (ResourceTexture*)App->resources->GetResource(RUID);
	if (!active || tex == nullptr)
		return false;

	if (tex != nullptr)
		glBindTexture(GL_TEXTURE_2D, tex->GL_id);
	else
		glColor3f(1, 1, 1);

	return true;
}

void ComponentTexture::DrawInfo()
{
	ResourceTexture* tex = (ResourceTexture*)App->resources->GetResource(RUID);

	ImGui::PushID("toggleTexture");
	ImGui::Checkbox("", &active);
	ImGui::PopID();
	ImGui::SameLine();

	if (ImGui::CollapsingHeader("Texture", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick))
	{
		std::string filename;
		std::string extension;
		if (tex != nullptr)
			App->fileSystem->splitPath(tex->GetFile(), nullptr, &filename, &extension);
		beginDroppableSpace((tex == nullptr) ? "No Texture" : (filename+'.'+extension).c_str(), tex ==nullptr);
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TEXTURE"))
			{
				std::string path = (const char*)payload->Data;
				path = path.substr(0, payload->DataSize); //For some reason, it reads more than data size, so cut it
				UID = App->resources->getMeta(path.c_str())->getValue("meta")->getUint("UID");
			}
			ImGui::EndDragDropTarget();
		}

		if (tex != nullptr)
		{
			ImGui::Text("Texture Size:\n Width: %d | Height: %d", tex->width, tex->height);
			float panelWidth = ImGui::GetWindowContentRegionWidth();
			if (panelWidth > 250)
				panelWidth = 250;
			float conversionFactor = panelWidth / tex->width;
			ImVec2 imageSize = { tex->height *conversionFactor, panelWidth };
			ImGui::Image((ImTextureID)tex->GL_id, imageSize);
		}
		else
		{
			
		}
	}
}

void ComponentTexture::Save(JSON_Value * component) const
{
	ResourceTexture* tex = (ResourceTexture*)App->resources->GetResource(RUID);

	JSON_Value* texture = component->createValue();

	texture->addInt("Type", type);
	texture->addUint("UID", UID);
	texture->addString("texture", tex->GetExportedFile());

	component->addValue("", texture);
}

void ComponentTexture::Load(JSON_Value* component)
{
	//TODO with resources
	/*Material = App->textures->loadTexture(component->getString("texture"));*/
}
