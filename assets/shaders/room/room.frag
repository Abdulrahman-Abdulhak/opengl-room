in vec3 vColor;
in vec3 vWorldPos;

out vec4 FragColor;

uniform sampler2D floorTex;
uniform sampler2D wallTex;
uniform sampler2D ceilTex;
uniform sampler2D glassTex;
uniform sampler2D painting1Tex;
uniform sampler2D painting2Tex;

uniform int uHasFloor; // 0/1
uniform int uHasWall;
uniform int uHasCeil;
uniform int uHasGlass;
uniform int uHasPainting1;
uniform int uHasPainting2;
uniform vec3 uPaint1Center;
uniform vec2 uPaint1Size;
uniform vec3 uPaint2Center;
uniform vec2 uPaint2Size;
uniform float uGlassOpacity; // multiplier used when glass texture has no alpha

uniform float uTile;       // e.g. 1.0, 2.0, 5.0
uniform vec3 uRoomCenter;  // usually model translation (or 0 if centered)
uniform vec3 uHalfSize;    // (width/2, height/2, depth/2) in WORLD space

void main() {
    // Position relative to room center
    vec3 p = vWorldPos - uRoomCenter;

    // Determine which face this fragment belongs to by closeness to the room planes
    float dx = abs(abs(p.x) - uHalfSize.x);
    float dy = abs(abs(p.y) - uHalfSize.y);
    float dz = abs(abs(p.z) - uHalfSize.z);

    vec2 uv;
    int face = 0; // 0=floor/ceil, 1=wall (left/right), 2=wall front/back

    // Project UVs based on dominant axis
    if (dx < dy && dx < dz) {
        // left/right wall: use Z and Y
        uv = vec2(p.z, p.y);
        face = 1;
    } else if (dz < dx && dz < dy) {
        // front/back wall: use X and Y
        uv = vec2(p.x, p.y);
        face = 2;
    } else {
        // floor/ceiling: use X and Z
        uv = vec2(p.x, p.z);
        face = 0;
    }

    uv *= uTile; // tiling happens here

    vec4 texColor = vec4(vColor, 1.0);

    // Sample appropriate texture if available
    if (face == 0) {
        // Distinguish floor vs ceiling by p.y sign (relative to room center):
        // positive -> ceiling, negative -> floor.
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

    // Override sampling for paintings (marked by vertex color markers)
    vec3 paint1Ref = vec3(0.2, 0.0, 0.0);
    vec3 paint2Ref = vec3(0.0, 0.2, 0.0);
    if (uHasPainting1 == 1 && distance(vColor, paint1Ref) < 0.01) {
        // Painting1 is on a north-facing wall (X-Y plane at Z ~= const).
        vec2 uvp;
        uvp.x = (p.x - (uPaint1Center.x - uPaint1Size.x * 0.5)) / uPaint1Size.x;
        uvp.y = (p.y - (uPaint1Center.y - uPaint1Size.y * 0.5)) / uPaint1Size.y;
        uvp = clamp(uvp, 0.0, 1.0);
        texColor = texture(painting1Tex, uvp);
        FragColor = texColor;
        return;
    }
    if (uHasPainting2 == 1 && distance(vColor, paint2Ref) < 0.01) {
        // Painting2 is on an east-facing wall (Z-Y plane at X ~= const).
        vec2 uvp;
        uvp.x = (p.z - (uPaint2Center.z - uPaint2Size.x * 0.5)) / uPaint2Size.x;
        uvp.y = (p.y - (uPaint2Center.y - uPaint2Size.y * 0.5)) / uPaint2Size.y;
        uvp = clamp(uvp, 0.0, 1.0);
        texColor = texture(painting2Tex, uvp);
        FragColor = texColor;
        return;
    }

    // Glass is usually a separate slightly-inset quad; if present and this
    // fragment belongs to the glass geometry it will typically have a
    // different vWorldPos (slightly inset). We cannot reliably detect glass
    // by position here for arbitrary rooms, so rely on the caller to set
    // appropriate geometry. If glass texture exists and the fragment's
    // material is similar to vColor==vec3(0.6,0.8,1.0) we try to sample it.
    if (uHasGlass == 1) {
        // Heuristic: if the vertex color is close to the default glass color,
        // prefer sampling the glass texture. This lets the generated glass
        // quad use the glass texture while other geometry uses wall/floor.
        vec3 glassRef = vec3(0.6, 0.8, 1.0);
        if (distance(vColor, glassRef) < 0.1) {
            vec4 g = texture(glassTex, uv);
            // If texture has alpha channel, use it. Otherwise derive alpha
            // from luminance and the provided uniform multiplier.
            float alpha = (g.a > 0.001) ? g.a : (clamp((g.r + g.g + g.b) / 3.0 * uGlassOpacity, 0.0, 1.0));
            // Blend glass color over the base based on computed alpha.
            vec3 glassCol = g.rgb * vColor;
            texColor.rgb = mix(texColor.rgb, glassCol, alpha);
            texColor.a = alpha;
        }
    }

    FragColor = texColor;
}
