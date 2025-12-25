#version 330 core
in vec3 vColor;
in vec3 vWorldPos;

out vec4 FragColor;

uniform sampler2D floorTex;
uniform sampler2D wallTex;
uniform sampler2D ceilTex;
uniform sampler2D glassTex;
uniform sampler2D painting1Tex;
uniform sampler2D painting2Tex;

uniform int uHasFloor;
uniform int uHasWall;
uniform int uHasCeil;
uniform int uHasGlass;
uniform int uHasPainting1;
uniform int uHasPainting2;
uniform vec3 uPaint1Center;
uniform vec2 uPaint1Size;
uniform vec3 uPaint2Center;
uniform vec2 uPaint2Size;
uniform float uGlassOpacity;

uniform float uTile;
uniform vec3 uRoomCenter;
uniform vec3 uHalfSize;

void main() {
    vec3 p = vWorldPos - uRoomCenter;

    float dx = abs(abs(p.x) - uHalfSize.x);
    float dy = abs(abs(p.y) - uHalfSize.y);
    float dz = abs(abs(p.z) - uHalfSize.z);

    vec2 uv;
    int face = 0;

    if (dx < dy && dx < dz) {
        uv = vec2(p.z, p.y);
        face = 1;
    } else if (dz < dx && dz < dy) {
        uv = vec2(p.x, p.y);
        face = 2;
    } else {
        uv = vec2(p.x, p.z);
        face = 0;
    }

    uv *= uTile;

    vec4 texColor = vec4(vColor, 1.0);

    if (face == 0) {
        if (p.y > 0.0) {
            if (uHasCeil == 1) texColor = texture(ceilTex, uv) * vec4(vColor, 1.0);
            else if (uHasFloor == 1) texColor = texture(floorTex, uv) * vec4(vColor, 1.0);
        } else {
            if (uHasFloor == 1) texColor = texture(floorTex, uv) * vec4(vColor, 1.0);
            else if (uHasCeil == 1) texColor = texture(ceilTex, uv) * vec4(vColor, 1.0);
        }
    } else {
        if (uHasWall == 1) texColor = texture(wallTex, uv) * vec4(vColor, 1.0);
    }

    vec3 paint1Ref = vec3(0.2, 0.0, 0.0);
    vec3 paint2Ref = vec3(0.0, 0.2, 0.0);
    if (uHasPainting1 == 1 && distance(vColor, paint1Ref) < 0.01) {
        vec2 uvp;
        uvp.x = (p.x - (uPaint1Center.x - uPaint1Size.x * 0.5)) / uPaint1Size.x;
        uvp.y = (p.y - (uPaint1Center.y - uPaint1Size.y * 0.5)) / uPaint1Size.y;
        uvp = clamp(uvp, 0.0, 1.0);
        texColor = texture(painting1Tex, uvp);
        FragColor = texColor;
        return;
    }
    if (uHasPainting2 == 1 && distance(vColor, paint2Ref) < 0.01) {
        vec2 uvp;
        uvp.x = (p.z - (uPaint2Center.z - uPaint2Size.x * 0.5)) / uPaint2Size.x;
        uvp.y = (p.y - (uPaint2Center.y - uPaint2Size.y * 0.5)) / uPaint2Size.y;
        uvp = clamp(uvp, 0.0, 1.0);
        texColor = texture(painting2Tex, uvp);
        FragColor = texColor;
        return;
    }

    if (uHasGlass == 1) {
        vec3 glassRef = vec3(0.6, 0.8, 1.0);
        if (distance(vColor, glassRef) < 0.1) {
            vec4 g = texture(glassTex, uv);
            float alpha = (g.a > 0.001) ? g.a : (clamp((g.r + g.g + g.b) / 3.0 * uGlassOpacity, 0.0, 1.0));
            vec3 glassCol = g.rgb * vColor;
            texColor.rgb = mix(texColor.rgb, glassCol, alpha);
            texColor.a = alpha;
        }
    }

    FragColor = texColor;
}
