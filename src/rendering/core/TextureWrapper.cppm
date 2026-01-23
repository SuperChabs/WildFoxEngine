module;

#include <string>
#include <type_traits>

#include <glad/glad.h>
#include <stb_image.h>

export module WFE.Rendering.Core.TextureWrapper;

export enum class TextureType 
{
    NONE             = GL_NONE,
    TEXTURE_2D       = GL_TEXTURE_2D,
    TEXTURE_3D       = GL_TEXTURE_3D,
    TEXTURE_CUBE_MAP = GL_TEXTURE_CUBE_MAP
};

export enum class TextureFormat 
{
    NONE    = GL_NONE,
    RGB8    = GL_RGB8,
    RGBA8   = GL_RGBA8,
    RGB16F  = GL_RGBA16F,
    RGBA16F = GL_RGBA16F
};

export enum  class TextureFilter
{
    LINEAR                 = GL_LINEAR,
    NEAREST                = GL_NEAREST,
    LINEAR_MIPMAP_LINEAR   = GL_LINEAR_MIPMAP_LINEAR,
    LINEAR_MIPMAP_NEAREST  = GL_LINEAR_MIPMAP_NEAREST,
    NEAREST_MIPMAP_LINEAR  = GL_NEAREST_MIPMAP_LINEAR,
    NEAREST_MIPMAP_NEAREST = GL_NEAREST_MIPMAP_NEAREST
};

export enum class TextureWrap
{
    REPEAT = GL_REPEAT,
    CLAMP_TO_EDGE = GL_CLAMP_TO_EDGE,
    CLAMP_TO_BORDER = GL_CLAMP_TO_BORDER,
    MIRRORED_REPEAT = GL_MIRRORED_REPEAT
};

export class Texture
{
    unsigned int textureID = 0;
    std::string path;
    std::string name;
    int width = 0;
    int height = 0;
    int channels = 0;
    bool isLoaded = false;

    TextureType type = TextureType::TEXTURE_2D;
    TextureFormat internalFormat = TextureFormat::RGBA8;

    TextureFilter minFilter = TextureFilter::LINEAR;
    TextureFilter magFilter = TextureFilter::LINEAR;
    TextureWrap wrapS = TextureWrap::REPEAT;
    TextureWrap wrapT = TextureWrap::REPEAT;

public:
    // Is using for creating empty texture
    Texture(int width, int height, TextureFormat format = TextureFormat::RGBA8);

    // Is using for creating normal texture
    Texture(const std::string& path, bool sRGB = false)
    {

    }
    
    ~Texture();
    
    Texture(const Texture&)             = delete;
    Texture& operator=(const Texture&)  = delete;
    Texture(Texture&& other)            = delete;
    Texture& operator=(Texture&& other) = delete;
    
    void Reload();
    
    void Bind(unsigned int slot = 0) const
    {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(toGL(type), textureID);
    }
    
    // void GenerateMipmaps()
    // {
    //     glGenerateMipmap();
    // }
    
    void SetMinFilter(TextureFilter filter) 
    { 
        if (textureID == 0)
            return;

        minFilter = filter; 

        glBindTexture(toGL(type), textureID);
        glTextureParameteri(toGL(type), GL_TEXTURE_MIN_FILTER, toGL(filter));
    }

    void SetMagFilter(TextureFilter filter) 
    { 
        if (textureID == 0)
            return;

        GLenum glFilter = (filter == TextureFilter::NEAREST) 
                            ? GL_NEAREST : GL_LINEAR; 

        glBindTexture(toGL(type), textureID);
        glTextureParameteri(toGL(type), GL_TEXTURE_MAG_FILTER, glFilter);
    }

    void SetWrapS(TextureWrap wrap)         
    { 
        if (textureID == 0)
            return;

        wrapS = wrap; 

        glBindTexture(toGL(type), textureID);
        glTextureParameteri(toGL(type), GL_TEXTURE_WRAP_S, toGL(wrap));
    }

    void SetWrapT(TextureWrap wrap)
    { 
        if (textureID == 0)
            return;

        wrapT = wrap; 

        glBindTexture(toGL(type), textureID);
        glTextureParameteri(toGL(type), GL_TEXTURE_WRAP_T, toGL(wrap));
    }

    void SetWrap(TextureWrap wrap)
    { 
        if (textureID == 0)
            return;

        wrapS = wrap; 
        wrapT = wrap; 

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTextureParameteri(toGL(type), GL_TEXTURE_WRAP_S, toGL(wrap));
        glTextureParameteri(toGL(type), GL_TEXTURE_WRAP_T, toGL(wrap));
    }

    // void SetData(const void* data, size_t size);
    void SetBorderColor(float r, float g, float b, float a)
    {
        if (textureID == 0)
            return;
        
        float borderColor[] = { r, g, b, a };
        
        glBindTexture(toGL(type), textureID);
        glTexParameterfv(toGL(type), GL_TEXTURE_BORDER_COLOR, borderColor);
        glBindTexture(toGL(type), 0);
    }
    
    unsigned int GetID()      const { return textureID; }
    int GetWidth()            const { return width; }
    int GetHeight()           const { return height; }
    int GetChannels()         const { return channels; }
    TextureType GetType()     const { return type; }
    TextureFormat GetFormat() const { return internalFormat; }
    const std::string& GetPath() const { return path; }
    const std::string& GetName() const { return name; }
    bool IsLoaded() const { return isLoaded; }
    bool IsValid() const { return textureID != 0; }

    // void Resize(int width, int height);
    
    template<typename Enum>
    constexpr std::underlying_type_t<Enum> toGL(Enum e) const noexcept 
    {
        return static_cast<std::underlying_type_t<Enum>>(e);
    }

private:
    void CreateTexture()  { glGenTextures(1, &textureID); }
    void DestroyTexture() { glDeleteTextures(1, &textureID); }

    void LoadFromFile(const std::string& path, bool sRGB) 
    {

    }
};