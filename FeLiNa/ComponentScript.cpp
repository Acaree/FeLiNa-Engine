#include "ComponentScript.h"
#include "GameObject.h"
#include "ImGui/imgui.h"

ComponentScript::ComponentScript(GameObject* parent) : Component(parent)
{
	type = Component_Script;
	graph = new NodeGraph();
}

ComponentScript::~ComponentScript()
{
	RELEASE(graph);
}

void ComponentScript::DrawInspector()
{
	if (ImGui::TreeNodeEx("Script"))
	{
		if (graph == nullptr)
		{
			ImGui::Text("Don't have any Graph asociated");
			ImGui::Button("Drag Script Here");

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Assets_Nodes"))
				{
					std::string payload_n = (char*)payload->Data;
					int position = payload_n.find(".json");

					if (position != std::string::npos)
					{
						//TODO the super resource!
					}
					
				}

				ImGui::EndDragDropTarget();
			}

		}
		else
		{
			ImGui::Text(graph->name);

			if (ImGui::Button("Open Graph"))
			{
				open_graph = true;
			}

			if (open_graph)
				graph->DrawNodeGraph();


		}
		ImGui::TreePop();
	}

}