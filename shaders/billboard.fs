uniform sampler2D gColorMap;

in vec2 texCoord0;

out vec4 fragColor;

void main(void) {
    fragColor = texture2D(gColorMap, texCoord0);
    if (fragColor.a < 0.1f)
        discard;
}