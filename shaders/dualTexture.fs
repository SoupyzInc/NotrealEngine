#version 330 core
in vec3 ourColor;
in vec2 TexCoord;

out vec4 FragColor;

uniform float mixValue;
uniform sampler2D texture1;
uniform sampler2D texture2;

void main() {
    // If there is transparency, do not mix with other texture.
    // Instead just display the other texture.
    // TODO: Replace branch with LERP on mix value.
     if (texture(texture1, TexCoord).a != 1) {
         FragColor = texture(texture2, TexCoord);
     } else if (texture(texture2, TexCoord).a != 1) {
         FragColor = texture(texture1, TexCoord);
     } else {
         FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), mixValue);
     }
}