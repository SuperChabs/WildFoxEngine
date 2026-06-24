#include "Model.h"

Model::Model(const std::string &path)
    : filepath(path) {
    size_t lastSlash = path.find_last_of("/");
    size_t lastDot = path.find_last_of(".");

    if (lastSlash != std::string::npos && lastDot != std::string::npos)
        name = path.substr(lastSlash + 1, lastDot - lastSlash - 1);
    else
        name = path;

    rootNode = std::make_shared<ModelNode>("Root");
}

void Model::AddMesh(Mesh &&mesh) {
    meshes.push_back(std::make_shared<Mesh>(std::move(mesh)));
}

void Model::AddMesh(std::shared_ptr<Mesh> mesh) {
    meshes.push_back(mesh);
}

std::shared_ptr<Mesh> Model::GetMesh(size_t index) {
    if (index < meshes.size())
        return meshes[index];
    return nullptr;
}

const std::vector<std::shared_ptr<Mesh> > &Model::GetMeshes() const {
    return meshes;
}

void Model::Draw(ShaderManager &shaderManager, const std::string &shaderName) {
    for (auto &mesh: meshes)
        if (mesh)
            mesh->Draw(shaderManager, shaderName);
}

void Model::SetTextures(const std::vector<Texture> &textures) {
    for (auto &mesh: meshes)
        if (mesh)
            mesh->SetTextures(textures);
}

void Model::SetMaterial(std::shared_ptr<Material> material, size_t index) {
    if (meshes[index])
        meshes[index]->SetMaterial(material);
}

void Model::SetMaterialForAll(std::shared_ptr<Material> material) {
    for (auto &mesh: meshes)
        if (mesh)
            mesh->SetMaterial(material);
}

size_t Model::GetMeshCount() const {
    return meshes.size();
}

const std::string &Model::GetName() const {
    return name;
}

const std::string &Model::GetFilepath() const {
    return filepath;
}

std::shared_ptr<ModelNode> Model::GetRootNode() const {
    return rootNode;
}

void Model::SetName(const std::string &n) {
    name = n;
}

void Model::SetFilepath(const std::string &path) {
    filepath = path;
}

void Model::SetRootNode(std::shared_ptr<ModelNode> node) {
    rootNode = node;
}