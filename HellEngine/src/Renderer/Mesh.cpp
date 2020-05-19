#include "hellpch.h"
#include "Mesh.h"
#include "Helpers/AssetManager.h"
#include <set>

namespace HellEngine
{
	Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, const char* name)
	{
		this->vertices = vertices;
		this->indices = indices;
		this->name = name;

		//Optimise();
		SetupMesh();
	}

	bool AreSame(double a, double b)
	{
		float EPSILON = 0.001f;
		return fabs(a - b) < EPSILON;
	}

	typedef std::pair<Vertex, int> VPair;



	bool CompareVertexPosition(Vertex v1, Vertex v2)
	{
		float eps = 0.00001;
		return ((fabs(v1.Position.x - v2.Position.x) < eps) && (fabs(v1.Position.y - v2.Position.y) > eps) && (fabs(v1.Position.z - v2.Position.z) > eps));
	}
		
	void Mesh::Optimise()
	{
		//std::map<Vertex*, unsigned int> newVertices;

		std::cout << "\n" << name << " old: " << vertices.size() << "\n";

		std::vector<std::pair<Vertex, unsigned int>> newVertices;
		std::vector<unsigned int> newIndices;

		//std::vector<Vertex>::iterator result;
		//result = find(newVertices.begin(), newVertices.end(), vertices[0]);


		for (int i = 0; i < vertices.size(); i++)
		{
			bool duplicateFound = false;

			for (int j = 0; j < newVertices.size(); j++)
			{
				if (CompareVertexPosition(vertices[i], newVertices[j].first))
				{
					duplicateFound = true;
					newIndices.push_back(newVertices[j].second);
				}
			}

			if (!duplicateFound)
			{
				newVertices.push_back(std::make_pair(vertices[i], newIndices.size()));
				newIndices.push_back(newIndices.size());
			}
		}
		

		vertices.clear();
		for (int i = 0; i < newVertices.size(); i++)
		{
			vertices.push_back(newVertices[i].first);
		}
		indices = newIndices;
		SetupMesh();

		std::cout << name << " new: " << vertices.size() << "\n\n";

		



		return;
		std::vector<std::pair<Vertex, int>> vBuffer;

		/*std::set<VPair, CmpClass> newVertices;
		int index = 0;

		for (int i = 0; i < vertices.size(); i++)
		{
			std::set<VPair>::iterator it = newVertices.find(std::make_pair(vertices[i], 0));
			if (it != newVertices.end()) indices.push_back(it->second);
			else
			{
				newVertices.insert(std::make_pair(vertices[i], index));
				newIndices.push_back(index++);
			}
		}*/

		// Notice that the vertices in the set are not sorted by the index
		// so you'll have to rearrange them like this:
		//vbuffer.resize(vertices.size());
		//for (set<VPair>::iterator it = vertices.begin(); it != vertices.end(); it++)
		//	vbuffer[it->second] = it->first;


		int index = 0;

		for (int i = 0; i < vertices.size(); i++)
		{
			bool duplicateFound = false;

			for (int j = 0; j < vBuffer.size(); j++)
			{
				if ((AreSame(vertices[i].Position.x, vBuffer[j].first.Position.x)) &&
					(AreSame(vertices[i].Position.y, vBuffer[j].first.Position.y)) &&
					(AreSame(vertices[i].Position.z, vBuffer[j].first.Position.z))) 
				{
					indices.push_back(vBuffer[j].second);
					duplicateFound = true;
				}

				if (duplicateFound)
				{
					continue;
				}

				vBuffer.push_back(std::pair<Vertex, int>(vertices[i], index));
				newIndices.push_back(newIndices.size());  
			}

			// Check if it exists
			//std::map<Vertex, unsigned int>::iterator it;

			/*if (newVertices.find(&vertices[i]) == newVertices.end()) {

				newVertices.emplace(&vertices[i], newIndices.size());
				indices.push_back(newIndices.size());
			}
			else {
				//indices.push_back(value);
				continue;
			}*/

			/*
			for (auto const& [key, value] : newVertices)
			{
				if ((AreSame(key->Position.x, vertices[i].Position.x)) &&
					(AreSame(key->Position.y, vertices[i].Position.y)) &&
					(AreSame(key->Position.z, vertices[i].Position.z))) {

					indices.push_back(value);
					continue;
				}
				//newVertices.insert({ vertices[i], newIndices.size() });
				//indices.push_back(newIndices.size());
			}*/

			/*newVertices.emplace(&vertices[i], newIndices.size());
			indices.push_back(newIndices.size());
			*/

			//map<Vertex, unsigned int>::iterator i = m.find("f");

			//if (i == m.end()) { /* Not found */ }
			//else { /* Found, i->first is f, i->second is ++-- */ }

			/*if (newVertices.find(vertices[i]) == newVertices.end()) {
				// not found
			}
			else {
				//map.insert({ vertices[i], newIndices.size() });
				//indices.push_back(newIndices.size());
			}*/
		}

		std::cout << name << " new: " << newVertices.size() << "\n\n";

	}

	/*Transform& Mesh::GetTransform() 
	{
		return transform; 
	}*/

	void Mesh::Draw()
	{
		AssetManager::BindMaterial(materialID);
		glBindVertexArray(VAO);



	//	glPointSize(4.0); 
		
		glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);
	//	glDrawArrays(GL_TRIANGLES, 0, (GLsizei)vertices.size());
	//	glDrawArrays(GL_POINTS, 0, (GLsizei)vertices.size());
	//	glDrawElements(GL_POINTS, 1000, GL_UNSIGNED_INT, 0);

	//	glDrawArrays(GL_POINTS, 0, (GLsizei)vertices.size());

//		glBindVertexArray(0);
	}

	void Mesh::SetupMesh()
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
		glEnableVertexAttribArray(5);
		glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, BlendingIndex));
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, BlendingWeight));
	}
}