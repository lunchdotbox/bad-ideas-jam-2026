#ifndef TEXT_H
#define TEXT_H

#include "buffer.h"
#include "device_loop.h"
#include "texture.h"
#include <elc/core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "graphics_pipeline.h"

typedef struct TextPushConstant {
    u32 texture_id;
    u32 transform_buffer_id;
} TextPushConstant;

typedef struct TextCharacter {
    vec3 transform1;
    u32 character;
    vec3 transform2;
    float padding1;
    vec3 transform3;
    float padding2;
} TextCharacter; // TODO: optimize for better memory usage

typedef struct TextRenderer {
    VkPipeline pipeline;
} TextRenderer;

typedef struct TextFont {
    Texture texture;
    ValidBuffer buffer;
    TextCharacter* buffer_mapped;
    u64 buffer_size, n_text;
    TextPushConstant push;
} TextFont;

TextRenderer createTextRenderer(Device device, PipelineConfig config);
void destroyTextRenderer(Device device, TextRenderer renderer);
TextFont createTextFont(Device device, DeviceLoop* loop, u64 buffer_size);
void destroyTextFont(Device device, TextFont font);
void drawTextFont(VkCommandBuffer command, Device device, TextRenderer renderer, TextFont* font);
void addFontCharacters(TextFont* font, TextCharacter* text, u64 n_text);
TextCharacter makeTextCharacter(mat3 transform, u32 text);
void addFontLetter(TextFont* font, mat3 transform, char letter);
void addFontText(TextFont* font, mat3 trans, const char* text);

#endif
